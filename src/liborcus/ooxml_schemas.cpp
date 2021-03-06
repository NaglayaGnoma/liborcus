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

#include "ooxml_schemas.hpp"

namespace orcus {

schema_t SCH_opc_content_types            = "http://schemas.openxmlformats.org/package/2006/content-types";
schema_t SCH_opc_rels                     = "http://schemas.openxmlformats.org/package/2006/relationships";
schema_t SCH_opc_rels_metadata_core_props = "http://schemas.openxmlformats.org/package/2006/relationships/metadata/core-properties";
schema_t SCH_od_rels_connections          = "http://schemas.openxmlformats.org/officeDocument/2006/relationships/connections";
schema_t SCH_od_rels_printer_settings     = "http://schemas.openxmlformats.org/officeDocument/2006/relationships/printerSettings";
schema_t SCH_od_rels_shared_strings       = "http://schemas.openxmlformats.org/officeDocument/2006/relationships/sharedStrings";
schema_t SCH_od_rels_styles               = "http://schemas.openxmlformats.org/officeDocument/2006/relationships/styles";
schema_t SCH_od_rels_theme                = "http://schemas.openxmlformats.org/officeDocument/2006/relationships/theme";
schema_t SCH_od_rels_worksheet            = "http://schemas.openxmlformats.org/officeDocument/2006/relationships/worksheet";
schema_t SCH_od_rels_extended_props       = "http://schemas.openxmlformats.org/officeDocument/2006/relationships/extended-properties";
schema_t SCH_od_rels_office_doc           = "http://schemas.openxmlformats.org/officeDocument/2006/relationships/officeDocument";
schema_t SCH_xlsx_main                    = "http://schemas.openxmlformats.org/spreadsheetml/2006/main";


namespace {

schema_t schs[] = {
    SCH_opc_content_types,
    SCH_opc_rels,
    SCH_opc_rels_metadata_core_props,
    SCH_od_rels_connections,
    SCH_od_rels_printer_settings,
    SCH_od_rels_shared_strings,
    SCH_od_rels_styles,
    SCH_od_rels_theme,
    SCH_od_rels_worksheet,
    SCH_od_rels_extended_props,
    SCH_od_rels_office_doc,
    SCH_xlsx_main,
    NULL
};

}

schema_t* SCH_all = schs;

}
