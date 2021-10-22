
// EMFExplorerDoc.cpp : implementation of the CEMFExplorerDoc class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "EMFExplorer.h"
#endif

#include "EMFExplorerDoc.h"

#include <propkey.h>

#pragma comment(lib, "GdiPlus.lib")

// disable this to avoid the following compile error:
// error C2660: 'Gdiplus::GdiplusBase::operator new': function does not take 3 arguments

// #ifdef _DEBUG
// #define new DEBUG_NEW
// #endif

static	int			l_nGdiplusRequire = 0;
static	ULONG_PTR	l_lptrGdiplus = NULL;

int	GdiplusBegin()
{
	if (0 == l_nGdiplusRequire)
	{
		Gdiplus::GdiplusStartupInput input;
		Gdiplus::GdiplusStartup(&l_lptrGdiplus, &input, NULL);
	}

	return (++l_nGdiplusRequire);
}

int GdiplusEnd()
{
	--l_nGdiplusRequire;
	if (0 == l_nGdiplusRequire && 0 != l_lptrGdiplus)
	{
		Gdiplus::GdiplusShutdown(l_lptrGdiplus);
		l_lptrGdiplus = NULL;
	}
	return (l_nGdiplusRequire);
}

// CEMFExplorerDoc

IMPLEMENT_DYNCREATE(CEMFExplorerDoc, CDocument)

BEGIN_MESSAGE_MAP(CEMFExplorerDoc, CDocument)
END_MESSAGE_MAP()


// CEMFExplorerDoc construction/destruction

CEMFExplorerDoc::CEMFExplorerDoc() noexcept
{
	// TODO: add one-time construction code here
	GdiplusBegin();
}

CEMFExplorerDoc::~CEMFExplorerDoc()
{
	GdiplusEnd();
}

BOOL CEMFExplorerDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)


	return TRUE;
}

void CEMFExplorerDoc::UpdateEMFData(const std::vector<emfplus::u8t>& data, EMFType type)
{
	m_emf = std::make_shared<EMFAccess>(data);
	m_type = type;
}

void CEMFExplorerDoc::SetEMFAccess(std::shared_ptr<EMFAccess> emf, EMFType type)
{
	m_emf = emf;
	m_type = type;
}


BOOL CEMFExplorerDoc::DoFileSave()
{
	DWORD dwAttrib = GetFileAttributes(m_strPathName);
	BOOL bUsePathName = m_type == EMFType::FromFile && !(dwAttrib & FILE_ATTRIBUTE_READONLY);
	LPCTSTR pszFilePath = bUsePathName ? m_strPathName : nullptr;
	if (!DoSave(pszFilePath, FALSE))
	{
		TRACE(traceAppMsg, 0, "Warning: File save failed.\n");
		return FALSE;
	}
	return TRUE;
}

BOOL CEMFExplorerDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	if (!m_emf)
		return FALSE;
	return m_emf->SaveToFile(lpszPathName);
}

// CEMFExplorerDoc serialization

void CEMFExplorerDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		ASSERT(m_emf.get());
	}
	else
	{
		// Memory leak?
		ASSERT(!m_emf.get());
		CFile* pFile = ar.GetFile();
		auto nSize = (UINT)pFile->GetLength();
		std::vector<BYTE> vBuffer(nSize);
		pFile->Read(vBuffer.data(), nSize);
		UpdateEMFData(vBuffer, EMFType::FromFile);
	}
}

void CEMFExplorerDoc::DeleteContents()
{
	m_emf.reset();
	m_type = EMFType::Invalid;
}

#ifdef SHARED_HANDLERS

#if 0
// Support for thumbnails
void CEMFExplorerDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));
	if (m_emf.get())
	{
		Gdiplus::Graphics gg(dc.GetSafeHdc());
		gg.SetCompositingQuality(Gdiplus::CompositingQualityHighQuality);
		gg.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
		gg.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias8x8);
		auto hdr = m_emf->GetMetafileHeader();
		CSize szImg(hdr.Width, hdr.Height);
		CRect rcDraw = GetFitRect(lprcBounds, szImg, true);
// 		Gdiplus::Pen pen(Gdiplus::Color::Black);
// 		Gdiplus::SolidBrush brush(Gdiplus::Color(10, 0,0,0));
// 		Gdiplus::Rect rcDrawP(rcDraw.left, rcDraw.top, rcDraw.Width(), rcDraw.Height());
// 		gg.FillRectangle(&brush, rcDrawP);
// 		gg.DrawRectangle(&pen, rcDrawP);
		m_emf->DrawMetafile(gg, rcDraw);
	}
	else
	{
		CString strText = _T("EMFExplorer: failed to read EMF");
		LOGFONT lf;

		CFont* pDefaultGUIFont = CFont::FromHandle((HFONT)GetStockObject(DEFAULT_GUI_FONT));
		pDefaultGUIFont->GetLogFont(&lf);
		lf.lfHeight = 36;

		CFont fontDraw;
		fontDraw.CreateFontIndirect(&lf);

		CFont* pOldFont = dc.SelectObject(&fontDraw);
		auto oldTxtColor = dc.SetTextColor(RGB(255, 0, 0));
		dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
		dc.SetTextColor(oldTxtColor);
		dc.SelectObject(pOldFont);
	}
}
#endif

BOOL CEMFExplorerDoc::GetThumbnail(UINT cx, HBITMAP* phbmp, WTS_ALPHATYPE* pdwAlpha)
{
	ASSERT(phbmp != NULL);
	if (pdwAlpha != NULL)
	{
		*pdwAlpha = WTSAT_ARGB;
	}
	CRect rectDocBounds = CRect(0, 0, cx, cx);
	Gdiplus::Bitmap bmp(cx, cx, PixelFormat32bppARGB);
	Gdiplus::Graphics gg(&bmp);
	gg.SetCompositingQuality(Gdiplus::CompositingQualityHighQuality);
	gg.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
	gg.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias8x8);
	gg.SetTextRenderingHint(Gdiplus::TextRenderingHintClearTypeGridFit);

	if (m_emf)
	{
		auto hdr = m_emf->GetMetafileHeader();
		CSize szImg(hdr.Width, hdr.Height);
		CRect rcDraw = GetFitRect(rectDocBounds, szImg, true);
		m_emf->DrawMetafile(gg, rcDraw);
	}
	else
	{
		CClientDC dc(nullptr);
		Gdiplus::Font font(dc.GetSafeHdc(), (HFONT)GetStockObject(DEFAULT_GUI_FONT));
		Gdiplus::PointF pt(0, 0);
		Gdiplus::SolidBrush brush(Gdiplus::Color::Black);
		gg.DrawString(L"EMFExplorer: failed to read EMF", -1, &font, pt, &brush);
	}
	HBITMAP hBmp = nullptr;
	bmp.GetHBITMAP(Gdiplus::Color(0, 0, 0, 0), &hBmp);
	*phbmp = hBmp;
	return TRUE;
}

// Support for Search Handlers
void CEMFExplorerDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// Set search contents from document's data.
	// The content parts should be separated by ";"

	// For example:  strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CEMFExplorerDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = nullptr;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != nullptr)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CEMFExplorerDoc diagnostics

#ifdef _DEBUG
void CEMFExplorerDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CEMFExplorerDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CEMFExplorerDoc commands
