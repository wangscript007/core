#pragma once
#include "LogicItems.h"
#include <float.h>


/*******************************************************/
// TODO: сделать конвертилку глав по bookmark'ам pdf
// определяются заголовки довольно плохо, так как часто 
// это короткая формула, либо просто подпись к рисунку.
// поэтому пока отключено. Чтобы включить - нужно определить
// EBOOK_FROM_PDF_CALCULATE_HEADING_LEVEL (см. ниже)
/*******************************************************/
//#define EBOOK_FROM_PDF_CALCULATE_HEADING_LEVEL

/*******************************************************/
// отключить, если не надо определять колонтитулы
/*******************************************************/
#define EBOOK_FROM_PDF_CALCULATE_HEADER_FOOTER
#include "TableDetector.h"

namespace NSEBook
{
	class CRenderers
	{
	public:
		AVSGraphics::IAVSGraphicsRenderer*		m_pRendererGraphics;
		AVSGraphics::IAVSGraphicsRenderer*		m_pRenderer;

		MediaCore::IAVSUncompressedVideoFrame*	m_pGraphicFrame;
		MediaCore::IAVSUncompressedVideoFrame*	m_pFrame;

		double							m_dWidth;
		double							m_dHeight;

		LONG							m_lWidthPix;
		LONG							m_lHeightPix;

	public:
		CRenderers()
		{
			m_pRenderer			= NULL;
			m_pRendererGraphics	= NULL;

			m_pGraphicFrame		= NULL;
			m_pFrame			= NULL;

			m_dWidth			= -1;
			m_dHeight			= -1;

			m_lWidthPix			= -1;
			m_lHeightPix		= -1;
		}
		~CRenderers()
		{
			RELEASEINTERFACE(m_pRendererGraphics);
			RELEASEINTERFACE(m_pRenderer);
			RELEASEINTERFACE(m_pGraphicFrame);
			RELEASEINTERFACE(m_pFrame);
		}

		void NewPage(double dWidth, double dHeight)
		{
			if (dWidth != m_dWidth || dHeight != m_dHeight)
			{
				RELEASEINTERFACE(m_pGraphicFrame);
				RELEASEINTERFACE(m_pFrame);

				m_dWidth	= dWidth;
				m_dHeight	= dHeight;
			}

			RELEASEINTERFACE(m_pRenderer);
			RELEASEINTERFACE(m_pRendererGraphics);

			m_lWidthPix		= (LONG)(96 * dWidth / 25.4);
			m_lHeightPix	= (LONG)(96 * dHeight / 25.4);

			if (NULL == m_pFrame)
			{
				CoCreateInstance(MediaCore::CLSID_CAVSUncompressedVideoFrame, NULL, CLSCTX_ALL, MediaCore::IID_IAVSUncompressedVideoFrame, (void**)&m_pGraphicFrame);
				CoCreateInstance(MediaCore::CLSID_CAVSUncompressedVideoFrame, NULL, CLSCTX_ALL, MediaCore::IID_IAVSUncompressedVideoFrame, (void**)&m_pFrame);

				m_pGraphicFrame->put_ColorSpace( ( 1 << 6) | ( 1 << 31) ); // CPS_BGRA | CPS_FLIP
				m_pGraphicFrame->put_Width( m_lWidthPix );
				m_pGraphicFrame->put_Height( m_lHeightPix );
				m_pGraphicFrame->put_AspectRatioX( m_lWidthPix );
				m_pGraphicFrame->put_AspectRatioY( m_lHeightPix );
				m_pGraphicFrame->put_Interlaced( VARIANT_FALSE );
				m_pGraphicFrame->put_Stride( 0, 4 * m_lWidthPix );
				m_pGraphicFrame->AllocateBuffer( -1 );

				m_pFrame->put_ColorSpace( ( 1 << 6) | ( 1 << 31) ); // CPS_BGRA | CPS_FLIP
				m_pFrame->put_Width( m_lWidthPix );
				m_pFrame->put_Height( m_lHeightPix );
				m_pFrame->put_AspectRatioX( m_lWidthPix );
				m_pFrame->put_AspectRatioY( m_lHeightPix );
				m_pFrame->put_Interlaced( VARIANT_FALSE );
				m_pFrame->put_Stride( 0, 4 * m_lWidthPix );
				m_pFrame->AllocateBuffer( -1 );

				BYTE* pBufferG = NULL;
				m_pGraphicFrame->get_Buffer(&pBufferG);
				BYTE* pBuffer = NULL;
				m_pFrame->get_Buffer(&pBuffer);

				memset(pBufferG, 0xFF, 4 * m_lWidthPix * m_lHeightPix);
				memset(pBuffer, 0xFF, 4 * m_lWidthPix * m_lHeightPix);
			}

			CoCreateInstance(AVSGraphics::CLSID_CAVSGraphicsRenderer, NULL, CLSCTX_ALL, AVSGraphics::IID_IAVSGraphicsRenderer, (void**)&m_pRendererGraphics);
			CoCreateInstance(AVSGraphics::CLSID_CAVSGraphicsRenderer, NULL, CLSCTX_ALL, AVSGraphics::IID_IAVSGraphicsRenderer, (void**)&m_pRenderer);
			//ставим FontManager
			VARIANT vtVariant;
			vtVariant.vt = VT_UNKNOWN;
			vtVariant.punkVal = NULL;
			m_pRendererGraphics->SetAdditionalParam( L"FontManager", vtVariant );
			m_pRenderer->SetAdditionalParam( L"FontManager", vtVariant );

			m_pRendererGraphics->put_Width(m_dWidth);
			m_pRendererGraphics->put_Height(m_dHeight);
			m_pRendererGraphics->CreateFromMediaData((IUnknown*)m_pGraphicFrame, 0, 0, m_lWidthPix, m_lHeightPix );

			m_pRenderer->put_Width(m_dWidth);
			m_pRenderer->put_Height(m_dHeight);
			m_pRenderer->CreateFromMediaData((IUnknown*)m_pFrame, 0, 0, m_lWidthPix, m_lHeightPix );

			BYTE* pBufferG = NULL;
			m_pGraphicFrame->get_Buffer(&pBufferG);
			BYTE* pBuffer = NULL;
			m_pFrame->get_Buffer(&pBuffer);

			memset(pBufferG, 0xFF, 4 * m_lWidthPix * m_lHeightPix);
			memset(pBuffer, 0xFF, 4 * m_lWidthPix * m_lHeightPix);
		}

