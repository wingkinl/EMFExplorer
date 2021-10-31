
#pragma once
#include <memory>
#include "EMFRecAccess.h"

/////////////////////////////////////////////////////////////////////////////
// CEMFRecListCtrl window

using CEMFRecListCtrlBase = CMFCListCtrl;

using CEMFRecTooltipCtrlBase = CMFCToolTipCtrl;

class CEMFRecTooltipCtrl : public CEMFRecTooltipCtrlBase
{
public:
	CEMFRecTooltipCtrl();
private:
	CSize OnDrawDescription(CDC* pDC, CRect rect, BOOL bCalcOnly) override;
private:
	afx_msg void OnPop(NMHDR* pNMHDR, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()
private:
	friend class CEMFRecListCtrl;
	EMFRecAccess* m_pRec = nullptr;
};

class CEMFRecListCtrl : public CEMFRecListCtrlBase
{
// Construction
public:
	CEMFRecListCtrl() noexcept;
public:
	BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);

	void OnChangeVisualStyle();

	void SetEMFAccess(std::shared_ptr<EMFAccess> emf);

	void LoadEMFDataEvent(bool bBefore);

	inline EMFRecAccess* GetEMFRecord(int nRow) const;

	int GetCurSelRecIndex(BOOL bHottrack = FALSE) const;

	void SetCurSelRecIndex(int index);
// Overrides
protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);

	void OnDrawItem(LPNMLVCUSTOMDRAW lplvcd) const;

	BOOL IsDarkTheme() const;

	void AdjustColumnWidth(int cx);

	void SetCustomHotItem(int nItem);

	// We don't want sort
	void Sort(int iColumn, BOOL bAscending = TRUE, BOOL bAdd = FALSE) override {};

	BOOL PreTranslateMessage(MSG* pMsg) override;
// Implementation
public:
	virtual ~CEMFRecListCtrl();
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnGetDispInfo(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnItemChange(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndScroll(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg UINT OnGetDlgCode();
protected:
	COLORREF m_crfDefaultBkColor = (COLORREF)-1;
	std::shared_ptr<EMFAccess>	m_emf;
	CEMFRecTooltipCtrl			m_ToolTip;
	int							m_nTipItem = -1;
	int							m_nHotItem = -1;
protected:
	DECLARE_MESSAGE_MAP()
};
