
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
END_MESSAGE_MAP()

// CEMFExplorerView construction/destruction

CEMFExplorerView::CEMFExplorerView() noexcept
{
	// TODO: add construction code here

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

// CEMFExplorerView drawing

void CEMFExplorerView::OnDraw(CDC* /*pDC*/)
{
	CEMFExplorerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
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
