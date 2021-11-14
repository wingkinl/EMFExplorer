
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
#ifndef SHARED_HANDLERS
#include "MainFrm.h"
#endif // SHARED_HANDLERS

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CEMFExplorerView

IMPLEMENT_DYNCREATE(CEMFExplorerView, CEMFExplorerViewBase)

BEGIN_MESSAGE_MAP(CEMFExplorerView, CEMFExplorerViewBase)
#ifdef HANDLE_EXPLORER_VIEW_PAINT_WITH_DOUBLE_BUFFER
	ON_WM_ERASEBKGND()
#endif // HANDLE_EXPLORER_VIEW_PAINT_WITH_DOUBLE_BUFFER
	ON_WM_PAINT()
	ON_WM_CONTEXTMENU()
#ifndef SHARED_HANDLERS
	ON_WM_LBUTTONDOWN()
#endif // SHARED_HANDLERS
	ON_WM_RBUTTONUP()
#ifdef SHARED_HANDLERS
	ON_WM_INITMENUPOPUP()
#endif // SHARED_HANDLERS
	ON_UPDATE_COMMAND_UI(ID_FILE_NEW, &CEMFExplorerView::OnUpdateFileNew)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, &CEMFExplorerView::OnUpdateNeedDoc)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, &CEMFExplorerView::OnUpdateNeedDoc)
	ON_COMMAND(ID_EDIT_COPY, &CEMFExplorerView::OnEditCopy)
	ON_COMMAND_RANGE(ID_BACKGROUND_NONE, ID_BACKGROUND_WHITE, &CEMFExplorerView::OnViewImgBk)
	ON_UPDATE_COMMAND_UI_RANGE(ID_BACKGROUND_NONE, ID_BACKGROUND_WHITE, &CEMFExplorerView::OnUpdateViewImgBk)
#ifndef SHARED_HANDLERS
	ON_COMMAND(ID_ZOOM_ACTUALSIZE, &CEMFExplorerView::OnZoomActualSize)
	ON_UPDATE_COMMAND_UI(ID_ZOOM_ACTUALSIZE, &CEMFExplorerView::OnUpdateZoomActualSize)
	ON_COMMAND(ID_ZOOM_IN, &CEMFExplorerView::OnZoomIn)
	ON_UPDATE_COMMAND_UI(ID_ZOOM_IN, &CEMFExplorerView::OnUpdateZoomIn)
	ON_COMMAND(ID_ZOOM_OUT, &CEMFExplorerView::OnZoomOut)
	ON_UPDATE_COMMAND_UI(ID_ZOOM_OUT, &CEMFExplorerView::OnUpdateZoomOut)
	ON_COMMAND_RANGE(ID_ZOOM_1, ID_ZOOM_3200, &CEMFExplorerView::OnZoomPresetFactor)
	ON_UPDATE_COMMAND_UI_RANGE(ID_ZOOM_1, ID_ZOOM_3200, &CEMFExplorerView::OnUpdateZoomPresetFactor)
#endif // SHARED_HANDLERS
	ON_COMMAND(ID_ZOOM_CENTER, &CEMFExplorerView::OnZoomCenter)
	ON_UPDATE_COMMAND_UI(ID_ZOOM_CENTER, &CEMFExplorerView::OnUpdateZoomCenter)
	ON_COMMAND(ID_ZOOM_FITTOWINDOW, &CEMFExplorerView::OnZoomFitToWindow)
	ON_UPDATE_COMMAND_UI(ID_ZOOM_FITTOWINDOW, &CEMFExplorerView::OnUpdateZoomFitToWindow)
	ON_COMMAND(ID_ZOOM_FITWIDTH, &CEMFExplorerView::OnZoomFitWidth)
	ON_UPDATE_COMMAND_UI(ID_ZOOM_FITWIDTH, &CEMFExplorerView::OnUpdateZoomFitWidth)
	ON_COMMAND(ID_ZOOM_FITHEIGHT, &CEMFExplorerView::OnZoomFitHeight)
	ON_UPDATE_COMMAND_UI(ID_ZOOM_FITHEIGHT, &CEMFExplorerView::OnUpdateZoomFitHeight)
#ifndef SHARED_HANDLERS
	ON_UPDATE_COMMAND_UI(ID_STATUSBAR_PANE_COORD, &CEMFExplorerView::OnUpdateStatusBarCoordinates)
	ON_UPDATE_COMMAND_UI(ID_STATUSBAR_PANE_ZOOM, &CEMFExplorerView::OnUpdateStatusBarZoom)
#endif // SHARED_HANDLERS
END_MESSAGE_MAP()

// CEMFExplorerView construction/destruction

CEMFExplorerView::CEMFExplorerView() noexcept
{
	BOOL bCenter = TRUE;
#ifdef SHARED_HANDLERS
	
#else
	m_nImgBackgroundType = (ImgBackgroundType)theApp.m_nImgBackgroundType;
	bCenter = theApp.m_bViewCenter;
#endif // SHARED_HANDLERS
	SetCenter(bCenter);
}

