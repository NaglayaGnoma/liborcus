/*************************************************************************
 *
 * Copyright (c) 2012 Kohei Yoshida
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 ************************************************************************/

#ifndef __ORCUS_SAX_PARSER_HPP__
#define __ORCUS_SAX_PARSER_HPP__

#include <exception>
#include <cassert>
#include <iostream>
#include <sstream>

#include "pstring.hpp"

#define ORCUS_DEBUG_SAX_PARSER 0

namespace orcus {

template<typename _Handler>
class sax_parser
{
public:
    typedef _Handler handler_type;

    class malformed_xml_error : public std::exception
    {
    public:
        malformed_xml_error(const std::string& msg) : m_msg(msg) {}
        virtual ~malformed_xml_error() throw() {}
        virtual const char* what() const throw()
        {
            return m_msg.c_str();
        }
    private:
        ::std::string m_msg;
    };

    sax_parser(const char* content, const size_t size, handler_type& handler);
    ~sax_parser();

    void parse();

private:

    std::string indent() const;

    void next() { ++m_pos; ++m_char; }

    void nest_up() { ++m_nest_level; }
    void nest_down()
    {
        assert(m_nest_level > 0);
        --m_nest_level;
    }

    char cur_char() const
    {
#if ORCUS_DEBUG_SAX_PARSER
        if (m_pos >= m_size)
            throw malformed_xml_error("xml stream ended prematurely.");
#endif
        return *m_char;
    }

    char next_char()
    {
        next();
#if ORCUS_DEBUG_SAX_PARSER
        if (m_pos >= m_size)
            throw malformed_xml_error("xml stream ended prematurely.");
#endif
        return *m_char;
    }

    void blank();

    /**
     * Parse XML header that occurs at the beginning of every XML stream i.e.
     * <?xml version="..." encoding="..." ?>
     */
    void header();
    void body();
    void element();
    void element_open();
    void element_close();
    void content();
    void characters();
    void attribute();

    void name(pstring& str);
    void value(pstring& str);

