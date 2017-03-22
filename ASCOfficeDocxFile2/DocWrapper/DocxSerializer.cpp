﻿/*
 * (c) Copyright Ascensio System SIA 2010-2017
 *
 * This program is a free software product. You can redistribute it and/or
 * modify it under the terms of the GNU Affero General Public License (AGPL)
 * version 3 as published by the Free Software Foundation. In accordance with
 * Section 7(a) of the GNU AGPL its Section 15 shall be amended to the effect
 * that Ascensio System SIA expressly excludes the warranty of non-infringement
 * of any third-party rights.
 *
 * This program is distributed WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR  PURPOSE. For
 * details, see the GNU AGPL at: http://www.gnu.org/licenses/agpl-3.0.html
 *
 * You can contact Ascensio System SIA at Lubanas st. 125a-25, Riga, Latvia,
 * EU, LV-1021.
 *
 * The  interactive user interfaces in modified source and object code versions
 * of the Program must display Appropriate Legal Notices, as required under
 * Section 5 of the GNU AGPL version 3.
 *
 * Pursuant to Section 7(b) of the License you must retain the original Product
 * logo when distributing the program. Pursuant to Section 7(e) we decline to
 * grant you any rights under trademark law for use of our trademarks.
 *
 * All the Product's GUI elements, including illustrations and icon sets, as
 * well as technical writing content are licensed under the terms of the
 * Creative Commons Attribution-ShareAlike 4.0 International. See the License
 * terms at http://creativecommons.org/licenses/by-sa/4.0/legalcode
 *
 */
#include "DocxSerializer.h"

#include "../../DesktopEditor/common/Directory.h"
#include "../../DesktopEditor/common/File.h"
#include "../../DesktopEditor/common/Path.h"
#include "../BinWriter/BinWriters.h"
#include "../BinReader/Readers.h"
#include "../../ASCOfficePPTXFile/Editor/FontPicker.h"
#include "../../OfficeUtils/src/OfficeUtils.h"

int BinDocxRW::g_nCurFormatVersion = 0;

