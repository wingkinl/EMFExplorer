
#include "pch.h"
#include "framework.h"
#include "EMFRecListCtrl.h"
#include "EMFExplorer.h"
#include "EMFAccess.h"

#undef min
#undef max

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

CEMFRecTooltipCtrl::CEMFRecTooltipCtrl()
{
	m_Params.m_bBoldLabel = TRUE;
	m_Params.m_bDrawDescription = TRUE;
	m_strDescription = _T("Dummy");
}

BEGIN_MESSAGE_MAP(CEMFRecTooltipCtrl, CEMFRecTooltipCtrlBase)
	ON_NOTIFY_REFLECT(TTN_POP, &CEMFRecTooltipCtrl::OnPop)
END_MESSAGE_MAP()

void CEMFRecTooltipCtrl::OnPop(NMHDR* pNMHDR, LRESULT* pResult)
{
	CEMFRecTooltipCtrlBase::OnPop(pNMHDR, pResult);
	m_strDescription = _T("Dummy");
}

CSize CEMFRecTooltipCtrl::OnDrawDescription(CDC* pDC, CRect rect, BOOL bCalcOnly)
{
	if (!m_pRec)
		return CSize(0, 0);
	EMFRecAccess::PreviewContext ctxt{0};
	ctxt.pDC = pDC;
	ctxt.rect = rect;
	ctxt.bCalcOnly = bCalcOnly;
	if (!m_pRec->DrawPreview(&ctxt))
		return CSize(0, 0);
	return ctxt.szPreferedSize;
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
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, &CEMFRecListCtrl::OnCustomDraw)
	ON_NOTIFY_REFLECT(LVN_GETDISPINFO, &CEMFRecListCtrl::OnGetDispInfo)
	ON_NOTIFY_REFLECT_EX(LVN_ITEMCHANGED, &CEMFRecListCtrl::OnItemChange)
	ON_NOTIFY_REFLECT(LVN_ENDSCROLL, &CEMFRecListCtrl::OnEndScroll)
	ON_WM_GETDLGCODE()
	ON_WM_MOUSEMOVE()
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
	if (pNMHDR && pNMHDR->code == (UINT)TTN_GETDISPINFOW)
	{
		NMTTDISPINFOW *pdi = (NMTTDISPINFOW*)pNMHDR;
		if (m_ToolTip.m_pRec)
		{
			swprintf(pdi->szText, _countof(pdi->szText), L"#%zu %s", 
				m_ToolTip.m_pRec->GetIndex()+1, m_ToolTip.m_pRec->GetRecordName());
		}
		else
		{
			pdi->szText[0] = TEXT('\0');
		}
	}

	return bRes;
}

int CEMFRecListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	int nRet = CEMFRecListCtrlBase::OnCreate(lpCreateStruct);
	if (nRet == 0)
	{
		DWORD dwExStyle = GetExtendedStyle();
		dwExStyle |= LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER | LVS_EX_LABELTIP;
		SetExtendedStyle(dwExStyle);

		m_crfDefaultBkColor = GetBkColor();
		if (theApp.IsDarkTheme())
			SetBkColor(theApp.m_crfDarkThemeBkColor);

		InsertColumn(ColumnTypeIndex, _T("#"));
		InsertColumn(ColumnTypeName, _T("Name"));

		_EnableListCtrlExplorerVisualStyles(GetSafeHwnd(), TRUE);

		//GetToolTips()->Activate(FALSE);

		m_ToolTip.Create(this, TTS_ALWAYSTIP);
		// It seems that there's a maximum delay for this, any value greater than
		// 30000 will be treated as the default value (which is 5 second)
		// https://forums.codeguru.com/showthread.php?64155-ToolTip-Problem
		m_ToolTip.SetDelayTime(TTDT_AUTOPOP, 30000);
		//m_ToolTip.Activate(TRUE);
		TOOLINFO ti = { sizeof(ti) };
		ti.uFlags = TTF_TRANSPARENT;
		ti.hwnd = GetSafeHwnd();
		ti.lpszText = LPSTR_TEXTCALLBACK;
		SetRectEmpty(&ti.rect);
		m_ToolTip.SendMessage(TTM_ADDTOOL, 0, (LPARAM)&ti);

		m_ToolTip.SetWindowPos(&wndTop, -1, -1, -1, -1, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOSIZE);
	}
	return nRet;
}

void CEMFRecListCtrl::OnDestroy()
{
	m_ToolTip.DestroyWindow();

	CEMFRecListCtrlBase::OnDestroy();
}

