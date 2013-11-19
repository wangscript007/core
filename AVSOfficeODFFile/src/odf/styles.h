#ifndef _CPDOCCORE_ODF_OFFCIE_STYLES_H_
#define _CPDOCCORE_ODF_OFFCIE_STYLES_H_

#include <string>
#include <vector>
#include <iosfwd>

#include <cpdoccore/CPNoncopyable.h>
#include <cpdoccore/xml/attributes.h>

#include <cpdoccore/CPSharedPtr.h>
#include <cpdoccore/CPWeakPtr.h>
#include <cpdoccore/CPOptional.h>

#include "office_elements.h"
#include "office_elements_create.h"
#include "stylefamily.h"
#include "style_ref.h"
#include "textalign.h"
#include "length.h"
#include "styleverticalrel.h"
#include "styleverticalpos.h"
#include "percent.h"
#include "header_footer.h"
#include "verticalalign.h"
#include "pageusage.h"
#include "tablecentering.h"
#include "layoutgridmode.h"
#include "direction.h"
#include "linestyle.h"
#include "styletype.h"
#include "common_attlists.h"
#include "noteclass.h"

#include "styles_list.h"
#include "style_map.h"


namespace cpdoccore { namespace odf { 

class style_text_properties;
class style_paragraph_properties;
class style_graphic_properties;
class style_section_properties;
class style_table_cell_properties;
class style_table_row_properties;
class style_table_column_properties;
class style_chart_properties;

class style_content : noncopyable
{
public:
    void add_child_element( xml::sax * Reader, const ::std::wstring & Ns, const ::std::wstring & Name, document_context * Context);
    void docx_convert(oox::docx_conversion_context & Context);
    void xlsx_convert(oox::xlsx_conversion_context & Context);
    
    style_text_properties * get_style_text_properties() const;
    style_paragraph_properties * get_style_paragraph_properties() const;
    style_graphic_properties * get_style_graphic_properties() const;
    style_section_properties * get_style_section_properties() const;
    style_table_cell_properties * get_style_table_cell_properties() const;
    style_table_row_properties * get_style_table_row_properties() const;
    style_table_column_properties * get_style_table_column_properties() const;
    style_chart_properties * get_style_chart_properties() const;

private:
    style_family style_family_;

    office_element_ptr style_text_properties_;
    office_element_ptr style_paragraph_properties_;
    office_element_ptr style_section_properties_;
    office_element_ptr style_ruby_properties_;
    office_element_ptr style_table_properties_;
    office_element_ptr style_table_column_properties_;
    office_element_ptr style_table_row_properties_;
    office_element_ptr style_chart_properties_;
    office_element_ptr style_graphic_properties_;
    office_element_ptr style_table_cell_properties_;

};

/// \class  default_style
///         style:default-style
class default_style : public office_element_impl<default_style>
{
public:
    static const wchar_t * ns;
    static const wchar_t * name;
    static const xml::NodeType xml_type = xml::typeElement;
    static const ElementType type = typeStyleDefaultStyle;
    CPDOCCORE_DEFINE_VISITABLE();

public:
    virtual ::std::wostream & text_to_stream(::std::wostream & _Wostream) const;

public:
    default_style() {};

private:
    virtual void add_attributes( const xml::attributes_wc_ptr & Attributes );
    virtual void add_child_element( xml::sax * Reader, const ::std::wstring & Ns, const ::std::wstring & Name);
    virtual void add_text(const std::wstring & Text);

private:
    style_family style_family_;

    style_content style_content_;

    friend class odf_document;

};

CP_REGISTER_OFFICE_ELEMENT2(default_style);


/////////////////////////////////////////////////////////////////////////////////////////////////
class draw_gradient_properties
{
public:
    void add_attributes( const xml::attributes_wc_ptr & Attributes );

public:
	_CP_OPT(std::wstring)		draw_name_;
	_CP_OPT(std::wstring)		draw_display_name_;

	_CP_OPT(color)				draw_start_color_;
	_CP_OPT(color)				draw_end_color_;

