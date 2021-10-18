
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
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_UPDATE_COMMAND_UI(ID_FILE_NEW, &CEMFExplorerView::OnUpdateFileNew)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, &CEMFExplorerView::OnUpdateNeedDoc)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, &CEMFExplorerView::OnUpdateNeedDoc)
	ON_COMMAND(ID_EDIT_PASTE, &CEMFExplorerView::OnEditPaste)
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
	cs.dwExStyle |= WS_EX_COMPOSITED;

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

BOOL CEMFExplorerView::OnEraseBkgnd(CDC* /*pDC*/)
{
	return TRUE;
}

void CEMFExplorerView::OnPaint()
{
	CPaintDC dc(this);
	//CMemDC dcMem(dc, this);
	//CDC* pDCDraw = &dcMem.GetDC();
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
	CSize szImage = GetViewSize();
	float factor = GetRealZoomFactor();
	CSize szImageScaled;
	szImageScaled.cx = (int)(szImage.cx * factor);
	szImageScaled.cy = (int)(szImage.cy * factor);
	CPoint ptImg(0, 0);
	ptImg = -GetDeviceScrollPosition();

	ptImg.x = (int)(ptImg.x * factor);
	ptImg.y = (int)(ptImg.y * factor);

	if (GetCenter())
	{
		if (szImageScaled.cx < rect.Width())
			ptImg.x = (rect.Width() - szImageScaled.cx) / 2;
		if (szImageScaled.cy < rect.Height())
			ptImg.y = (rect.Height() - szImageScaled.cy) / 2;
	}

	CRect rcImg(ptImg, szImageScaled);

	if (GetShowTransparentGrid())
	{
		DrawTransparentGrid(pDCDraw, rcImg);
	}

	Gdiplus::Graphics gg(pDCDraw->GetSafeHdc());
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

void CEMFExplorerView::OnEditPaste()
{
	CEMFExplorerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	if (!OpenClipboard())
	{
		AfxMessageBox(_T("Failed to open clipboard!"), MB_ICONERROR);
		return;
	}
	std::vector<BYTE> vBuffer;
	BOOL bOK = FALSE;
	auto hEMF = (HENHMETAFILE)GetClipboardData(CF_ENHMETAFILE);
	if (hEMF)
	{
		UINT nSize = GetEnhMetaFileBits(hEMF, 0, nullptr);
		vBuffer.resize((size_t)nSize);
		auto nRead = GetEnhMetaFileBits(hEMF, nSize, vBuffer.data());
		bOK = nRead == nSize;
	}
	else
	{
		AfxMessageBox(_T("Not an EMF format!"), MB_ICONERROR);
	}
	CloseClipboard();
	if (bOK)
	{
		pDoc->UpdateEMFData(vBuffer);
		auto pEMF = pDoc->GetEMFAccess();
		if (pEMF)
		{
			auto hdr = pEMF->GetMetafileHeader();
			SetScrollSizes(MM_ISOTROPIC, {hdr.Width, hdr.Height});
		}
	}
}

void CEMFExplorerView::OnUpdateNeedClip(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(CheckClipboardForEMF());
}

#ifndef SHARED_HANDLERS
void CEMFExplorerView::OnZoomIn()
{
	m_nLastSelPresetFactorID = 0;
	SetZoom(true);
}

void CEMFExplorerView::OnUpdateZoomIn(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(CanDoZoom(true));
}

void CEMFExplorerView::OnZoomOut()
{
	m_nLastSelPresetFactorID = 0;
	SetZoom(false);
}

void CEMFExplorerView::OnUpdateZoomOut(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(CanDoZoom(false));
}

void CEMFExplorerView::OnZoomActualSize()
{
	m_nLastSelPresetFactorID = 0;
	SetFitToWindow(FitToNone);
}

void CEMFExplorerView::OnUpdateZoomActualSize(CCmdUI* pCmdUI)
{
	BOOL bActualSize = GetFitToWindow() == FitToNone && GetZoomFactor() == 1.0f;
	pCmdUI->SetCheck(bActualSize);
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
	m_nLastSelPresetFactorID = 0;
	SetFitToWindow(FitToBoth);
}

void CEMFExplorerView::OnUpdateZoomFitToWindow(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(GetFitToWindow() == FitToBoth);
}

void CEMFExplorerView::OnZoomFitWidth()
{
	m_nLastSelPresetFactorID = 0;
	SetFitToWindow(FitToWidth);
}

void CEMFExplorerView::OnUpdateZoomFitWidth(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(GetFitToWindow() == FitToWidth);
}

void CEMFExplorerView::OnZoomFitHeight()
{
	m_nLastSelPresetFactorID = 0;
	SetFitToWindow(FitToHeight);
}

void CEMFExplorerView::OnUpdateZoomFitHeight(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(GetFitToWindow() == FitToHeight);
}

void CEMFExplorerView::OnZoomPresetFactor(UINT nID)
{
	const float factors[]{
		0.01f, 0.05f, 0.10f, 0.20f, 0.25f, 1.0f/3, 0.5f, 2.0f/3, 3.0f/4, 
		1.0f, 1.5f, 2.0f, 4.0f, 8.0f, 16.0f, 32.0f
	};
	m_nLastSelPresetFactorID = nID;
	auto nIdx = nID - ID_ZOOM_1;
	SetZoomFactor(factors[nIdx]);
}

void CEMFExplorerView::OnUpdateZoomPresetFactor(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(m_nLastSelPresetFactorID == pCmdUI->m_nID);
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
