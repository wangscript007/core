#pragma once

#include "OpenXmlPackage.h"

namespace DocFileFormat
{
	class WordprocessingDocument: public OpenXmlPackage
	{
		friend class Converter;
		friend class MainDocumentMapping;
		friend class CommandTableMapping;
		friend class StyleSheetMapping;
		friend class NumberingMapping;
		friend class FontTableMapping;
		friend class FootnotesMapping;
		friend class EndnotesMapping;
		friend class CommentsMapping;
		friend class SettingsMapping;
		friend class VMLPictureMapping;
		friend class VMLShapeMapping;
		friend class SectionPropertiesMapping;
		friend class HeaderMapping;
		friend class FooterMapping;
		friend class OleObjectMapping;

	private:
		wstring FileName;

		wstring FontTableXML;
		wstring DocumentXML;
		wstring StyleSheetXML;
		wstring FootnotesXML;
		wstring EndnotesXML;
		wstring NumberingXML;
		wstring CommentsXML;
		wstring SettingsXML;
		wstring CommandTableXML;

		list<ImageFileStructure> ImagesList;
		list<OleObjectFileStructure> OleObjectsList;

		list<wstring> HeaderXMLList;
		list<wstring> FooterXMLList;

#ifdef CREATE_ZIPPED_DOCX

		zipFile zf;

#endif // CREATE_ZIPPED_DOCX

	public:
		WordprocessingDocument( const WCHAR* _fileName, const WordDocument* _docFile );
		virtual ~WordprocessingDocument();
		void SaveDocument();
		void SaveDocument( const WCHAR *file );
	};
}