	public:
		// pen --------------------------------------------------------------------------------------
		AVSINLINE HRESULT SetPen(BSTR bsXML)
		{
			if (NULL != m_pRenderer)
			{
				m_pRenderer->SetPen(bsXML);
				m_pRendererGraphics->SetPen(bsXML);
			}
			return S_OK;
		}
		AVSINLINE HRESULT put_PenColor(LONG lColor)
		{
			if (NULL != m_pRenderer)
			{
				m_pRenderer->put_PenColor(lColor);
				m_pRendererGraphics->put_PenColor(lColor);
			}
			return S_OK;
		}
		AVSINLINE HRESULT put_PenAlpha(LONG lAlpha)
		{
			if (NULL != m_pRenderer)
			{
				m_pRenderer->put_PenAlpha(lAlpha);
				m_pRendererGraphics->put_PenAlpha(lAlpha);
			}
			return S_OK;
		}
		AVSINLINE HRESULT put_PenSize(double dSize)
		{
			if (NULL != m_pRenderer)
			{
				m_pRenderer->put_PenSize(dSize);
				m_pRendererGraphics->put_PenSize(dSize);
			}
			return S_OK;
		}
		AVSINLINE HRESULT put_PenDashStyle(BYTE val)
		{
			if (NULL != m_pRenderer)
			{
				m_pRenderer->put_PenDashStyle(val);
				m_pRendererGraphics->put_PenDashStyle(val);
			}
			return S_OK;
		}
		AVSINLINE HRESULT put_PenLineStartCap(BYTE val)
		{
			if (NULL != m_pRenderer)
			{
				m_pRenderer->put_PenLineStartCap(val);
				m_pRendererGraphics->put_PenLineStartCap(val);
			}
			return S_OK;
		}
		AVSINLINE HRESULT put_PenLineEndCap(BYTE val)
		{
			if (NULL != m_pRenderer)
			{
				m_pRenderer->put_PenLineEndCap(val);
				m_pRendererGraphics->put_PenLineEndCap(val);
			}
			return S_OK;
		}
		AVSINLINE HRESULT put_PenLineJoin(BYTE val)
		{
			if (NULL != m_pRenderer)
			{
				m_pRenderer->put_PenLineJoin(val);
				m_pRendererGraphics->put_PenLineJoin(val);
			}
			return S_OK;
		}
		AVSINLINE HRESULT put_PenDashOffset(double val)
		{
			if (NULL != m_pRenderer)
			{
				m_pRenderer->put_PenDashOffset(val);
				m_pRendererGraphics->put_PenDashOffset(val);
			}
			return S_OK;
		}
		AVSINLINE HRESULT put_PenAlign(LONG val)
		{
			if (NULL != m_pRenderer)
			{
				m_pRenderer->put_PenAlign(val);
				m_pRendererGraphics->put_PenAlign(val);
			}
			return S_OK;
		}
		AVSINLINE HRESULT put_PenMiterLimit(double val)
		{
			if (NULL != m_pRenderer)
			{
				m_pRenderer->put_PenMiterLimit(val);
				m_pRendererGraphics->put_PenMiterLimit(val);
			}
			return S_OK;
		}
		AVSINLINE HRESULT PenDashPattern(SAFEARRAY* pPattern)
		{
			if (NULL != m_pRenderer)
			{
				m_pRenderer->PenDashPattern(pPattern);
				m_pRendererGraphics->PenDashPattern(pPattern);
			}
			return S_OK;
		}
		// brush ------------------------------------------------------------------------------------
		AVSINLINE HRESULT SetBrush(BSTR bsXML)
		{
			if (NULL != m_pRenderer)
			{
				m_pRenderer->SetBrush(bsXML);
				m_pRendererGraphics->SetBrush(bsXML);
			}
			return S_OK;
		}
		AVSINLINE HRESULT put_BrushType(LONG lType)
		{
			if (NULL != m_pRenderer)
			{
				m_pRenderer->put_BrushType(lType);
				m_pRendererGraphics->put_BrushType(lType);
			}
			return S_OK;
		}
		AVSINLINE HRESULT put_BrushColor1(LONG lColor)
		{
			if (NULL != m_pRenderer)
			{
				m_pRenderer->put_BrushColor1(lColor);
				m_pRendererGraphics->put_BrushColor1(lColor);
			}
			return S_OK;
		}
		AVSINLINE HRESULT put_BrushAlpha1(LONG lAlpha)
		{
			if (NULL != m_pRenderer)
			{
				m_pRenderer->put_BrushAlpha1(lAlpha);
				m_pRendererGraphics->put_BrushAlpha1(lAlpha);
			}
			return S_OK;
		}
		AVSINLINE HRESULT put_BrushColor2(LONG lColor)
		{
			if (NULL != m_pRenderer)
			{
				m_pRenderer->put_BrushColor2(lColor);
				m_pRendererGraphics->put_BrushColor2(lColor);
			}
			return S_OK;
		}
		AVSINLINE HRESULT put_BrushAlpha2(LONG lAlpha)
		{
			if (NULL != m_pRenderer)
			{
				m_pRenderer->put_BrushAlpha2(lAlpha);
				m_pRendererGraphics->put_BrushAlpha2(lAlpha);
			}
			return S_OK;
		}
		AVSINLINE HRESULT put_BrushTexturePath(BSTR bsPath)
		{
			if (NULL != m_pRenderer)
			{
				m_pRenderer->put_BrushTexturePath(bsPath);
				m_pRendererGraphics->put_BrushTexturePath(bsPath);
			}
			return S_OK;
		}
		AVSINLINE HRESULT put_BrushTextureMode(LONG lMode)
		{
			if (NULL != m_pRenderer)
			{
				m_pRenderer->put_BrushTextureMode(lMode);
				m_pRendererGraphics->put_BrushTextureMode(lMode);
			}
			return S_OK;
		}
		AVSINLINE HRESULT put_BrushTextureAlpha(LONG lTxAlpha)
		{
			if (NULL != m_pRenderer)
			{
				m_pRenderer->put_BrushTextureAlpha(lTxAlpha);
				m_pRendererGraphics->put_BrushTextureAlpha(lTxAlpha);
			}
			return S_OK;
		}
		AVSINLINE HRESULT put_BrushLinearAngle(double dAngle)
		{
			if (NULL != m_pRenderer)
			{
				m_pRenderer->put_BrushLinearAngle(dAngle);
				m_pRendererGraphics->put_BrushLinearAngle(dAngle);
			}
			return S_OK;
		}
		AVSINLINE HRESULT BrushRect(BOOL val, double left, double top, double width, double height)
		{
			if (NULL != m_pRenderer)
			{
				m_pRenderer->BrushRect(val, left, top, width, height);
				m_pRendererGraphics->BrushRect(val, left, top, width, height);
			}
			return S_OK;
		}
		// font -------------------------------------------------------------------------------------
		AVSINLINE HRESULT SetFont(BSTR bsXML)
		{
			if (NULL != m_pRenderer)
			{
				m_pRenderer->SetFont(bsXML);
				m_pRendererGraphics->SetFont(bsXML);
			}
			return S_OK;
		}
		AVSINLINE HRESULT put_FontName(BSTR bsName)
		{
			if (NULL != m_pRenderer)
			{
				m_pRenderer->put_FontName(bsName);
				m_pRendererGraphics->put_FontName(bsName);
			}
			return S_OK;
		}
		AVSINLINE HRESULT put_FontPath(BSTR bsName)
		{
			if (NULL != m_pRenderer)
			{
				m_pRenderer->put_FontPath(bsName);
				m_pRendererGraphics->put_FontPath(bsName);
			}
			return S_OK;
		}
		AVSINLINE HRESULT put_FontSize(double dSize)
		{
			if (NULL != m_pRenderer)
			{
				m_pRenderer->put_FontSize(dSize);
				m_pRendererGraphics->put_FontSize(dSize);
			}
			return S_OK;
		}
		AVSINLINE HRESULT put_FontStyle(LONG lStyle)
		{
			if (NULL != m_pRenderer)
			{
				m_pRenderer->put_FontStyle(lStyle);
				m_pRendererGraphics->put_FontStyle(lStyle);
			}
			return S_OK;
		}
		AVSINLINE HRESULT put_FontStringGID(BOOL bGID)
		{
			if (NULL != m_pRenderer)
			{
				m_pRenderer->put_FontStringGID(bGID);
				m_pRendererGraphics->put_FontStringGID(bGID);
			}
			return S_OK;
		}
		AVSINLINE HRESULT put_FontCharSpace(double dSpace)
		{
			if (NULL != m_pRenderer)
			{
				m_pRenderer->put_FontCharSpace(dSpace);
				m_pRendererGraphics->put_FontCharSpace(dSpace);
			}
			return S_OK;
		}
		// shadow -----------------------------------------------------------------------------------
		AVSINLINE HRESULT SetShadow(BSTR bsXML)
		{
			if (NULL != m_pRenderer)
			{
				m_pRenderer->SetShadow(bsXML);
				m_pRendererGraphics->SetShadow(bsXML);
			}
			return S_OK;
		}
		AVSINLINE HRESULT put_ShadowDistanceX(double val)
		{
			if (NULL != m_pRenderer)
			{
				m_pRenderer->put_ShadowDistanceX(val);
				m_pRendererGraphics->put_ShadowDistanceX(val);
			}
			return S_OK;
		}
		AVSINLINE HRESULT put_ShadowDistanceY(double val)
		{
			if (NULL != m_pRenderer)
			{
				m_pRenderer->put_ShadowDistanceY(val);
				m_pRendererGraphics->put_ShadowDistanceY(val);
			}
			return S_OK;
		}
		AVSINLINE HRESULT put_ShadowBlurSize(double val)
		{
			if (NULL != m_pRenderer)
			{
				m_pRenderer->put_ShadowBlurSize(val);
				m_pRendererGraphics->put_ShadowBlurSize(val);
			}
			return S_OK;
		}
		AVSINLINE HRESULT put_ShadowColor(LONG val)
		{
			if (NULL != m_pRenderer)
			{
				m_pRenderer->put_ShadowColor(val);
				m_pRendererGraphics->put_ShadowColor(val);
			}
			return S_OK;
		}
		AVSINLINE HRESULT put_ShadowAlpha(LONG val)
		{
			if (NULL != m_pRenderer)
			{
				m_pRenderer->put_ShadowAlpha(val);
				m_pRendererGraphics->put_ShadowAlpha(val);
			}
			return S_OK;
		}
		AVSINLINE HRESULT put_ShadowVisible(BOOL val)
		{
			if (NULL != m_pRenderer)
			{
				m_pRenderer->put_ShadowVisible(val);
				m_pRendererGraphics->put_ShadowVisible(val);
			}
			return S_OK;
		}
		// edge -------------------------------------------------------------------------------------
		AVSINLINE HRESULT SetEdgeText(BSTR bsXML)
		{
			if (NULL != m_pRenderer)
			{
				m_pRenderer->SetEdgeText(bsXML);
				m_pRendererGraphics->SetEdgeText(bsXML);
			}
			return S_OK;
		}
		AVSINLINE HRESULT put_EdgeVisible(LONG val)
		{
			if (NULL != m_pRenderer)
			{
				m_pRenderer->put_EdgeVisible(val);
				m_pRendererGraphics->put_EdgeVisible(val);
			}
			return S_OK;
		}
		AVSINLINE HRESULT put_EdgeColor(LONG val)
		{
			if (NULL != m_pRenderer)
			{
				m_pRenderer->put_EdgeColor(val);
				m_pRendererGraphics->put_EdgeColor(val);
			}
			return S_OK;
		}
		AVSINLINE HRESULT put_EdgeAlpha(LONG val)
		{
			if (NULL != m_pRenderer)
			{
				m_pRenderer->put_EdgeAlpha(val);
				m_pRendererGraphics->put_EdgeAlpha(val);
			}
			return S_OK;
		}
		AVSINLINE HRESULT put_EdgeDist(double val)
		{
			if (NULL != m_pRenderer)
			{
				m_pRenderer->put_EdgeDist(val);
				m_pRendererGraphics->put_EdgeDist(val);
			}
			return S_OK;
		}