CEMFExplorerView::~CEMFExplorerView()
{
}

BOOL CEMFExplorerView::PreCreateWindow(CREATESTRUCT& cs)
{
#ifndef HANDLE_EXPLORER_VIEW_PAINT_WITH_DOUBLE_BUFFER
	// double buffer
	cs.dwExStyle |= WS_EX_COMPOSITED;
#endif // HANDLE_EXPLORER_VIEW_PAINT_WITH_DOUBLE_BUFFER

	return CEMFExplorerViewBase::PreCreateWindow(cs);
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

void CEMFExplorerView::SetImgBackgroundType(ImgBackgroundType val)
{
	m_nImgBackgroundType = val;
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

#ifdef HANDLE_EXPLORER_VIEW_PAINT_WITH_DOUBLE_BUFFER
BOOL CEMFExplorerView::OnEraseBkgnd(CDC* /*pDC*/)
{
	return TRUE;
}
#endif // HANDLE_EXPLORER_VIEW_PAINT_WITH_DOUBLE_BUFFER

void CEMFExplorerView::OnPaint()
{
	CPaintDC dc(this);
#ifdef HANDLE_EXPLORER_VIEW_PAINT_WITH_DOUBLE_BUFFER
	CMemDC dcMem(dc, this);
	CDC* pDCDraw = &dcMem.GetDC();
#endif // HANDLE_EXPLORER_VIEW_PAINT_WITH_DOUBLE_BUFFER
	CDC* pDCDraw = &dc;

	CRect rcClient;
	GetClientRect(&rcClient);

	COLORREF crfBk = GetSysColor(COLOR_WINDOW);
#ifndef SHARED_HANDLERS
	if (theApp.IsDarkTheme())
		crfBk = theApp.m_crfDarkThemeBkColor;
#endif // SHARED_HANDLERS
	pDCDraw->FillSolidRect(rcClient, crfBk);

	if (m_bInitialRedraw)
		return;

	CEMFExplorerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	auto pEMF = pDoc->GetEMFAccess();
	if (!pEMF)
		return;

	CRect rcImg = GetScrolledZoomedView();

	if (GetImgBackgroundType() != ImgBackgroundTypeNone)
	{
		CRect rcGrid(rcImg);
		if (rcGrid.left < rcClient.left)
			rcGrid.left = rcClient.left;
		if (rcGrid.top < rcClient.top)
			rcGrid.top = rcClient.top;
		if (rcGrid.right > rcClient.right)
			rcGrid.right = rcClient.right;
		if (rcGrid.bottom > rcClient.bottom)
			rcGrid.bottom = rcClient.bottom;
		switch (GetImgBackgroundType())
		{
		case ImgBackgroundTypeTransparentGrid:
			DrawTransparentGrid(pDCDraw, rcGrid);
			break;
		case ImgBackgroundTypeWhite:
			pDCDraw->FillSolidRect(rcGrid, RGB(255,255,255));
			break;
		}
	}

#ifndef SHARED_HANDLERS
	size_t nDrawToRecord = GetDrawToRecordIndex();
#endif // SHARED_HANDLERS

	Gdiplus::Graphics gg(pDCDraw->GetSafeHdc());
	gg.SetCompositingQuality(Gdiplus::CompositingQualityHighQuality);
	gg.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
	gg.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias8x8);
	gg.SetTextRenderingHint(Gdiplus::TextRenderingHintClearTypeGridFit);

#ifndef SHARED_HANDLERS
	if (nDrawToRecord != (size_t)-1)
	{
		pEMF->DrawMetafileUntilRecord(gg, rcImg, nDrawToRecord);
	}
	else
#endif // SHARED_HANDLERS
	{
		pEMF->DrawMetafile(gg, rcImg);
	}
}

#ifndef SHARED_HANDLERS
void CEMFExplorerView::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (nFlags & MK_CONTROL)
	{
		CEMFExplorerDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		if (pDoc)
		{
			auto pEMF = pDoc->GetEMFAccess();
			if (pEMF)
			{
				ClientToView(point);
				auto pRec = pEMF->HitTest(point);
				if (pRec)
				{
					auto idx = pRec->GetIndex();
					auto pFrameWnd = DYNAMIC_DOWNCAST(CMainFrame, GetParent());
					pFrameWnd->SelectRecord(idx);
					return;
				}
			}
		}
	}
	CEMFExplorerViewBase::OnLButtonDown(nFlags, point);
}
#endif // SHARED_HANDLERS

void CEMFExplorerView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CEMFExplorerView::OnViewImgBk(UINT nID)
{
	auto nType = nID - ID_BACKGROUND_NONE;
#ifndef SHARED_HANDLERS
	if (!IsSubEMFView())
		theApp.m_nImgBackgroundType = nType;
#endif // SHARED_HANDLERS
	SetImgBackgroundType((CEMFExplorerView::ImgBackgroundType)nType);
}

