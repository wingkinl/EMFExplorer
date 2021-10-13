
// EMFExplorerView.h : interface of the CEMFExplorerView class
//

#pragma once


class CEMFExplorerView : public CView
{
protected: // create from serialization only
	CEMFExplorerView() noexcept;
	DECLARE_DYNCREATE(CEMFExplorerView)

// Attributes
public:
	CEMFExplorerDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	BOOL CheckClipboardForEMF() const;

	BOOL HasValidEMFInDoc() const;
// Implementation
public:
	virtual ~CEMFExplorerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnUpdateFileNew(CCmdUI* pCmdUI);
	afx_msg void OnUpdateNeedDoc(CCmdUI* pCmdUI);
	afx_msg void OnEditPaste();
	afx_msg void OnUpdateNeedClip(CCmdUI* pCmdUI);
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in EMFExplorerView.cpp
inline CEMFExplorerDoc* CEMFExplorerView::GetDocument() const
   { return reinterpret_cast<CEMFExplorerDoc*>(m_pDocument); }
#endif

