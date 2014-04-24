//todoooo ����� ��� ���������� ����� ����� ��������� ���� �� �������

#pragma once
#include "stdAfx.h"

#include "Converter.h"

#include "XlsxConverter.h"
#include "DocxConverter.h"

#include "odf_conversion_context.h"

#include "odf_text_context.h"
#include "odf_drawing_context.h"

namespace Oox2Odf
{
	static double pt2emu(double Val)
	{
		return (Val * 360000 * 2.54) / 72;
	}
	Converter::Converter(const std::wstring & path) 
    {
		std::wstring  type = L"spreadsheet";

		if (type == L"text")			impl_ = new DocxConverter(path);
		if (type == L"spreadsheet")		impl_ = new XlsxConverter(path);
	}

	Converter::~Converter() 
    {
		if (impl_ )delete impl_ ;
	}
    void Converter::convert()
    {
		if (!impl_)return;
        impl_->convertDocument();
    }
    void Converter::write(const std::wstring & path) const
    {
        return impl_->write(path);
    }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void OoxConverter::convert(OOX::WritingElement  *oox_unknown)
{
	if (oox_unknown == NULL)return;

	switch(oox_unknown->getType())
	{
		case OOX::et_a_prstGeom:
		{
			OOX::Drawing::CPresetGeometry2D* pPresetGeom = static_cast<OOX::Drawing::CPresetGeometry2D*>(oox_unknown);
			convert(pPresetGeom);
		}break;
		case OOX::et_a_custGeom:
		{
			OOX::Drawing::CCustomGeometry2D* pCustomGeom = static_cast<OOX::Drawing::CCustomGeometry2D*>(oox_unknown);
			convert(pCustomGeom);
		}break;
		case OOX::et_a_lnTo:
		{
			OOX::Drawing::CPath2DLineTo* pLineTo = static_cast<OOX::Drawing::CPath2DLineTo*>(oox_unknown);
			convert(pLineTo);
		}break;
		case OOX::et_a_moveTo:
		{
			OOX::Drawing::CPath2DMoveTo* pMoveTo = static_cast<OOX::Drawing::CPath2DMoveTo*>(oox_unknown);
			convert(pMoveTo);
		}break;
		case OOX::et_a_arcTo:
		{
			OOX::Drawing::CPath2DArcTo* pArcTo = static_cast<OOX::Drawing::CPath2DArcTo*>(oox_unknown);
			convert(pArcTo);
		}break;
		case OOX::et_a_quadBezTo:
		{
			OOX::Drawing::CPath2DQuadBezierTo* pQuadBezierTo = static_cast<OOX::Drawing::CPath2DQuadBezierTo*>(oox_unknown);
			convert(pQuadBezierTo);
		}break;
		case OOX::et_a_cubicBezTo:
		{
			OOX::Drawing::CPath2DCubicBezierTo* pCubicBezierT = static_cast<OOX::Drawing::CPath2DCubicBezierTo*>(oox_unknown);
			convert(pCubicBezierT);
		}break;
		case OOX::et_a_close:
		{
			OOX::Drawing::CPath2DClose* pClose= static_cast<OOX::Drawing::CPath2DClose*>(oox_unknown);
			convert(pClose);
		}break;
		default:
		{
			std::wstringstream ss;
			ss << L"[warning] :  no convert element(" << oox_unknown->getType() << L")\n";
			_CP_LOG(error) << ss.str();
		}
	}
}

void OoxConverter::convert_SpPr(OOX::Drawing::CShapeProperties *   oox_spPr)
{
	if (!oox_spPr) return;

	switch(oox_spPr->m_eGeomType)
	{
	case OOX::Drawing::geomtypeCustom :
			if(oox_spPr->m_oCustGeom.IsInit())convert(oox_spPr->m_oCustGeom.GetPointer());break;
	case OOX::Drawing::geomtypePreset:
			if(oox_spPr->m_oPrstGeom.IsInit())convert(oox_spPr->m_oPrstGeom.GetPointer());break;
	case OOX::Drawing::geomtypeUnknown:
	default:
		break;
	}
	odf_context()->drawing_context().start_area_properies();
	{
		switch (oox_spPr->m_eFillType)
		{
			case OOX::Drawing::filltypeBlip:		convert(oox_spPr->m_oBlipFill.GetPointer());break;
			case OOX::Drawing::filltypeGradient:	convert(oox_spPr->m_oGradFill.GetPointer());break;
			case OOX::Drawing::filltypePattern:		convert(oox_spPr->m_oPattFill.GetPointer());break;
			case OOX::Drawing::filltypeSolid:		convert(oox_spPr->m_oSolidFill.GetPointer());break;
			case OOX::Drawing::filltypeGroup:
			case OOX::Drawing::filltypeNo:			odf_context()->drawing_context().set_no_fill();
				break;
		}
		//....
	}
	odf_context()->drawing_context().end_area_properies();

	odf_context()->drawing_context().start_line_properies();
	{
		convert(oox_spPr->m_oLn.GetPointer());	//CLineProperties
	}
	odf_context()->drawing_context().end_line_properies();
////////

	//shadow 
///////////

	if (oox_spPr->m_oXfrm.IsInit())	//CTransform2D
	{
		if (oox_spPr->m_oXfrm->m_oFlipH.GetValue() == SimpleTypes::onoffTrue)
			odf_context()->drawing_context().set_flip_H(true);
		if (oox_spPr->m_oXfrm->m_oFlipV.GetValue() == SimpleTypes::onoffTrue)
			odf_context()->drawing_context().set_flip_V(true);
		if (oox_spPr->m_oXfrm->m_oRot.GetValue() > 0)
			odf_context()->drawing_context().set_rotate(oox_spPr->m_oXfrm->m_oRot.GetValue());
	}

	//nullable<OOX::Drawing::CEffectContainer>          m_oEffectDag;
	//nullable<OOX::Drawing::CEffectList>               m_oEffectList;

	//nullable<OOX::Drawing::COfficeArtExtensionList>   m_oExtLst;
	//nullable<OOX::Drawing::CScene3D>                  m_oScene3D;
	//nullable<OOX::Drawing::CShape3D>                  m_oSp3D;

}
void OoxConverter::convert_CNvPr(OOX::Drawing::CNonVisualDrawingProps * oox_cnvPr)
{
	if (!oox_cnvPr) return;

	if (oox_cnvPr->m_sName.IsInit())
	{
		std::wstring name = string2std_string(oox_cnvPr->m_sName.get());
		odf_context()->drawing_context().set_name(name);
	}
	if (oox_cnvPr->m_oId.IsInit())
	{
		int id =oox_cnvPr->m_oId->GetValue();
		odf_context()->drawing_context().set_z_order(id);
	}
	//nullable<CString>                               m_sDescr;
	//nullable<SimpleTypes::COnOff<>>                 m_oHidden;
	//nullable<CString>                               m_sTitle;
}

void OoxConverter::convert(OOX::Drawing::CCustomGeometry2D *oox_cust_geom)
{
	if (!oox_cust_geom)return;

	for (long i=0; i< oox_cust_geom->m_oPthLst.m_arrPath.GetSize();i++)
	{
		convert(&oox_cust_geom->m_oPthLst.m_arrPath[i]);
	}
}
void OoxConverter::convert(OOX::Drawing::CLineProperties *oox_line_prop)
{
	if (!oox_line_prop)return;
	
	switch (oox_line_prop->m_eFillType)
	{
		case OOX::Drawing::filltypeGradient:	convert(oox_line_prop->m_oGradFill.GetPointer());break;
		case OOX::Drawing::filltypePattern:		convert(oox_line_prop->m_oPattFill.GetPointer());break;
		case OOX::Drawing::filltypeSolid:		convert(oox_line_prop->m_oSolidFill.GetPointer());break;
		case OOX::Drawing::filltypeGroup:
		case OOX::Drawing::filltypeNo:			odf_context()->drawing_context().set_no_fill();
			break;
	}
	if (oox_line_prop->m_oW.IsInit())
	{
		odf_context()->drawing_context().set_line_width(oox_line_prop->m_oW->ToPoints());
	}
	if (oox_line_prop->m_oHeadEnd.IsInit())
	{
		int type =0, w=1, len =1;//medium arrow
		if (oox_line_prop->m_oHeadEnd->m_oLen.IsInit())		len = oox_line_prop->m_oHeadEnd->m_oLen->GetValue();
		if (oox_line_prop->m_oHeadEnd->m_oType.IsInit())	type = oox_line_prop->m_oHeadEnd->m_oType->GetValue();
		if (oox_line_prop->m_oHeadEnd->m_oW.IsInit())		w=oox_line_prop->m_oHeadEnd->m_oW->GetValue();
		odf_context()->drawing_context().set_line_head(type, len, w);
	}
	if (oox_line_prop->m_oTailEnd.IsInit())
	{
		int type =0, w=1, len =1;//medium arrow
		if (oox_line_prop->m_oTailEnd->m_oLen.IsInit())		len = oox_line_prop->m_oTailEnd->m_oLen->GetValue();
		if (oox_line_prop->m_oTailEnd->m_oType.IsInit())	type = oox_line_prop->m_oTailEnd->m_oType->GetValue();
		if (oox_line_prop->m_oTailEnd->m_oW.IsInit())		w = oox_line_prop->m_oTailEnd->m_oW->GetValue();
			
		odf_context()->drawing_context().set_line_tail(type, len, w);

	}

	if (oox_line_prop->m_oCustDash.IsInit())
	{
		//custom not support
	}
	if (oox_line_prop->m_oPrstDash.IsInit())
	{

	}
	//nullable<SimpleTypes::CPenAlignment<>>            m_oAlgn;
	//nullable<SimpleTypes::CLineCap<>>                 m_oCap;
	//nullable<SimpleTypes::CCompoundLine<>>            m_oCmpd;


	//ELineJoinType                                     m_eJoinType;   // ��� ���������� �����
	//nullable<OOX::Drawing::CLineJoinBevel>            m_oBevel;
	//nullable<OOX::Drawing::CLineJoinMiterProperties>  m_oMiter;
	//nullable<OOX::Drawing::CLineJoinRound>            m_oRound;

}

void OoxConverter::convert(OOX::Drawing::CPresetGeometry2D *oox_prst_geom)
{
	if (!oox_prst_geom)return;

	if (oox_prst_geom->m_oAvLst.IsInit())
	{
		for (long i=0; i<oox_prst_geom->m_oAvLst->m_arrGd.GetSize(); i++)
		{
			odf_context()->drawing_context().add_modifier(string2std_string(oox_prst_geom->m_oAvLst->m_arrGd[i].m_oFmla.GetValue()));
		}
	}
}
void OoxConverter::convert(OOX::Drawing::CPath2D *oox_geom_path)
{
	if (!oox_geom_path) return;

	odf_context()->drawing_context().set_viewBox(oox_geom_path->m_oW.GetValue(), oox_geom_path->m_oH.GetValue());

	for (long i =0 ; i< oox_geom_path->m_arrItems.GetSize(); i++)
	{
		convert(oox_geom_path->m_arrItems[i]);
	}
}
//////////////////////////////////////////////////////////////////////////////////////////
void OoxConverter::convert(OOX::Drawing::CBlipFillProperties *oox_bitmap_fill)
{
	if (!oox_bitmap_fill)return;

	//odf_context()->drawing_context().start_bitmap_fill();

	//odf_context()->drawing_context().end_bitmap_fill();

}

void OoxConverter::convert(OOX::Drawing::CGradientFillProperties *oox_grad_fill)
{
	if (!oox_grad_fill)return;
	//����� ������ ����� ������� ��������, � �� �������� ����� ����� !!! - ����� � ��� � ���� ...

	//odf_context()->drawing_context().start_gradient_fill();

	//odf::office_element_ptr gradient_element;

	//odf::create_element(L"draw",L"gradient",gradient_element,odf_context());
	//odf_context()->styles_context().add_style(gradient_element,false,true);

	//odf_context()->drawing_context().end_gradient_fill();
}
void OoxConverter::convert(OOX::Drawing::CPatternFillProperties *oox_pattern_fill)
{
	if (!oox_pattern_fill)return;
	
	//odf_context()->drawing_context().start_pattern_fill();


	//odf_context()->drawing_context().end_pattern_fill();

}
void OoxConverter::convert(OOX::Drawing::CSolidColorFillProperties *oox_solid_fill)
{
	if (!oox_solid_fill)return;

	std::wstring hexColor;
	_CP_OPT(double) opacity;
	switch( oox_solid_fill->m_eType )
	{
		case OOX::Drawing::colorSheme:	convert(&oox_solid_fill->m_oShemeClr,	hexColor, opacity);		break;
		case OOX::Drawing::colorHsl:	convert(&oox_solid_fill->m_oHslClr,		hexColor, opacity);		break;
		case OOX::Drawing::colorPrst:	convert(&oox_solid_fill->m_oPrstClr,	hexColor, opacity);		break;
		case OOX::Drawing::colorScRgb:	convert(&oox_solid_fill->m_oScrgbClr,	hexColor, opacity);		break;
		case OOX::Drawing::colorSRgb:	convert(&oox_solid_fill->m_oSrgbClr,	hexColor, opacity);		break;
		case OOX::Drawing::colorSys:	convert(&oox_solid_fill->m_oSysClr,		hexColor, opacity);		break;		
	}	
	odf_context()->drawing_context().set_solid_fill(hexColor);

	if (opacity)
	{
		odf_context()->drawing_context().set_opacity(*opacity);
	}
}

void OoxConverter::convert(OOX::Drawing::Colors::CColorTransform     *oox_Clr,	std::wstring & hexString, _CP_OPT(double) &opacity)
{
	if (!oox_Clr)return;
	BYTE ucA=0, ucG=0, ucB=0, ucR =0;
	oox_Clr->GetRGBA(ucR,ucG,ucB,ucA);

	SimpleTypes::CHexColor<SimpleTypes::hexcolorRGB> *oRgbColor = new SimpleTypes::CHexColor<SimpleTypes::hexcolorRGB>(ucR,ucG,ucB);
	if (oRgbColor)
	{		
		hexString = string2std_string(oRgbColor->ToString().Right(6));
		delete oRgbColor;
	}
	if (ucA !=255)opacity = (ucA/255.)* 100.;
}
void OoxConverter::convert(OOX::Drawing::CSchemeColor     *oox_ShemeClr,	std::wstring & hexString, _CP_OPT(double) &opacity)
{
	OOX::CTheme * theme= oox_theme();
	if (!oox_ShemeClr || !theme)return;

	int theme_ind = oox_ShemeClr->m_oVal.GetValue();
	
	BYTE ucA=0, ucG=0, ucB=0, ucR =0;
	bool result = false;

	switch(theme_ind)
	{
		case SimpleTypes::shemecolorvalLt1:
			result = theme->m_oThemeElements.m_oClrScheme.m_oLt1.tryGetRgb(ucR, ucG, ucB, ucA); break;
		case SimpleTypes::shemecolorvalLt2:
			result = theme->m_oThemeElements.m_oClrScheme.m_oLt2.tryGetRgb(ucR, ucG, ucB, ucA); break;
		case SimpleTypes::shemecolorvalDk1:
			result = theme->m_oThemeElements.m_oClrScheme.m_oDk1.tryGetRgb(ucR, ucG, ucB, ucA); break;
		case SimpleTypes::shemecolorvalDk2:
			result = theme->m_oThemeElements.m_oClrScheme.m_oDk2.tryGetRgb(ucR, ucG, ucB, ucA); break;
		case SimpleTypes::shemecolorvalAccent1:
			result = theme->m_oThemeElements.m_oClrScheme.m_oAccent1.tryGetRgb(ucR, ucG, ucB, ucA); break;
		case SimpleTypes::shemecolorvalAccent2:
			result = theme->m_oThemeElements.m_oClrScheme.m_oAccent2.tryGetRgb(ucR, ucG, ucB, ucA); break;
		case SimpleTypes::shemecolorvalAccent3:
			result = theme->m_oThemeElements.m_oClrScheme.m_oAccent3.tryGetRgb(ucR, ucG, ucB, ucA); break;
		case SimpleTypes::shemecolorvalAccent4:
			result = theme->m_oThemeElements.m_oClrScheme.m_oAccent4.tryGetRgb(ucR, ucG, ucB, ucA); break;
		case SimpleTypes::shemecolorvalAccent5:
			result = theme->m_oThemeElements.m_oClrScheme.m_oAccent5.tryGetRgb(ucR, ucG, ucB, ucA); break;
		case SimpleTypes::shemecolorvalAccent6:
			result = theme->m_oThemeElements.m_oClrScheme.m_oAccent6.tryGetRgb(ucR, ucG, ucB, ucA); break;
		case SimpleTypes::shemecolorvalFolHlink:
			result = theme->m_oThemeElements.m_oClrScheme.m_oFolHlink.tryGetRgb(ucR, ucG, ucB, ucA); break;
		case SimpleTypes::shemecolorvalHlink:
			result = theme->m_oThemeElements.m_oClrScheme.m_oHlink.tryGetRgb(ucR, ucG, ucB, ucA); break;
	}
	if (result == true)
	{
		oox_ShemeClr->SetRGBA(ucR,ucG,ucB,ucA);
		oox_ShemeClr->GetRGBA(ucR,ucG,ucB,ucA);

		SimpleTypes::CHexColor<SimpleTypes::hexcolorRGB> *oRgbColor = new SimpleTypes::CHexColor<SimpleTypes::hexcolorRGB>(ucR,ucG,ucB);
		if (oRgbColor)
		{		
			hexString = string2std_string(oRgbColor->ToString().Right(6));
			delete oRgbColor;
		}
		if (ucA !=255)opacity = (ucA/255.)* 100.;
	}
}
void OoxConverter::convert(OOX::Drawing::CPath2DLineTo *oox_geom_path)
{
	if (!oox_geom_path) return;

	std::wstring path_elm =				boost::lexical_cast<std::wstring> ( (int)pt2emu(oox_geom_path->m_oPt.m_oX.GetValue())) + 
					std::wstring(L" ")+ boost::lexical_cast<std::wstring> ((int)pt2emu(oox_geom_path->m_oPt.m_oY.GetValue()));
	
	odf_context()->drawing_context().add_path_element(std::wstring(L"L"), path_elm);
}
void OoxConverter::convert(OOX::Drawing::CPath2DMoveTo *oox_geom_path)
{
	if (!oox_geom_path) return;
	
	std::wstring path_elm =						boost::lexical_cast<std::wstring> ((int)pt2emu(oox_geom_path->m_oPt.m_oX.GetValue())) + 
							std::wstring(L" ")+ boost::lexical_cast<std::wstring> ((int)pt2emu(oox_geom_path->m_oPt.m_oY.GetValue()));	
	odf_context()->drawing_context().add_path_element(std::wstring(L"M"), path_elm);
}
void OoxConverter::convert(OOX::Drawing::CPath2DArcTo *oox_geom_path)
{
	if (!oox_geom_path) return;
	
	//std::wstring path_elm =						boost::lexical_cast<std::wstring> ((int)pt2emu(oox_geom_path->m_oPt.m_oX.GetValue())) + 
	//						std::wstring(L" ")+ boost::lexical_cast<std::wstring> ((int)pt2emu(oox_geom_path->m_oPt.m_oY.GetValue()));
	//
	//odf_context()->drawing_context().add_path_element(std::wstring(L"A"), path_elm);
}
void OoxConverter::convert(OOX::Drawing::CPath2DQuadBezierTo *oox_geom_path)
{
	if (!oox_geom_path) return;
	
	std::wstring path_elm =							boost::lexical_cast<std::wstring> ((int)pt2emu(oox_geom_path->m_oCtrl.m_oX.GetValue())) + 
							std::wstring(L" ") +	boost::lexical_cast<std::wstring> ((int)pt2emu(oox_geom_path->m_oCtrl.m_oY.GetValue())) + 
							std::wstring(L" ") +	boost::lexical_cast<std::wstring> ((int)pt2emu(oox_geom_path->m_oEnd.m_oX.GetValue())) +
							std::wstring(L" ") +	boost::lexical_cast<std::wstring> ((int)pt2emu(oox_geom_path->m_oEnd.m_oY.GetValue())) ;	
	odf_context()->drawing_context().add_path_element(std::wstring(L"S"), path_elm);

}
void OoxConverter::convert(OOX::Drawing::CPath2DCubicBezierTo *oox_geom_path)
{
	if (!oox_geom_path) return;

	std::wstring path_elm =							boost::lexical_cast<std::wstring> ((int)pt2emu(oox_geom_path->m_oCtrl1.m_oX.GetValue())) + 
							std::wstring(L" ") +	boost::lexical_cast<std::wstring> ((int)pt2emu(oox_geom_path->m_oCtrl1.m_oY.GetValue())) + 
							std::wstring(L" ") +	boost::lexical_cast<std::wstring> ((int)pt2emu(oox_geom_path->m_oCtrl2.m_oX.GetValue())) +
							std::wstring(L" ") +	boost::lexical_cast<std::wstring> ((int)pt2emu(oox_geom_path->m_oCtrl2.m_oY.GetValue())) +
							std::wstring(L" ") +	boost::lexical_cast<std::wstring> ((int)pt2emu(oox_geom_path->m_oEnd.m_oX.GetValue())) +
							std::wstring(L" ") +	boost::lexical_cast<std::wstring> ((int)pt2emu(oox_geom_path->m_oEnd.m_oY.GetValue())) ;	
	odf_context()->drawing_context().add_path_element(std::wstring(L"C"), path_elm);
}
void OoxConverter::convert(OOX::Drawing::CPath2DClose *oox_geom_path)
{
	if (!oox_geom_path) return;

	std::wstring path_elm ;	
	odf_context()->drawing_context().add_path_element(std::wstring(L"N"), path_elm);
}
} // namespace Docx2Odt