    static bool is_blank(char c);
    static bool is_alpha(char c);
    static bool is_name_char(char c);
    static bool is_numeric(char c);

private:
    const char* m_content;
    const char* m_char;
    const size_t m_size;
    size_t m_pos;
    size_t m_nest_level;
    handler_type& m_handler;
};

template<typename _Handler>
sax_parser<_Handler>::sax_parser(
    const char* content, const size_t size, handler_type& handler) :
    m_content(content),
    m_char(content),
    m_size(size),
    m_pos(0),
    m_nest_level(0),
    m_handler(handler)
{
}

template<typename _Handler>
sax_parser<_Handler>::~sax_parser()
{
}

template<typename _Handler>
void sax_parser<_Handler>::parse()
{
    using namespace std;
    m_pos = 0;
    m_nest_level = 0;
    m_char = m_content;
    header();
    blank();
    body();
    cout << "finished parsing" << endl;
}

template<typename _Handler>
::std::string sax_parser<_Handler>::indent() const
{
    ::std::ostringstream os;
    for (size_t i = 0; i < m_nest_level; ++i)
        os << "  ";
    return os.str();
}

template<typename _Handler>
void sax_parser<_Handler>::blank()
{
    char c = cur_char();
    while (is_blank(c))
        c = next_char();
}

template<typename _Handler>
void sax_parser<_Handler>::header()
{
    char c = cur_char();
    if (c != '<' || next_char() != '?' || next_char() != 'x' || next_char() != 'm' || next_char() != 'l')
        throw malformed_xml_error("xml header must begin with '<?xml'.");

    next();
    blank();
    while (cur_char() != '?')
    {
        attribute();
        blank();
    }
    if (next_char() != '>')
        throw malformed_xml_error("xml header must end with '?>'.");

    next();
}

template<typename _Handler>
void sax_parser<_Handler>::body()
{
    while (m_pos < m_size)
    {
        if (cur_char() == '<')
            element();
        else
            characters();
    }
}

template<typename _Handler>
void sax_parser<_Handler>::element()
{
    assert(cur_char() == '<');
    char c = next_char();
    if (c == '/')
        element_close();
    else
        element_open();
}

template<typename _Handler>
void sax_parser<_Handler>::element_open()
{
    assert(is_alpha(cur_char()));

    pstring ns_name, elem_name;
    name(elem_name);
    if (cur_char() == ':')
    {
        // this element name is namespaced.
        ns_name = elem_name;
        next();
        name(elem_name);
    }

    while (true)
    {
        blank();
        char c = cur_char();
        if (c == '/')
        {
            // Self-closing element: <element/>
            if (next_char() != '>')
                throw malformed_xml_error("expected '/>' to self-close the element.");
            next();
            m_handler.start_element(ns_name, elem_name);
            m_handler.end_element(ns_name, elem_name);
            return;
        }
        else if (c == '>')
        {
            // End of opening element: <element>
            next();
            m_handler.start_element(ns_name, elem_name);
            nest_up();
            return;
        }
        else
            attribute();
    }
}

template<typename _Handler>
void sax_parser<_Handler>::element_close()
{
    assert(cur_char() == '/');
    nest_down();
    next();
    pstring ns_name, elem_name;
    name(elem_name);
    if (cur_char() == ':')
    {
        ns_name = elem_name;
        next();
        name(elem_name);
    }

    if (cur_char() != '>')
        throw malformed_xml_error("expected '>' to close the element.");
    next();

    m_handler.end_element(ns_name, elem_name);
}

template<typename _Handler>
void sax_parser<_Handler>::characters()
{
    size_t first = m_pos;
    char c = cur_char();
    while (c != '<')
        c = next_char();

    if (m_pos > first)
    {
        size_t size = m_pos - first;
        pstring val(reinterpret_cast<const char*>(m_content) + first, size);
        m_handler.characters(val);
    }
}

template<typename _Handler>
void sax_parser<_Handler>::attribute()
{
    pstring attr_ns_name, attr_name, attr_value;
    name(attr_name);
    if (cur_char() == ':')
    {
        // Attribute name is namespaced.
        attr_ns_name = attr_name;
        next();
        name(attr_name);
    }

    char c = cur_char();
    if (c != '=')
        throw malformed_xml_error("attribute must begin with 'name=..");
    next();
    value(attr_value);

    m_handler.attribute(attr_ns_name, attr_name, attr_value);
}

template<typename _Handler>
void sax_parser<_Handler>::name(pstring& str)
{
    size_t first = m_pos;
    char c = cur_char();
    if (!is_alpha(c))
    {
        ::std::ostringstream os;
        os << "name must begin with an alphabet, but got this instead '" << c << "'";
        throw malformed_xml_error(os.str());
    }

    while (is_alpha(c) || is_numeric(c) || is_name_char(c))
        c = next_char();

    size_t size = m_pos - first;
    str = pstring(reinterpret_cast<const char*>(m_content) + first, size);
}

template<typename _Handler>
void sax_parser<_Handler>::value(pstring& str)
{
    char c = cur_char();
    if (c != '"')
        throw malformed_xml_error("attribute value must be quoted");

    c = next_char();
    size_t first = m_pos;
    while (c != '"')
        c = next_char();

    size_t size = m_pos - first;
    str = pstring(reinterpret_cast<const char*>(m_content) + first, size);

    // Skip the closing quote.
    next();
}

template<typename _Handler>
bool sax_parser<_Handler>::is_blank(char c)
{
    if (c == ' ')
        return true;
    if (c == 0x0A || c == 0x0D)
        // LF or CR
        return true;
    return false;
}

template<typename _Handler>
bool sax_parser<_Handler>::is_alpha(char c)
{
    if ('a' <= c && c <= 'z')
        return true;
    if ('A' <= c && c <= 'Z')
        return true;
    return false;
}

template<typename _Handler>
bool sax_parser<_Handler>::is_name_char(char c)
{
    if (c == '-')
        return true;

    return false;
}

template<typename _Handler>
bool sax_parser<_Handler>::is_numeric(char c)
{
    if ('0' <= c && c <= '9')
        return true;
    return false;
}

}

#endif