	_CP_OPT(percent)			draw_end_intensity_; 
	_CP_OPT(percent)			draw_start_intensity_;

	_CP_OPT(length_or_percent)	draw_cy_;//%
	_CP_OPT(length_or_percent)	draw_cx_;
	
	_CP_OPT(percent)			draw_border_;
	_CP_OPT(int)				draw_angle_;
	_CP_OPT(std::wstring)		draw_style_;//"square" 
     
};

/// \class  style_draw_gradient_properties
///         style_draw_gradient_properties
class draw_gradient : public office_element_impl<draw_gradient>
{
public:
    static const wchar_t * ns;
    static const wchar_t * name;
    static const xml::NodeType xml_type = xml::typeElement;
    static const ElementType type = typeStyleDrawGradientPropertis;

    CPDOCCORE_DEFINE_VISITABLE();

    const draw_gradient_properties & content() const { return draw_gradient_properties_; }


private:
    virtual void add_attributes( const xml::attributes_wc_ptr & Attributes );
    virtual void add_child_element( xml::sax * Reader, const ::std::wstring & Ns, const ::std::wstring & Name);
 
    draw_gradient_properties draw_gradient_properties_;

    friend class odf_document;

};
CP_REGISTER_OFFICE_ELEMENT2(draw_gradient);
/////////////////////////////////////////////////////////////////////////////////////////////////
class style;
typedef shared_ptr<style>::Type style_ptr;
typedef weak_ptr<style>::Type style_weak_ptr;
typedef ::std::vector< style_weak_ptr > style_weak_ptr_array;
typedef ::std::vector< style_ptr > style_ptr_array;

/// \class  style
///         style:style
class style : public office_element_impl<style>
{
public:
    static const wchar_t * ns;
    static const wchar_t * name;
    static const xml::NodeType xml_type = xml::typeElement;
    static const ElementType type = typeStyleStyle;
    CPDOCCORE_DEFINE_VISITABLE();

public:
    virtual ::std::wostream & text_to_stream(::std::wostream & _Wostream) const;

public:
    style() : style_auto_update_(false) { }

private:
    virtual void add_attributes( const xml::attributes_wc_ptr & Attributes );
    virtual void add_child_element( xml::sax * Reader, const ::std::wstring & Ns, const ::std::wstring & Name);
    virtual void add_text(const std::wstring & Text);
    
private:
    // attr
    std::wstring style_name_;
    _CP_OPT( std::wstring ) style_display_name_; // optional 
    style_family style_family_;

    _CP_OPT( std::wstring ) style_parent_style_name_; // optional 
    _CP_OPT( std::wstring ) style_next_style_name_; // optional 
    _CP_OPT( std::wstring ) style_list_style_name_; // optional, may be empty
    _CP_OPT( std::wstring ) style_master_page_name_; // optional
    bool style_auto_update_; // optional, default = false
    _CP_OPT( std::wstring ) style_data_style_name_; // // optional
    _CP_OPT( std::wstring ) style_class_; // optional
    _CP_OPT(std::wstring) style_default_outline_level_;  // optional

private:
    style_content style_content_;
    office_element_ptr_array style_map_;

    friend class odf_document;

};

CP_REGISTER_OFFICE_ELEMENT2(style);

/// \class  styles
class styles
{
public:
    void add_child_element( xml::sax * Reader, const ::std::wstring & Ns, const ::std::wstring & Name, document_context * Context);

private:
    office_element_ptr_array style_style_;      ///< style-style
    office_element_ptr_array text_list_style_;  ///< text-list-style

    // number-number-style
    // number-currency-style
    // number-percentage-style
    // number-date-style
    // number-time-style
    // number-boolean-style
    office_element_ptr_array number_styles_; 

