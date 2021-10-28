
#pragma once
#include <memory>
#include "EMFRecAccess.h"

/////////////////////////////////////////////////////////////////////////////
// CEMFRecListCtrl window

using CEMFRecListCtrlBase = CMFCListCtrl;

class CEMFRecListCtrl : public CEMFRecListCtrlBase
{
// Construction
public:
	CEMFRecListCtrl() noexcept;
public:
	void OnChangeVisualStyle();

	void SetEMFAccess(std::shared_ptr<EMFAccess> emf);

	void LoadEMFDataEvent(bool bBefore);

	inline EMFRecAccess* GetEMFRecord(int nRow) const;

// Overrides
protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);

	void OnPreDrawSubItem(LPNMLVCUSTOMDRAW lplvcd) const;

	BOOL IsDarkTheme() const;

	void AdjustColumnWidth(int cx);

	// We don't want sort
	void Sort(int iColumn, BOOL bAscending = TRUE, BOOL bAdd = FALSE) override {};
// Implementation
public:
	virtual ~CEMFRecListCtrl();
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnGetDispInfo(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnItemChange(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndScroll(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg UINT OnGetDlgCode();
protected:
	COLORREF m_crfDefaultBkColor = (COLORREF)-1;
	std::shared_ptr<EMFAccess>	m_emf;
protected:
	DECLARE_MESSAGE_MAP()
};
