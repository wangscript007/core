#pragma once
#ifndef OOX_RPR_FILE_INCLUDE_H_
#define OOX_RPR_FILE_INCLUDE_H_

#include "../../Base/Nullable.h"
#include "../../Common/SimpleTypes_Word.h"
#include "../../Common/SimpleTypes_Shared.h"
#include "../../Common/ComplexTypes.h"

#include "../SimpleTypes_Spreadsheet.h"

namespace OOX
{
	namespace Spreadsheet
	{
		class CColor
		{
		public:
			WritingElement_AdditionConstructors(CColor)
			CColor()
			{
			}
			virtual ~CColor()
			{
			}

			virtual void    fromXML(XmlUtils::CXmlLiteReader& oReader)
			{
				ReadAttributes( oReader );

				if ( !oReader.IsEmptyNode() )
					oReader.ReadTillEnd();
			}
		private:
			void ReadAttributes(XmlUtils::CXmlLiteReader& oReader)
			{
				// ������ ��������
				WritingElement_ReadAttributes_Start( oReader )

					WritingElement_ReadAttributes_Read_if     ( oReader, _T("auto"),      m_oAuto )
					WritingElement_ReadAttributes_Read_if     ( oReader, _T("indexed"),      m_oIndexed )
					WritingElement_ReadAttributes_Read_if     ( oReader, _T("rgb"),      m_oRgb )
					WritingElement_ReadAttributes_Read_if     ( oReader, _T("theme"),      m_oTheme )
					WritingElement_ReadAttributes_Read_if     ( oReader, _T("tint"),      m_oTint )

					WritingElement_ReadAttributes_End( oReader )
			}
		public:
			nullable<SimpleTypes::COnOff<>>						m_oAuto;
			nullable<SimpleTypes::CUnsignedDecimalNumber<>>		m_oIndexed;
			nullable<CString>									m_oRgb;
			nullable<SimpleTypes::CUnsignedDecimalNumber<>>		m_oTheme;
			nullable<SimpleTypes::CDouble>						m_oTint;
		};
		//������������:
		class CRPr : public WritingElementWithChilds
		{
		public:
			WritingElement_AdditionConstructors(CRPr)
			CRPr()
			{
			}
			virtual ~CRPr()
			{
			}

		public:
			virtual void         fromXML(XmlUtils::CXmlLiteReader& oReader)
			{
				ReadAttributes( oReader );

				if ( oReader.IsEmptyNode() )
					return;

				int nCurDepth = oReader.GetDepth();
				while( oReader.ReadNextSiblingNode( nCurDepth ) )
				{
					CWCharWrapper sName = oReader.GetName();

					if ( _T("b") == sName )
						m_oBold = oReader;
					else if ( _T("charset") == sName )
						;//pItem = new CText( oReader );
					else if ( _T("color") == sName )
						m_oColor = oReader;
					else if ( _T("condense") == sName )
						m_oCondense = oReader;
					else if ( _T("extend") == sName )
						m_oExtend = oReader;
					else if ( _T("family") == sName )
						;//m_oFamily = oReader;
					else if ( _T("i") == sName )
						m_oItalic = oReader;
					else if ( _T("outline") == sName )
						m_oOutline = oReader;
					else if ( _T("rFont") == sName )
						m_oRFont = oReader;
					else if ( _T("scheme") == sName )
						;//m_oScheme = oReader;
					else if ( _T("shadow") == sName )
						m_oShadow = oReader;
					else if ( _T("strike") == sName )
						m_oStrike = oReader;
					else if ( _T("sz") == sName )
						m_oSz = oReader;
					else if ( _T("u") == sName )
						m_oUnderline = oReader;
					else if ( _T("vertAlign") == sName )
						m_oVertAlign = oReader;

				}
			}

			virtual EElementType getType () const
			{
				return et_rPr;
			}

		private:
			void ReadAttributes(XmlUtils::CXmlLiteReader& oReader)
			{
			}
		public:
			nullable<ComplexTypes::Word::COnOff2<SimpleTypes::onoffTrue> >	m_oBold;
			nullable<ComplexTypes::Word::COnOff2<SimpleTypes::onoffTrue> >	m_oCharset;
			nullable<CColor>												m_oColor;
			nullable<ComplexTypes::Word::COnOff2<SimpleTypes::onoffTrue> >	m_oCondense;
			nullable<ComplexTypes::Word::COnOff2<SimpleTypes::onoffTrue> >	m_oExtend;
			//nullable<SimpleTypes::Spreadsheet::CFontFamily<> >				m_oFamily;
			nullable<ComplexTypes::Word::COnOff2<SimpleTypes::onoffTrue> >	m_oItalic;
			nullable<ComplexTypes::Word::COnOff2<SimpleTypes::onoffTrue> >	m_oOutline;
			nullable<ComplexTypes::Word::CString_ >							m_oRFont;
			//nullable<SimpleTypes::Spreadsheet::CFontScheme<> >				m_oScheme;
			nullable<ComplexTypes::Word::COnOff2<SimpleTypes::onoffTrue> >	m_oShadow;
			nullable<ComplexTypes::Word::COnOff2<SimpleTypes::onoffTrue> >	m_oStrike;
			nullable<ComplexTypes::Word::CDouble2>							m_oSz;
			nullable<SimpleTypes::Spreadsheet::CUnderline<> >				m_oUnderline;
			nullable<SimpleTypes::CVerticalAlignRun<> >						m_oVertAlign;
		};
	} //Spreadsheet
} // namespace OOX

#endif // OOX_RPR_FILE_INCLUDE_H_