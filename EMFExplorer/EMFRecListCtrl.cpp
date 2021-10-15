
#include "pch.h"
#include "framework.h"
#include "EMFRecListCtrl.h"
#ifndef SHARED_HANDLERS
#include "EMFExplorer.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

LRESULT _EnableWindowTheme(HWND hwnd, LPCWSTR classList, LPCWSTR subApp, LPCWSTR idlist)
{
	LRESULT lResult = SetWindowTheme(hwnd, subApp, idlist);
	return lResult;
}

bool _IsThemeEnabled()
{
	return IsAppThemed() && IsThemeActive();
}

LRESULT		_EnableExplorerVisualStyles(HWND hWnd, LPCWSTR classList, BOOL bEnable)
{
	if ( !_IsThemeEnabled() )
	{
		return S_FALSE;
	}

	LRESULT rc = S_FALSE;
	if (bEnable)
		rc = _EnableWindowTheme(hWnd, classList, L"Explorer", NULL);
	else
		rc = _EnableWindowTheme(hWnd, L"", L"", NULL);

	return rc;
}

LRESULT		_EnableListCtrlExplorerVisualStyles(HWND hWnd, BOOL bEnable)
{
	return _EnableExplorerVisualStyles(hWnd, L"ListView", bEnable);
}

/////////////////////////////////////////////////////////////////////////////
// CEMFRecListCtrl

CEMFRecListCtrl::CEMFRecListCtrl() noexcept
{
}

CEMFRecListCtrl::~CEMFRecListCtrl()
{
}

BEGIN_MESSAGE_MAP(CEMFRecListCtrl, CEMFRecListCtrlBase)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEMFRecListCtrl message handlers

BOOL CEMFRecListCtrl::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	BOOL bRes = CEMFRecListCtrlBase::OnNotify(wParam, lParam, pResult);

	NMHDR* pNMHDR = (NMHDR*)lParam;
	ASSERT(pNMHDR != nullptr);

#pragma warning(suppress : 26454)
	if (pNMHDR && pNMHDR->code == TTN_SHOW && GetToolTips() != nullptr)
	{
		GetToolTips()->SetWindowPos(&wndTop, -1, -1, -1, -1, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOSIZE);
	}

	return bRes;
}

int CEMFRecListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	int nRet = CEMFRecListCtrlBase::OnCreate(lpCreateStruct);
	if (nRet == 0)
	{
		DWORD dwExStyle = GetExtendedStyle();
		dwExStyle |= LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER;
		SetExtendedStyle(dwExStyle);

		m_crfDefaultBkColor = GetBkColor();
		if (theApp.IsDarkTheme())
			SetBkColor(theApp.m_crfDarkThemeBkColor);

		_EnableListCtrlExplorerVisualStyles(GetSafeHwnd(), TRUE);
	}
	return nRet;
}

void CEMFRecListCtrl::OnSize(UINT nType, int cx, int cy)
{
	SetColumnWidth(0, cx);
	CEMFRecListCtrlBase::OnSize(nType, cx, cy);
}

COLORREF CEMFRecListCtrl::OnGetCellTextColor(int nRow, int nColum)
{
	if (theApp.IsDarkTheme())
		return theApp.m_crfDarkThemeTxtColor;
	return CEMFRecListCtrlBase::OnGetCellTextColor(nRow, nColum);
}

COLORREF CEMFRecListCtrl::OnGetCellBkColor(int nRow, int nColum)
{
	if (theApp.IsDarkTheme())
		return theApp.m_crfDarkThemeBkColor;
	return CEMFRecListCtrlBase::OnGetCellBkColor(nRow, nColum);
}

void CEMFRecListCtrl::OnChangeVisualStyle()
{
	BOOL bDark = theApp.IsDarkTheme();
	SetBkColor(bDark ? theApp.m_crfDarkThemeBkColor : m_crfDefaultBkColor);
}

