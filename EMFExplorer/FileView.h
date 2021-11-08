
#pragma once

#include "EMFRecAccess.h"
#include "EMFRecListCtrl.h"

using CSearchComboBoxEditBase = CMFCEditBrowseCtrl;

class CSearchComboBoxEdit : public CSearchComboBoxEditBase
{
public:
	void OnBrowse() override;

	void OnDrawBrowseButton(CDC* pDC, CRect rect, BOOL bIsButtonPressed, BOOL bIsButtonHot) override;

	void Init();
protected:
	void OnAfterUpdate() override;

	BOOL PreTranslateMessage(MSG* pMsg) override;
protected:
	afx_msg BOOL OnEnChange();

	DECLARE_MESSAGE_MAP()
};

using CFindComboBoxBase = CComboBox;

class CFindComboBox : public CFindComboBoxBase
{
public:
	CFindComboBox();
	~CFindComboBox();
public:
	BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID) override;

	BOOL	m_bSearchOK = TRUE;

	void OnAfterClearText();
private:
	CSearchComboBoxEdit	m_edit;
	CBrush	m_brErr;
private:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
private:
	DECLARE_MESSAGE_MAP()
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

	void EnableHoverNotification(BOOL enable);
private:
	bool CanViewCurSelRecord() const;
// Attributes
protected:

	CEMFRecListCtrl m_wndRecList;
	CFindComboBox	m_wndFindCombo;
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
	afx_msg void OnFindEditChange();
	afx_msg void OnFindRecord();

	DECLARE_MESSAGE_MAP()
};

