
#pragma once

#include "EMFRecAccess.h"
#include "EMFRecListCtrl.h"

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

	void EnableHoverNotification(BOOL enable);
private:
	bool CanViewCurSelRecord() const;
// Attributes
protected:

	CEMFRecListCtrl m_wndRecList;
	CComboBox		m_wndFindCombo;
	CFont			m_fntFindCombo;
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