		//-------- Функции для вывода текста --------------------------------------------------------
		AVSINLINE HRESULT CommandDrawText(BSTR bsText, double fX, double fY, double fWidth, double fHeight, double fBaseLineOffset)
		{
			if (NULL != m_pRenderer)
			{
				m_pRenderer->CommandDrawText(bsText, fX, fY, fWidth, fHeight, fBaseLineOffset);
			}
			return S_OK;
		}
		AVSINLINE HRESULT CommandDrawTextEx(BSTR bsUnicodeText, BSTR bsGidText, BSTR bsSourceCodeText, double fX, double fY, double fWidth, double fHeight, double fBaseLineOffset, DWORD lFlags)
		{
			if (NULL != m_pRenderer)
			{
				m_pRenderer->CommandDrawTextEx(bsUnicodeText, bsGidText, bsSourceCodeText, fX, fY, fWidth, fHeight, fBaseLineOffset, lFlags);
			}
			return S_OK;
		}
		//-------- Маркеры для команд ---------------------------------------------------------------
		AVSINLINE HRESULT BeginCommand(DWORD lType)
		{
			if (NULL != m_pRenderer)
			{
				m_pRenderer->BeginCommand(lType);
				m_pRendererGraphics->BeginCommand(lType);
			}
			return S_OK;
		}
		AVSINLINE HRESULT EndCommand(DWORD lType)
		{
			if (NULL != m_pRenderer)
			{
				m_pRenderer->EndCommand(lType);
				m_pRendererGraphics->EndCommand(lType);
			}
			return S_OK;
		}
		//-------- Функции для работы с Graphics Path -----------------------------------------------
		AVSINLINE HRESULT PathCommandMoveTo(double fX, double fY)
		{
			if (NULL != m_pRenderer)
			{
				m_pRenderer->PathCommandMoveTo(fX, fY);
				m_pRendererGraphics->PathCommandMoveTo(fX, fY);
			}
			return S_OK;
		}
		AVSINLINE HRESULT PathCommandLineTo(double fX, double fY)
		{
			if (NULL != m_pRenderer)
			{
				m_pRenderer->PathCommandLineTo(fX, fY);
				m_pRendererGraphics->PathCommandLineTo(fX, fY);
			}
			return S_OK;
		}
		AVSINLINE HRESULT PathCommandLinesTo(SAFEARRAY* pPoints)
		{
			if (NULL != m_pRenderer)
			{
				m_pRenderer->PathCommandLinesTo(pPoints);
				m_pRendererGraphics->PathCommandLinesTo(pPoints);
			}
			return S_OK;
		}
		AVSINLINE HRESULT PathCommandCurveTo(double fX1, double fY1, double fX2, double fY2, double fX3, double fY3)
		{
			if (NULL != m_pRenderer)
			{
				m_pRenderer->PathCommandCurveTo(fX1, fY1, fX2, fY2, fX3, fY3);
				m_pRendererGraphics->PathCommandCurveTo(fX1, fY1, fX2, fY2, fX3, fY3);
			}
			return S_OK;
		}
		AVSINLINE HRESULT PathCommandCurvesTo(SAFEARRAY* pPoints)
		{
			if (NULL != m_pRenderer)
			{
				m_pRenderer->PathCommandCurvesTo(pPoints);
				m_pRendererGraphics->PathCommandCurvesTo(pPoints);
			}
			return S_OK;
		}
		AVSINLINE HRESULT PathCommandArcTo(double fX, double fY, double fWidth, double fHeight, double fStartAngle, double fSweepAngle)
		{
			if (NULL != m_pRenderer)
			{
				m_pRenderer->PathCommandArcTo(fX, fY, fWidth, fHeight, fStartAngle, fSweepAngle);
				m_pRendererGraphics->PathCommandArcTo(fX, fY, fWidth, fHeight, fStartAngle, fSweepAngle);
			}
			return S_OK;
		}
		AVSINLINE HRESULT PathCommandClose()
		{
			if (NULL != m_pRenderer)
			{
				m_pRenderer->PathCommandClose();
				m_pRendererGraphics->PathCommandClose();
			}
			return S_OK;
		}
		AVSINLINE HRESULT PathCommandEnd()
		{
			if (NULL != m_pRenderer)
			{
				m_pRenderer->PathCommandEnd();
				m_pRendererGraphics->PathCommandEnd();
			}
			return S_OK;
		}
		AVSINLINE HRESULT DrawPath(long nType)
		{
			if (NULL != m_pRenderer)
			{
				m_pRenderer->DrawPath(nType);
				m_pRendererGraphics->DrawPath(nType);
			}
			return S_OK;
		}
		AVSINLINE HRESULT PathCommandStart()
		{
			if (NULL != m_pRenderer)
			{
				m_pRenderer->PathCommandStart();
				m_pRendererGraphics->PathCommandStart();
			}
			return S_OK;
		}
		AVSINLINE HRESULT PathCommandText(BSTR bsText, double fX, double fY, double fWidth, double fHeight, double fBaseLineOffset)
		{
			if (NULL != m_pRenderer)
			{
				m_pRenderer->PathCommandText(bsText, fX, fY, fWidth, fHeight, fBaseLineOffset);
				m_pRendererGraphics->PathCommandText(bsText, fX, fY, fWidth, fHeight, fBaseLineOffset);
			}
			return S_OK;
		}
		AVSINLINE HRESULT PathCommandTextEx(BSTR bsUnicodeText, BSTR bsGidText, BSTR bsSourceCodeText, double fX, double fY, double fWidth, double fHeight, double fBaseLineOffset, DWORD lFlags)
		{
			if (NULL != m_pRenderer)
			{
				m_pRenderer->PathCommandTextEx(bsUnicodeText, bsGidText, bsSourceCodeText, fX, fY, fWidth, fHeight, fBaseLineOffset, lFlags);
				m_pRendererGraphics->PathCommandTextEx(bsUnicodeText, bsGidText, bsSourceCodeText, fX, fY, fWidth, fHeight, fBaseLineOffset, lFlags);
			}
			return S_OK;
		}
		AVSINLINE HRESULT SetCommandParams(double dAngle, double dLeft, double dTop, double dWidth, double dHeight, DWORD lFlags)
		{
			if (NULL != m_pRenderer)
			{
				m_pRenderer->SetCommandParams(dAngle, dLeft, dTop, dWidth, dHeight, lFlags);
				m_pRendererGraphics->SetCommandParams(dAngle, dLeft, dTop, dWidth, dHeight, lFlags);
			}
			return S_OK;
		}
		//-------- Функции для вывода изображений --------------------------------------------------
		AVSINLINE HRESULT DrawImage(IUnknown* pInterface, double fX, double fY, double fWidth, double fHeight)
		{
			if (NULL != m_pRenderer)
			{
				m_pRenderer->DrawImage(pInterface, fX, fY, fWidth, fHeight);
				m_pRendererGraphics->DrawImage(pInterface, fX, fY, fWidth, fHeight);
			}
			return S_OK;
		}
		AVSINLINE HRESULT DrawImageFromFile(BSTR bstrVal, double fX, double fY, double fWidth, double fHeight)
		{
			if (NULL != m_pRenderer)
			{
				m_pRenderer->DrawImageFromFile(bstrVal, fX, fY, fWidth, fHeight);
				m_pRendererGraphics->DrawImageFromFile(bstrVal, fX, fY, fWidth, fHeight);
			}
			return S_OK;
		}
		//------------------------------------------------------------------------------------------
		AVSINLINE HRESULT SetAdditionalParam(BSTR ParamName, VARIANT ParamValue)
		{
			if (NULL != m_pRenderer)
			{
				m_pRenderer->SetAdditionalParam(ParamName, ParamValue);
				m_pRendererGraphics->SetAdditionalParam(ParamName, ParamValue);
			}
			return S_OK;
		}
		AVSINLINE HRESULT SetTransform(double dA, double dB, double dC, double dD, double dE, double dF)
		{
			if (NULL != m_pRenderer)
			{
				m_pRenderer->SetTransform(dA, dB, dC, dD, dE, dF);
				m_pRendererGraphics->SetTransform(dA, dB, dC, dD, dE, dF);
			}
			return S_OK;
		}
		AVSINLINE HRESULT ResetTransform(void)
		{
			if (NULL != m_pRenderer)
			{
				m_pRenderer->ResetTransform();
				m_pRendererGraphics->ResetTransform();
			}
			return S_OK;
		}
		AVSINLINE HRESULT put_ClipMode(LONG lMode)
		{
			if (NULL != m_pRenderer)
			{
				m_pRenderer->put_ClipMode(lMode);
				m_pRendererGraphics->put_ClipMode(lMode);
			}
			return S_OK;
		}
	};

	class CLogicTextLine : public ILogicItem
	{
	public:
		CAtlList<NSEBook::CEbookParagraph::CSpan*> m_arSpans;
		CLogicImage* m_pImage;
		
		double m_dBaseLine;
		double m_dLineHeight;

	public:
		CLogicTextLine() : m_arSpans()
		{
			m_eType		= ILogicItem::elitTextLine;
			m_pImage	= NULL;

			m_rcBounds.left		= 100000;
			m_rcBounds.top		= 100000;
			m_rcBounds.right	= -100000;
			m_rcBounds.bottom	= -100000;

			m_dBaseLine = 0.0;
			m_dLineHeight = 0.0;
		}
		~CLogicTextLine()
		{
			m_arSpans.RemoveAll();
		}

		CLogicTextLine(const CLogicTextLine& oSrc)
		{
			*this = oSrc;
		}
		CLogicTextLine& operator=(const CLogicTextLine& oSrc)
		{
			Clear();
			
			m_dBaseLine		= oSrc.m_dBaseLine;
			m_dLineHeight	= oSrc.m_dLineHeight;
			m_pImage		= oSrc.m_pImage;

			m_arSpans.AddTailList(&oSrc.m_arSpans);

			return *this;
		}

	public:
		void Clear()
		{
			m_rcBounds.left		= 100000;
			m_rcBounds.top		= 100000;
			m_rcBounds.right	= -100000;
			m_rcBounds.bottom	= -100000;

			m_arSpans.RemoveAll();

			RELEASEOBJECT(m_pImage);
		}
		
		virtual double GetPositionY()
		{
			return m_dBaseLine;
		}
		virtual double GetPositionX()
		{
			return m_rcBounds.left;
		}
	};

	class CLinePDF
	{
	public:
		CImageInfo	m_oImageInfo;
		CAtlList<CEbookParagraph::CSpan*>	m_listSpans;

		double m_dBaseline;
		double m_dLeft;
		double m_dRight;

	public:
		CLinePDF() : m_listSpans(), m_oImageInfo()
		{
			m_dBaseline		= 0.0;
			m_dLeft			= 0.0;
			m_dRight		= 0.0;
		}
		~CLinePDF()
		{
			POSITION pos = m_listSpans.GetHeadPosition();
			while (NULL != pos)
			{
				CEbookParagraph::CSpan* pSpan = m_listSpans.GetNext(pos);
				RELEASEOBJECT(pSpan);
			}
			m_listSpans.RemoveAll();
		}

	public:
		AVSINLINE void ToLine(NSEBook::CEbookParagraph::CLine& oLine)
		{
			oLine.m_oImageInfo		= m_oImageInfo;
			oLine.m_dBaselinePos	= m_dBaseline;

			POSITION pos = m_listSpans.GetHeadPosition();
			while (NULL != pos)
			{
				oLine.m_listSpans.AddTail(*m_listSpans.GetNext(pos));
			}
		}
		AVSINLINE double GetNaturalWidth()
		{
			if (-1 == m_oImageInfo.m_lID)
			{
				double dWidth = 0;
				POSITION pos = m_listSpans.GetHeadPosition();
				while (NULL != pos)
				{
					CEbookParagraph::CSpan* pSpan = m_listSpans.GetNext(pos);
					dWidth += pSpan->m_dWidthSpan;
				}
				return dWidth;
			}
			return (m_dRight - m_dLeft);
		}
		AVSINLINE bool CheckTitle()
		{
			if (-1 != m_oImageInfo.m_lID)
				return false;

			POSITION pos = m_listSpans.GetHeadPosition();
			while (NULL != pos)
			{
				CEbookParagraph::CSpan* pSpan = m_listSpans.GetNext(pos);
				if (pSpan->m_dBaselineOffset != 0)
					return false;
			}
			return true;
		}
		AVSINLINE bool IsNewPar()
		{
			if ((-1 != m_oImageInfo.m_lID) || (0 == m_listSpans.GetCount()))
				return false;

			CEbookParagraph::CSpan* pSpan = m_listSpans.GetHead();

			size_t nCurSize = pSpan->m_oText.GetCurSize();
			if (0 == nCurSize)
				return false;

			if (WCHAR(61607) == pSpan->m_oText[0])
				return true;
			if (WCHAR(61623) == pSpan->m_oText[0])
				return true;

			size_t nCurrentIndex = 0;
			for (; nCurrentIndex < nCurSize; ++nCurrentIndex)
			{
				if (!XmlUtils::IsDigit(pSpan->m_oText[nCurrentIndex]))
					break;
			}

			if (0 == nCurrentIndex)
				return false;
			if (nCurSize == nCurrentIndex)
				return false;

			if (WCHAR('.') == pSpan->m_oText[nCurrentIndex])
				return true;
			if (WCHAR(')') == pSpan->m_oText[nCurrentIndex])
				return true;
			
			return false;
		}

		AVSINLINE bool IsLineIndividualPar()
		{
			if (GetNaturalWidth() < (2 * (m_dRight - m_dLeft) / 3))
			{
				return true;
			}
			if (-1 != m_oImageInfo.m_lID)
				return false;

			LONG lCountSpaces = 0;	// пробелы/табы (заглушка под содержание, которое обычно сделано пробелами или табами)
			LONG lCountPoints = 0;	// точки (заглушка под содержание, которое обычно сделано точками)

			POSITION pos = m_listSpans.GetHeadPosition();
			while (NULL != pos)
			{
				CEbookParagraph::CSpan* pSpan = m_listSpans.GetNext(pos);

				size_t nCountSymbols = pSpan->m_oText.GetCurSize();
				for (size_t i = 0; i < nCountSymbols; ++i)
				{
					if ((WCHAR(' ') == pSpan->m_oText[i]) || (WCHAR('\t') == pSpan->m_oText[i]))
					{
						++lCountSpaces;
						if ((lCountSpaces + lCountPoints) >= 7)
							return true;

						if (1 < lCountSpaces)
							lCountPoints = 0;
					}
					else if (WCHAR('.') == pSpan->m_oText[i])
					{
						++lCountPoints;
						if ((lCountSpaces + lCountPoints) >= 7)
							return true;

						if (1 < lCountPoints)
							lCountSpaces = 0;
					}
					else
					{
						lCountSpaces = 0;
						lCountPoints = 0;
					}
				}
			}
			return false;
		}
	};

	class CLineContainer
	{
	public:
		// CLogicTextLine or CLogicImage
		CAtlList<ILogicItem*>	m_arItems;
				
		double m_dBaseLinePosition;
		double m_dLeft;
		double m_dRight;
		
