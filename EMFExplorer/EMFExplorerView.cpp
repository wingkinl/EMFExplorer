
// EMFExplorerView.cpp : implementation of the CEMFExplorerView class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "EMFExplorer.h"
#endif

#include "EMFExplorerDoc.h"
#include "EMFExplorerView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CEMFExplorerView

IMPLEMENT_DYNCREATE(CEMFExplorerView, CEMFExplorerViewBase)

BEGIN_MESSAGE_MAP(CEMFExplorerView, CEMFExplorerViewBase)
#ifdef HANDLE_PAINT_WITH_DOUBLE_BUFFER
	ON_WM_ERASEBKGND()
#endif // HANDLE_PAINT_WITH_DOUBLE_BUFFER
	ON_WM_PAINT()
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_UPDATE_COMMAND_UI(ID_FILE_NEW, &CEMFExplorerView::OnUpdateFileNew)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, &CEMFExplorerView::OnUpdateNeedDoc)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, &CEMFExplorerView::OnUpdateNeedDoc)
	ON_COMMAND(ID_EDIT_COPY, &CEMFExplorerView::OnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, &CEMFExplorerView::OnUpdateNeedClip)
#ifndef SHARED_HANDLERS
	ON_COMMAND(ID_ZOOM_ACTUALSIZE, &CEMFExplorerView::OnZoomActualSize)
	ON_UPDATE_COMMAND_UI(ID_ZOOM_ACTUALSIZE, &CEMFExplorerView::OnUpdateZoomActualSize)
	ON_COMMAND(ID_ZOOM_IN, &CEMFExplorerView::OnZoomIn)
	ON_UPDATE_COMMAND_UI(ID_ZOOM_IN, &CEMFExplorerView::OnUpdateZoomIn)
	ON_COMMAND(ID_ZOOM_OUT, &CEMFExplorerView::OnZoomOut)
	ON_UPDATE_COMMAND_UI(ID_ZOOM_OUT, &CEMFExplorerView::OnUpdateZoomOut)
	ON_COMMAND(ID_ZOOM_CENTER, &CEMFExplorerView::OnZoomCenter)
	ON_UPDATE_COMMAND_UI(ID_ZOOM_CENTER, &CEMFExplorerView::OnUpdateZoomCenter)
	ON_COMMAND(ID_ZOOM_FITTOWINDOW, &CEMFExplorerView::OnZoomFitToWindow)
	ON_UPDATE_COMMAND_UI(ID_ZOOM_FITTOWINDOW, &CEMFExplorerView::OnUpdateZoomFitToWindow)
	ON_COMMAND(ID_ZOOM_FITWIDTH, &CEMFExplorerView::OnZoomFitWidth)
	ON_UPDATE_COMMAND_UI(ID_ZOOM_FITWIDTH, &CEMFExplorerView::OnUpdateZoomFitWidth)
	ON_COMMAND(ID_ZOOM_FITHEIGHT, &CEMFExplorerView::OnZoomFitHeight)
	ON_UPDATE_COMMAND_UI(ID_ZOOM_FITHEIGHT, &CEMFExplorerView::OnUpdateZoomFitHeight)
	ON_COMMAND_RANGE(ID_ZOOM_1, ID_ZOOM_3200, &CEMFExplorerView::OnZoomPresetFactor)
	ON_UPDATE_COMMAND_UI_RANGE(ID_ZOOM_1, ID_ZOOM_3200, &CEMFExplorerView::OnUpdateZoomPresetFactor)
#endif // SHARED_HANDLERS
END_MESSAGE_MAP()

// CEMFExplorerView construction/destruction

CEMFExplorerView::CEMFExplorerView() noexcept
{
#ifndef SHARED_HANDLERS
	m_bShowTransparentGrid = theApp.m_bShowTransparentBkGrid;
#endif // SHARED_HANDLERS
}

CEMFExplorerView::~CEMFExplorerView()
{
}

BOOL CEMFExplorerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	// double buffer
#ifndef HANDLE_PAINT_WITH_DOUBLE_BUFFER
	cs.dwExStyle |= WS_EX_COMPOSITED;
#endif // HANDLE_PAINT_WITH_DOUBLE_BUFFER

	return CEMFExplorerViewBase::PreCreateWindow(cs);
}

BOOL CEMFExplorerView::CheckClipboardForEMF() const
{
	BOOL bOK = IsClipboardFormatAvailable(CF_ENHMETAFILE);
	return bOK;
}

BOOL CEMFExplorerView::HasValidEMFInDoc() const
{
	CEMFExplorerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return FALSE;
	return pDoc->GetEMFType() != CEMFExplorerDoc::EMFType::Invalid;
}

void CEMFExplorerView::LoadEMFDataEvent(bool bBefore)
{
	SetRedraw(!bBefore);
	if (bBefore)
	{

	}
	else
	{
		SetFitToWindow(FitToBoth);
	}
}

// CEMFExplorerView drawing

