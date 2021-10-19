
#pragma once

#include "EMFRecAccess.h"
#include "EMFRecListCtrl.h"

class CFileViewToolBar : public CMFCToolBar
{
	virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)
	{
		CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
	}

	virtual BOOL AllowShowOnList() const { return FALSE; }
};

class CFileView : public CDockablePane
{
// Construction
public:
	CFileView() noexcept;
public:
	void AdjustLayout();
	void OnChangeVisualStyle();
public:
	void SetEMFAccess(std::shared_ptr<EMFAccess> emf);
// Attributes
protected:

	CEMFRecListCtrl m_wndRecList;
	CImageList m_FileViewImages;
	CFileViewToolBar m_wndToolBar;

protected:

// Implementation
public:
	virtual ~CFileView();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnProperties();
	afx_msg void OnEditCopy();
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);

	DECLARE_MESSAGE_MAP()
};

