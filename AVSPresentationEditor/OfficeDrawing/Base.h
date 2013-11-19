#pragma once
#include "../../Common/DocxFormat/Source/SystemUtility/File.h"

namespace NSPresentationEditor
{
	// ����� xml��� ���������

	class IBase
	{
	public:
		virtual void ReadFromXml(XmlUtils::CXmlNode& oNode)		= 0;
		virtual void WriteToXml(XmlUtils::CXmlWriter& oWriter)	= 0;
	
	public:
		virtual CString SaveToXML()
		{
			XmlUtils::CXmlWriter oWriter;
			WriteToXml(oWriter);
			return oWriter.GetXmlString();
		}
		virtual void LoadFromXML(CString& sXML)
		{
			XmlUtils::CXmlNode oNode;
			if (oNode.FromXmlString(sXML))
			{
				ReadFromXml(oNode);
			}
		}

	public:
		
		virtual void SaveToFile(CString& strFileName)
		{
			CFile oFile;
			if (S_OK == oFile.CreateFile(strFileName))
			{
				CString strXml = SaveToXML();
				oFile.WriteFile(strXml.GetBuffer(), strXml.GetLength() * sizeof(TCHAR));
			}
			oFile.CloseFile();
		}
		virtual void LoadFromFile(CString& strFileName)
		{
			CFile oFile;
			if (S_OK == oFile.OpenFile(strFileName))
			{
				TCHAR* pBuffer = new TCHAR[(size_t)(oFile.GetFileSize() / sizeof(TCHAR))];
				oFile.ReadFile((BYTE*)pBuffer, (DWORD)oFile.GetFileSize());

				CString strXML(pBuffer);
				RELEASEARRAYOBJECTS(pBuffer);
				LoadFromXML(strXML);
			}
		}
	};


	// common function
	template <typename TKey, typename TVal>
	void AppendAtlMap(CAtlMap<TKey, TVal>* pMapDst, const CAtlMap<TKey, TVal>* pMapSrc)
	{
		if ((NULL == pMapDst) || (NULL == pMapSrc))
			return;

		POSITION pos = pMapSrc->GetStartPosition();
		while (NULL != pos)
		{
			TKey key	= pMapSrc->GetKeyAt(pos);
			TVal value	= pMapSrc->GetNextValue(pos);
			pMapDst->SetAt(key, value);
		}
	}
}
