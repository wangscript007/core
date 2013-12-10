#include "precompiled_cpodf.h"
#include "draw_page.h"

#include <cpdoccore/xml/xmlchar.h>
#include <cpdoccore/xml/serialize.h>
#include <cpdoccore/xml/attributes.h>
#include <cpdoccore/xml/utils.h>
#include <cpdoccore/formulasconvert.h>

#include <boost/lexical_cast.hpp>

#include "serialize_elements.h"
#include "odfcontext.h"
#include <cpdoccore/odf/odf_document.h>

#include "calcs_styles.h"
#include "search_table_cell.h"

#include <cpdoccore/xml/simple_xml_writer.h>

namespace cpdoccore { 
namespace odf {

void draw_page::pptx_convert(oox::pptx_conversion_context & Context)
{
	const std::wstring pageStyleName	= draw_page_attr_.draw_style_name_.get_value_or(L"");
    const std::wstring pageName			= draw_page_attr_.draw_name_.get_value_or(L"");
    const std::wstring layoutName		= draw_page_attr_.page_layout_name_.get_value_or(L"");
    const std::wstring masterName		= draw_page_attr_.master_page_name_.get_value_or(L"");

    _CP_LOG(info) << L"[info][xlsx] process page(slide) \"" << pageName /*L"" */<< L"\"" << std::endl;

    Context.start_page(pageName, pageStyleName, layoutName,masterName);

	BOOST_FOREACH(const office_element_ptr& elm, content_)
    {
		elm->pptx_convert(Context);
	}

    Context.end_page();
}

}
}