
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
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, &CEMFRecListCtrl::OnCustomDraw)
	ON_NOTIFY_REFLECT(LVN_GETDISPINFO, &CEMFRecListCtrl::OnGetDispInfo)
	ON_NOTIFY_REFLECT_EX(LVN_ITEMCHANGED, &CEMFRecListCtrl::OnItemChange)
	ON_WM_GETDLGCODE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEMFRecListCtrl message handlers

enum ColumnType
{
	ColumnTypeIndex,
	ColumnTypeName,
};

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

		InsertColumn(ColumnTypeIndex, _T("Index"));
		InsertColumn(ColumnTypeName, _T("Record Name"));

		_EnableListCtrlExplorerVisualStyles(GetSafeHwnd(), TRUE);
	}
	return nRet;
}

void CEMFRecListCtrl::OnSize(UINT nType, int cx, int cy)
{
	SetColumnWidth(ColumnTypeIndex, LVSCW_AUTOSIZE);
	int cxIdx = GetColumnWidth(ColumnTypeIndex);
	SetColumnWidth(ColumnTypeName, cx - cxIdx);
	CEMFRecListCtrlBase::OnSize(nType, cx, cy);
}

void CEMFRecListCtrl::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult)
{
	ENSURE(pNMHDR != NULL);
	LPNMLVCUSTOMDRAW lplvcd = (LPNMLVCUSTOMDRAW)pNMHDR;

	switch (lplvcd->nmcd.dwDrawStage)
	{
	case CDDS_PREPAINT:
		*pResult = CDRF_NOTIFYITEMDRAW;
		break;

	case CDDS_ITEMPREPAINT:
		*pResult = CDRF_NOTIFYSUBITEMDRAW;
		break;

	case CDDS_ITEMPREPAINT | CDDS_SUBITEM:
		OnPreDrawSubItem(lplvcd);
		*pResult = CDRF_DODEFAULT;
		break;
	}
}

void CEMFRecListCtrl::OnGetDispInfo(NMHDR* pNMHDR, LRESULT* pResult)
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	ASSERT(pDispInfo);
	LVITEM& item = pDispInfo->item;

	if (item.mask & LVIF_TEXT)
	{
		// See document for LVITEM:
		// the list-view control allows any length string to be stored as item text, only the first 260 TCHARs are displayed.
		// so better truncate the text here instead of leaving junk/gibberish string
		auto pRec = GetEMFRecord(item.iItem);
		switch (item.iSubItem)
		{
		case ColumnTypeIndex:
			_sntprintf_s(item.pszText, item.cchTextMax, item.cchTextMax, _T("%ld"), item.iItem+1);
			break;
		case ColumnTypeName:
			{
				auto szText = pRec->GetRecordName();
				_tcsncpy_s(item.pszText, item.cchTextMax, szText, item.cchTextMax);
			}
			break;
		}
	}

	if (item.mask & LVIF_IMAGE)
	{
		//item.iImage = GetItemIconIndex(item.iItem);
	}

	*pResult = 0;
}

BOOL CEMFRecListCtrl::OnItemChange(NMHDR* pNMHDR, LRESULT* pResult)
{
	Invalidate();
	// Returns FALSE here so we allow parent to handle this too
	return FALSE;
}

UINT CEMFRecListCtrl::OnGetDlgCode()
{
	// Add DLGC_WANTMESSAGE so we have NM_RETURN notification
	return CEMFRecListCtrlBase::OnGetDlgCode() | DLGC_WANTMESSAGE;
}

void CEMFRecListCtrl::OnPreDrawSubItem(LPNMLVCUSTOMDRAW lplvcd) const
{
	int nColumn = lplvcd->iSubItem;
	int nRow = (int) lplvcd->nmcd.dwItemSpec;

	lplvcd->clrText = IsDarkTheme() ? theApp.m_crfDarkThemeTxtColor : GetTextColor();
	
	auto pRec = GetEMFRecord(nRow);
	if (nColumn == ColumnTypeIndex)
	{
		int nSel = GetNextItem(-1, LVNI_SELECTED);
		if (nSel >= 0 && nSel != nRow)
		{
			auto pRecSel = GetEMFRecord(nSel);
			if (pRec->IsLinked(pRecSel))
			{
				lplvcd->clrTextBk = RGB(255, 150, 50);
				lplvcd->clrText = RGB(0, 0, 0);
				return;
			}
		}
	}
	if (pRec->IsDrawingRecord())
		lplvcd->clrTextBk = theApp.IsDarkTheme() ? RGB(3, 136, 87) : RGB(4, 170, 109);
	else
		lplvcd->clrTextBk = GetBkColor();
}

BOOL CEMFRecListCtrl::IsDarkTheme() const
{
#ifdef SHARED_HANDLERS
	return TRUE;
#else
	return theApp.IsDarkTheme();
#endif
}

void CEMFRecListCtrl::SetEMFAccess(std::shared_ptr<EMFAccess> emf)
{
	m_emf = emf;
}

void CEMFRecListCtrl::LoadEMFDataEvent(bool bBefore)
{
	if (bBefore)
	{
		SetRedraw(FALSE);
		m_emf = nullptr;
	}
	else
	{
		int nCount = m_emf ? (int)m_emf->GetRecordCount() : 0;
		SetItemCount(nCount);
		// EnsureVisible helps fix the repaint issue when the list control was previously scrolled
		EnsureVisible(0, FALSE);
		Invalidate();
		SetRedraw(TRUE);
	}
}

EMFRecAccess* CEMFRecListCtrl::GetEMFRecord(int nRow) const
{
	auto pRec = m_emf->GetRecord(nRow);
	return pRec;
}

void CEMFRecListCtrl::OnChangeVisualStyle()
{
	BOOL bDark = theApp.IsDarkTheme();
	SetBkColor(bDark ? theApp.m_crfDarkThemeBkColor : m_crfDefaultBkColor);
}


