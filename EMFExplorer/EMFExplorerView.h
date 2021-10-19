
// EMFExplorerView.h : interface of the CEMFExplorerView class
//

#pragma once

#include "ScrollZoomView.h"

using CEMFExplorerViewBase = CScrollZoomView;

// It seems that WS_EX_COMPOSITED is enough
//#define HANDLE_PAINT_WITH_DOUBLE_BUFFER

class CEMFExplorerView : public CEMFExplorerViewBase
{
protected: // create from serialization only
	CEMFExplorerView() noexcept;
	DECLARE_DYNCREATE(CEMFExplorerView)

// Attributes
public:
	CEMFExplorerDoc* GetDocument() const;

// Operations
public:
	void LoadEMFDataEvent(bool bBefore);
// Overrides
public:
	CSize GetViewSize() const override;

	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
public:
	bool GetShowTransparentGrid() const { return m_bShowTransparentGrid; }
	void SetShowTransparentGrid(bool val);
protected:
	BOOL CheckClipboardForEMF() const;

	BOOL HasValidEMFInDoc() const;

	void DrawTransparentGrid(CDC* pDC, const CRect& rect);

	bool IsZoomAllowed() const override;

	bool PutBitmapToClipboard(Gdiplus::Image* pImg);
// Implementation
public:
	virtual ~CEMFExplorerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	bool	m_bShowTransparentGrid = true;
// Generated message map functions
protected:
#ifdef HANDLE_PAINT_WITH_DOUBLE_BUFFER
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
#endif // HANDLE_PAINT_WITH_DOUBLE_BUFFER
	afx_msg void OnPaint();
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnUpdateFileNew(CCmdUI* pCmdUI);
	afx_msg void OnUpdateNeedDoc(CCmdUI* pCmdUI);
	afx_msg void OnUpdateNeedClip(CCmdUI* pCmdUI);
	afx_msg void OnEditCopy();

#ifndef SHARED_HANDLERS
	afx_msg void OnZoomIn();
	afx_msg void OnUpdateZoomIn(CCmdUI* pCmdUI);
	afx_msg void OnZoomOut();
	afx_msg void OnUpdateZoomOut(CCmdUI* pCmdUI);
	afx_msg void OnZoomActualSize();
	afx_msg void OnUpdateZoomActualSize(CCmdUI* pCmdUI);
	afx_msg void OnZoomCenter();
	afx_msg void OnUpdateZoomCenter(CCmdUI* pCmdUI);
	afx_msg void OnZoomFitToWindow();
	afx_msg void OnUpdateZoomFitToWindow(CCmdUI* pCmdUI);
	afx_msg void OnZoomFitWidth();
	afx_msg void OnUpdateZoomFitWidth(CCmdUI* pCmdUI);
	afx_msg void OnZoomFitHeight();
	afx_msg void OnUpdateZoomFitHeight(CCmdUI* pCmdUI);
	afx_msg void OnZoomPresetFactor(UINT nID);
	afx_msg void OnUpdateZoomPresetFactor(CCmdUI* pCmdUI);
#endif
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in EMFExplorerView.cpp
inline CEMFExplorerDoc* CEMFExplorerView::GetDocument() const
   { return reinterpret_cast<CEMFExplorerDoc*>(m_pDocument); }
#endif

