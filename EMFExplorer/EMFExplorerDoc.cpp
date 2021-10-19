
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


// CEMFExplorerDoc serialization

void CEMFExplorerDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
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

// Support for thumbnails
void CEMFExplorerDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));
	if (m_emf.get())
	{
// 		CRect rcDraw = m_emf->GetFittingDrawRect(rcBounds);
// 		Gdiplus::Graphics gg(dc.GetSafeHdc());
// 		m_emf->DrawMetafile(gg, rcDraw);
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
