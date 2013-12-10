#include "precompiled_cpodf.h"
#include "pptx_conversion_context.h"

#include "measuredigits.h"
#include "pptx_package.h"

#include <cpdoccore/odf/odf_document.h>
#include <cpdoccore/xml/simple_xml_writer.h>
#include <cpdoccore/CPAssert.h>

#include <iostream>

#include "../odf/calcs_styles.h"
#include "../odf/odfcontext.h"

#include "pptx_default_serializes.h"

namespace cpdoccore { 

namespace odf 
{
    class odf_document;
}

namespace oox {
    
namespace package
{
    class pptx_document;
}

pptx_conversion_context::
pptx_conversion_context(::cpdoccore::oox::package::pptx_document * outputDocument,
                        ::cpdoccore::odf::odf_document * odfDocument): 
	output_document_(outputDocument)
	,odf_document_(odfDocument)
	,pptx_text_context_(odf_document_->odf_context(),*this)
	,pptx_slide_context_(*this/*, pptx_text_context_*/)
{
}
//
//void pptx_conversion_context::start_chart(std::wstring const & name)
//{
//	charts_.push_back(oox_chart_context::create(name));
//	//��������� ����� ����� ��� ���������
//	 //� ��� ����� �������������� ����� - � ��� ������� ������ ��� � ���� xml (�� - �� ����� ��������)
//	//���� �������� ����� ���������� � ����
//
//}
//
//void pptx_conversion_context::end_chart()
//{
//	//current_chart().set_drawing_link(current_sheet().get_drawing_link());
//	//�������� ����
//}

void pptx_conversion_context::process_layouts()
{
	odf::presentation_layouts_instance & layouts = root()->odf_context().styleContainer().presentation_layouts();
	//����� ������ ����������
	for (int layout_index =0; layout_index < layouts.content.size(); layout_index++)
	{
		start_layout(layout_index);

		odf::style_presentation_page_layout * layout = 
			root()->odf_context().pageLayoutContainer().presentation_page_layout_by_name(layouts.content[layout_index].layout_name);
		
		if (layout)
		{
			layout->pptx_convert(*this);
		}

		end_layout();	
	}
}
void pptx_conversion_context::process_master_pages()
{
	odf::presentation_masters_instance & masters = root()->odf_context().styleContainer().presentation_masters();

	//����� ������ ����������
	for (int master_index =0; master_index < masters.content.size();master_index++)
	{
		start_master(master_index);
		
		odf::style_master_page * master = 
			root()->odf_context().pageLayoutContainer().master_page_by_name(masters.content[master_index].master_name);

		master->pptx_convert(*this);
		
		end_master();	
	}
}

void pptx_conversion_context::process_styles()
{
 
}
void pptx_conversion_context::process_theme()
{
	std::wstring name = L"Users Theme";
 	start_theme(name);
	//
		pptx_serialize_clrScheme(current_theme().clrSchemeData());
		pptx_serialize_fmtScheme(current_theme().fmtSchemeData());
		pptx_serialize_fontScheme(current_theme().fontSchemeData());
	//
	current_theme();
	end_theme();

}
void pptx_conversion_context::start_document()
{
    odf::odf_read_context & odfContext = root()->odf_context();
    std::vector<const odf::style_instance *> instances;
    
	instances.push_back(odfContext.styleContainer().style_default_by_type(odf::style_family::Presentation));
	instances.push_back(odfContext.styleContainer().style_by_name(L"Default",odf::style_family::Presentation,false));

    odf::text_format_properties_content			textFormatProperties	= calc_text_properties_content(instances);
    odf::paragraph_format_properties			parFormatProperties		= calc_paragraph_properties_content(instances);


}
//void pptx_conversion_context::start_hyperlink(const std::wstring & styleName)
//{	
//	pptx_text_context_.start_hyperlink();
//	pptx_text_context_.start_span(styleName);//???
//}
//
//void pptx_conversion_context::end_hyperlink(std::wstring const & href)
//{
//	//std::wstring content = pptx_text_context_.end_span2();//????
//
//	//hId from href
//
//	std::wstring hId = get_slide_context().hyperlinks_.add(href);
//	pptx_text_context_.end_hyperlink(get_table_context().end_hyperlink(current_cell_address(), href, L""));
//}

void pptx_conversion_context::end_document()
{
    unsigned int count = 0;
   
	BOOST_FOREACH(const pptx_xml_slideMaster_ptr& slideM, slideMasters_)
    {
        package::slide_content_ptr content = package::slide_content::create();

		slideM->write_to(content->content());
        content->add_rels(slideM->slideMasterRels());//media & links rels

        output_document_->get_ppt_files().add_slideMaster(content);//slideMaster.xml

        CP_XML_WRITER(presentation_.slideMastersData())//presentation.xml
        {
            CP_XML_NODE(L"p:sldMasterId")
            {
                CP_XML_ATTR(L"id", 0x80000000 + count); 
                CP_XML_ATTR(L"r:id", slideM->rId());            
            }
        }
        count++;		
	}
////////////////////////////////////////////////////////////////////////////////////////////////////
	count=0;
	BOOST_FOREACH(const pptx_xml_slide_ptr& slide, slides_)
    {
        package::slide_content_ptr content = package::slide_content::create();

		slide->write_to(content->content());
        content->add_rels(slide->slideRels());//media & links rels

        output_document_->get_ppt_files().add_slide(content);//slide.xml

        CP_XML_WRITER(presentation_.slidesData())//presentation.xml
        {
            CP_XML_NODE(L"p:sldId")
            {
                CP_XML_ATTR(L"id", 0x100 + count); 
                CP_XML_ATTR(L"r:id", slide->rId());            
            }
        }
		count++;
    }
///////////////////////////////////////////////////////////////////////////////////////////
	BOOST_FOREACH(const pptx_xml_slideLayout_ptr& slideL, slideLayouts_)
    {
        package::slide_content_ptr content = package::slide_content::create();

		slideL->write_to(content->content());
        content->add_rels(slideL->slideLayoutRels());//media & links rels

        output_document_->get_ppt_files().add_slideLayout(content);//slideMaster.xml
	}
/////////////////////////////////////////////////////////////////////////////////
    odf::odf_read_context & context =  root()->odf_context();
    odf::page_layout_container & pageLayouts = context.pageLayoutContainer();
	if ((pageLayouts.master_pages().size()>0) && (pageLayouts.master_pages()[0]->style_master_page_attlist_.style_name_))//default
	{
		const std::wstring masterStyleName = pageLayouts.master_pages()[0]->style_master_page_attlist_.style_name_->style_name();
		const std::wstring pageProperties = root()->odf_context().pageLayoutContainer().page_layout_name_by_style(masterStyleName);

		odf::page_layout_instance *pages_layouts = root()->odf_context().pageLayoutContainer().page_layout_by_name(pageProperties);
		
		if (pages_layouts)pages_layouts->pptx_convert(*this);
	}

	pptx_serialize_size(current_presentation().notesSlidesSize(),6858000,9144000,L"p:notesSz");

////////////////////////////////////////////////////////////////////////////////////////////////////////////	
	output_document_->get_ppt_files().set_presentation(presentation_);
       
	output_document_->get_ppt_files().set_themes(theme_);

	output_document_->get_ppt_files().set_media(get_mediaitems());
}

void pptx_conversion_context::start_body()
{}

void pptx_conversion_context::end_body()
{}

pptx_xml_slide & pptx_conversion_context::current_slide()
{
    if (!slides_.empty())
    {
        return *slides_.back().get();
    }
    else
    {
        CP_ASSERT(false);
        throw std::runtime_error("internal error");
    }
}
pptx_xml_slideLayout & pptx_conversion_context::current_layout()
{
    if (!slideLayouts_.empty())
    {
        return *slideLayouts_.back().get();
    }
    else
    {
        CP_ASSERT(false);
        throw std::runtime_error("internal error");
    }
}
pptx_xml_theme & pptx_conversion_context::current_theme()
{
    if (theme_)
    {
        return *theme_;
    }
    else
    {
        CP_ASSERT(false);
        throw std::runtime_error("internal error");
    }
}
pptx_xml_presentation & pptx_conversion_context::current_presentation()
{
	return presentation_;
}

oox_chart_context & pptx_conversion_context::current_chart()
{
    if (!charts_.empty())
    {
        return *charts_.back().get();
    }
    else
    {
        CP_ASSERT(false);
        throw std::runtime_error("internal error");
    }
}
pptx_xml_slideMaster & pptx_conversion_context::current_master()
{
    if (!slideMasters_.empty())
    {
        return *slideMasters_.back().get();
    }
    else
    {
        CP_ASSERT(false);
        throw std::runtime_error("internal error");
    }
}
void pptx_conversion_context::create_new_slide(std::wstring const & name)
{
	pptx_xml_slide_ptr s = pptx_xml_slide::create(name,slides_.size()+1);
    slides_.push_back(s);
}
void pptx_conversion_context::create_new_slideLayout(int id)
{
	pptx_xml_slideLayout_ptr s = pptx_xml_slideLayout::create(id);
    slideLayouts_.push_back(s);
}
void pptx_conversion_context::create_new_slideMaster(int id)
{
	pptx_xml_slideMaster_ptr s = pptx_xml_slideMaster::create(id);
    slideMasters_.push_back(s);
}

bool pptx_conversion_context::start_page(const std::wstring & pageName,	const std::wstring & pageStyleName,
																		const std::wstring & pageLayoutName,
																		const std::wstring & pageMasterName)
{
    create_new_slide(pageName);
	get_slide_context().start_slide();//pageName, pageStyleName);

	current_master_page_name_ = pageMasterName;
	current_layout_page_name_ = pageLayoutName;
	
	//const std::wstring masterPageNameLayout = root()->odf_context().pageLayoutContainer().page_layout_name_by_style(current_master_page_name_);

	std::pair<int,std::wstring> layout_id =
					root()->odf_context().styleContainer().presentation_layouts().add_or_find(pageLayoutName,pageMasterName);

	current_slide().slideRels().add(relationship(layout_id.second, L"http://schemas.openxmlformats.org/officeDocument/2006/relationships/slideLayout",
		std::wstring(L"../slideLayouts/slideLayout")  + boost::lexical_cast<std::wstring>(layout_id.first) + L".xml"));

    return true;
}

bool pptx_conversion_context::start_layout(int layout_index)
{
	odf::presentation_layouts_instance & layouts = root()->odf_context().styleContainer().presentation_layouts();

	create_new_slideLayout(layouts.content[layout_index].Id);
	
	get_slide_context().start_slide();//layouts.content[layout_index].layout_name, L"");//?????

	current_master_page_name_ = layouts.content[layout_index].master_name;
	current_layout_page_name_ = L"";
	
	std::pair<int,std::wstring> master_id = //std::pair<int,std::wstring>(1,L"smId1");
			root()->odf_context().styleContainer().presentation_masters().add_or_find(layouts.content[layout_index].master_name);

	root()->odf_context().styleContainer().presentation_masters().add_layout_to(layouts.content[layout_index].master_name,layouts.content[layout_index]);

	current_layout().slideLayoutRels().add(relationship(master_id.second, L"http://schemas.openxmlformats.org/officeDocument/2006/relationships/slideMaster",
		std::wstring(L"../slideMasters/slideMaster")  + boost::lexical_cast<std::wstring>(master_id.first) + L".xml"));

//layout type

//<xsd:enumeration value="title"/>
//1375 <xsd:enumeration value="tx"/>
//1376 <xsd:enumeration value="twoColTx"/>
//1377 <xsd:enumeration value="tbl"/>
//1378 <xsd:enumeration value="txAndChart"/>
//1379 <xsd:enumeration value="chartAndTx"/>
//1380 <xsd:enumeration value="dgm"/>
//1381 <xsd:enumeration value="chart"/>
//1382 <xsd:enumeration value="txAndClipArt"/>
//1383 <xsd:enumeration value="clipArtAndTx"/>
//1384 <xsd:enumeration value="titleOnly"/>
//1385 <xsd:enumeration value="blank"/>
//1386 <xsd:enumeration value="txAndObj"/>
//1387 <xsd:enumeration value="objAndTx"/>
//1388 <xsd:enumeration value="objOnly"/>
//1389 <xsd:enumeration value="obj"/>
//1390 <xsd:enumeration value="txAndMedia"/>
//1391 <xsd:enumeration value="mediaAndTx"/>
//1392 <xsd:enumeration value="objOverTx"/>
//1393 <xsd:enumeration value="txOverObj"/>
//1394 <xsd:enumeration value="txAndTwoObj"/>
//1395 <xsd:enumeration value="twoObjAndTx"/>
//1396 <xsd:enumeration value="twoObjOverTx"/>
//1397 <xsd:enumeration value="fourObj"/>
//1398 <xsd:enumeration value="vertTx"/>
//1399 <xsd:enumeration value="clipArtAndVertTx"/>
//1400 <xsd:enumeration value="vertTitleAndTx"/>
//1401 <xsd:enumeration value="vertTitleAndTxOverChart"/>
//1402 <xsd:enumeration value="twoObj"/>
//1403 <xsd:enumeration value="objAndTwoObj"/>
//1404 <xsd:enumeration value="twoObjAndObj"/>
//1405 <xsd:enumeration value="cust"/>---------------------------------- !!!!!!!!!!!!!
//1406 <xsd:enumeration value="secHead"/>
//1407 <xsd:enumeration value="twoTxTwoObj"/>
//1408 <xsd:enumeration value="objTx"/>
//1409 <xsd:enumeration value="picTx"/>
 
	return true;
}
bool pptx_conversion_context::start_master(int master_index)
{
	odf::presentation_masters_instance & masters = root()->odf_context().styleContainer().presentation_masters();

	create_new_slideMaster(masters.content[master_index].Id);
	
	get_slide_context().start_slide();

	current_master_page_name_ = L"";
	current_layout_page_name_ = L"";
	
	//add default theme (�������� !!!!)
	current_master().add_theme(1, L"tId1");	//std::pair<int,std::wstring> thema_id = std::pair<int,std::wstring>(1,L"tmId1");//thema

	for (long i=0;i<masters.content[master_index].layouts.size();i++)
	{
		current_master().add_layout(masters.content[master_index].layouts[i].Id, masters.content[master_index].layouts[i].rId);
	}


	return true;
}
void pptx_conversion_context::end_page()
{
	get_slide_context().serialize(current_slide().slideData());
	get_slide_context().dump_rels(current_slide().slideRels());//hyperlinks, mediaitems, ...

	get_slide_context().end_slide();
}
void pptx_conversion_context::end_layout()
{
	get_slide_context().serialize(current_layout().slideLayoutData());
	get_slide_context().dump_rels(current_layout().slideLayoutRels());//hyperlinks, mediaitems, ...

	get_slide_context().end_slide();
}

void pptx_conversion_context::end_master()
{
	//get_slide_context().pptx_serializ(current_master().slideMasterData());
//	get_slide_context().dump_rels(current_master().slideMasterRels());//hyperlinks, mediaitems, ...

	get_slide_context().end_slide();
}
void pptx_conversion_context::start_theme(std::wstring & name)
{
	theme_ = pptx_xml_theme::create(name,1);
}
void pptx_conversion_context::end_theme()
{
}
void pptx_conversion_context::start_office_presentation()
{
}

void pptx_conversion_context::end_office_presentation()
{
}
void pptx_conversion_context::start_chart(std::wstring const & name)
{
	charts_.push_back(oox_chart_context::create(name));
	//��������� ����� ����� ��� ���������
	 //� ��� ����� �������������� ����� - � ��� ������� ������ ��� � ���� xml (�� - �� ����� ��������)
	//���� �������� ����� ���������� � ����

}

void pptx_conversion_context::end_chart()
{
	//current_chart().set_drawing_link(current_sheet().get_drawing_link());
	//�������� ����
}

}
}
