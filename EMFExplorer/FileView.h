
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

	void LoadEMFDataEvent(bool bBefore);

	int GetCurSelRecIndex(BOOL bHottrack = FALSE) const;

	void SetCurSelRecIndex(int index);
private:
	bool CanViewCurSelRecord() const;
// Attributes
protected:

	CEMFRecListCtrl m_wndRecList;
	CFileViewToolBar m_wndToolBar;

protected:

// Implementation
public:
	virtual ~CFileView();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnListItemChange(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnListHotTrack(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnListEnter(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnListDblClk(NMHDR* pNMHDR, LRESULT* pResult);

	DECLARE_MESSAGE_MAP()
};