    friend class odf_document;
};


/// \class  office_automatic_styles
/// \brief  office:automatic-styles
class office_automatic_styles : public office_element_impl<office_automatic_styles>
{
public:
    static const wchar_t * ns;
    static const wchar_t * name;
    static const xml::NodeType xml_type = xml::typeElement;
    static const ElementType type = typeOfficeAutomaticStyles;
    CPDOCCORE_DEFINE_VISITABLE();

public:
    virtual ::std::wostream & text_to_stream(::std::wostream & _Wostream) const;

private:
    virtual void add_attributes( const xml::attributes_wc_ptr & Attributes );
    virtual void add_child_element( xml::sax * Reader, const ::std::wstring & Ns, const ::std::wstring & Name);
    virtual void add_text(const std::wstring & Text);

private:
    styles styles_; ///< styles
    office_element_ptr_array style_page_layout_; //< TODO
    friend class odf_document;
};

CP_REGISTER_OFFICE_ELEMENT2(office_automatic_styles);

/// \class  office_master_styles
/// \brief  office:master-styles
class office_master_styles : public office_element_impl<office_master_styles>
{
public:
    static const wchar_t * ns;
    static const wchar_t * name;
    static const xml::NodeType xml_type = xml::typeElement;
    static const ElementType type = typeOfficeMasterStyles;
    CPDOCCORE_DEFINE_VISITABLE();

public:
    virtual ::std::wostream & text_to_stream(::std::wostream & _Wostream) const;

private:
    virtual void add_attributes( const xml::attributes_wc_ptr & Attributes );
    virtual void add_child_element( xml::sax * Reader, const ::std::wstring & Ns, const ::std::wstring & Name);
    virtual void add_text(const std::wstring & Text);

private:
    office_element_ptr_array style_master_page_;
    office_element_ptr style_handout_master_; // TODO
    office_element_ptr draw_layer_set_; // TODO

    friend class odf_document;
};

CP_REGISTER_OFFICE_ELEMENT2(office_master_styles);

/// \class class style_master_page_attlist
/// \brief style-master-page-attlist
class style_master_page_attlist
{
public:
    void add_attributes( const xml::attributes_wc_ptr & Attributes );
    
public:
    _CP_OPT(style_ref) style_name_;
    _CP_OPT(std::wstring) style_display_name_;
    _CP_OPT(style_ref) style_page_layout_name_;
    _CP_OPT(style_ref) draw_style_name_;
    _CP_OPT(style_ref) style_next_style_name_;
};

/// \class  style_master_page
/// \brief  style:master-page
class style_master_page: public office_element_impl<style_master_page>
{
public:
    static const wchar_t * ns;
    static const wchar_t * name;
    static const xml::NodeType xml_type = xml::typeElement;
    static const ElementType type = typeStyleMasterPage;
    CPDOCCORE_DEFINE_VISITABLE();

public:
    virtual ::std::wostream & text_to_stream(::std::wostream & _Wostream) const;

private:
    virtual void add_attributes( const xml::attributes_wc_ptr & Attributes );
    virtual void add_child_element( xml::sax * Reader, const ::std::wstring & Ns, const ::std::wstring & Name);
    virtual void add_text(const std::wstring & Text);

public:
    style_master_page_attlist style_master_page_attlist_;

    office_element_ptr style_header_;  
    office_element_ptr style_header_left_; 
    office_element_ptr style_header_first_; 

	office_element_ptr style_footer_;      
    office_element_ptr style_footer_left_; 
    office_element_ptr style_footer_first_; 

	office_element_ptr office_forms_;       // TODO
    office_element_ptr_array style_style_;
    office_element_ptr_array shape_;        // TODO
    office_element_ptr presentation_notes_; // TODO           
};

CP_REGISTER_OFFICE_ELEMENT2(style_master_page);

/// \class  office_styles
/// \brief  office:styles
class office_styles : public office_element_impl<office_styles>
{
public:
    static const wchar_t * ns;
    static const wchar_t * name;
    static const xml::NodeType xml_type = xml::typeElement;
    static const ElementType type = typeOfficeStyles;
    CPDOCCORE_DEFINE_VISITABLE();

private:
    virtual void add_attributes( const xml::attributes_wc_ptr & Attributes );
    virtual void add_child_element( xml::sax * Reader, const ::std::wstring & Ns, const ::std::wstring & Name);

private:
    styles styles_;
    office_element_ptr_array style_default_style_;
    
