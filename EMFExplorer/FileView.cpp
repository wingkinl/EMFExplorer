
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

BEGIN_MESSAGE_MAP(CSearchComboBoxEdit, CSearchComboBoxEditBase)
	ON_CONTROL_REFLECT_EX(EN_CHANGE, OnEnChange)
END_MESSAGE_MAP()

void CSearchComboBoxEdit::OnBrowse()
{
	ASSERT_VALID(this);
	ENSURE(GetSafeHwnd() != NULL);
	SetWindowText(_T(""));
	EnableBrowseButton(FALSE);
	OnAfterUpdate();
	CSearchComboBoxEditBase::OnBrowse();
}

#define COLOR_RECORD_NOT_FOUND	RGB(0xff, 0xd0, 0xfd)

void CSearchComboBoxEdit::OnDrawBrowseButton(CDC* pDC, CRect rect, BOOL bIsButtonPressed, BOOL bIsButtonHot)
{
	ASSERT(m_Mode != BrowseMode_None);
	ASSERT_VALID(pDC);
	CFindComboBox* pComboBox = STATIC_DOWNCAST(CFindComboBox, GetParent());
	COLORREF clrBk = pComboBox->m_bSearchOK ? GetGlobalData()->clrWindow : COLOR_RECORD_NOT_FOUND;
	if (bIsButtonPressed)
	{
		clrBk = RGB(0x00, 0x7A, 0xCC);
	}
	else if (bIsButtonHot)
	{
		clrBk = RGB(0xC9, 0xDE, 0xF5);
	}

	pDC->FillSolidRect(rect, clrBk);

	CMenuImages::Draw(pDC, CMenuImages::IdCloseSmall, rect);
}

void CSearchComboBoxEdit::Init()
{
	m_Mode = BrowseMode_None;
	m_bDefaultImage = FALSE;

	OnChangeLayout();
}

void CSearchComboBoxEdit::OnAfterUpdate()
{
	CSearchComboBoxEditBase::OnAfterUpdate();
	RedrawWindow(NULL, NULL, RDW_FRAME | RDW_INVALIDATE);
	CFindComboBox* pComboBox = STATIC_DOWNCAST(CFindComboBox, GetParent());
	if (pComboBox)
		pComboBox->OnAfterClearText();
}

BOOL CSearchComboBoxEdit::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if ((GetKeyState(VK_MENU) >= 0) && (GetKeyState(VK_CONTROL) >= 0))
		{
			switch (pMsg->wParam)
			{
			case VK_RETURN:
				{
					CFindComboBox* pComboBox = STATIC_DOWNCAST(CFindComboBox, GetParent());
					if (pComboBox)
					{
						auto pRecView = pComboBox->GetParent();
						if (pRecView)
							pRecView->PostMessage(WM_COMMAND, MAKEWPARAM(pComboBox->GetDlgCtrlID(), 0), (LPARAM) pComboBox->GetSafeHwnd());
					}
				}
				return TRUE;
			}
		}
	}

	return CSearchComboBoxEditBase::PreTranslateMessage(pMsg);
}

BOOL CSearchComboBoxEdit::OnEnChange()
{
	bool bShowClearBtn = GetWindowTextLength() > 0;
	bool bOldShowClearBtn = m_Mode != BrowseMode_None;
	if (bShowClearBtn ^ bOldShowClearBtn)
	{
		EnableBrowseButton(bShowClearBtn);
		RedrawWindow(NULL, NULL, RDW_FRAME | RDW_INVALIDATE);
	}
	return FALSE;	// allow parent to handle it too
}

CFindComboBox::CFindComboBox()
{

}

CFindComboBox::~CFindComboBox()
{

}