	public:
		CLineContainer() : m_arItems()
		{
			m_dBaseLinePosition		= 0.0;
			m_dLeft					= 0.0;
			m_dRight				= 0.0;
		}
		~CLineContainer()
		{
		}

		CLinePDF* ToLinePDF()
		{
			CLinePDF* pLinePDF = new CLinePDF();
			// для начала отсортируем все по x
			CAtlList<ILogicItem*> arraySort;
		
			while (0 != m_arItems.GetCount())
			{
				POSITION pos = m_arItems.GetHeadPosition();
				POSITION posNeed = pos;

				ILogicItem* pItem = m_arItems.GetNext(pos);
				double dMin = pItem->GetPositionX();
				while (NULL != pos)
				{
					POSITION posOld = pos;
					pItem = m_arItems.GetNext(pos);

					double dMem = pItem->GetPositionX();
					if (dMem < dMin)
					{
						dMin = dMem;
						posNeed = posOld;
					}
				}

				ILogicItem* pNeed = m_arItems.GetAt(posNeed);
				m_arItems.RemoveAt(posNeed);
				arraySort.AddTail(pNeed);
			}

			// теперь определяем right и baselinePos
			pLinePDF->m_dRight = -10000;
			bool bIsFoundBaseLine = false;
			bool bIsFirst = true;
			double dFirstBaseline = 0;

			POSITION pos = arraySort.GetHeadPosition();
			while (NULL != pos)
			{
				ILogicItem* pItem = arraySort.GetNext(pos);

				if (bIsFirst)
				{
					bIsFirst = false;
					pLinePDF->m_dLeft = pItem->GetPositionX();
					dFirstBaseline = pItem->GetPositionY();
				}

				if (pLinePDF->m_dRight < pItem->m_rcBounds.right)
					pLinePDF->m_dRight = pItem->m_rcBounds.right;

				if (pItem->m_eType == ILogicItem::elitTextLine && !bIsFoundBaseLine)
				{
					bIsFoundBaseLine = true;
					pLinePDF->m_dBaseline = pItem->GetPositionY();
				}				
			}

			if (!bIsFoundBaseLine)
				pLinePDF->m_dBaseline = dFirstBaseline;

			double dRight = 0;
			bIsFirst = true;

			// все, все параметры выставлены, все позиции определены.
			// теперь просто пробегаемся по всем элементам и заменяем их спанами
			while (0 != arraySort.GetCount())
			{
				ILogicItem* pItem = arraySort.RemoveHead();

				if (pItem->m_eType == ILogicItem::elitTextLine)
				{
					CLogicTextLine* pLogicLine = (CLogicTextLine*)pItem;
					double dSpanBaseLine = pLogicLine->m_dBaseLine;

					POSITION posSpan = pLogicLine->m_arSpans.GetHeadPosition();
					while (NULL != posSpan)
					{
						CEbookParagraph::CSpan* pSpan = pLogicLine->m_arSpans.GetNext(posSpan);

						if (!bIsFirst)
						{
							if ((pSpan->m_dX - dRight) > 1)
							{
								pSpan->m_oText.AddSpaceFirst();
							}
						}
						else
						{
							bIsFirst = false;
						}
						dRight = pSpan->m_dX + pSpan->m_dImageWidth;
						
						pSpan->m_dBaselineOffset = pLinePDF->m_dBaseline - dSpanBaseLine;

						if (fabs(pSpan->m_dBaselineOffset) < 0.2)
							pSpan->m_dBaselineOffset = 0;

						pLinePDF->m_listSpans.AddTail(pSpan);
					}

					// надо удалить все, а то вызовутся деструкторы
					pLogicLine->m_arSpans.RemoveAll();
				}
				else if (pItem->m_eType == ILogicItem::elitGraphic)
				{
					CLogicImage* pLogicImage = (CLogicImage*)pItem;
					dRight = pItem->m_rcBounds.right;

					NSEBook::CEbookParagraph::CSpan* pSpan = new NSEBook::CEbookParagraph::CSpan();
					pSpan->m_oImageInfo = pLogicImage->m_oImageInfo;
					pSpan->m_dX = pLogicImage->m_rcBounds.left;
					pSpan->m_dImageWidth = pLogicImage->m_rcBounds.GetWidth();
					pSpan->m_dImageHeight = pLogicImage->m_rcBounds.GetHeight();

					pLinePDF->m_listSpans.AddTail(pSpan);
				}

				RELEASEOBJECT(pItem);
			}

			// теперь смотрим. Если в линии всего одна картинка - то удаляем спан и 
			// заполняем pLinePDF->m_oImageData
			if (1 == pLinePDF->m_listSpans.GetCount())
			{
				CEbookParagraph::CSpan* pSpan = pLinePDF->m_listSpans.GetHead();
				if (-1 != pSpan->m_oImageInfo.m_lID)
				{
					pLinePDF->m_oImageInfo = pSpan->m_oImageInfo;
					pLinePDF->m_dLeft	= pSpan->m_dX;
					pLinePDF->m_dRight	= pSpan->m_dX + pSpan->m_dImageWidth;
					pLinePDF->m_listSpans.RemoveAll();
					RELEASEOBJECT(pSpan);
				}
			}

			return pLinePDF;
		}
	};

	class CTextPDF
	{
	public:
		CFontManager*		m_pFontManager;
		CFontDstGenerator*	m_pFontsDst;
		
		CLogicTextLine*		m_pCurrentLine;

		double			m_dDpiX;
		double			m_dDpiY;

		NSStructures::CPen*		m_pPen;
		NSStructures::CBrush*	m_pBrush;
		NSStructures::CFont*	m_pFont;

		NSCommon::CMatrix*		m_pTransform;

		bool			m_bIsNewLine;

		CTextItem		m_oTextItem;

	private:
		double m_dEpsX;
		double m_dEpsY;
		
	public:
		CTextPDF() : m_pFontManager(NULL), m_pFontsDst(NULL), m_oTextItem(10)
 		{
			m_pPen		= NULL;
			m_pBrush	= NULL;
			m_pFont		= NULL;

			m_pTransform	= NULL;

			m_bIsNewLine = false;

			m_pCurrentLine = new CLogicTextLine();

			m_dEpsX		= 0.5;
			m_dEpsY		= 0.5;
		}

		void SetParams(NSStructures::CPen* pPen, NSStructures::CBrush* pBrush, NSStructures::CFont* pFont, NSCommon::CMatrix* pTransform)
		{
			m_pPen					= pPen;
			m_pBrush				= pBrush;
			m_pFont					= pFont;

			m_pFontManager->m_pFont	= pFont;
			m_pFontManager->m_pTransform = pTransform;
			m_pTransform			= pTransform;
		}

		~CTextPDF()
		{
		}

		void NewPage(double& dDpiX, double& dDpiY)
		{
			m_dDpiX	= dDpiX;
			m_dDpiY = dDpiY;

			m_pPen->SetDefaultParams();
			m_pBrush->SetDefaultParams();
			m_pFont->SetDefaultParams();

			RELEASEOBJECT(m_pCurrentLine);
			m_pCurrentLine = new CLogicTextLine();
		}
		CLogicTextLine* ClosePage()
		{
			if (m_pCurrentLine->m_arSpans.GetCount() != 0)
			{
				CLogicTextLine* pReturn = m_pCurrentLine;
				m_pCurrentLine = new CLogicTextLine();
				return pReturn;
			}
			return NULL;
		}

		void NewDocument()
		{
			m_pPen->SetDefaultParams();
			m_pBrush->SetDefaultParams();
			m_pFont->SetDefaultParams();
		}

	
	protected:

		AVSINLINE bool GetIsNewLine()
		{
			if (m_bIsNewLine)
			{
				m_bIsNewLine = false;
				return true;
			}
			return false;
		}
		
		CLogicTextLine* CommandText(BSTR bsGid, double& x, double& y, double& width, double& height, double& baselineoffset)
		{
			bool bIsNewLine = GetIsNewLine();

			double dScaleFont = ((m_pTransform->m_agg_mtx.sx + m_pTransform->m_agg_mtx.sy) / 2);
			BOOL bIsFontChanged = !m_pFontManager->m_oFont.m_oFont.IsEqual2(m_pFont, dScaleFont);

			if (bIsFontChanged)
			{
				m_pFontManager->m_oFont.m_oFont			= *m_pFont;
				m_pFontManager->m_oFont.m_oFont.Size	*= dScaleFont;

				m_pFontManager->LoadCurrentFont();
				m_pFontManager->CalculateSpace();

				m_pFont->SetStyle(m_pFontManager->m_oFont.m_oFont.GetStyle());

				m_pFontManager->GenerateFontName(m_oTextItem, true);
			}
			else
			{
				m_pFontManager->GenerateFontName(m_oTextItem, false);
			}

			double dOffset = m_pFontManager->m_oFont.m_dBaselineOffsetHTML;
			
			dOffset *= c_dPtToMM;

			y -= dOffset;
			baselineoffset = dOffset;

			// текст не измерен. для начала нужно его измерить
			double _x = 0;
			double _y = 0;
			double _w = 0;
			double _h = 0;

			bool bIsMeasuring = false;

			if (0 >= width)
			{	
				if (NULL == bsGid)
				{
					m_pFontManager->SetStringGid(0);
					BSTR bsText = m_oTextItem.GetCString().AllocSysString();
					m_pFontManager->MeasureString(bsText, x, y, _x, _y, _w, _h, CFontManager::MeasureTypePosition);
					SysFreeString(bsText);
				}
				else
				{
					m_pFontManager->SetStringGid(1);
					m_pFontManager->MeasureString(bsGid, x, y, _x, _y, _w, _h, CFontManager::MeasureTypePosition);
				}

				width  = _w;
				height = _h;

				bIsMeasuring = true;
			}

			if (_T("") != m_pFontManager->m_strCurrentPickFont)
			{
				m_pFontsDst->AddFont2(m_pFont, bIsFontChanged, m_pFontManager);
			}
			else
			{
				// по идее такого быть не должно
				m_pFontsDst->AddFont(m_pFont, bIsFontChanged, m_pFontManager);
			}

			double dYPos		= y + baselineoffset;
			LONG lCountWords	= (LONG)m_pCurrentLine->m_arSpans.GetCount();

			if (0 == lCountWords)
			{
				CreateNewLineBySpan(x, y, width, height, dYPos);
				return NULL;
			}

			double dYPosLine = m_pCurrentLine->m_dBaseLine;

			if (abs(dYPos - dYPosLine) > 0.5)
			{
				// новая линия
				// сначала запишем предыдущую линию
				CLogicTextLine* pReturn = m_pCurrentLine;
				m_pCurrentLine = new CLogicTextLine();

				CreateNewLineBySpan(x, y, width, height, dYPos);
				return pReturn;
			}

			// теперь проверяем, продолжать ли слово
			//double dSpace = MMToPX(m_oFontManager.m_dSpaceWidthMM, m_dDpiX) / 2.0;
			double dSpace = 0.2;
			if (bIsMeasuring)
			{
				dSpace = m_pFontManager->m_dSpaceWidthMM;
			}

			NSEBook::CEbookParagraph::CSpan* pSpanLast = m_pCurrentLine->m_arSpans.GetTail();

			double dRight = pSpanLast->m_dX + pSpanLast->m_dWidthSpan;
			double dDelta = (x - (pSpanLast->m_dX + pSpanLast->m_dWidthSpan));
			// с пробелом - тоже самое что и с линией. Чтобы было участие в сортировке по x
			if (bIsNewLine || (dDelta > dSpace) || (dDelta < -2))
			{
				// создадим новую линию
				CLogicTextLine* pReturn = m_pCurrentLine;
				m_pCurrentLine = new CLogicTextLine();

				CreateNewLineBySpan(x, y, width, height, dYPos);
				return pReturn;
			}

			bool bIsBrushChanged = !pSpanLast->m_oBrush.IsEqual(m_pBrush);
			if (bIsFontChanged || bIsBrushChanged)
			{
				if (m_oTextItem.IsSpace())
				{
					pSpanLast->m_oText += m_oTextItem;
					pSpanLast->m_dWidthSpan = x + width - pSpanLast->m_dX;
					return NULL;
				}
				
				if (dDelta > dSpace)
					pSpanLast->m_oText.AddSpace();

				if (bIsFontChanged || bIsBrushChanged)
				{
					AddSpan(x, y, width, height, dYPos);
				}
				else
				{
					pSpanLast->m_oText		+= m_oTextItem;
					pSpanLast->m_dWidthSpan	= x + width - pSpanLast->m_dX;

					m_pCurrentLine->m_rcBounds.right = x + width;
				}
			}
			else
			{
				// продолжаем слово
				pSpanLast->m_oText		+= m_oTextItem;
				pSpanLast->m_dWidthSpan	= x + width - pSpanLast->m_dX; // чтобы не накапливалась ошибка, не используем сумму длин

				m_pCurrentLine->m_rcBounds.right = x + width;
			}

			return NULL;
		}
		