void CEMFExplorerView::DrawTransparentGrid(CDC* pDC, const CRect& rect)
{
	pDC->FillSolidRect(rect, GetSysColor(COLOR_WINDOW));

	int nSavedDC = pDC->SaveDC();
	pDC->IntersectClipRect(rect);

	int nDPI = pDC->GetDeviceCaps(LOGPIXELSX);
	double dScale = (double)nDPI / 96;
	const int nGrid = (int)dScale * 8;
	const int nGrid2 = nGrid * 2;
	for (LONG yy = rect.top, row = 0; yy < rect.bottom; yy += nGrid, ++row)
	{
		for (LONG xx = rect.left+((row&1)?0:nGrid); xx < rect.right; xx += nGrid2)
		{
			pDC->FillSolidRect(xx, yy, nGrid, nGrid, RGB(0xcc, 0xcc, 0xcc));
		}
	}
	pDC->RestoreDC(nSavedDC);
}

bool CEMFExplorerView::IsZoomAllowed() const
{
	return !!HasValidEMFInDoc();
}

void CEMFExplorerView::SetShowTransparentGrid(bool val)
{
	m_bShowTransparentGrid = val;
	Invalidate();
}

CSize CEMFExplorerView::GetViewSize() const
{
	CEMFExplorerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (pDoc)
	{
		auto pEMF = pDoc->GetEMFAccess();
		if (pEMF)
		{
			auto hdr = pEMF->GetMetafileHeader();
			return CSize(hdr.Width, hdr.Height);
		}
	}
	return CEMFExplorerViewBase::GetViewSize();
}

#ifdef HANDLE_PAINT_WITH_DOUBLE_BUFFER
BOOL CEMFExplorerView::OnEraseBkgnd(CDC* /*pDC*/)
{
	return TRUE;
}
#endif // HANDLE_PAINT_WITH_DOUBLE_BUFFER

void CEMFExplorerView::OnPaint()
{
	CPaintDC dc(this);
#ifdef HANDLE_PAINT_WITH_DOUBLE_BUFFER
	CMemDC dcMem(dc, this);
	CDC* pDCDraw = &dcMem.GetDC();
#endif // HANDLE_PAINT_WITH_DOUBLE_BUFFER
	CDC* pDCDraw = &dc;

	CRect rect;
	GetClientRect(&rect);

	COLORREF crfBk = GetSysColor(COLOR_WINDOW);
#ifndef SHARED_HANDLERS
	if (theApp.IsDarkTheme())
		crfBk = theApp.m_crfDarkThemeBkColor;
#endif // SHARED_HANDLERS
	pDCDraw->FillSolidRect(rect, crfBk);

	if (m_bInitialRedraw)
		return;

	CEMFExplorerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	auto pEMF = pDoc->GetEMFAccess();
	if (!pEMF)
		return;
	auto fitToWin = GetFitToWindow();
	CSize szImg = fitToWin ? GetRealViewSize() : m_totalDev;
	ASSERT(fitToWin || GetRealViewSize() == m_totalDev);

	CPoint ptImg = -GetDeviceScrollPosition();
	if (GetCenter())
	{
		if (szImg.cx < rect.Width())
			ptImg.x = (rect.Width() - szImg.cx) / 2;
		if (szImg.cy < rect.Height())
			ptImg.y = (rect.Height() - szImg.cy) / 2;
	}

	CRect rcImg(ptImg, szImg);

	if (GetShowTransparentGrid())
	{
		CRect rcClient;
		GetClientRect(rcClient);
		CRect rcGrid(rcImg);
		if (rcGrid.left < 0)
			rcGrid.left = 0;
		if (rcGrid.top < 0)
			rcGrid.top = 0;
		if (rcGrid.right > rcClient.right)
			rcGrid.right = rcClient.right;
		if (rcGrid.bottom > rcClient.bottom)
			rcGrid.bottom = rcClient.bottom;
		DrawTransparentGrid(pDCDraw, rcGrid);
	}

	Gdiplus::Graphics gg(pDCDraw->GetSafeHdc());
	gg.SetCompositingQuality(Gdiplus::CompositingQualityHighQuality);
	gg.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
	gg.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias8x8);
	pEMF->DrawMetafile(gg, rcImg);
}

void CEMFExplorerView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CEMFExplorerView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


void CEMFExplorerView::OnUpdateFileNew(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(FALSE);
}

void CEMFExplorerView::OnUpdateNeedDoc(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(HasValidEMFInDoc());
}

void CEMFExplorerView::OnUpdateNeedClip(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(CheckClipboardForEMF());
}