BinDocxRW::CDocxSerializer::CDocxSerializer()
{
	m_pParamsWriter = NULL;
	m_pCurFileWriter = NULL;
	m_bIsNoBase64Save = false;
	m_bSaveChartAsImg = false;
}
bool BinDocxRW::CDocxSerializer::ConvertDocxToDoct(const std::wstring& sSrcFileName, const std::wstring& sDstFileName, const std::wstring& sTmpDir, const std::wstring& sXMLOptions)
{
    std::wstring strDirSrc      = NSSystemPath::Combine(sTmpDir,	L"from");
    std::wstring strDirDst      = NSSystemPath::Combine(sTmpDir,	L"to");
    std::wstring strEditorBin   = NSSystemPath::Combine(strDirDst,	L"Editor.bin");

    NSDirectory::CreateDirectory(strDirSrc);
	NSDirectory::CreateDirectory(strDirDst);

    COfficeUtils oCOfficeUtils(NULL);

    if(S_OK == oCOfficeUtils.ExtractToDirectory(sSrcFileName, strDirSrc, NULL, 0))
		if(saveToFile(strEditorBin, strDirSrc, sXMLOptions))
            if(S_OK == oCOfficeUtils.CompressFileOrDirectory(strDirDst, sDstFileName))
				return true;
	return false;
}
bool BinDocxRW::CDocxSerializer::ConvertDoctToDocx(const std::wstring& sSrcFileName, const std::wstring& sDstFileName, const std::wstring& sTmpDir, const std::wstring& sXMLOptions)
{
	std::wstring strDirSrc		= NSSystemPath::Combine(sTmpDir,	L"from");
	std::wstring strEditorBin	= NSSystemPath::Combine(strDirSrc,	L"Editor.bin");
	std::wstring strDirDst		= NSSystemPath::Combine(sTmpDir,	L"to");
	
	NSDirectory::CreateDirectory(strDirSrc);
	NSDirectory::CreateDirectory(strDirDst);
	
    std::wstring sEditorBin = strEditorBin;

    COfficeUtils oCOfficeUtils(NULL);

    if(S_OK == oCOfficeUtils.ExtractToDirectory(sSrcFileName, strDirSrc, NULL, 0))
    {
        std::wstring sMediaPath;
        std::wstring sThemePath;
        std::wstring sEmbedPath;

        CreateDocxFolders(strDirDst, sThemePath, sMediaPath, sEmbedPath);

        if(loadFromFile(sEditorBin, strDirDst, sXMLOptions, sThemePath, sMediaPath, sEmbedPath))
        {
            if(S_OK == oCOfficeUtils.CompressFileOrDirectory(strDirDst, sDstFileName, true))
				return true;
        }
    }
    return false;
}
bool BinDocxRW::CDocxSerializer::saveToFile(const std::wstring& sSrcFileName, const std::wstring& sDstPath, const std::wstring& sXMLOptions)
{
	OOX::CPath pathMain(sSrcFileName);
    
	OOX::CPath pathMedia = pathMain.GetDirectory() + FILE_SEPARATOR_STR + _T("media");
	NSDirectory::CreateDirectory(pathMedia.GetPath());

	COfficeFontPicker* pFontPicker = new COfficeFontPicker();
	pFontPicker->Init(m_sFontDir);
	CFontManager* pFontManager = pFontPicker->get_FontManager();
	DocWrapper::FontProcessor fp;
	fp.setFontManager(pFontManager);
	NSBinPptxRW::CDrawingConverter oDrawingConverter;
	oDrawingConverter.SetFontManager(pFontManager);
	NSBinPptxRW::CBinaryFileWriter& oBufferedStream = *oDrawingConverter.m_pBinaryWriter;

	NSFontCutter::CEmbeddedFontsManager* pEmbeddedFontsManager = NULL;
    if(false == m_sEmbeddedFontsDir.empty())
	{
		NSDirectory::CreateDirectory(m_sEmbeddedFontsDir);

		pFontPicker->SetEmbeddedFontsDirectory(m_sEmbeddedFontsDir);

		pEmbeddedFontsManager = pFontPicker->GetNativeCutter();

		//добавляем весь латинский алфавит для списков.
        pEmbeddedFontsManager->CheckString(std::wstring(_T("abcdefghijklmnopqrstuvwxyz")));

		//добавим мега шрифт
		pEmbeddedFontsManager->CheckFont(_T("Wingdings 3"), fp.getFontManager());
		pEmbeddedFontsManager->CheckFont(_T("Arial"), fp.getFontManager());
		//pEmbeddedFontsManager добавляются все цифры
	}

	oDrawingConverter.SetFontDir(m_sFontDir);
	oDrawingConverter.SetFontPicker(pFontPicker);
	oDrawingConverter.SetMainDocument(this);
	oDrawingConverter.SetMediaDstPath(pathMedia.GetPath());
	
	m_pParamsWriter = new ParamsWriter(&oBufferedStream, &fp, &oDrawingConverter, pEmbeddedFontsManager);

	BinaryFileWriter oBinaryFileWriter(*m_pParamsWriter);
	oBinaryFileWriter.intoBindoc(sDstPath);
	BYTE* pbBinBuffer = oBufferedStream.GetBuffer();
	int nBinBufferLen = oBufferedStream.GetPosition();

    if (m_bIsNoBase64Save)
	{
		NSFile::CFileBinary oFile;
		oFile.CreateFileW(sSrcFileName);
		oFile.WriteFile(pbBinBuffer, nBinBufferLen);
		oFile.CloseFile();
	}
	else
	{
		int nBase64BufferLen = Base64::Base64EncodeGetRequiredLength(nBinBufferLen, Base64::B64_BASE64_FLAG_NOCRLF);
        BYTE* pbBase64Buffer = new BYTE[nBase64BufferLen + 64];
//        if(TRUE == Base64::Base64Encode(pbBinBuffer, nBinBufferLen, (LPSTR)pbBase64Buffer, &nBase64BufferLen, Base64::B64_BASE64_FLAG_NOCRLF))
        if(true == Base64_1::Base64Encode(pbBinBuffer, nBinBufferLen, pbBase64Buffer, &nBase64BufferLen))
		{
			NSFile::CFileBinary oFile;
			oFile.CreateFileW(sSrcFileName);
			oFile.WriteStringUTF8(oBinaryFileWriter.WriteFileHeader(nBinBufferLen));
			oFile.WriteFile(pbBase64Buffer, nBase64BufferLen);
			oFile.CloseFile();
		}
		RELEASEARRAYOBJECTS(pbBase64Buffer);
	}
	RELEASEOBJECT(m_pParamsWriter);
	RELEASEOBJECT(pFontPicker);
	return true;
}