void CEMFRecListCtrl::AdjustColumnWidth(int cx)
{
	SetColumnWidth(ColumnTypeIndex, LVSCW_AUTOSIZE);
	int cxIdx = GetColumnWidth(ColumnTypeIndex);
	SetColumnWidth(ColumnTypeName, cx - cxIdx);
}

BOOL CEMFRecListCtrl::PreTranslateMessage(MSG* pMsg)
{
	switch (pMsg->message)
	{
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MBUTTONUP:
	case WM_MOUSEMOVE:
		if (m_ToolTip.GetSafeHwnd() != NULL)
		{
			CPoint pt;
			GetCursorPos(&pt);
			ScreenToClient(&pt);
			int nItem = HitTest(pt);
 			if (nItem != m_nTipItem)
 			{
 				if (m_nTipItem >= 0)
 					m_ToolTip.SendMessage(TTM_POP);
				auto pRec = GetEMFRecord(nItem);
				bool bShowTip = pRec && pRec->DrawPreview();
				m_ToolTip.m_pRec = bShowTip ? pRec : nullptr;
				if (bShowTip)
				{
					m_ToolTip.SetWindowTextW(pRec->GetRecordName());
				}
				m_nTipItem = bShowTip ? nItem : -1;
				m_ToolTip.Activate(bShowTip);
			}
			m_ToolTip.RelayEvent(pMsg);
		}
		break;
	}
	return CEMFRecListCtrlBase::PreTranslateMessage(pMsg);
}

void CEMFRecListCtrl::OnSize(UINT nType, int cx, int cy)
{
	AdjustColumnWidth(cx);
	if (m_ToolTip.GetSafeHwnd())
	{
		TOOLINFO ti = { sizeof(ti) };
		ti.hwnd = GetSafeHwnd();
		ti.uId = 0;
		GetClientRect(&ti.rect);
		m_ToolTip.SendMessage(TTM_NEWTOOLRECT, 0, (LPARAM)&ti);
	}

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

void CEMFRecListCtrl::OnEndScroll(NMHDR* pNMHDR, LRESULT* pResult)
{
	CRect rc;
	GetClientRect(&rc);
	AdjustColumnWidth(rc.Width());
}

UINT CEMFRecListCtrl::OnGetDlgCode()
{
	// Add DLGC_WANTMESSAGE so we have NM_RETURN notification
	return CEMFRecListCtrlBase::OnGetDlgCode() | DLGC_WANTMESSAGE;
}

// void CEMFRecListCtrl::OnMouseMove(UINT nFlags, CPoint point)
// {
// 	CEMFRecListCtrlBase::OnMouseMove(nFlags, point);
// 
// 	int nItem = HitTest(point);
// 	if (nItem != m_nTipItem)
// 	{
// 		if (m_nTipItem >= 0)
// 		{
// 			//m_ToolTip.SendMessage(TTM_POP);
// 			m_ToolTip.SendMessage(TTM_TRACKACTIVATE, FALSE);
// 		}
// 		if (nItem >= 0)
// 		{
// 			ClientToScreen(&point);
// 			m_ToolTip.SendMessage(TTM_TRACKPOSITION, 0, MAKELPARAM(point.x, point.y));
// 			TOOLINFO ti = { sizeof(ti) };
// 			ti.uFlags = TTF_TRANSPARENT | TTF_TRACK;
// 			ti.hwnd = GetSafeHwnd();
// 			ti.lpszText = (LPTSTR)_T("Placeholder");
// 			SetRectEmpty(&ti.rect);
// 			m_ToolTip.SendMessage(TTM_TRACKACTIVATE, TRUE, (LPARAM)&ti);
// 		}
// 		m_nTipItem = nItem;
// 	}
// }

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
		if (m_nTipItem >= 0)
		{
			m_ToolTip.SendMessage(TTM_POP);
			m_nTipItem = -1;
		}
		m_emf = nullptr;
		// There could be repaint issue when the list control was previously scrolled
		// Steps to reproduce:
		// 1) Load EMF 1
		// 2) Scroll to some position around the middle of the list control
		// 3) Load EMF 2
		// ==> No visible items are shown, but if you move the mouse cursor over
		// the list control, or scroll down a little, part of the items will be drawn
		// DeleteAllItems() seems to fix the issue
		DeleteAllItems();
		// EnsureVisible may also help fix the painting issue, but it seems not enough
		// when for some reason the list is scrolled, then loads empty list, then load 
		// another non-empty list
		//EnsureVisible(0, FALSE);
	}
	else
	{
		int nCount = m_emf ? (int)m_emf->GetRecordCount() : 0;
		SetItemCount(nCount);
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