void CEMFExplorerView::OnUpdateViewImgBk(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(GetImgBackgroundType() == pCmdUI->m_nID - ID_BACKGROUND_NONE);
}

void CEMFExplorerView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifdef SHARED_HANDLERS
	CMenu menu;
	menu.LoadMenu(IDR_POPUP_CTXT);
	CMenu* pCtxtMenu = menu.GetSubMenu(0);
	pCtxtMenu->TrackPopupMenu(0, point.x, point.y, this);
#else
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

#ifndef SHARED_HANDLERS
void CEMFExplorerView::OnAfterUpdateZoomedViewSize()
{
	if (!GetSafeHwnd())
		return;
	auto pFrameWnd = DYNAMIC_DOWNCAST(CMainFrame, GetParent());
	pFrameWnd->SendMessage(MainFrameMsgViewUpdateSizeScroll, TRUE);
}

size_t CEMFExplorerView::GetDrawToRecordIndex() const
{
	auto pFrameWnd = DYNAMIC_DOWNCAST(CMainFrame, GetParent());
	return pFrameWnd->GetDrawToRecordIndex();
}

BOOL CEMFExplorerView::OnScrollBy(CSize sizeScroll, BOOL bDoScroll)
{
	BOOL bRet = CEMFExplorerViewBase::OnScrollBy(sizeScroll, bDoScroll);
	auto pFrameWnd = DYNAMIC_DOWNCAST(CMainFrame, GetParent());
	pFrameWnd->SendMessage(MainFrameMsgViewUpdateSizeScroll, FALSE);
	return bRet;
}
#endif // SHARED_HANDLERS

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

bool CEMFExplorerView::IsSubEMFView() const
{
#ifdef SHARED_HANDLERS
	return false;
#else
	auto pFrameWnd = DYNAMIC_DOWNCAST(CMainFrame, GetParent());
	return pFrameWnd && pFrameWnd->IsSubEMFFrame();
#endif // SHARED_HANDLERS
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

void CEMFExplorerView::OnZoomCenter()
{
	BOOL bCenter = !GetCenter();
	SetCenter(bCenter);
#ifndef SHARED_HANDLERS
	if (!IsSubEMFView())
		theApp.m_bViewCenter = bCenter;
#endif // SHARED_HANDLERS
}

void CEMFExplorerView::OnUpdateZoomCenter(CCmdUI* pCmdUI)
{
	BOOL bCenter = GetCenter();
#ifndef SHARED_HANDLERS
	if (!IsSubEMFView())
		bCenter = theApp.m_bViewCenter;
#endif // SHARED_HANDLERS
	pCmdUI->SetCheck(bCenter);
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

COLORREF CEMFExplorerView::GetCursorColor() const
{
	BOOL bValid = HasValidEMFInDoc();
	if (bValid)
	{
		CPoint pos;
		GetCursorPos(&pos);
		ScreenToClient(&pos);
		CClientDC dc(const_cast<CEMFExplorerView*>(this));
		auto clr = dc.GetPixel(pos);
		return clr;
	}
	return 0;
}

#ifndef SHARED_HANDLERS
void CEMFExplorerView::OnUpdateStatusBarCoordinates(CCmdUI* pCmdUI)
{
	BOOL bValid = HasValidEMFInDoc();
	pCmdUI->Enable(bValid);
	CString str;
	if (bValid)
	{
		CPoint pos;
		GetCursorPos(&pos);
		ScreenToClient(&pos);
		ClientToView(pos);
 		str.Format(_T(" (%d, %d) "), pos.x, pos.y);
	}
	else
	{
		str.LoadString(ID_STATUSBAR_PANE_COORD);
	}
	pCmdUI->SetText(str);
}

void CEMFExplorerView::OnUpdateStatusBarZoom(CCmdUI* pCmdUI)
{
	BOOL bValid = HasValidEMFInDoc();
	pCmdUI->Enable(bValid);
	CString str;
	if (bValid)
	{
		float factor = GetRealZoomFactor();
		str.Format(_T("%d%%"), (int)(factor*100));
		pCmdUI->SetText(str);
	}
	else
	{
		str.LoadString(ID_STATUSBAR_PANE_ZOOM);
	}
	pCmdUI->SetText(str);
}
#endif

#ifdef SHARED_HANDLERS
void CEMFExplorerView::OnInitMenuPopup(CMenu* popup, UINT nIndex, BOOL bSysMenu)
{
	if(popup && !bSysMenu)
	{
		CCmdUI state;
		state.m_pMenu = popup;
		state.m_nIndexMax = popup->GetMenuItemCount();
		for(UINT i = 0; i < state.m_nIndexMax; i++)
		{
			state.m_nIndex = i;
			state.m_nID = popup->GetMenuItemID(i);
			state.DoUpdate(this, FALSE);
		}
	}
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