	office_element_ptr text_outline_style_; // < TODO
    office_element_ptr_array text_notes_configuration_; // < TODO
    office_element_ptr text_bibliography_configuration_; // < TODO
    office_element_ptr text_linenumbering_configuration_; // < TODO
   
	office_element_ptr_array draw_gradient_; 
   
	office_element_ptr_array svg_linearGradient_; // < TODO
    office_element_ptr_array svg_radialGradient_; // < TODO
    
	office_element_ptr_array draw_hatch_; // < TODO
   
	office_element_ptr_array draw_fill_image_; // < TODO
    
	office_element_ptr_array draw_marker_; // < TODO
    office_element_ptr_array draw_stroke_dash_; // < TODO
    office_element_ptr_array draw_opacity_; // < TODO
    
	office_element_ptr_array style_presentation_page_layout_; // TODO

    friend class odf_document;
    
   
};

CP_REGISTER_OFFICE_ELEMENT2(office_styles);

class header_footer_content_impl
{
public:
    common_style_header_footer_attlist common_style_header_footer_attlist_;
    header_footer_content header_footer_content_;
};

class header_footer_impl
{
public:
    header_footer_content_impl & content() { return content_; }
    const header_footer_content_impl & content() const { return content_; }
    virtual ~header_footer_impl() {}
    void docx_convert(oox::docx_conversion_context & Context);
private:
    header_footer_content_impl content_;

};

/// \class  style_header
///         style:header
///         style-header
class style_header : public office_element_impl<style_header>, public header_footer_impl
{
public:
    static const wchar_t * ns;
    static const wchar_t * name;
    static const xml::NodeType xml_type = xml::typeElement;
    static const ElementType type = typeStyleHeader;
    CPDOCCORE_DEFINE_VISITABLE();

    virtual void docx_convert(oox::docx_conversion_context & Context) ;

private:
    virtual void add_attributes( const xml::attributes_wc_ptr & Attributes );
    virtual void add_child_element( xml::sax * Reader, const ::std::wstring & Ns, const ::std::wstring & Name);

public:
    //header_footer_content_impl content_;

};

CP_REGISTER_OFFICE_ELEMENT2(style_header);

/// \class  style_footer
///         style:footer
///         style-header
class style_footer : public office_element_impl<style_footer>, public header_footer_impl
{
public:
    static const wchar_t * ns;
    static const wchar_t * name;
    static const xml::NodeType xml_type = xml::typeElement;
    static const ElementType type = typeStyleFooter;
    CPDOCCORE_DEFINE_VISITABLE();

    virtual void docx_convert(oox::docx_conversion_context & Context) ;

private:
    virtual void add_attributes( const xml::attributes_wc_ptr & Attributes );
    virtual void add_child_element( xml::sax * Reader, const ::std::wstring & Ns, const ::std::wstring & Name);

public:
   // header_footer_content_impl content_;

};

CP_REGISTER_OFFICE_ELEMENT2(style_footer);
/// \class  style_header_first
///         style:header-first
///         style-header
class style_header_first : public office_element_impl<style_header_first>, public header_footer_impl
{
public:
    static const wchar_t * ns;
    static const wchar_t * name;
    static const xml::NodeType xml_type = xml::typeElement;
    static const ElementType type = typeStyleHeaderFirst;
    CPDOCCORE_DEFINE_VISITABLE();

    virtual void docx_convert(oox::docx_conversion_context & Context) ;

private:
    virtual void add_attributes( const xml::attributes_wc_ptr & Attributes );
    virtual void add_child_element( xml::sax * Reader, const ::std::wstring & Ns, const ::std::wstring & Name);

public:
    //header_footer_content_impl content_;

};

CP_REGISTER_OFFICE_ELEMENT2(style_header_first);

/// \class  style_footer_first
///         style:footer_first
///         style-header
class style_footer_first : public office_element_impl<style_footer_first>, public header_footer_impl
{
public:
    static const wchar_t * ns;
    static const wchar_t * name;
    static const xml::NodeType xml_type = xml::typeElement;
    static const ElementType type = typeStyleFooterFirst;
    CPDOCCORE_DEFINE_VISITABLE();

