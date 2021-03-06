/*************************************************************************
 *
 * Copyright (c) 2010 Kohei Yoshida
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

#include "xml_stream_handler.hpp"
#include "xml_context_base.hpp"

#include "orcus/exception.hpp"

namespace orcus {

xml_stream_handler::xml_stream_handler(xml_context_base* root_context) :
    mp_root_context(root_context)
{
    m_context_stack.push_back(root_context);
}

xml_stream_handler::~xml_stream_handler()
{
    delete mp_root_context;
}

void xml_stream_handler::start_element(const sax_token_parser_element& elem)
{
    xml_context_base& cur = get_current_context();
    if (!cur.can_handle_element(elem.ns, elem.name))
        m_context_stack.push_back(cur.create_child_context(elem.ns, elem.name));

    get_current_context().start_element(elem.ns, elem.name, elem.attrs);
}

void xml_stream_handler::end_element(const sax_token_parser_element& elem)
{
    bool ended = get_current_context().end_element(elem.ns, elem.name);

    if (ended)
    {
        size_t n = m_context_stack.size();

        if (n > 1)
        {
            // Call end_child_context of the parent context to provide a way for
            // the two adjacent contexts to communicate with each other.
            context_stack_type::reverse_iterator itr_cur = m_context_stack.rbegin();
            context_stack_type::reverse_iterator itr_par = itr_cur + 1;
            (*itr_par)->end_child_context(elem.ns, elem.name, *itr_cur);
        }

        m_context_stack.pop_back();
    }
}

void xml_stream_handler::characters(const pstring& str, bool transient)
{
    get_current_context().characters(str, transient);
}

xml_context_base& xml_stream_handler::get_current_context()
{
    if (m_context_stack.empty())
        return *mp_root_context;

    return *m_context_stack.back();
}

}
