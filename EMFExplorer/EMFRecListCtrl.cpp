
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

#define TIMER_ID_ADJUST_COLUMN_WIDTH_EVENT	0x00010000
#define TIMER_ADJUST_COLUMN_WIDTH_DELAY		400

BEGIN_MESSAGE_MAP(CEMFRecListCtrl, CEMFRecListCtrlBase)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, &CEMFRecListCtrl::OnCustomDraw)
	ON_NOTIFY_REFLECT(LVN_GETDISPINFO, &CEMFRecListCtrl::OnGetDispInfo)
	ON_NOTIFY_REFLECT_EX(LVN_ITEMCHANGED, &CEMFRecListCtrl::OnItemChange)
	ON_NOTIFY_REFLECT(LVN_ENDSCROLL, &CEMFRecListCtrl::OnEndScroll)
	ON_WM_GETDLGCODE()
	ON_COMMAND(ID_EDIT_COPY_RECORD_LIST, &CEMFRecListCtrl::CopySelectedItemsToClipboard)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_VIEW_RECORD, &CEMFRecListCtrl::OnViewRecord)
	ON_UPDATE_COMMAND_UI(ID_VIEW_RECORD, &CEMFRecListCtrl::OnUpdateViewRecord)
	ON_WM_TIMER()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEMFRecListCtrl message handlers

enum ColumnType
{
	ColumnTypeIndex,
	ColumnTypeName,
	ColumnTypeCount,
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

BOOL CEMFRecListCtrl::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	dwStyle |= LVS_REPORT | LVS_SHOWSELALWAYS | LVS_OWNERDATA;
	return CEMFRecListCtrlBase::Create(dwStyle, rect, pParentWnd, nID);
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

		auto lpszResourceName = ATL_MAKEINTRESOURCE(IDR_RECORD_LIST_ACCELERATOR);
		HINSTANCE hAccelInst = AfxFindResourceHandle(lpszResourceName, ATL_RT_ACCELERATOR);
		m_hAccelTable = ::LoadAccelerators(hAccelInst, lpszResourceName);
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

void CEMFRecListCtrl::OnContextMenu(CWnd* pWnd, CPoint point)
{
	if (pWnd->GetSafeHwnd() != GetSafeHwnd())
	{
		CEMFRecListCtrlBase::OnContextMenu(pWnd, point);
		return;
	}
	int nRow = -1;
	// Select clicked item:
	if (point == CPoint(-1, -1))
	{
		nRow = GetCurSelRecIndex();
	}
	else
	{
		ScreenToClient(&point);
		nRow = HitTest(point);
	}
	if (nRow < 0)
		return;
	CRect rect;
	GetItemRect(nRow, &rect, LVIR_BOUNDS);

	if (point == CPoint(-1, -1))
	{
		point.x = rect.left;
		point.y = rect.bottom;
	}
	CRect rcClient;
	GetClientRect(rcClient);
	if (point.y < rcClient.top)
		point.y = rcClient.top;
	else if (point.y > rcClient.bottom)
		point.y = rcClient.bottom;
	ClientToScreen(&point);

	SetFocus();
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EXPLORER, point.x, point.y, this, TRUE);
}

void CEMFRecListCtrl::SetCustomHotItem(int nItem)
{
	if (m_nHotItem != nItem)
	{
		if (m_nHotItem >= 0)
			RedrawItems(m_nHotItem, m_nHotItem);
		if (nItem >= 0)
			RedrawItems(nItem, nItem);
		m_nHotItem = nItem;
	}
}