    virtual void docx_convert(oox::docx_conversion_context & Context) ;

private:
    virtual void add_attributes( const xml::attributes_wc_ptr & Attributes );
    virtual void add_child_element( xml::sax * Reader, const ::std::wstring & Ns, const ::std::wstring & Name);

public:
   // header_footer_content_impl content_;

};

CP_REGISTER_OFFICE_ELEMENT2(style_footer_first);
/// \class  style_header_left
///         style:header-left
///         style-header-left
class style_header_left : public office_element_impl<style_header_left>, public header_footer_impl
{
public:
    static const wchar_t * ns;
    static const wchar_t * name;
    static const xml::NodeType xml_type = xml::typeElement;
    static const ElementType type = typeStyleHeaderLeft;
    CPDOCCORE_DEFINE_VISITABLE();

    virtual void docx_convert(oox::docx_conversion_context & Context) ;

private:
    virtual void add_attributes( const xml::attributes_wc_ptr & Attributes );
    virtual void add_child_element( xml::sax * Reader, const ::std::wstring & Ns, const ::std::wstring & Name);

public:
   // header_footer_content_impl content_;

};

CP_REGISTER_OFFICE_ELEMENT2(style_header_left);

/// \class  style_footer_left
///         style:footer-left
///         style-footer-left
class style_footer_left : public office_element_impl<style_footer_left>, public header_footer_impl
{
public:
    static const wchar_t * ns;
    static const wchar_t * name;
    static const xml::NodeType xml_type = xml::typeElement;
    static const ElementType type = typeStyleFooterLeft;
    CPDOCCORE_DEFINE_VISITABLE();

    virtual void docx_convert(oox::docx_conversion_context & Context) ;

private:
    virtual void add_attributes( const xml::attributes_wc_ptr & Attributes );
    virtual void add_child_element( xml::sax * Reader, const ::std::wstring & Ns, const ::std::wstring & Name);

public:
   // header_footer_content_impl content_;

};

CP_REGISTER_OFFICE_ELEMENT2(style_footer_left);

/// style_columns
/// style-columns
/// style:columns
class style_columns : public office_element_impl<style_columns>
{
public:
    static const wchar_t * ns;
    static const wchar_t * name;
    static const xml::NodeType xml_type = xml::typeElement;
    static const ElementType type = typeStyleColumns;
    CPDOCCORE_DEFINE_VISITABLE();

private:
    virtual void add_attributes( const xml::attributes_wc_ptr & Attributes );
    virtual void add_child_element( xml::sax * Reader, const ::std::wstring & Ns, const ::std::wstring & Name);

public:
    _CP_OPT(unsigned int) fo_column_count_;
    _CP_OPT(length) fo_column_gap_;

