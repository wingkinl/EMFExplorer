
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
	DWORD dwListStyle = WS_CHILD | WS_VISIBLE;

	if (!m_wndRecList.Create(dwListStyle, rectDummy, this, IDC_FILE_VIEW_CTRL))
	{
		TRACE0("Failed to create file view\n");
		return -1;      // fail to create
	}

	// Create combo:
	const DWORD dwComboStyle = WS_CHILD | WS_VISIBLE | CBS_DROPDOWN
		| WS_BORDER | CBS_SORT | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	if (!m_wndFindCombo.Create(dwComboStyle, rectDummy, this, 1))
	{
		TRACE0("Failed to create Properies Combo \n");
		return -1;      // fail to create
	}

	LOGFONT lf;
	afxGlobalData.fontRegular.GetLogFont(&lf);

	NONCLIENTMETRICS info;
	info.cbSize = sizeof(info);

	afxGlobalData.GetNonClientMetrics(info);

	lf.lfHeight = info.lfMenuFont.lfHeight;
	lf.lfWeight = info.lfMenuFont.lfWeight;
	lf.lfItalic = info.lfMenuFont.lfItalic;

	m_fntFindCombo.CreateFontIndirect(&lf);

	m_wndFindCombo.SetFont(&m_fntFindCombo);

	m_wndFindCombo.SetCueBanner(L"Find...");

	OnChangeVisualStyle();

	AdjustLayout();

	return 0;
}

void CFileView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void CFileView::AdjustLayout()
{
	if (GetSafeHwnd() == nullptr)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	CRect rectCombo;

	m_wndFindCombo.GetWindowRect(&rectCombo);

	int cyCmb = rectCombo.Size().cy;
	
	m_wndFindCombo.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), 200, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndRecList.SetWindowPos(nullptr, rectClient.left + 1, rectClient.top + cyCmb + 1, 
		rectClient.Width() - 2, rectClient.Height() - cyCmb - 2, SWP_NOACTIVATE | SWP_NOZORDER);
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
	if (!m_wndRecList.IsHoverNotificationEnabled())
		return;
	NM_LISTVIEW* pNMLV = (NM_LISTVIEW*)pNMHDR;
	auto pMainWnd = GetTopLevelFrame();
	pMainWnd->SendMessage(MainFrameMsgOnSelectRecordItem, pNMLV->iItem, TRUE);
}

void CFileView::OnListEnter(NMHDR* pNMHDR, LRESULT* pResult)
{
	if (CanViewCurSelRecord())
		m_wndRecList.ViewCurSelRecord();
}

void CFileView::OnListDblClk(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMITEMACTIVATE* pItemActivate = (NMITEMACTIVATE*)pNMHDR;
	if (pItemActivate->iItem >= 0)
	{
		if (CanViewCurSelRecord())
			m_wndRecList.ViewCurSelRecord();
	}
}

void CFileView::OnChangeVisualStyle()
{
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

void CFileView::EnableHoverNotification(BOOL enable)
{
	m_wndRecList.EnableHoverNotification(enable);
}

bool CFileView::CanViewCurSelRecord() const
{
	return m_wndRecList.CanViewCurSelRecord();
}

