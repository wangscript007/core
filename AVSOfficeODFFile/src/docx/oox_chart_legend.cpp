#include "precompiled_cpodf.h"

#include <boost/foreach.hpp>
#include <boost/functional.hpp>
#include <cpdoccore/CPHash.h>
#include <cpdoccore/xml/simple_xml_writer.h>

#include "oox_chart_legend.h"

#include "oox_chart_shape.h"

namespace cpdoccore {
namespace oox {


//<c:txPr>
//	<a:bodyPr/>
//	<a:lstStyle/>
//	<a:p>
//		<a:pPr>
//			<a:defRPr sz="1340" b="1" i="1" baseline="0">
//			<a:solidFill><a:schemeClr val="tx2">
//			<a:lumMod val="75000"/></a:schemeClr>
//			</a:solidFill></a:defRPr>
//		</a:pPr>
//		<a:endParaRPr lang="ru-RU"/>
//	</a:p>
//</c:txPr>

void oox_chart_legend::oox_serialize_default_text(std::wostream & _Wostream)
{
	if (content_.text_properties_.size()<1)return;

	CP_XML_WRITER(_Wostream)
    {
		CP_XML_NODE(L"c:txPr")
		{	
			CP_XML_NODE(L"a:bodyPr");
			CP_XML_NODE(L"a:lstStyle");
			CP_XML_NODE(L"a:p")
			{
				CP_XML_NODE(L"a:pPr")
				{
					CP_XML_NODE(L"a:defRPr")
					{							
						_CP_OPT(double) dVal;
						_CP_OPT(int) iVal;	
						_CP_OPT(std::wstring) sVal;

						if (odf::GetProperty(content_.text_properties_,L"font-size",dVal))
							CP_XML_ATTR(L"sz", (int)(dVal.get()*100));
						
						if ((odf::GetProperty(content_.text_properties_,L"font-style",iVal)) && (*iVal >0))
							CP_XML_ATTR(L"i", "true");
						if ((odf::GetProperty(content_.text_properties_,L"font-weight",iVal)) && (*iVal >0))
							CP_XML_ATTR(L"b", "true");		
				
						if (odf::GetProperty(content_.text_properties_,L"font-color",sVal))
							CP_XML_NODE(L"a:solidFill")	
							{
								CP_XML_NODE(L"a:srgbClr"){CP_XML_ATTR(L"val", sVal.get());}
							}
					}

				}
			}
		}
	}
}

void oox_chart_legend::oox_serialize(std::wostream & _Wostream)
{
	oox_chart_shape shape;

	CP_XML_WRITER(_Wostream)
    {
		CP_XML_NODE(L"c:legend")
        {
            layout_.oox_serialize(CP_XML_STREAM());
			CP_XML_NODE(L"c:overlay")
			{
				CP_XML_ATTR(L"val", 0);
			}
			CP_XML_NODE(L"c:legendPos")
			{
				CP_XML_ATTR(L"val", "r");//  "b" | "l" |  "r" |  "t"// == bottom left right top

			}
			shape.content_ = content_.graphic_properties_;
			shape.oox_serialize(CP_XML_STREAM());
			
			oox_chart_shape shape;
			shape.content_ = content_.graphic_properties_;

			oox_serialize_default_text(CP_XML_STREAM());

		}
    }

}
}
}