    office_element_ptr style_column_sep_;
    office_element_ptr_array style_column_;

};

CP_REGISTER_OFFICE_ELEMENT2(style_columns);

/// style_column
/// style-column
/// style:column
class style_column : public office_element_impl<style_column>
{
public:
    static const wchar_t * ns;
    static const wchar_t * name;
    static const xml::NodeType xml_type = xml::typeElement;
    static const ElementType type = typeStyleColumn;
    CPDOCCORE_DEFINE_VISITABLE();

private:
    virtual void add_attributes( const xml::attributes_wc_ptr & Attributes );
    virtual void add_child_element( xml::sax * Reader, const ::std::wstring & Ns, const ::std::wstring & Name);

private:
    _CP_OPT(length) style_rel_width_;
    length fo_start_indent_;
    length fo_end_indent_;
    length fo_space_before_;
    length fo_space_after_;
    
};

CP_REGISTER_OFFICE_ELEMENT2(style_column);

/// style_column_sep
/// style-column-sep
/// style:column-sep
class style_column_sep : public office_element_impl<style_column_sep>
{
public:
    static const wchar_t * ns;
    static const wchar_t * name;
    static const xml::NodeType xml_type = xml::typeElement;
    static const ElementType type = typeStyleColumnSep;
    CPDOCCORE_DEFINE_VISITABLE();

private:
    virtual void add_attributes( const xml::attributes_wc_ptr & Attributes );
    virtual void add_child_element( xml::sax * Reader, const ::std::wstring & Ns, const ::std::wstring & Name);

private:
    std::wstring style_style_; // default solid
    _CP_OPT(length) style_width_;
    percent style_height_; // default 100
    vertical_align style_vertical_align_; //default top
    color style_color_; // default #000000
};

CP_REGISTER_OFFICE_ELEMENT2(style_column_sep);

/// \class  style_section_properties
///         style-section-properties
///         style:section-properties
class style_section_properties : public office_element_impl<style_section_properties>
{
public:
    static const wchar_t * ns;
    static const wchar_t * name;
    static const xml::NodeType xml_type = xml::typeElement;
    static const ElementType type = typeStyleSectionProperties;
    CPDOCCORE_DEFINE_VISITABLE();

private:
    virtual void add_attributes( const xml::attributes_wc_ptr & Attributes );
    virtual void add_child_element( xml::sax * Reader, const ::std::wstring & Ns, const ::std::wstring & Name);

public:

    // 15.7.1
    common_background_color_attlist common_background_color_attlist_;
    office_element_ptr style_background_image_;

    // 15.7.2
    common_horizontal_margin_attlist common_horizontal_margin_attlist_;

    // 15.7.3
    office_element_ptr style_columns_;

    // 15.7.6
    bool style_protect_; // default false

    // 15.7.7
    _CP_OPT(bool) text_dont_balance_text_columns_;

    // 15.7.8
    common_writing_mode_attlist common_writing_mode_attlist_;
    
    // 15.7.9
    // TODO text-notes-configuration

};

CP_REGISTER_OFFICE_ELEMENT2(style_section_properties);

/// \class  style_page_layout_attlist
///         style-page-layout-attlist
class style_page_layout_attlist
{
public:
    void add_attributes( const xml::attributes_wc_ptr & Attributes );
    const std::wstring & get_style_name() const { return style_name_; }

private:
    std::wstring style_name_;
    page_usage style_page_usage_; // default All
        
};

/// \class style_header_style
/// style:header-style
/// style-header-style
class style_header_style : public office_element_impl<style_header_style>
{
public:
    static const wchar_t * ns;
    static const wchar_t * name;
    static const xml::NodeType xml_type = xml::typeElement;
    static const ElementType type = typeStyleHeaderStyle;
    CPDOCCORE_DEFINE_VISITABLE();

public:
    virtual ::std::wostream & text_to_stream(::std::wostream & _Wostream) const;

private:
    virtual void add_attributes( const xml::attributes_wc_ptr & Attributes );
    virtual void add_child_element( xml::sax * Reader, const ::std::wstring & Ns, const ::std::wstring & Name);

public:
	office_element_ptr style_header_footer_properties_;

};

CP_REGISTER_OFFICE_ELEMENT2(style_header_style);

/// \class style_footer_style
/// style:footer-style
/// style-footer-style
class style_footer_style : public office_element_impl<style_header_style>
{
public:
    static const wchar_t * ns;
    static const wchar_t * name;
    static const xml::NodeType xml_type = xml::typeElement;
    static const ElementType type = typeStyleFooterStyle;
    CPDOCCORE_DEFINE_VISITABLE();

public:
    virtual ::std::wostream & text_to_stream(::std::wostream & _Wostream) const;

private:
    virtual void add_attributes( const xml::attributes_wc_ptr & Attributes );
    virtual void add_child_element( xml::sax * Reader, const ::std::wstring & Ns, const ::std::wstring & Name);

public:
	office_element_ptr style_header_footer_properties_;

};

CP_REGISTER_OFFICE_ELEMENT2(style_footer_style);

/// \class style_page_layout
/// style:page-layout
/// style-page-layout
class style_page_layout : public office_element_impl<style_page_layout>
{
public:
    static const wchar_t * ns;
    static const wchar_t * name;
    static const xml::NodeType xml_type = xml::typeElement;
    static const ElementType type = typeStylePageLayout;
    CPDOCCORE_DEFINE_VISITABLE();

public:
    virtual ::std::wostream & text_to_stream(::std::wostream & _Wostream) const;
   