BOOL CEMFRecListCtrl::PreTranslateMessage(MSG* pMsg)
{
	switch (pMsg->message)
	{
	case WM_MOUSELEAVE:
		if (m_bTracked)
		{
			if (!m_bNotifyHover && m_nHotItem >= 0)
			{
				SetCustomHotItem(-1);
			}
			m_bTracked = FALSE;
		}
		break;
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MBUTTONUP:
	case WM_MOUSEMOVE:
		{
			CPoint pt;
			GetCursorPos(&pt);
			ScreenToClient(&pt);
			int nItem = HitTest(pt);
			if (WM_MOUSEMOVE == pMsg->message)
			{
				if (!m_bNotifyHover && !m_bTracked)
				{
					m_bTracked = TRUE;

					TRACKMOUSEEVENT trackmouseevent;
					trackmouseevent.cbSize = sizeof(trackmouseevent);
					trackmouseevent.dwFlags = TME_LEAVE;
					trackmouseevent.hwndTrack = GetSafeHwnd();
					TrackMouseEvent(&trackmouseevent);
				}
				SetCustomHotItem(nItem);
			}
			if (m_ToolTip.GetSafeHwnd() != NULL)
			{
				if (nItem != m_nTipItem)
				{
					if (m_nTipItem >= 0)
						m_ToolTip.SendMessage(TTM_POP);
					auto pRec = GetEMFRecord(nItem);
					bool bShowTip = pRec && pRec->DrawPreview();
					m_ToolTip.m_pRec = bShowTip ? pRec : nullptr;
					m_nTipItem = bShowTip ? nItem : -1;
					m_ToolTip.Activate(bShowTip);
				}
				m_ToolTip.RelayEvent(pMsg);
			}
		}
		break;
	}
	if (pMsg->message >= WM_KEYFIRST && pMsg->message <= WM_KEYLAST)
	{
		return m_hAccelTable != NULL && ::TranslateAccelerator(m_hWnd, m_hAccelTable, pMsg);
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
		OnDrawItem(lplvcd);
		*pResult = CDRF_SKIPDEFAULT;
		break;
	}
}

void CEMFRecListCtrl::OnDrawItem(LPNMLVCUSTOMDRAW lplvcd) const
{
	int nRow = (int)lplvcd->nmcd.dwItemSpec;
	CDC* pDC = CDC::FromHandle(lplvcd->nmcd.hdc);
	
	auto state = GetItemState(nRow, LVIS_SELECTED | LVIS_FOCUSED);

	BOOL bDarkTheme = IsDarkTheme();
	auto pRec = GetEMFRecord(nRow);

	int nSel = GetNextItem(-1, LVNI_FOCUSED);
	EMFRecAccess* pRecSel = nullptr;
	if (nSel >= 0)
	{
		pRecSel = GetEMFRecord(nSel);
	}
	int nSelRecType = pRecSel ? (int)pRecSel->GetRecordType() : -1;

	lplvcd->clrText = theApp.m_crfDarkThemeTxtColor;
	bool bHotItem = m_nHotItem == nRow;
	if (state & LVIS_FOCUSED)
	{
		lplvcd->clrTextBk = RGB(123, 31, 162);
	}
	else if (state & LVIS_SELECTED)
	{
		lplvcd->clrTextBk = RGB(2, 119, 189);
	}
	else
	{
		if (!bHotItem)
		{
			if (nSelRecType >= (int)emfplus::EmfRecordTypeMin && (int)pRec->GetRecordType() == nSelRecType)
				lplvcd->clrTextBk = RGB(83, 144, 217);
			else if (pRec->IsDrawingRecord())
				lplvcd->clrTextBk = theApp.IsDarkTheme() ? RGB(3, 136, 87) : RGB(4, 170, 109);
			else
				lplvcd->clrTextBk = GetBkColor();
		}
		if (!bDarkTheme)
			lplvcd->clrText = GetTextColor();
	}
	if (bHotItem && !(state & LVIS_FOCUSED))
	{
		lplvcd->clrTextBk = RGB(156, 77, 204);
	}

	CRect rcRow = lplvcd->nmcd.rc;
	GetItemRect(nRow, &rcRow, LVIR_SELECTBOUNDS);
	pDC->FillSolidRect(&rcRow, lplvcd->clrTextBk);

	auto oldTextColor = pDC->SetTextColor(lplvcd->clrText);
	auto oldBkMode = pDC->GetBkMode();

	CRect rcText = lplvcd->nmcd.rc;
	for (int nCol = 0; nCol < ColumnTypeCount; ++nCol)
	{
		GetSubItemRect(nRow, nCol, LVIR_LABEL, rcText);
		bool bLink = false;
		if (nCol == ColumnTypeIndex)
		{
			if (pRecSel && nSel != nRow)
			{
				if (pRec->IsLinked(pRecSel))
				{
					bLink = true;
					lplvcd->clrTextBk = RGB(74, 0, 114);
					CRect rcLink = rcText;
					rcLink.left = 0;
					pDC->FillSolidRect(&rcLink, lplvcd->clrTextBk);
					if (!bDarkTheme)
						pDC->SetTextColor(theApp.m_crfDarkThemeTxtColor);
				}
			}
		}
		CString str;
		LVITEM item = { 0 };
		item.iItem = nRow;
		item.iSubItem = nCol;
		item.cchTextMax = 260;
		item.pszText = str.GetBuffer(item.cchTextMax);
		GetDispItemText(item);
		str.ReleaseBuffer();
		pDC->SetBkMode(TRANSPARENT);

		if (nCol == ColumnTypeName && !m_strSearch.IsEmpty())
		{
			auto szFind = StrStrIW(item.pszText, (LPCWSTR)m_strSearch);
			if (szFind)
			{
				GCP_RESULTS res = { 0 };
				res.lStructSize = sizeof(res);
				std::vector<int> vCaretPos((size_t)str.GetLength());
				std::vector<int> vDX(vCaretPos.size());
				res.lpCaretPos = vCaretPos.data();
				res.lpDx = vDX.data();
				res.nGlyphs = str.GetLength();
				GetCharacterPlacement(pDC->GetSafeHdc(), (LPCTSTR)str, str.GetLength(), 0, &res, 0);

				auto pos = (int)(szFind - item.pszText);
				CRect rcBk = rcText;
				auto nEnd = pos + m_strSearch.GetLength();
				if (nEnd < str.GetLength())
					rcBk.right = rcBk.left + vCaretPos[nEnd];
				else
					rcBk.right = rcBk.left + vCaretPos.back() + vDX.back();
				rcBk.left += vCaretPos[pos];
				pDC->FillSolidRect(rcBk, RGB(255,150,50));
			}
		}
		UINT nFormat = DT_SINGLELINE;
		pDC->DrawText(str, &rcText, nFormat|DT_END_ELLIPSIS);

		if (bLink && !bDarkTheme)
		{
			pDC->SetTextColor(lplvcd->clrText);
		}
	}


	pDC->SetTextColor(oldTextColor);
	pDC->SetBkMode(oldBkMode);

	if (state & LVIS_FOCUSED)
	{
		if (GetFocus() == this)
			pDC->DrawFocusRect(rcRow);
	}
}