	public:
		AVSINLINE CLogicTextLine* CommandText(BSTR& bsText, BSTR& bsGid, double& x, double& y, double& width, double& height, double& baselineoffset)
		{
			if (NULL == bsText)
				return NULL;

			m_oTextItem.SetText(bsText);

			if (0 == m_oTextItem.GetCurSize())
				return NULL;

			if (bsGid != NULL)
				m_oTextItem.CorrectUnicode(m_pFontManager->m_mapUnicode);

			return CommandText(bsGid, x, y, width, height, baselineoffset); 
		}

	protected:
		AVSINLINE void CreateSpan(NSEBook::CEbookParagraph::CSpan* pSpan, 
			const double& x, const double& y, const double& width, const double& height, const double& dYPos)
		{
			pSpan->m_oFont			= m_pFontManager->m_oFont.m_oFont;
			pSpan->m_oBrush			= *m_pBrush;
			pSpan->m_oText			= m_oTextItem;
			pSpan->m_dX				= x;
			pSpan->m_dWidthSpan		= width;

			if (_T("") != m_pFontManager->m_strCurrentPickFont)
			{
				pSpan->m_oFont.Name	= m_pFontManager->m_strCurrentPickFont;
				pSpan->m_oFont.SetStyle(m_pFontManager->m_lCurrentPictFontStyle);
			}
		}
		AVSINLINE void CreateNewLineBySpan(const double& x, const double& y, const double& width, const double& height, const double& dYPos)
		{
			NSEBook::CEbookParagraph::CSpan* pSpan = new NSEBook::CEbookParagraph::CSpan();
			CreateSpan(pSpan, x, y, width, height, dYPos);

			m_pCurrentLine->m_rcBounds.left			= x;
			m_pCurrentLine->m_rcBounds.top			= y;
			m_pCurrentLine->m_rcBounds.right		= x + width;
			m_pCurrentLine->m_rcBounds.bottom		= y + height;

			m_pCurrentLine->m_dLineHeight			= 0.8 * m_pFontManager->m_oFont.m_dLineSpacingMM;
			m_pCurrentLine->m_dBaseLine				= dYPos;

			m_pCurrentLine->m_arSpans.AddTail(pSpan);
		}
		AVSINLINE void AddSpan(const double& x, const double& y, const double& width, const double& height, const double& dYPos)
		{
			NSEBook::CEbookParagraph::CSpan* pSpan = new NSEBook::CEbookParagraph::CSpan();
			CreateSpan(pSpan, x, y, width, height, dYPos);

			m_pCurrentLine->m_dLineHeight			= max(0.8 * m_pFontManager->m_oFont.m_dLineSpacingMM, m_pCurrentLine->m_dLineHeight);
			m_pCurrentLine->m_rcBounds.right		= x + width;
			m_pCurrentLine->m_rcBounds.left			= min(m_pCurrentLine->m_rcBounds.left, x);

			m_pCurrentLine->m_arSpans.AddTail(pSpan);
		}
	};

	class CFigure
	{
	public:
		CAtlArray<RECT>	m_arComplete;
		CAtlArray<RECT>	m_arNeedCalcs;

	public:
		BYTE*					m_pBuffer;
		LONG					m_lWidth;
		LONG					m_lHeight;

		LONG					m_lEpsilon;

	public:

		void Parce()
		{
			m_arComplete.RemoveAll();
			m_arNeedCalcs.RemoveAll();

			RECT oRect;
			oRect.left		= 0;
			oRect.top		= 0;
			oRect.right		= m_lWidth - 1;
			oRect.bottom	= m_lHeight - 1;

			m_lEpsilon		= 10;

			m_arNeedCalcs.Add(oRect);

			while (true)
			{
				if (0 == m_arNeedCalcs.GetCount())
					break;

				Graduation();
			}
		}
		void Graduation()
		{
			size_t nCountOld = m_arNeedCalcs.GetCount();
			for (size_t i = 0; i < nCountOld; ++i)
				Graduation(i);
			m_arNeedCalcs.RemoveAt(0, nCountOld);
		}
		void Graduation(size_t nIndex)
		{
			RECT oRect = m_arNeedCalcs[nIndex];

			CAtlArray<LONG> arrHors;
			CAtlArray<LONG> arrVers;

			// hors -----------------------------------------------------------------------------------
			LONG lCurrent = oRect.left;
			bool bIsEmptyOld = true;
			while (lCurrent <= oRect.right)
			{
				DWORD* pBufferMem = (DWORD*)(m_pBuffer + 4 * oRect.top * m_lWidth + 4 * lCurrent);

				bool bIsEmptyCur = true;

				long lCritical = oRect.bottom + 1;
				for (long i = oRect.top; i != lCritical; ++i, pBufferMem += m_lWidth)
				{
					if (*pBufferMem != 0xFFFFFFFF)
					{
						bIsEmptyCur = false;
						break;
					}
				}

				if (bIsEmptyCur != bIsEmptyOld)
				{
					bIsEmptyOld = bIsEmptyCur;
					arrHors.Add(lCurrent);
				}

				++lCurrent;
			}
			arrHors.Add(oRect.right);

			if (0 != m_lEpsilon)
			{
				int nNecet = 1;
				int nCount = (int)arrHors.GetCount();
				for (; nNecet < (nCount - 1);)
				{
					if (((arrHors[nNecet] + m_lEpsilon) > arrHors[nNecet + 1]) && (nNecet + 1 != (nCount - 1)))
					{
						arrHors.RemoveAt(nNecet, 2);
						nCount -= 2;
					}
					else
					{
						nNecet += 2;
					}
				}
			}
			// ---------------------------------------------------------------------------------------
			// vers -----------------------------------------------------------------------------------
			lCurrent = oRect.top;
			bIsEmptyOld = true;
			while (lCurrent <= oRect.bottom)
			{
				DWORD* pBufferMem = (DWORD*)(m_pBuffer + 4 * lCurrent * m_lWidth + 4 * oRect.left);

				bool bIsEmptyCur = true;

				long lCritical = oRect.right + 1;
				for (long i = oRect.left; i != lCritical; ++i, ++pBufferMem)
				{
					if (*pBufferMem != 0xFFFFFFFF)
					{
						bIsEmptyCur = false;
						break;
					}
				}

				if (bIsEmptyCur != bIsEmptyOld)
				{
					bIsEmptyOld = bIsEmptyCur;
					arrVers.Add(lCurrent);
				}

				++lCurrent;
			}
			arrVers.Add(oRect.bottom);

			if (0 != m_lEpsilon)
			{
				int nNecet = 1;
				int nCount = (int)arrVers.GetCount();
				for (; nNecet < (nCount - 1);)
				{
					if (((arrVers[nNecet] + m_lEpsilon) > arrVers[nNecet + 1]) && (nNecet + 1 != (nCount - 1)))
					{
						arrVers.RemoveAt(nNecet, 2);
						nCount -= 2;
					}
					else
					{
						nNecet += 2;
					}
				}
			}
			// ---------------------------------------------------------------------------------------

			LONG lCountH = (LONG)arrHors.GetCount() / 2;
			LONG lCountV = (LONG)arrVers.GetCount() / 2;

			if ((1 == lCountH) && (1 == lCountV))
			{
				RECT oCompleteRect;
				oCompleteRect.left		= arrHors[0];
				oCompleteRect.right		= arrHors[1];
				oCompleteRect.top		= arrVers[0];
				oCompleteRect.bottom	= arrVers[1];

				m_arComplete.Add(oCompleteRect);
				return;
			}

			for (LONG lJ = 0; lJ < lCountV; ++lJ)
			{
				for (LONG lI = 0; lI < lCountH; ++lI)
				{
					RECT oNeedRect;
					oNeedRect.left		= arrHors[2 * lI];
					oNeedRect.right		= arrHors[2 * lI + 1];
					oNeedRect.top		= arrVers[2 * lJ];
					oNeedRect.bottom	= arrVers[2 * lJ + 1];

					m_arNeedCalcs.Add(oNeedRect);
				}
			}

			return;
		}