bool CEMFExplorerView::PutBitmapToClipboard(Gdiplus::Image* pImg)
{
	Gdiplus::Bitmap bmpPlus(pImg->GetWidth(), pImg->GetHeight(), PixelFormat32bppARGB);
	Gdiplus::Graphics gg(&bmpPlus);
	gg.DrawImage(pImg, 0, 0);

	HBITMAP hBitmap;
	auto status = bmpPlus.GetHBITMAP(Gdiplus::Color::White, &hBitmap);
	if(status != Gdiplus::Ok)
		return false;
	CBitmap bmp; bmp.Attach(hBitmap);
	BITMAP bm;
	GetObject(hBitmap, sizeof bm, &bm);

	BITMAPINFOHEADER bi =
	{ sizeof bi, bm.bmWidth, bm.bmHeight, 1, bm.bmBitsPixel, BI_RGB };

	std::vector<BYTE> vec(bm.bmWidthBytes * bm.bmHeight);
	auto hDC = ::GetDC(NULL);
	GetDIBits(hDC, hBitmap, 0, bi.biHeight, vec.data(), (BITMAPINFO*)&bi, 0);
	::ReleaseDC(NULL, hDC);

	auto hMem = GlobalAlloc(GMEM_MOVEABLE, sizeof bi + vec.size());
	auto buffer = (BYTE*)GlobalLock(hMem);
	memcpy(buffer, &bi, sizeof bi);
	memcpy(buffer + sizeof bi, vec.data(), vec.size());
	GlobalUnlock(hMem);
	SetClipboardData(CF_DIB, hMem);
	return true;
}

void CEMFExplorerView::OnEditCopy()
{
	CEMFExplorerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
	{
		ASSERT(0);
		return;
	}
	auto pEMF = pDoc->GetEMFAccess();
	if (!pEMF)
	{
		ASSERT(0);
		return;
	}
	if (!OpenClipboard())
	{
		AfxMessageBox( _T("Cannot open the Clipboard") );
		return;
	}
	EmptyClipboard();

	std::unique_ptr<Gdiplus::Image> pImg(pEMF->CloneMetafile());
	
	PutBitmapToClipboard(pImg.get());

	auto pNewEMF = (Gdiplus::Metafile*)pImg.get();
	auto hEMF = pNewEMF->GetHENHMETAFILE();
	if (!SetClipboardData(CF_ENHMETAFILE, hEMF))
	{
		::DeleteEnhMetaFile(hEMF);
	}
	CloseClipboard();
}

#ifndef SHARED_HANDLERS
void CEMFExplorerView::OnZoomIn()
{
	SetZoom(true);
}

void CEMFExplorerView::OnUpdateZoomIn(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(CanDoZoom(true));
}

void CEMFExplorerView::OnZoomOut()
{
	SetZoom(false);
}

void CEMFExplorerView::OnUpdateZoomOut(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(CanDoZoom(false));
}

void CEMFExplorerView::OnZoomActualSize()
{
	SetFitToWindow(FitToNone);
}

void CEMFExplorerView::OnUpdateZoomActualSize(CCmdUI* pCmdUI)
{
	BOOL bActualSize = GetFitToWindow() == FitToNone && GetZoomFactor() == 1.0f;
	pCmdUI->SetCheck(bActualSize);
	pCmdUI->Enable(IsZoomAllowed());
}

void CEMFExplorerView::OnZoomCenter()
{
	SetCenter(!GetCenter());
}

void CEMFExplorerView::OnUpdateZoomCenter(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(GetCenter());
}

void CEMFExplorerView::OnZoomFitToWindow()
{
	SetFitToWindow(FitToBoth);
}

void CEMFExplorerView::OnUpdateZoomFitToWindow(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(GetFitToWindow() == FitToBoth);
}

void CEMFExplorerView::OnZoomFitWidth()
{
	SetFitToWindow(FitToWidth);
}

void CEMFExplorerView::OnUpdateZoomFitWidth(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(GetFitToWindow() == FitToWidth);
}

void CEMFExplorerView::OnZoomFitHeight()
{
	SetFitToWindow(FitToHeight);
}

void CEMFExplorerView::OnUpdateZoomFitHeight(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(GetFitToWindow() == FitToHeight);
}

const float zoomFactors[]{
	0.01f, 0.05f, 0.10f, 0.20f, 0.25f, 1.0f/3, 0.5f, 2.0f/3, 3.0f/4, 
	1.0f, 1.5f, 2.0f, 4.0f, 8.0f, 16.0f, 32.0f
};

void CEMFExplorerView::OnZoomPresetFactor(UINT nID)
{
	auto nIdx = nID - ID_ZOOM_1;
	SetZoomFactor(zoomFactors[nIdx]);
}

void CEMFExplorerView::OnUpdateZoomPresetFactor(CCmdUI* pCmdUI)
{
	BOOL bCheck = FALSE;
	if (GetFitToWindow() == FitToNone)
	{
		auto factor = GetZoomFactor();
		auto nIdx = pCmdUI->m_nID - ID_ZOOM_1;
		bCheck = zoomFactors[nIdx] == factor;
	}
	pCmdUI->SetRadio(bCheck);
	pCmdUI->Enable(IsZoomAllowed());
}
#endif // SHARED_HANDLERS

// CEMFExplorerView diagnostics

#ifdef _DEBUG
void CEMFExplorerView::AssertValid() const
{
	CEMFExplorerViewBase::AssertValid();
}

void CEMFExplorerView::Dump(CDumpContext& dc) const
{
	CEMFExplorerViewBase::Dump(dc);
}

CEMFExplorerDoc* CEMFExplorerView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CEMFExplorerDoc)));
	return (CEMFExplorerDoc*)m_pDocument;
}
#endif //_DEBUG


// CEMFExplorerView message handlers
