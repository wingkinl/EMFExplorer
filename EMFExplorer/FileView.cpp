
#include "pch.h"
#include "framework.h"
#include "mainfrm.h"
#include "FileView.h"
#include "Resource.h"
#include "EMFExplorer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CFileView

CFileView::CFileView() noexcept
{
}

CFileView::~CFileView()
{
}

#define IDC_FILE_VIEW_CTRL	4

BEGIN_MESSAGE_MAP(CFileView, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_VIEW_RECORD, OnViewRecord)
	ON_UPDATE_COMMAND_UI(ID_VIEW_RECORD, OnUpdateViewRecord)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_FILE_VIEW_CTRL, &CFileView::OnListItemChange)
	ON_NOTIFY(LVN_HOTTRACK, IDC_FILE_VIEW_CTRL, &CFileView::OnListHotTrack)
	ON_NOTIFY(NM_RETURN, IDC_FILE_VIEW_CTRL, &CFileView::OnListEnter)
	ON_NOTIFY(NM_DBLCLK, IDC_FILE_VIEW_CTRL, &CFileView::OnListDblClk)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceBar message handlers

int CFileView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// Create view:
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE;

	if (!m_wndRecList.Create(dwViewStyle, rectDummy, this, IDC_FILE_VIEW_CTRL))
	{
		TRACE0("Failed to create file view\n");
		return -1;      // fail to create
	}

	m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_EXPLORER);
	m_wndToolBar.LoadToolBar(IDR_EXPLORER, 0, 0, TRUE /* Is locked */);

	OnChangeVisualStyle();

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));

	m_wndToolBar.SetOwner(this);

	// All commands will be routed via this control , not via the parent frame:
	m_wndToolBar.SetRouteCommandsViaFrame(FALSE);

	AdjustLayout();

	return 0;
}

void CFileView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void CFileView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	if (pWnd->GetSafeHwnd() != m_wndRecList.GetSafeHwnd())
	{
		CDockablePane::OnContextMenu(pWnd, point);
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
		m_wndRecList.ScreenToClient(&point);
		nRow = m_wndRecList.HitTest(point);
	}
	if (nRow < 0)
		return;
	CRect rect;
	m_wndRecList.GetItemRect(nRow, &rect, LVIR_BOUNDS);

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
	m_wndRecList.ClientToScreen(&point);

	m_wndRecList.SetFocus();
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EXPLORER, point.x, point.y, this, TRUE);
}

void CFileView::AdjustLayout()
{
	if (GetSafeHwnd() == nullptr)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	int cyTlb = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;

	m_wndToolBar.SetWindowPos(nullptr, rectClient.left, rectClient.top, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndRecList.SetWindowPos(nullptr, rectClient.left + 1, rectClient.top + cyTlb + 1, rectClient.Width() - 2, rectClient.Height() - cyTlb - 2, SWP_NOACTIVATE | SWP_NOZORDER);
}

void CFileView::OnViewRecord()
{
	int nRow = GetCurSelRecIndex();
	auto pMainWnd = GetTopLevelFrame();
	pMainWnd->SendMessage(MainFrameMsgOpenRecordItem, nRow);
}

void CFileView::OnUpdateViewRecord(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(CanViewCurSelRecord());
}

void CFileView::OnEditCopy()
{
	int nRow = GetCurSelRecIndex();
	auto pRec = nRow >= 0 ? m_wndRecList.GetEMFRecord(nRow) : nullptr;
	if (!pRec)
		return;
	if (!OpenClipboard())
	{
		AfxMessageBox( _T("Cannot open the Clipboard") );
		return;
	}
	EmptyClipboard();

	auto pszName = pRec->GetRecordName();
	auto nLen = wcslen(pszName);
	auto hglbCopy = GlobalAlloc(GMEM_MOVEABLE, 
		(nLen + 1) * sizeof(WCHAR));
	if (!hglbCopy)
	{
		CloseClipboard();
		return;
	}
	auto lptstrCopy = (WCHAR*)GlobalLock(hglbCopy); 
	memcpy(lptstrCopy, pszName, 
		nLen * sizeof(WCHAR)); 
	lptstrCopy[nLen] = (WCHAR)0;
	GlobalUnlock(hglbCopy); 
	SetClipboardData(CF_UNICODETEXT, hglbCopy);
	CloseClipboard();
}

void CFileView::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CRect rectTree;
	m_wndRecList.GetWindowRect(rectTree);
	ScreenToClient(rectTree);

	rectTree.InflateRect(1, 1);
	dc.Draw3dRect(rectTree, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}

void CFileView::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);

	m_wndRecList.SetFocus();
}

void CFileView::OnListItemChange(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMLV = (NM_LISTVIEW*)pNMHDR;
	if ((pNMLV->uChanged & LVIF_STATE) && (pNMLV->uNewState & LVIS_SELECTED))
	{
		auto pMainWnd = GetTopLevelFrame();
		pMainWnd->SendMessage(MainFrameMsgOnSelectRecordItem, pNMLV->iItem);
	}
}

void CFileView::OnListHotTrack(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMLV = (NM_LISTVIEW*)pNMHDR;
	auto pMainWnd = GetTopLevelFrame();
	pMainWnd->SendMessage(MainFrameMsgOnSelectRecordItem, pNMLV->iItem, TRUE);
}

void CFileView::OnListEnter(NMHDR* pNMHDR, LRESULT* pResult)
{
	if (CanViewCurSelRecord())
		OnViewRecord();
}

void CFileView::OnListDblClk(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMITEMACTIVATE* pItemActivate = (NMITEMACTIVATE*)pNMHDR;
	if (pItemActivate->iItem >= 0)
	{
		if (CanViewCurSelRecord())
			OnViewRecord();
	}
}

void CFileView::OnChangeVisualStyle()
{
	m_wndToolBar.CleanUpLockedImages();
	m_wndToolBar.LoadBitmap(theApp.m_bHiColorIcons ? IDB_EXPLORER_24 : IDR_EXPLORER, 0, 0, TRUE /* Locked */);
	m_wndRecList.OnChangeVisualStyle();
}

void CFileView::SetEMFAccess(std::shared_ptr<EMFAccess> emf)
{
	m_wndRecList.SetEMFAccess(emf);
}

void CFileView::LoadEMFDataEvent(bool bBefore)
{
	m_wndRecList.LoadEMFDataEvent(bBefore);
}

int CFileView::GetCurSelRecIndex(BOOL bHottrack) const
{
	return m_wndRecList.GetCurSelRecIndex(bHottrack);
}

void CFileView::SetCurSelRecIndex(int index)
{
	m_wndRecList.SetCurSelRecIndex(index);
	m_wndRecList.SetFocus();
}

bool CFileView::CanViewCurSelRecord() const
{
	int nRow = GetCurSelRecIndex();
	auto pMainWnd = GetTopLevelFrame();
	auto bCanOpen = pMainWnd->SendMessage(MainFrameMsgCanOpenRecordItem, nRow);
	return bCanOpen != 0;
}