		void Convert(ILogicPage* pPage, CRenderers* pRenderers, bool bIsScan)
		{
			if ((NULL == pRenderers->m_pGraphicFrame) || (NULL == pRenderers->m_pFrame) || (NULL == pPage))
				return;

			pRenderers->m_pGraphicFrame->get_Buffer(&m_pBuffer);
			pRenderers->m_pGraphicFrame->get_Width(&m_lWidth);
			pRenderers->m_pGraphicFrame->get_Height(&m_lHeight);

			Parce();

			BYTE* pBufferSrc = NULL;
			pRenderers->m_pFrame->get_Buffer(&pBufferSrc);

			/*
			if (0 == m_arComplete.GetCount())
			{
				RECT rect;
				rect.left		= 0;
				rect.top		= 0;
				rect.right		= m_lWidth - 1;
				rect.bottom		= m_lHeight - 1;
				m_arComplete.Add(rect);
			}
			*/
			size_t nCount = m_arComplete.GetCount();

			if (bIsScan && (nCount > 2))
			{
				m_arComplete.RemoveAll();
				RECT oRect;
				oRect.left		= 0;
				oRect.top		= 0;
				oRect.right		= m_lWidth - 1;
				oRect.bottom	= m_lHeight - 1;
				m_arComplete.Add(oRect);
				nCount = 1;
			}

			for (size_t nIndex = 0; nIndex < nCount; ++nIndex)
			{
				BYTE* pBufferSrcMem = pBufferSrc + 4 * m_arComplete[nIndex].top * m_lWidth + 4 * m_arComplete[nIndex].left;
				LONG lWidthShape	= m_arComplete[nIndex].right - m_arComplete[nIndex].left + 1;
				LONG lHeightShape	= m_arComplete[nIndex].bottom - m_arComplete[nIndex].top + 1;

				MediaCore::IAVSUncompressedVideoFrame* pShapePicture = NULL;
				CoCreateInstance(MediaCore::CLSID_CAVSUncompressedVideoFrame, NULL, CLSCTX_ALL, 
										MediaCore::IID_IAVSUncompressedVideoFrame, (void**)&pShapePicture);

				pShapePicture->put_ColorSpace( ( 1 << 6) | ( 1 << 31) ); // CPS_BGRA | CPS_FLIP
				pShapePicture->put_Width( lWidthShape );
				pShapePicture->put_Height( lHeightShape );
				pShapePicture->put_AspectRatioX( lWidthShape );
				pShapePicture->put_AspectRatioY( lHeightShape );
				pShapePicture->put_Interlaced( VARIANT_FALSE );
				pShapePicture->put_Stride( 0, 4 * lWidthShape );
				pShapePicture->AllocateBuffer( -1 );

				BYTE* pBufferDst = NULL;
				pShapePicture->get_Buffer(&pBufferDst);
				
				for (LONG lLine = 0; lLine < lHeightShape; ++lLine)
				{
					memcpy(pBufferDst, pBufferSrcMem, 4 * lWidthShape);
					pBufferDst		+= 4 * lWidthShape;
					pBufferSrcMem	+= 4 * m_lWidth;
				}

				double dL = 25.4 * m_arComplete[nIndex].left / 96.0;
				double dT = 25.4 * m_arComplete[nIndex].top / 96.0;
				double dW = 25.4 * lWidthShape / 96.0;
				double dH = 25.4 * lHeightShape / 96.0;

				double dHeightMM	= 25.4 * m_lHeight / 96.0;
				dT = (dHeightMM - dT - dH);
				
				pPage->WriteImage((IUnknown*)pShapePicture, dL, dT, dW, dH);

				RELEASEINTERFACE(pShapePicture);
			}
		}
	};

	class CCurrentParagraph
	{
	public:
		static const BYTE c_align_left		= 0x01;
		static const BYTE c_align_right		= 0x02;
		static const BYTE c_align_center	= 0x04;
		static const BYTE c_align_justify	= 0x08;

	public:
		DWORD m_dwCurrentAlign;
		CAtlArray<CLinePDF*> m_arLines;

		// класс, куда все добавляется
		ILogicPage* m_pLogicPage;
		CFormat* m_pFormat;
		double m_dWidthPage;
		double m_dHeightPage;

	public:
		CCurrentParagraph(): m_arLines(), m_pFormat(NULL), m_dwCurrentAlign(0)
		{
			m_dWidthPage	= 0;
			m_dHeightPage	= 0;
			m_pLogicPage	= NULL;
		}

		void AddLine(CLinePDF* pLinePDF)
		{
			// важная функция. удаление всех ненужных объектов здесь!

			// 1) могла придти картинка
			if (-1 != pLinePDF->m_oImageInfo.m_lID)
			{
				// пришла картинка. теперь нужно записать все, что уже сформировано
				WriteParagraph();
				// и создаем графический объект
				if (0 == m_pFormat->m_listSections.GetCount())
					m_pFormat->m_listSections.AddHead();
				CSection& oSection = m_pFormat->m_listSections.GetTail();

				CEbookGraphicObject* pObject = new CEbookGraphicObject();
				pObject->m_oInfo = pLinePDF->m_oImageInfo;
				oSection.m_arItems.AddTail(pObject);

				RELEASEOBJECT(pLinePDF);

				m_pLogicPage->CheckWatermark(*m_pFormat);
				return;
			}
			
			LONG lCountLines = (LONG)m_arLines.GetCount();
			if (0 == lCountLines)
			{
				// выставим возможные алигны. т.е. просто все
				m_dwCurrentAlign = (c_align_left | c_align_center | c_align_right | c_align_justify);
				m_arLines.Add(pLinePDF);

				if ((pLinePDF->GetNaturalWidth()) < (m_dWidthPage / 2.0))
				{
					// больно уж короткий параграф - определяем его как заголовок
					WriteParagraph();
				}
				return;
			}

			if (pLinePDF->IsNewPar())
			{
				WriteParagraph();

				m_dwCurrentAlign = (c_align_left | c_align_center | c_align_right | c_align_justify);
				m_arLines.Add(pLinePDF);

				if ((pLinePDF->GetNaturalWidth()) < (m_dWidthPage / 2.0))
				{
					// больно уж короткий параграф
					WriteParagraph();
				}
				return;
			}

			// сначала посмотрим ширину линии. Если она слишком маленькая - значит 
			// это либо конец текущего параграфа, либо начало нового
			bool bIsLineNoWide = false;
			if ((pLinePDF->m_dRight - pLinePDF->m_dLeft) < (m_dWidthPage / 2.0))
				bIsLineNoWide = true;

			CLinePDF* pLastLine = m_arLines[lCountLines - 1];

			if (pLastLine->IsLineIndividualPar())
			{
				// это новый параграф
				WriteParagraph();

				m_dwCurrentAlign = (c_align_left | c_align_center | c_align_right | c_align_justify);
				m_arLines.Add(pLinePDF);

				if ((pLinePDF->GetNaturalWidth()) < (m_dWidthPage / 2.0))
				{
					// больно уж короткий параграф
					WriteParagraph();
				}
				return;
			}

			DWORD dwOldAlign = m_dwCurrentAlign;

			if (0 != (m_dwCurrentAlign & c_align_right))
			{
				// правая граница должна совпадать с самой первой линии
				if (fabs(pLastLine->m_dRight - pLinePDF->m_dRight) > 1)
				{
					m_dwCurrentAlign -= c_align_right;
				}
			}
			if (0 != (m_dwCurrentAlign & c_align_justify))
			{
				// обе границы - только после второй линии
				if ((1 < lCountLines) && (fabs(pLastLine->m_dRight - pLinePDF->m_dRight) > 0.3))
				{
					m_dwCurrentAlign -= c_align_justify;

					// до этого линии удовлетворяли. Вывод - это последняя строка параграфа
					if (2 < lCountLines)
					{
						// проверим, не накопилась ли ошибка?? Могла быть ступенька, 
						// плавно расширяющаяся/сужающаяся
						CLinePDF* pFirstLine = m_arLines[0];
						if ((fabs(pFirstLine->m_dRight - pLinePDF->m_dRight) < 0.3))
						{
							// нет ступеньки (вообще пробежаться бы по всем линиям)
							bIsLineNoWide = true;
						}
					}
				}
				if ((1 < lCountLines) && (0 != (m_dwCurrentAlign & c_align_justify)))
				{
					if (fabs(pLastLine->m_dLeft - pLinePDF->m_dLeft) > 1)
					{
						m_dwCurrentAlign -= c_align_justify;
					}
				}
			}
			if ((1 < lCountLines) && (0 != (m_dwCurrentAlign & c_align_left)))
			{
				// не забываем про первую линию
				if (fabs(pLastLine->m_dLeft - pLinePDF->m_dLeft) > 1)
				{
					m_dwCurrentAlign -= c_align_left;					
				}
			}
			if (0 != (m_dwCurrentAlign & c_align_center))
			{
				// тут должно быть совпадение с первой линии, и до конца
				double dCenter1 = (pLastLine->m_dLeft + pLastLine->m_dRight) / 2.0;
				double dCenter2 = (pLinePDF->m_dLeft + pLinePDF->m_dRight) / 2.0;

				if (fabs(dCenter1 - dCenter2) > 1)
				{
					m_dwCurrentAlign -= c_align_center;
				}
			}

			if (0 == m_dwCurrentAlign)
			{
				m_dwCurrentAlign = dwOldAlign;
				WriteParagraph();
				m_dwCurrentAlign = (c_align_left | c_align_center | c_align_center | c_align_justify);
				m_arLines.Add(pLinePDF);

				if ((pLinePDF->GetNaturalWidth()) < (m_dWidthPage / 2.0))
				{
					// больно уж короткий параграф - определяем его как заголовок
					WriteParagraph();
				}
			}
			else
			{
				m_arLines.Add(pLinePDF);

				if (bIsLineNoWide)
				{
					WriteParagraph();
					m_dwCurrentAlign = (c_align_left | c_align_center | c_align_center | c_align_justify);
				}
			}
		}
		void WriteParagraph()
		{
			// если список линий не пуст - то
			// записываем последний параграф файла
			LONG lCountLines = (LONG)m_arLines.GetCount();
			if (0 == lCountLines)
				return;
			
			// 1) определяемся с выравниванием. здесь могут стоять разные флаги.
			// поэтому все if'ы по порядку (приоритету align'ов)
			CEbookParagraph* pEbookParagraph = new CEbookParagraph();
			if (0 != (m_dwCurrentAlign & c_align_justify))
				pEbookParagraph->m_lTextAlign = 3;
			else if (0 != (m_dwCurrentAlign & c_align_left))
				pEbookParagraph->m_lTextAlign = 0;
			else if (0 != (m_dwCurrentAlign & c_align_right))
				pEbookParagraph->m_lTextAlign = 2;
			else
				pEbookParagraph->m_lTextAlign = 1;

			// 2) теперь нужно понять уровень текста.
			// заголовок, подзаголовок. Или просто текст

			// признак заголовка или подзаголовка - длина параграфа меньше
			// заголовок - больший размер шрифта.
			double dLeft = 10000;
			double dRight = -10000;
			size_t nCountLines = m_arLines.GetCount();

			bool bIsFirst = true;
			double dFontSize = 0;
			BOOL bIsBold = FALSE;

			for (size_t nIndex = 0; nIndex < nCountLines; ++nIndex)
			{
				CLinePDF* pLine = m_arLines[nIndex];

				if (dLeft > pLine->m_dLeft)
					dLeft = pLine->m_dLeft;
				if (dRight < pLine->m_dRight)
					dRight = pLine->m_dRight;

				pEbookParagraph->m_arLines.AddTail();
				CEbookParagraph::CLine& oLine = pEbookParagraph->m_arLines.GetTail();

				POSITION pos = pLine->m_listSpans.GetHeadPosition();
				while (NULL != pos)
				{
					CEbookParagraph::CSpan* pSpan = pLine->m_listSpans.GetNext(pos);
					oLine.m_listSpans.AddTail(*pSpan);

					if (bIsFirst)
					{
						bIsFirst = false;
						dFontSize = pSpan->m_oFont.Size;
						bIsBold = pSpan->m_oFont.Bold;
					}
				}

				oLine.m_dBaselinePos = pLine->m_dBaseline;
				oLine.m_dTop = oLine.m_dBaselinePos;

				RELEASEOBJECT(pLine);
			}

			m_arLines.RemoveAll();

			pEbookParagraph->m_lStyle = 0;

			#ifdef EBOOK_FROM_PDF_CALCULATE_HEADING_LEVEL
			if (pEbookParagraph->CheckTitle())
			{
				if (((dRight - dLeft) < (m_dWidthPage / 2.0)) && (pEbookParagraph->m_arLines.GetCount() < 3))
				{
					if ((dFontSize >= 14) || (fabs(dLeft + dRight - m_dWidthPage) < 20))
					{
						pEbookParagraph->m_lStyle = 1;
					}
					else
					{
						pEbookParagraph->m_lStyle = 2;
					}
				}
			}
			#endif

			if (0 == m_pFormat->m_listSections.GetCount())
			{
				m_pFormat->m_listSections.AddTail();
				CSection& oSection = m_pFormat->m_listSections.GetTail();

				oSection.m_arItems.AddTail(pEbookParagraph);
			}
			else
			{
				CSection& oSection = m_pFormat->m_listSections.GetTail();

				if (1 != pEbookParagraph->m_lStyle)
				{
					oSection.m_arItems.AddTail(pEbookParagraph);
				}
				else
				{
					bool bIsFoundNoTitle = false;
					POSITION pos = oSection.m_arItems.GetHeadPosition();
					while (NULL != pos)
					{
						IEbookItem* pItem = oSection.m_arItems.GetNext(pos);
						if (IEbookItem::ebitParagraph != pItem->m_eType)
						{
							bIsFoundNoTitle = true;
							break;
						}
						CEbookParagraph* pEbPar = (CEbookParagraph*)pItem;
						if (1 != pEbPar->m_lStyle)
						{
							bIsFoundNoTitle = true;
							break;
						}
					}

					if (bIsFoundNoTitle)
					{
						m_pFormat->m_listSections.AddTail();
						CSection& oNewSection = m_pFormat->m_listSections.GetTail();
						oNewSection.m_arItems.AddTail(pEbookParagraph);
					}
					else
					{
						oSection.m_arItems.AddTail(pEbookParagraph);
					}
				}
			}

			m_pLogicPage->CheckWatermark(*m_pFormat);
		}
	};
}

