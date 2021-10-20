
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
// Overrides
protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);

	COLORREF OnGetCellTextColor(int nRow, int nColum) override;
	COLORREF OnGetCellBkColor(int nRow, int nColum) override;
// Implementation
public:
	virtual ~CEMFRecListCtrl();
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetDispInfo(NMHDR* pNMHDR, LRESULT* pResult);
protected:
	COLORREF m_crfDefaultBkColor = (COLORREF)-1;
	std::shared_ptr<EMFAccess>	m_emf;
protected:
	DECLARE_MESSAGE_MAP()
};