BOOL CFindComboBox::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	dwStyle |= CBS_DROPDOWN | CBS_SORT | CBS_AUTOHSCROLL;
	if (!CFindComboBoxBase::Create(dwStyle, rect, pParentWnd, nID))
		return FALSE;
	SetCueBanner(L"Find...");

	COMBOBOXINFO info;
	memset(&info, 0, sizeof(COMBOBOXINFO));
	info.cbSize = sizeof(COMBOBOXINFO);

	GetComboBoxInfo(&info);
	if (info.hwndItem)
	{
		m_edit.SubclassWindow(info.hwndItem);
		m_edit.Init();
	}

	return TRUE;
}

BEGIN_MESSAGE_MAP(CFindComboBox, CFindComboBoxBase)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

void CFindComboBox::OnAfterClearText()
{
	m_bSearchOK = TRUE;
	RedrawWindow(NULL, NULL, RDW_FRAME | RDW_INVALIDATE);
	auto pParent = GetParent();
	if (pParent->GetSafeHwnd())
		pParent->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), CBN_EDITCHANGE), (LPARAM)GetSafeHwnd());
}

HBRUSH CFindComboBox::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	if (CTLCOLOR_EDIT == nCtlColor && !m_bSearchOK)
	{
		COLORREF crfBK = COLOR_RECORD_NOT_FOUND;
		pDC->SetBkColor(crfBK);
		pDC->SetBkMode(TRANSPARENT);
		if (!m_brErr.GetSafeHandle())
			m_brErr.CreateSolidBrush(crfBK);
		return (HBRUSH)m_brErr.GetSafeHandle();
	}
	return CFindComboBoxBase::OnCtlColor(pDC, pWnd, nCtlColor);
}

/////////////////////////////////////////////////////////////////////////////
// CFileView

CFileView::CFileView() noexcept
{
}

CFileView::~CFileView()
{
}

#define IDC_RECORD_VIEW_CTRL	4
#define IDC_FIND_COMBO			5

BEGIN_MESSAGE_MAP(CFileView, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_RECORD_VIEW_CTRL, &CFileView::OnListItemChange)
	ON_NOTIFY(LVN_HOTTRACK, IDC_RECORD_VIEW_CTRL, &CFileView::OnListHotTrack)
	ON_NOTIFY(NM_RETURN, IDC_RECORD_VIEW_CTRL, &CFileView::OnListEnter)
	ON_NOTIFY(NM_DBLCLK, IDC_RECORD_VIEW_CTRL, &CFileView::OnListDblClk)
	ON_CBN_EDITCHANGE(IDC_FIND_COMBO, &CFileView::OnFindEditChange)
	ON_COMMAND(IDC_FIND_COMBO, &CFileView::OnFindRecord)
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

	if (!m_wndRecList.Create(dwListStyle, rectDummy, this, IDC_RECORD_VIEW_CTRL))
	{
		TRACE0("Failed to create file view\n");
		return -1;      // fail to create
	}

	// Create combo:
	const DWORD dwComboStyle = WS_CHILD | WS_VISIBLE
		| WS_BORDER | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	if (!m_wndFindCombo.Create(dwComboStyle, rectDummy, this, IDC_FIND_COMBO))
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

void CFileView::OnFindEditChange()
{
	BOOL bFindOK = TRUE;
	CStringW str;
	m_wndFindCombo.GetWindowTextW(str);
	m_wndRecList.SetSearchText(str);
	if (!str.IsEmpty())
	{
		int nRecItem = m_wndRecList.FindRecord((LPCWSTR)str);
		bFindOK = nRecItem >= 0;
	}
	m_wndFindCombo.m_bSearchOK = bFindOK;
	m_wndFindCombo.Invalidate();
	m_wndFindCombo.RedrawWindow(NULL, NULL, RDW_FRAME | RDW_INVALIDATE | RDW_ALLCHILDREN | RDW_ERASE | RDW_UPDATENOW);

	m_wndRecList.Invalidate();
}

void CFileView::OnFindRecord()
{
	if (!m_wndRecList.GoToNextSearchItem())
		MessageBeep((UINT)-1);
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