namespace NSEBook
{
	class CLogicPagePDF : public ILogicPage
	{
	public:
		CRenderers				m_oRenderers;
		CTextPDF				m_oText;

		BOOL					m_bIsImageWriteEnabled;

		CAtlList<CLogicImage*>		m_arImages;
		CAtlList<CLogicTextLine*>	m_arTextLines;
		CAtlList<CLineContainer>	m_arContainers;
		CAtlList<CLinePDF*>			m_arLines;

		CCurrentParagraph			m_oCurrentParagraph;

		double						m_dKoefForDistance;
		double						m_dHeaderDist;
		double						m_dFooterDist;

		// определение сканированного PDF
		bool						m_bIsBigPicture;
		LONG						m_lCountText;

		//FILE* m_pFile; // временно для теста!!!
		//int m_nNewCommand;
		//double m_arBoxPoints[4][2];
		//int m_nCountBoxPoints;
		//CTableDetector detector;

	public:
		CLogicPagePDF() : ILogicPage()
		{
			m_bIsImageWriteEnabled	= FALSE;
			m_dKoefForDistance		= 0.15;
			m_dHeaderDist			= 0;
			m_dFooterDist			= 0;

			//m_pFile = ::fopen( "d:\\_1p.txt", "wb" );
			//m_nNewCommand = 0;
		}
		~CLogicPagePDF()
		{
			//::fclose( m_pFile );
		}

	public:
		virtual void ConvertToEbookPage(NSEBook::CFormat& oFormat)
		{
			m_dHeaderDist		= m_dKoefForDistance * m_dHeight;
			m_dFooterDist		= m_dHeight - m_dKoefForDistance * m_dHeight - 1;

			m_oCurrentParagraph.m_pFormat = &oFormat;
			m_oCurrentParagraph.m_pLogicPage = this;

			Prepare_GenerateImages();
			Prepare_DeleteTextLineInImage();
			Prepare_Footers();
			Prepare_LineContainers();
			Prepare_SortLineContainers();
			GenerateItems();

			POSITION pos = m_arItems.GetHeadPosition();
			while (NULL != pos)
			{
				CSection* pSection = NULL;
				bool bIsFirstSection = true;
				if (0 == oFormat.m_listSections.GetCount())
				{
					oFormat.m_listSections.AddTail();
					pSection = &oFormat.m_listSections.GetTail();
				}
				else
				{
					pSection = &oFormat.m_listSections.GetTail();
					bIsFirstSection = false;
				}

				ILogicItem* pItem = m_arItems.GetNext(pos);

				if (ILogicItem::elitTextParagraph == pItem->m_eType)
				{
					CLogicParagraph* paragraph = dynamic_cast<CLogicParagraph*>(pItem);
					
					if (paragraph->m_oParagraph.m_lStyle == 1 && !bIsFirstSection)
					{
						// это два параграфа подряд title. если так будет выглядеть не очень - то будем менять
						if (1 < pSection->m_arItems.GetCount())
						{
							oFormat.m_listSections.AddTail();
							pSection = &oFormat.m_listSections.GetTail();
						}
					}
				}

				switch (pItem->m_eType)
				{
				case ILogicItem::elitTextParagraph:
					{
						CEbookParagraph* pParagraph = new CEbookParagraph(((CLogicParagraph*)pItem)->m_oParagraph);
						pSection->m_arItems.AddTail(pParagraph);
						break;
					}
				case ILogicItem::elitTable:
					{
						// такого здесь пока нет. На это надо потратить время.
						// все измерения есть, все объекты есть. Просто нужно посидеть
						// и распознать таблицы
						CEbookTable* pTable = new CEbookTable();
						pTable->m_strTableXml = ((CLogicTable*)pItem)->m_strTableXml;
						pSection->m_arItems.AddTail(pTable);
						break;
					}
				case ILogicItem::elitGraphic:
					{
						CLogicImage* pLogicImage = ((CLogicImage*)pItem);
						
						CEbookGraphicObject* pImage = new CEbookGraphicObject();						
						pImage->m_oInfo = pLogicImage->m_oImageInfo;
						pSection->m_arItems.AddTail(pImage);
						break;
					}
				default:
					break;
				}
			}
		}

	protected:
		AVSINLINE void Prepare_GenerateImages()
		{
			// здесь запускается анализатор графики
			// из всевозможных патов и картинок создаются
			// картинки, и записываются в m_arImages

			m_bIsImageWriteEnabled = TRUE;
			
			CFigure oFigure;
			oFigure.Convert(this, &m_oRenderers, ((0 == m_lCountText) && m_bIsBigPicture));

			m_bIsImageWriteEnabled = FALSE;
		}
		void Prepare_DeleteTextLineInImage()
		{
			// здесь пробегаем по всем текстовым линиям, и смотрим,
			// если линия попадает в имадж - то удаляем эту линию

			POSITION posTextLine = m_arTextLines.GetHeadPosition();
			while (NULL != posTextLine)
			{
				POSITION posTextLineOld = posTextLine;
				CLogicTextLine* pTextLine = m_arTextLines.GetNext(posTextLine);

				double _x = pTextLine->m_rcBounds.left;
				double _y = pTextLine->m_dBaseLine - pTextLine->m_dLineHeight;
				double _r = pTextLine->m_rcBounds.right;
				double _b = pTextLine->m_dBaseLine;

				bool bDelete = false;
				POSITION posImage = m_arImages.GetHeadPosition();
				while (NULL != posImage)
				{
					CLogicImage* pImage = m_arImages.GetNext(posImage);

					// под "единственную вычисляемую" переменную _y - заглушка в виде 5мм
					// остальным - 1мм

					if (((_x + 1) >= pImage->m_rcBounds.left) && ((_y + 5) >= pImage->m_rcBounds.top) &&
						((_r - 1) <= pImage->m_rcBounds.right) && ((_b - 1) <= pImage->m_rcBounds.bottom))
					{
						bDelete = true;
						break;
					}
				}

				if (bDelete)
				{
					m_arTextLines.RemoveAt(posTextLineOld);
				}
			}
		}
		void Prepare_Footers()
		{
			#ifdef EBOOK_FROM_PDF_CALCULATE_HEADER_FOOTER
			// здесь выкидываем все, что попадает в колонтитулы
			POSITION pos = m_arImages.GetHeadPosition();
			while (NULL != pos)
			{
				CLogicImage* pLogicImage = m_arImages.GetNext(pos);				

				if ((pLogicImage->m_rcBounds.top < m_dHeaderDist) && (pLogicImage->m_rcBounds.bottom > m_dHeaderDist))
				{
					// картинка должна сдвинуть m_dHeaderDist
					m_dHeaderDist = max(0, pLogicImage->m_rcBounds.top);
				}
				if ((pLogicImage->m_rcBounds.top < m_dFooterDist) && (pLogicImage->m_rcBounds.bottom > m_dFooterDist))
				{
					// картинка должна сдвинуть m_dHeaderDist
					m_dFooterDist = min(m_dHeight - 1, pLogicImage->m_rcBounds.bottom);
				}
			}

			// теперь пробегаемся по картинкам, и смотрим, какие имаджы нужно выкинуть,
			// а заодно и определим (уточним) сами размеры колонтитулов
			double dHeader = 0;
			double dFooter = m_dHeight - 1;

			pos = m_arImages.GetHeadPosition();
			while (NULL != pos)
			{
				POSITION posOld = pos;
				CLogicImage* pLogicImage = m_arImages.GetNext(pos);				
				
				if (pLogicImage->m_rcBounds.bottom < m_dHeaderDist)
				{
					dHeader = max(dHeader, pLogicImage->m_rcBounds.bottom);
					m_arImages.RemoveAt(posOld);
				}
				else if (pLogicImage->m_rcBounds.top > m_dFooterDist)
				{
					dFooter = min(pLogicImage->m_rcBounds.top, dFooter);
					m_arImages.RemoveAt(posOld);
				}
			}

			m_dHeaderDist = dHeader;
			m_dFooterDist = dFooter;

			// теперь просто удаляем все линии, которые попали в колонтитулы
			POSITION posTextLine = m_arTextLines.GetHeadPosition();
			while (NULL != posTextLine)
			{
				POSITION posTextLineOld = posTextLine;
				CLogicTextLine* pTextLine = m_arTextLines.GetNext(posTextLine);

				if (pTextLine->m_dBaseLine < m_dHeaderDist)
				{
					// header
					m_arTextLines.RemoveAt(posTextLineOld);
				}
				if (pTextLine->m_dBaseLine > m_dFooterDist)
				{
					// footer
					m_arTextLines.RemoveAt(posTextLineOld);
				}
			}
			#endif

			// теперь нужно удалить все "очень тонкие картинки"
			pos = m_arImages.GetHeadPosition();
			while (NULL != pos)
			{
				POSITION posOld = pos;
				CLogicImage* pLogicImage = m_arImages.GetNext(pos);				
				
				if (pLogicImage->m_rcBounds.GetHeight() < 1)
				{
					m_arImages.RemoveAt(posOld);
				}
			}
		}
		void Prepare_LineContainers()
		{
			// здесь создаем настоящие линии. Просто происходит набивка
			// элементов, пересекающихся по горизонтальной baseline (у имаджа
			// за baseline будем брать нижнюю границу)
			// все линии набиваются непосредственно в m_arContainers
			
			while (0 != m_arTextLines.GetCount())
			{
				CLogicTextLine* pLINE = m_arTextLines.RemoveHead();
				
				m_arContainers.AddTail();
				CLineContainer& oContainer = m_arContainers.GetTail();

				oContainer.m_arItems.AddTail(pLINE);

				double dY1 = pLINE->m_dBaseLine - pLINE->m_dLineHeight;
				double dY2 = pLINE->m_dBaseLine;

				// смотрим на текстовые линии
				POSITION pos = m_arTextLines.GetHeadPosition();
				while (NULL != pos)
				{
					POSITION oldPos = pos;
					CLogicTextLine* pTextLine = m_arTextLines.GetNext(pos);

					double dY12 = pTextLine->m_dBaseLine - pTextLine->m_dLineHeight;
					double dY22 = pTextLine->m_dBaseLine;

					// есть ли пересечение отрезков??
					if (((dY12 >= dY1) && (dY12 <= dY2)) || ((dY22 >= dY1) && (dY22 <= dY2)) ||
						((dY12 < dY1) && (dY22 > dY2)))
					{
						m_arTextLines.RemoveAt(oldPos);
						oContainer.m_arItems.AddTail(pTextLine);
					}
				}

				// смотрим на картинки
				POSITION posImage = m_arImages.GetHeadPosition();
				while (NULL != posImage)
				{
					POSITION oldPos = posImage;
					CLogicImage* pImage = m_arImages.GetNext(posImage);

					double dY12 = pImage->m_rcBounds.top;
					double dY22 = pImage->m_rcBounds.bottom;

					// есть ли пересечение отрезков??
					if (((dY12 >= dY1) && (dY12 <= dY2)) || ((dY22 >= dY1) && (dY22 <= dY2)) ||
						((dY12 < dY1) && (dY22 > dY2)))
					{
						// теперь посмотрим: расстояние top до baseline не должно превышать lineheight
						if (fabs(dY2 - dY22) < pLINE->m_dLineHeight)
						{
							m_arImages.RemoveAt(oldPos);
							oContainer.m_arItems.AddTail(pImage);
						}
					}
				}
			}

			// все, текстовых линий больше нет
			// теперь оставшиеся картинки отправим в свои собственные линии
			POSITION posImage = m_arImages.GetHeadPosition();
			while (NULL != posImage)
			{
				m_arContainers.AddTail();
				CLineContainer& oContainer = m_arContainers.GetTail();
				oContainer.m_arItems.AddTail(m_arImages.GetNext(posImage));
			}

			// удаляем то, что больше не пригодится
			m_arTextLines.RemoveAll();
			m_arImages.RemoveAll();
		}
		void Prepare_SortLineContainers()
		{
			// здесь:
			// 1) сортируем каждую линию отдельно (положение по горизонтали)
			// 2) сортируем линии по baseline (положение по вертикали)
			
			POSITION pos = m_arContainers.GetHeadPosition();
			while (NULL != pos)
			{
				CLineContainer& oContainer = m_arContainers.GetNext(pos);
				CLinePDF* pLine = oContainer.ToLinePDF();
				m_arLines.AddTail(pLine);
			}
			// контейнеры теперь не нужны
			m_arContainers.RemoveAll();

			// теперь сортируем линии
			CAtlList<CLinePDF*> arraySort;
			
			while (0 != m_arLines.GetCount())
			{
				POSITION pos = m_arLines.GetHeadPosition();
				POSITION posNeed = pos;

				CLinePDF* pItem = m_arLines.GetNext(pos);
				double dMin = pItem->m_dBaseline;
				while (NULL != pos)
				{
					POSITION posOld = pos;
					pItem = m_arLines.GetNext(pos);

					double dMem = pItem->m_dBaseline;
					if (dMem < dMin)
					{
						dMin = dMem;
						posNeed = posOld;
					}
				}

				CLinePDF* pNeed = m_arLines.GetAt(posNeed);
				m_arLines.RemoveAt(posNeed);
				arraySort.AddTail(pNeed);
			}
			m_arLines.AddTailList(&arraySort);
			arraySort.RemoveAll();

			// теперь смотрим первую и последнюю линии. Попробуем удалить нумерацию
			if (0 < m_arLines.GetCount())
			{
				CLinePDF* pHead = m_arLines.GetHead();
				if (10 > pHead->GetNaturalWidth())
				{
					m_arLines.RemoveHead();
					RELEASEOBJECT(pHead);					
				}
			}
			if (0 < m_arLines.GetCount())
			{
				CLinePDF* pTail = m_arLines.GetTail();
				if (10 > pTail->GetNaturalWidth())
				{
					m_arLines.RemoveTail();
					RELEASEOBJECT(pTail);					
				}
			}
		}
		void GenerateItems()
		{
			// здесь генерируем параграфы (с выравниванием) по линиям, и отдельные имаджы
			// также здесь определяем заголовки
			while (0 != m_arLines.GetCount())
			{
				CLinePDF* pHead = m_arLines.RemoveHead();
				m_oCurrentParagraph.AddLine(pHead);
			}
		}

