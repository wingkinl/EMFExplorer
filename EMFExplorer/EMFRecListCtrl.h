
#pragma once

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
protected:
	COLORREF m_crfDefaultBkColor = (COLORREF)-1;
protected:
	DECLARE_MESSAGE_MAP()
};