	style_page_layout_attlist style_page_layout_attlist_;
    office_element_ptr style_page_layout_properties_;
    office_element_ptr style_header_style_;
    office_element_ptr style_footer_style_;

private:
    virtual void add_attributes( const xml::attributes_wc_ptr & Attributes );
    virtual void add_child_element( xml::sax * Reader, const ::std::wstring & Ns, const ::std::wstring & Name);

    
};

CP_REGISTER_OFFICE_ELEMENT2(style_page_layout);

// style-page-layout-properties-attlist
class style_page_layout_properties_attlist
{
public:
    void add_attributes( const xml::attributes_wc_ptr & Attributes );

    void docx_convert(oox::docx_conversion_context & Context);

public:
    _CP_OPT(length) fo_page_width_;         // +
    _CP_OPT(length) fo_page_height_;        // +
    common_num_format_attlist common_num_format_attlist_;
    common_num_format_prefix_suffix_attlist common_num_format_prefix_suffix_attlist_;
    _CP_OPT(std::wstring) style_paper_tray_name_;
    _CP_OPT(std::wstring) style_print_orientation_; // +
    //15.2.5
    common_horizontal_margin_attlist common_horizontal_margin_attlist_;
    common_vertical_margin_attlist common_vertical_margin_attlist_;
    common_margin_attlist common_margin_attlist_;
    
    // 15.2.6
    common_border_attlist common_border_attlist_;
    // 15.2.7 
    common_border_line_width_attlist common_border_line_width_attlist_;
    // 15.2.8
    common_padding_attlist common_padding_attlist_;
    // 15.2.9
    common_shadow_attlist common_shadow_attlist_;
    
    // 15.2.10 
    common_background_color_attlist common_background_color_attlist_;
    // 15.2.12
    _CP_OPT(style_ref) style_register_truth_ref_style_name_;
    // 15.2.13 TODO
    _CP_OPT(std::wstring) style_print_;
        
    // 15.2.14
    _CP_OPT(direction) style_print_page_order_;

    // 15.2.15 TODO
    _CP_OPT(std::wstring) style_first_page_number_;

    // 15.2.16
    _CP_OPT(percent) style_scale_to_;
    _CP_OPT(unsigned int) style_scale_to_pages_;

    // 15.2.17
    _CP_OPT(table_centering) style_table_centering_;
    // 15.2.18
    _CP_OPT(length) style_footnote_max_height_;

    // 15.2.19
    common_writing_mode_attlist common_writing_mode_attlist_;

    // 15.2.21
    _CP_OPT(layout_grid_mode) style_layout_grid_mode_;

    // 15.2.22
    _CP_OPT(length) style_layout_grid_base_height_;

    // 15.2.23
    _CP_OPT(length) style_layout_grid_ruby_height_;

    // 15.2.24
    _CP_OPT(unsigned int) style_layout_grid_lines_;

    // 15.2.25
    _CP_OPT(color) style_layout_grid_color_;

    // 15.2.26
    _CP_OPT(bool) style_layout_grid_ruby_below_;  

    // 15.2.27
    _CP_OPT(bool) style_layout_grid_print_;