	public:
		virtual void MoveTo(double& x, double& y)
		{
			//if( m_nNewCommand != 1 )
			//{
			//	m_nNewCommand = 1;
			//	::fprintf( m_pFile, "move: " );
			//}
			//else
			//{
			//	::fprintf( m_pFile, "    : " );
			//}
			//::fprintf( m_pFile, "%#4.16g, %#4.16g\r\n", x, y );
		}
		virtual void LineTo(double& x, double& y)
		{
			//if( m_nNewCommand != 2 )
			//{
			//	m_nCountBoxPoints = (m_nNewCommand == 1) ? 0 : 4;
			//	
			//	m_nNewCommand = 2;
			//	::fprintf( m_pFile, "line: " );
			//}
			//else
			//{
			//	::fprintf( m_pFile, "    : " );
			//}
			//
			//if( m_nCountBoxPoints < 4 )
			//{
			//	m_arBoxPoints[m_nCountBoxPoints][0] = x;
			//	m_arBoxPoints[m_nCountBoxPoints][1] = y;

			//	if( ++m_nCountBoxPoints == 4 )
			//	{
			//		detector.AddBlock( m_arBoxPoints );
			//	}
			//}
			//
			//::fprintf( m_pFile, "%#4.16g, %#4.16g\r\n", x, y );
		}
		virtual void CurveTo(double& x1, double& y1, double& x2, double& y2, double& x3, double& y3)
		{
			//if( m_nNewCommand != 3 )
			//{
			//	m_nNewCommand = 3;
			//	::fprintf( m_pFile, "curv: " );
			//}
			//else
			//{
			//	::fprintf( m_pFile, "    : " );
			//}
			//::fprintf( m_pFile, "%#.16g, %#.16g, %#.16g, %#.16g, %#.16g, %#.16g\r\n", x1, y1, x2, y2, x3, y3 );
		}
		virtual void Close()
		{
		}
		virtual void BeginPath()
		{
		}
		virtual void EndPath()
		{
			//m_nNewCommand = 0;
		}

		virtual void WriteText(BSTR& bsText, BSTR& bsGid, double& x, double& y, double& width, double& height, double& baselineoffset)
		{
			CLogicTextLine* pTextLine = m_oText.CommandText(bsText, bsGid, x, y, width, height, baselineoffset);
			if (NULL != pTextLine)
				m_arTextLines.AddTail(pTextLine);
			++m_lCountText;
		}

		virtual void WriteTable(NSEBook::ILogicItem* pItem)
		{
			// эти методы не нужны. в пдф нету никаких шейпов. придется самому все разруливать
		}
		virtual void BeginShape()
		{
			// эти методы не нужны. в пдф нету никаких шейпов. придется самому все разруливать
		}
		virtual void EndShape()
		{
			// эти методы не нужны. в пдф нету никаких шейпов. придется самому все разруливать
		}

		virtual void DrawPath(LONG lType)
		{
		}
		virtual void Clear()
		{
			POSITION pos = m_arItems.GetHeadPosition();
			while (NULL != pos)
			{
				ILogicItem* pItem = m_arItems.GetNext(pos);
				RELEASEOBJECT(pItem);
			}
			m_arItems.RemoveAll();
		}

		virtual void BeginPage()
		{
			m_oRenderers.NewPage(m_dWidth, m_dHeight);
			m_oCurrentParagraph.m_dWidthPage	= m_dWidth;
			m_oCurrentParagraph.m_dHeightPage	= m_dHeight;
			double dDpiX = 96.0;
			double dDpiY = 96.0; 
			m_oText.NewPage(dDpiX, dDpiY);

			m_bIsBigPicture = false;
			m_lCountText	= 0;

			//m_nNewCommand = 0;
			//::fprintf( m_pFile, "\r\nbegin page:\r\n" );
			//detector.Clear();
		}
		virtual void EndPage()
		{
			CLogicTextLine* pTextLine = m_oText.ClosePage();
			if (NULL != pTextLine)
				m_arTextLines.AddTail(pTextLine);

			m_bIsDumpWatermark = true;

			//m_nNewCommand = 0;
			//::fprintf( m_pFile, "\r\ncount hor blocks = %d\r\ncount ver blocks = %d\r\nend page\r\n", detector.GetCountHorBlocks(), detector.GetCountVerBlocks() );
			//detector.Calculate();
			//detector.Clear();
		}
		virtual void CloseFile(NSEBook::CFormat& oFormat)
		{
			m_oCurrentParagraph.m_pFormat = &oFormat;
			m_oCurrentParagraph.m_pLogicPage = this;
			m_oCurrentParagraph.WriteParagraph();
		}

		virtual void WriteImage(IUnknown* punkImage, double& x, double& y, double& width, double& height)
		{
			if (!m_bIsImageWriteEnabled)
			{
				if ((width > 3 * m_dWidth / 4) && (height > 3 * m_dHeight / 4))
					m_bIsBigPicture = true;

				return;
			}

			CLogicImage* pImage = new CLogicImage();
			pImage->m_oImageInfo = m_pImageManager->WriteImage(punkImage, x, y, width, height);
			pImage->m_rcBounds.left		= x;
			pImage->m_rcBounds.top		= y;
			pImage->m_rcBounds.right	= x + width;
			pImage->m_rcBounds.bottom	= y + height;
			m_arImages.AddTail(pImage);
		}
		virtual void WriteImage(CString strFilePath, double& x, double& y, double& width, double& height)
		{
			if (!m_bIsImageWriteEnabled)
			{
				if ((width > 3 * m_dWidth / 4) && (height > 3 * m_dHeight / 4))
					m_bIsBigPicture = true;
				return;
			}

			CLogicImage* pImage = new CLogicImage();
			pImage->m_oImageInfo = m_pImageManager->WriteImage(strFilePath, x, y, width, height);
			pImage->m_rcBounds.left		= x;
			pImage->m_rcBounds.top		= y;
			pImage->m_rcBounds.right	= x + width;
			pImage->m_rcBounds.bottom	= y + height;
			m_arImages.AddTail(pImage);
		}

		virtual void InitProp()
		{
			m_oText.m_pFontsDst = &m_oFontsGen;
			m_oText.m_pFontManager = &m_oFontManager;
			m_oText.SetParams(m_pPen, m_pBrush, m_pFont, m_pTransform);
			m_oFontsGen.m_lCountFonts = 0;
		}
	};
}