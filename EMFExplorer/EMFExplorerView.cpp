
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

IMPLEMENT_DYNCREATE(CEMFExplorerView, CView)

BEGIN_MESSAGE_MAP(CEMFExplorerView, CView)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_UPDATE_COMMAND_UI(ID_FILE_NEW, &CEMFExplorerView::OnUpdateFileNew)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, &CEMFExplorerView::OnUpdateNeedDoc)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, &CEMFExplorerView::OnUpdateNeedDoc)
	ON_COMMAND(ID_EDIT_PASTE, &CEMFExplorerView::OnEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, &CEMFExplorerView::OnUpdateNeedClip)
END_MESSAGE_MAP()

// CEMFExplorerView construction/destruction

CEMFExplorerView::CEMFExplorerView() noexcept
{
	
}

CEMFExplorerView::~CEMFExplorerView()
{
}

BOOL CEMFExplorerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
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

void CEMFExplorerView::OnDraw(CDC* pDC)
{
	CEMFExplorerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	CRect rect;
	GetClientRect(&rect);
	
	COLORREF crfBk = GetSysColor(COLOR_WINDOW);
#ifndef SHARED_HANDLERS
	if (theApp.m_bBackgroundDark)
		crfBk = theApp.m_crfBkColorDark;
#endif // SHARED_HANDLERS
	pDC->FillSolidRect(rect, crfBk);


// 	pDC->FillSolidRect(rect, GetSysColor(COLOR_WINDOW));
// 
// 	int nDPI = pDC->GetDeviceCaps(LOGPIXELSX);
// 	double dScale = (double)nDPI / 96;
// 	const int nGrid = (int)dScale * 8;
// 	const int nGrid2 = nGrid * 2;
// 	for (LONG yy = rect.top, row = 0; yy < rect.bottom; yy += nGrid, ++row)
// 	{
// 		for (LONG xx = rect.left+((row&1)?0:nGrid); xx < rect.right; xx += nGrid2)
// 		{
// 			pDC->FillSolidRect(xx, yy, nGrid, nGrid, RGB(0xcc, 0xcc, 0xcc));
// 		}
// 	}
// 
// 	EMFAccess* pEMF = pDoc->GetEMFAccess();
// 	if (pEMF)
// 	{
// 		CRect rcDraw = pEMF->GetFittingDrawRect(rect);
// 		Gdiplus::Graphics gg(pDC->GetSafeHdc());
// 		pEMF->DrawMetafile(gg, rcDraw);
// 	}
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

}

void CEMFExplorerView::OnUpdateNeedClip(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(CheckClipboardForEMF());
}

// CEMFExplorerView diagnostics

#ifdef _DEBUG
void CEMFExplorerView::AssertValid() const
{
	CView::AssertValid();
}

void CEMFExplorerView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CEMFExplorerDoc* CEMFExplorerView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CEMFExplorerDoc)));
	return (CEMFExplorerDoc*)m_pDocument;
}
#endif //_DEBUG


// CEMFExplorerView message handlers