    // 15.2.28
    _CP_OPT(bool) style_layout_grid_display_;

};

// style-footnote-sep-attlist
class style_footnote_sep_attlist
{
public:
    void add_attributes( const xml::attributes_wc_ptr & Attributes );

private:
    _CP_OPT(length) style_width_;
    _CP_OPT(percent) style_rel_width_;
    _CP_OPT(color) style_color_;
    _CP_OPT(line_style) style_line_style_;
    style_type style_adjustment_; // default Left
    _CP_OPT(length) style_distance_before_sep_;
    _CP_OPT(length) style_distance_after_sep_;
                
};

// \class   style_footnote_sep
//          style-footnote-sep
//          style:footnote-sep
class style_footnote_sep : public office_element_impl<style_footnote_sep>
{
public:
    static const wchar_t * ns;
    static const wchar_t * name;
    static const xml::NodeType xml_type = xml::typeElement;
    static const ElementType type = typeStyleFootnoteSep;
    CPDOCCORE_DEFINE_VISITABLE();

private:
    virtual void add_attributes( const xml::attributes_wc_ptr & Attributes );
    virtual void add_child_element( xml::sax * Reader, const ::std::wstring & Ns, const ::std::wstring & Name);

private:
    style_footnote_sep_attlist style_footnote_sep_attlist_;
    
};

CP_REGISTER_OFFICE_ELEMENT2(style_footnote_sep);

// style-page-layout-properties-elements
class style_page_layout_properties_elements
{
public:
    void add_child_element( xml::sax * Reader, const ::std::wstring & Ns, const ::std::wstring & Name, document_context * Context);

public:
    office_element_ptr style_background_image_;
    office_element_ptr style_columns_;
    
    // 15.2.20
    office_element_ptr  style_footnote_sep_;

};

// \class   style_page_layout_properties
//          style-page-layout-properties
//          style:page-layout-properties
class style_page_layout_properties : public office_element_impl<style_page_layout_properties>
{
public:
    static const wchar_t * ns;
    static const wchar_t * name;
    static const xml::NodeType xml_type = xml::typeElement;
    static const ElementType type = typeStylePageLayout;
    CPDOCCORE_DEFINE_VISITABLE();

    void docx_convert(oox::docx_conversion_context & Context);

    const style_page_layout_properties_attlist & get_style_page_layout_properties_attlist() const
    {
        return style_page_layout_properties_attlist_;
    };

public:
    style_page_layout_properties() { }

private:
    virtual void add_attributes( const xml::attributes_wc_ptr & Attributes );
    virtual void add_child_element( xml::sax * Reader, const ::std::wstring & Ns, const ::std::wstring & Name);

public:
    style_page_layout_properties_attlist style_page_layout_properties_attlist_;
    style_page_layout_properties_elements style_page_layout_properties_elements_;
    
};

CP_REGISTER_OFFICE_ELEMENT2(style_page_layout_properties);

// \class   text_notes_configuration
//          text-notes-configuration
//          text:notes-configuration
class text_notes_configuration : public office_element_impl<text_notes_configuration>
{
public:
    static const wchar_t * ns;
    static const wchar_t * name;
    static const xml::NodeType xml_type = xml::typeElement;
    static const ElementType type = typeTextNotesConfiguration;
    CPDOCCORE_DEFINE_VISITABLE();

private:
    virtual void add_attributes( const xml::attributes_wc_ptr & Attributes );
    virtual void add_child_element( xml::sax * Reader, const ::std::wstring & Ns, const ::std::wstring & Name);
    virtual void add_text(const std::wstring & Text);

public:
    noteclass noteclass_;
    _CP_OPT(std::wstring) text_citation_style_name_;
    _CP_OPT(std::wstring) text_citation_body_style_name_;
    _CP_OPT(std::wstring) text_default_style_name_;
    _CP_OPT(std::wstring) text_master_page_name_;
    _CP_OPT(unsigned int) text_start_value_;
    common_num_format_prefix_suffix_attlist common_num_format_prefix_suffix_attlist_;
    common_num_format_attlist common_num_format_attlist_;
    _CP_OPT(std::wstring) text_start_numbering_at_;   
    _CP_OPT(std::wstring) text_footnotes_position_;

    office_element_ptr text_note_continuation_notice_forward_;
    office_element_ptr text_note_continuation_notice_backward_;
    
};

CP_REGISTER_OFFICE_ELEMENT2(text_notes_configuration);

} // namespace odf
} // namespace cpdoccore

#endif