bool BinDocxRW::CDocxSerializer::CreateDocxFolders(std::wstring strDirectory, std::wstring& sThemePath, std::wstring& sMediaPath, std::wstring& sEmbedPath)
{
	bool res = true;
	// rels
    OOX::CPath pathRels = strDirectory + FILE_SEPARATOR_STR + _T("_rels");
	if (!NSDirectory::CreateDirectory(pathRels.GetPath()))	res = false;

	// word
    OOX::CPath pathWord = strDirectory + FILE_SEPARATOR_STR + _T("word");
    if (!NSDirectory::CreateDirectory(pathWord.GetPath()))	res = false;

	// documentRels
    OOX::CPath pathWordRels = pathWord + FILE_SEPARATOR_STR + _T("_rels");
    if (!NSDirectory::CreateDirectory(pathWordRels.GetPath()))res = false;

	//media
    OOX::CPath pathMedia = pathWord + FILE_SEPARATOR_STR + _T("media");
	if (!NSDirectory::CreateDirectory(pathMedia.GetPath()))		res = false;
	sMediaPath = pathMedia.GetPath();

	//embeddings
    OOX::CPath pathEmbeddings = pathWord + FILE_SEPARATOR_STR + _T("embeddings");
	if (!NSDirectory::CreateDirectory(pathEmbeddings.GetPath()))res = false;
	sEmbedPath = pathEmbeddings.GetPath();

	// theme
    OOX::CPath pathTheme = pathWord + FILE_SEPARATOR_STR + _T("theme");
	if (!NSDirectory::CreateDirectory(pathTheme.GetPath()))		res = false;

    OOX::CPath pathThemeRels = pathTheme + FILE_SEPARATOR_STR + _T("_rels");
	if (!NSDirectory::CreateDirectory(pathThemeRels.GetPath())) res = false;
	
    pathTheme = pathTheme + FILE_SEPARATOR_STR + _T("theme1.xml");
	sThemePath = pathTheme.GetPath();

	return res;
}
bool BinDocxRW::CDocxSerializer::loadFromFile(const std::wstring& sSrcFileName, const std::wstring& sDstPath, const std::wstring& sXMLOptions, const std::wstring& sThemePath, const std::wstring& sMediaPath, const std::wstring& sEmbedPath)
{
	bool bResultOk = false;
	
	NSFile::CFileBinary oFile;
	if(oFile.OpenFile(sSrcFileName))
	{
		DWORD nBase64DataSize = 0;
		BYTE* pBase64Data = new BYTE[oFile.GetFileSize()];
		oFile.ReadFile(pBase64Data, oFile.GetFileSize(), nBase64DataSize);
		oFile.CloseFile();

		//проверяем формат
		bool bValidFormat = false;
        std::wstring sSignature(g_sFormatSignature);
        int nSigLength = (int)sSignature.length();
		if(nBase64DataSize > nSigLength)
		{
            std::string sCurSig((char*)pBase64Data, nSigLength);
            if(sSignature == std::wstring(sCurSig.begin(), sCurSig.end()))
			{
                bValidFormat = true;
            }
		}
		if(bValidFormat)
		{
			//Читаем из файла версию и длину base64
			int nIndex = nSigLength;
			int nType = 0;
            std::string version = "";
            std::string dst_len = "";
			while (true)
			{
				nIndex++;
				BYTE _c = pBase64Data[nIndex];
				if (_c == ';')
				{
					if(0 == nType)
					{
						nType = 1;
						continue;
					}
                    else
					{
						nIndex++;
						break;
					}
				}
				if(0 == nType)
                    version += _c;
				else
                    dst_len += _c;
			}
			
            int nDataSize = atoi(dst_len.c_str());
			BYTE* pData = new BYTE[nDataSize];
			
            if(false != Base64::Base64Decode((const char*)(pBase64Data + nIndex), nBase64DataSize - nIndex, pData, &nDataSize))
			{
                NSBinPptxRW::CDrawingConverter oDrawingConverter;
				NSBinPptxRW::CBinaryFileReader& oBufferedStream = *oDrawingConverter.m_pReader;
				oBufferedStream.Init(pData, 0, nDataSize);

				int nVersion = g_nFormatVersion;
                if(!version.empty())
				{
                    version = version.substr(1);
                    int nTempVersion = atoi(version.c_str());
					if(0 != nTempVersion)
					{
						g_nCurFormatVersion = nVersion = nTempVersion;
					}
				}
				oDrawingConverter.SetMainDocument(this);
				oDrawingConverter.SetMediaDstPath(sMediaPath);
				oDrawingConverter.SetEmbedDstPath(sEmbedPath);
				m_pCurFileWriter = new Writers::FileWriter(sDstPath, m_sFontDir, false, nVersion, m_bSaveChartAsImg, &oDrawingConverter, sThemePath);

				//папка с картинками
				std::wstring strFileInDir = NSSystemPath::GetDirectoryName(sSrcFileName);
                std::wstring sFileInDir = strFileInDir.c_str();

                oDrawingConverter.SetSourceFileDir(sFileInDir);
	//default theme
				m_pCurFileWriter->m_oDefaultTheme.Write(sThemePath);
				
				BinaryFileReader oBinaryFileReader(sFileInDir, oBufferedStream, *m_pCurFileWriter);
				oBinaryFileReader.ReadFile();
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				OOX::CContentTypes oContentTypes;
	//docProps
                OOX::CPath pathDocProps = sDstPath + FILE_SEPARATOR_STR + _T("docProps");
                NSDirectory::CreateDirectory(pathDocProps.GetPath());
				
                OOX::CPath DocProps = std::wstring(_T("docProps"));

				OOX::CApp* pApp = new OOX::CApp();
				if (pApp)
				{
					pApp->SetApplication(_T("OnlyOffice"));
					pApp->SetAppVersion(_T("3.0000"));
					pApp->SetDocSecurity(0);
					pApp->SetScaleCrop(false);
					pApp->SetLinksUpToDate(false);
					pApp->SetSharedDoc(false);
					pApp->SetHyperlinksChanged(false);
					
					pApp->write(pathDocProps + FILE_SEPARATOR_STR + _T("app.xml"), DocProps, oContentTypes);
					delete pApp;
				}				
				OOX::CCore* pCore = new OOX::CCore();
				if (pCore)
				{
					pCore->SetCreator(_T(""));
					pCore->SetLastModifiedBy(_T(""));
					pCore->write(pathDocProps + FILE_SEPARATOR_STR + _T("core.xml"), DocProps, oContentTypes);
					delete pCore;
				} 
/////////////////////////////////////////////////////////////////////////////////////
                m_pCurFileWriter->m_oContentTypesWriter.AddOverrideRaw(oDrawingConverter.GetContentTypes());

				m_pCurFileWriter->m_oCommentsWriter.Write();
				m_pCurFileWriter->m_oChartWriter.Write();
				m_pCurFileWriter->m_oStylesWriter.Write();
				m_pCurFileWriter->m_oNumberingWriter.Write();
				m_pCurFileWriter->m_oFontTableWriter.Write();
				m_pCurFileWriter->m_oHeaderFooterWriter.Write();
				m_pCurFileWriter->m_oFootnotesWriter.Write();
				m_pCurFileWriter->m_oEndnotesWriter.Write();
				//Setting пишем после HeaderFooter, чтобы заполнить evenAndOddHeaders
				m_pCurFileWriter->m_oSettingWriter.Write();
				m_pCurFileWriter->m_oWebSettingsWriter.Write();
				//Document пишем после HeaderFooter, чтобы заполнить sectPr
				m_pCurFileWriter->m_oDocumentWriter.Write();
				//Rels и ContentTypes пишем в конце
				m_pCurFileWriter->m_oDocumentRelsWriter.Write();
				m_pCurFileWriter->m_oContentTypesWriter.Write();

				//CSerializer oSerializer = CSerializer();
				//if(false != oSerializer.Write(oBufferedStream, sDirectoryOut))
				//{
				bResultOk = true;
				//}
			}
		}
		RELEASEARRAYOBJECTS(pBase64Data);
	}
	return bResultOk;
}
bool BinDocxRW::CDocxSerializer::getXmlContent(NSBinPptxRW::CBinaryFileReader& oBufferedStream, long lLength, std::wstring& sOutputXml)
{
	long nLength = oBufferedStream.GetLong();
	Writers::ContentWriter oTempContentWriter;
	BinDocxRW::Binary_DocumentTableReader oBinary_DocumentTableReader(oBufferedStream, *m_pCurFileWriter, oTempContentWriter, m_pCurFileWriter->m_pComments);
	int res = oBinary_DocumentTableReader.Read1(nLength, &BinDocxRW::Binary_DocumentTableReader::ReadDocumentContent, &oBinary_DocumentTableReader, NULL);

    sOutputXml = oTempContentWriter.m_oContent.GetData();
	return true;
}
bool BinDocxRW::CDocxSerializer::getBinaryContent(const std::wstring& bsTxContent, NSBinPptxRW::CBinaryFileWriter& oBufferedStream, long& lDataSize)
{
	if(NULL == m_pParamsWriter)
		return false;
	long nStartPos = oBufferedStream.GetPosition();

	XmlUtils::CXmlLiteReader oReader;
	
//    std::wstring bsTxContentTemp = _T("<root xmlns:w15=\"http://schemas.microsoft.com/office/word/2012/wordml\" xmlns:wpc=\"http://schemas.microsoft.com/office/word/2010/wordprocessingCanvas\" xmlns:mc=\"http://schemas.openxmlformats.org/markup-compatibility/2006\" xmlns:o=\"urn:schemas-microsoft-com:office:office\" xmlns:r=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships\" xmlns:m=\"http://schemas.openxmlformats.org/officeDocument/2006/math\" xmlns:v=\"urn:schemas-microsoft-com:vml\" xmlns:wp14=\"http://schemas.microsoft.com/office/word/2010/wordprocessingDrawing\" xmlns:wp=\"http://schemas.openxmlformats.org/drawingml/2006/wordprocessingDrawing\" xmlns:w10=\"urn:schemas-microsoft-com:office:word\" xmlns:w=\"http://schemas.openxmlformats.org/wordprocessingml/2006/main\" xmlns:w14=\"http://schemas.microsoft.com/office/word/2010/wordml\" xmlns:wpg=\"http://schemas.microsoft.com/office/word/2010/wordprocessingGroup\" xmlns:wpi=\"http://schemas.microsoft.com/office/word/2010/wordprocessingInk\" xmlns:wne=\"http://schemas.microsoft.com/office/word/2006/wordml\" xmlns:wps=\"http://schemas.microsoft.com/office/word/2010/wordprocessingShape\">");
//
//    bsTxContentTemp += bsTxContent;
//    bsTxContentTemp + _T("</root>");
    
    std::wstring sBegin(_T("<root xmlns:w15=\"http://schemas.microsoft.com/office/word/2012/wordml\" xmlns:wpc=\"http://schemas.microsoft.com/office/word/2010/wordprocessingCanvas\" xmlns:mc=\"http://schemas.openxmlformats.org/markup-compatibility/2006\" xmlns:o=\"urn:schemas-microsoft-com:office:office\" xmlns:r=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships\" xmlns:m=\"http://schemas.openxmlformats.org/officeDocument/2006/math\" xmlns:v=\"urn:schemas-microsoft-com:vml\" xmlns:wp14=\"http://schemas.microsoft.com/office/word/2010/wordprocessingDrawing\" xmlns:wp=\"http://schemas.openxmlformats.org/drawingml/2006/wordprocessingDrawing\" xmlns:w10=\"urn:schemas-microsoft-com:office:word\" xmlns:w=\"http://schemas.openxmlformats.org/wordprocessingml/2006/main\" xmlns:w14=\"http://schemas.microsoft.com/office/word/2010/wordml\" xmlns:wpg=\"http://schemas.microsoft.com/office/word/2010/wordprocessingGroup\" xmlns:wpi=\"http://schemas.microsoft.com/office/word/2010/wordprocessingInk\" xmlns:wne=\"http://schemas.microsoft.com/office/word/2006/wordml\" xmlns:wps=\"http://schemas.microsoft.com/office/word/2010/wordprocessingShape\">"));
    
    std::wstring sEnd(_T("</root>"));
    std::wstring bsTxContentTemp = sBegin + bsTxContent + sEnd;
	
	OOX::Logic::CSdtContent oSdtContent;
	if (oReader.FromString(bsTxContentTemp))
	{
        oReader.ReadNextNode();//root
        oReader.ReadNextNode();//v:textbox
		std::wstring sRootName = XmlUtils::GetNameNoNS(oReader.GetName());
		if(_T("textbox") == sRootName)
			oReader.ReadNextNode();//w:txbxContent
		oSdtContent.fromXML(oReader);
	}
	BinDocxRW::ParamsWriter& oParamsWriter = *m_pParamsWriter;
	NSBinPptxRW::CBinaryFileWriter* pBufferedStreamOld = oParamsWriter.m_pCBufferedStream;
	oParamsWriter.m_pCBufferedStream = &oBufferedStream;

	BinDocxRW::BinaryCommonWriter oBinaryCommonWriter(oParamsWriter);
	int nCurPos = oBinaryCommonWriter.WriteItemWithLengthStart();
	BinDocxRW::ParamsDocumentWriter oParams(oParamsWriter.m_pCurRels, oParamsWriter.m_sCurDocumentPath);
	BinDocxRW::BinaryDocumentTableWriter oBinaryDocumentTableWriter(oParamsWriter, oParams, &oParamsWriter.m_mapIgnoreComments, NULL);
	oBinaryDocumentTableWriter.WriteDocumentContent(oSdtContent.m_arrItems);
	oBinaryCommonWriter.WriteItemWithLengthEnd(nCurPos);

	oParamsWriter.m_pCBufferedStream = pBufferedStreamOld;
	long nEndPos = oBufferedStream.GetPosition();
	lDataSize = nEndPos - nStartPos;
	return true;
}
bool BinDocxRW::CDocxSerializer::getBinaryContentElem(OOX::EElementType eElemType, void* pElem, NSBinPptxRW::CBinaryFileWriter& oBufferedStream, long& lDataSize)
{
	if(NULL == m_pParamsWriter)
		return false;
	long nStartPos = oBufferedStream.GetPosition();

	BinDocxRW::ParamsWriter& oParamsWriter = *m_pParamsWriter;
	NSBinPptxRW::CBinaryFileWriter* pBufferedStreamOld = oParamsWriter.m_pCBufferedStream;
	oParamsWriter.m_pCBufferedStream = &oBufferedStream;

	BinDocxRW::BinaryCommonWriter oBinaryCommonWriter(oParamsWriter);
	int nCurPos = oBinaryCommonWriter.WriteItemWithLengthStart();
	BinDocxRW::ParamsDocumentWriter oParams(oParamsWriter.m_pCurRels, oParamsWriter.m_sCurDocumentPath);
	BinDocxRW::BinaryDocumentTableWriter oBinaryDocumentTableWriter(oParamsWriter, oParams, &oParamsWriter.m_mapIgnoreComments, NULL);
	if(OOX::et_m_oMathPara == eElemType)
	{
		OOX::Logic::COMathPara* pMathPara = static_cast<OOX::Logic::COMathPara*>(pElem);
		oBinaryDocumentTableWriter.WriteMathOMathPara(pMathPara->m_arrItems);
	}
	else if(OOX::et_m_oMath == eElemType)
	{
		OOX::Logic::COMath* pMath = static_cast<OOX::Logic::COMath*>(pElem);
		oBinaryDocumentTableWriter.WriteMathArgNodes(pMath->m_arrItems);
	}
	oBinaryCommonWriter.WriteItemWithLengthEnd(nCurPos);

	oParamsWriter.m_pCBufferedStream = pBufferedStreamOld;
	long nEndPos = oBufferedStream.GetPosition();
	lDataSize = nEndPos - nStartPos;
	return true;
}