void CEMFRecListCtrl::GetDispItemText(LVITEM& item) const
{
	auto pRec = GetEMFRecord(item.iItem);
	// See document for LVITEM:
	// the list-view control allows any length string to be stored as item text, only the first 260 TCHARs are displayed.
	// so better truncate the text here instead of leaving junk/gibberish string
	switch (item.iSubItem)
	{
	case ColumnTypeIndex:
		_sntprintf_s(item.pszText, item.cchTextMax, item.cchTextMax, _T("%ld"), item.iItem + 1);
		break;
	case ColumnTypeName:
		{
			auto szText = pRec->GetRecordName();
			_tcsncpy_s(item.pszText, item.cchTextMax, szText, item.cchTextMax);
		}
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
		GetDispItemText(item);
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
	m_nAdjustColumnWidthTimerID = SetTimer(TIMER_ID_ADJUST_COLUMN_WIDTH_EVENT, TIMER_ADJUST_COLUMN_WIDTH_DELAY, NULL);
}

UINT CEMFRecListCtrl::OnGetDlgCode()
{
	// Add DLGC_WANTMESSAGE so we have NM_RETURN notification
	return CEMFRecListCtrlBase::OnGetDlgCode() | DLGC_WANTMESSAGE;
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
		SetCustomHotItem(-1);
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

int CEMFRecListCtrl::GetCurSelRecIndex(BOOL bHottrack) const
{
	if (bHottrack)
		return m_nHotItem;
	return GetNextItem(-1, LVNI_FOCUSED);
}

void CEMFRecListCtrl::SetCurSelRecIndex(int index)
{
	SetCustomHotItem(-1);
	SetItemState(-1, 0, LVIS_SELECTED|LVIS_FOCUSED);
	SetItemState(index, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
	EnsureVisible(index, FALSE);
	int nCountPerPage = GetCountPerPage();
	if (nCountPerPage > 0 && nCountPerPage < GetItemCount())
	{
		int nTop = GetTopIndex();
		int nPreferTop = index - nCountPerPage / 2;
		if (nPreferTop < 0)
			nPreferTop = 0;
		if (nPreferTop != nTop)
		{
			CRect rect;
			GetItemRect(0, rect, LVIR_BOUNDS);
			int nRows = nPreferTop - nTop;
			Scroll(CSize(0, nRows * rect.Height()));
		}
	}
}

void CEMFRecListCtrl::OnViewRecord()
{
	ViewCurSelRecord();
}

bool CEMFRecListCtrl::CanViewCurSelRecord() const
{
	int nRow = GetCurSelRecIndex();
	auto pMainWnd = GetTopLevelFrame();
	auto bCanOpen = pMainWnd->SendMessage(MainFrameMsgCanOpenRecordItem, nRow);
	return bCanOpen != 0;
}

bool CEMFRecListCtrl::ViewCurSelRecord() const
{
	int nRow = GetCurSelRecIndex();
	if (nRow < 0)
		return false;
	auto pMainWnd = GetTopLevelFrame();
	auto res = pMainWnd->SendMessage(MainFrameMsgOpenRecordItem, nRow);
	return res != 0;
}

void CEMFRecListCtrl::EnableHoverNotification(BOOL enable)
{
	m_bNotifyHover = enable;
}

int CEMFRecListCtrl::FindRecord(LPCWSTR str, int nStart)
{
	if (!m_emf)
		return -1;
	// nStart is excluded from the search
	nStart = nStart < 0 ? 0 : nStart + 1;
	auto count = m_emf->GetRecordCount();
	if (nStart >= (int)count)
		nStart = 0;
	for (int ii = nStart; ii < count; ++ii)
	{
		auto pRec = m_emf->GetRecord((size_t)ii);
		auto szName = pRec->GetRecordName();
		auto szFind = StrStrIW(szName, str);
		if (szFind)
			return ii;
	}
	return -1;
}

void CEMFRecListCtrl::SetSearchText(LPCWSTR str)
{
	m_strSearch = str;
}

BOOL CEMFRecListCtrl::GoToNextSearchItem()
{
	if (m_strSearch.IsEmpty())
		return FALSE;
	int nSel = GetNextItem(-1, LVNI_FOCUSED);
	int nFind = FindRecord(m_strSearch, nSel);
	if (nFind < 0)
	{
		nFind = FindRecord(m_strSearch);
		if (nFind < 0)
			return FALSE;
	}
	SetCurSelRecIndex(nFind);
	return TRUE;
}

void CEMFRecListCtrl::OnUpdateViewRecord(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(CanViewCurSelRecord());
}

void CEMFRecListCtrl::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == TIMER_ID_ADJUST_COLUMN_WIDTH_EVENT)
	{
		CRect rc;
		GetClientRect(&rc);
		AdjustColumnWidth(rc.Width());

		KillTimer(m_nAdjustColumnWidthTimerID);
		m_nAdjustColumnWidthTimerID = 0;
	}

	CEMFRecListCtrlBase::OnTimer(nIDEvent);
}

void CEMFRecListCtrl::CopySelectedItemsToClipboard()
{
	if (GetSelectedCount() <= 0)
		return;
	if (!OpenClipboard())
	{
		AfxMessageBox(_T("Cannot open the Clipboard"));
		return;
	}
	EmptyClipboard();

	CStringW str;
	auto pos = GetFirstSelectedItemPosition();
	while (pos)
	{
		int nItem = GetNextSelectedItem(pos);
		auto pRec = nItem >= 0 ? GetEMFRecord(nItem) : nullptr;
		if (pRec)
		{
			auto pszName = pRec->GetRecordName();
			if (!str.IsEmpty())
				str += L"\r\n";
			str += pszName;
		}
	}

	auto pszText = (LPCWSTR)str;
	auto nLen = str.GetLength();
	auto hglbCopy = GlobalAlloc(GMEM_MOVEABLE, (nLen + 1) * sizeof(WCHAR));
	auto lptstrCopy = hglbCopy ? (WCHAR*)GlobalLock(hglbCopy) : nullptr;
	if (lptstrCopy)
	{
		memcpy(lptstrCopy, pszText, nLen * sizeof(WCHAR));
		lptstrCopy[nLen] = (WCHAR)0;
		GlobalUnlock(hglbCopy);
		SetClipboardData(CF_UNICODETEXT, hglbCopy);
	}
	CloseClipboard();
}

