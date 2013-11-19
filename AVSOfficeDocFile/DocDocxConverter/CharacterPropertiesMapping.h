#pragma once

#include "IMapping.h"
#include "RevisionData.h"
#include "ParagraphPropertyExceptions.h"
#include "PropertiesMapping.h"
#include "LanguageId.h"
#include "LanguageIdMapping.h"
#include "StyleSheetMapping.h"
#include "BorderCode.h"
#include "ShadingDescriptor.h"
#include "WordDocument.h"

namespace DocFileFormat
{
	static const WCHAR SuperscriptIndex[3][12] =
	{
		_T( "baseline" ),
		_T( "superscript" ),
		_T( "subscript" )
	};

	class CharacterPropertiesMapping: public PropertiesMapping, public IMapping
	{
	public:
		CharacterPropertiesMapping( XmlUtils::CXmlWriter* writer, WordDocument* doc, RevisionData* rev, ParagraphPropertyExceptions* currentPapx, bool styleChpx, bool isRunStyleNeeded = true );
		CharacterPropertiesMapping( XMLTools::XMLElement<WCHAR>* rPr, WordDocument* doc, RevisionData* rev, ParagraphPropertyExceptions* currentPapx, bool styleChpx, bool isRunStyleNeeded = true );
		virtual ~CharacterPropertiesMapping();
		void Apply( IVisitable* chpx );
		bool CheckIsSymbolFont();

	private:
		void convertSprms( list<SinglePropertyModifier>* sprms, XMLTools::XMLElement<WCHAR>* parent );
		list<CharacterPropertyExceptions*> buildHierarchy( const StyleSheet* styleSheet, unsigned short istdStart );
		bool applyToggleHierachy( const SinglePropertyModifier& sprm );
		bool toogleValue( bool currentValue, byte toggle );

	protected:
		/// CHPX flags are special flags because the can be 0,1,128 and 129,
		/// so this method overrides the appendFlagElement method.
		virtual void appendFlagElement( XMLTools::XMLElement<WCHAR>* node, const SinglePropertyModifier& sprm, const WCHAR* elementName, bool unique );

	private:

		WordDocument* _doc;
		XMLTools::XMLElement<WCHAR>* _rPr; 
		unsigned short _currentIstd;
		RevisionData* _revisionData;
		bool _styleChpx;

		XmlUtils::CXmlWriter pRunPr;

		ParagraphPropertyExceptions* _currentPapx;
		list<CharacterPropertyExceptions*> _hierarchy;

		bool _isRunStyleNeeded;
		bool _isOwnRPr;

		std::wstring m_sAsciiFont;
		std::wstring m_sEastAsiaFont;
		std::wstring m_shAnsiFont;
	};
}