bool BinDocxRW::CDocxSerializer::getXmlContentElem(OOX::EElementType eType, NSBinPptxRW::CBinaryFileReader& oBufferedStream, std::wstring& sOutputXml)
{
	long nLength = oBufferedStream.GetLong();
	Writers::ContentWriter oTempContentWriter;
	BinDocxRW::Binary_DocumentTableReader oBinary_DocumentTableReader(oBufferedStream, *m_pCurFileWriter, oTempContentWriter, m_pCurFileWriter->m_pComments);

	if(OOX::et_m_oMathPara == eType)
	{
        oTempContentWriter.m_oContent.WriteString(std::wstring(_T("<m:oMathPara>")));
		oBinary_DocumentTableReader.Read1(nLength, &BinDocxRW::Binary_DocumentTableReader::ReadMathOMathPara, &oBinary_DocumentTableReader, NULL);
        oTempContentWriter.m_oContent.WriteString(std::wstring(_T("</m:oMathPara>")));
	}
	else if(OOX::et_m_oMath == eType)
	{
        oTempContentWriter.m_oContent.WriteString(std::wstring(_T("<m:oMath>")));
		oBinary_DocumentTableReader.Read1(nLength, &BinDocxRW::Binary_DocumentTableReader::ReadMathArg, &oBinary_DocumentTableReader, NULL);
        oTempContentWriter.m_oContent.WriteString(std::wstring(_T("</m:oMath>")));
	}


	sOutputXml = oTempContentWriter.m_oContent.GetData();
	return true;
}

void BinDocxRW::CDocxSerializer::setFontDir(const std::wstring& sFontDir)
{
	m_sFontDir = sFontDir;
}
void BinDocxRW::CDocxSerializer::setEmbeddedFontsDir(const std::wstring& sEmbeddedFontsDir)
{
	m_sEmbeddedFontsDir = sEmbeddedFontsDir;
}
void BinDocxRW::CDocxSerializer::setIsNoBase64Save(bool bIsNoBase64Save)
{
	m_bIsNoBase64Save = bIsNoBase64Save;
}
void BinDocxRW::CDocxSerializer::setSaveChartAsImg(bool bSaveChartAsImg)
{
	m_bSaveChartAsImg = bSaveChartAsImg;
}
