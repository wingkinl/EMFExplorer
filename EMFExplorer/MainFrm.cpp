
// MainFrm.cpp : implementation of the CMainFrame class
//

#include "pch.h"
#include "framework.h"
#include "EMFExplorer.h"

#include "MainFrm.h"
#include "EMFExplorerView.h"
#include "EMFRecAccessGDI.h"
#include "EMFRecAccessPlus.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWndEx)

const int  iMaxUserToolbars = 10;
const UINT uiFirstUserToolBarId = AFX_IDW_CONTROLBAR_FIRST + 40;
const UINT uiLastUserToolBarId = uiFirstUserToolBarId + iMaxUserToolbars - 1;

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWndEx)
	ON_WM_CREATE()
	ON_COMMAND(ID_VIEW_CUSTOMIZE, &CMainFrame::OnViewCustomize)
	ON_REGISTERED_MESSAGE(AFX_WM_CREATETOOLBAR, &CMainFrame::OnToolbarCreateNew)
	ON_REGISTERED_MESSAGE(AFX_WM_CHANGEVISUALMANAGER, &CMainFrame::OnChangeVisualManager)
	ON_COMMAND(ID_VIEW_THEME_DARK, &CMainFrame::OnViewThemeDark)
	ON_COMMAND(ID_VIEW_THEME_LIGHT, &CMainFrame::OnViewThemeLight)
	ON_UPDATE_COMMAND_UI(ID_VIEW_THEME_DARK, &CMainFrame::OnUpdateViewThemeDark)
	ON_UPDATE_COMMAND_UI(ID_VIEW_THEME_LIGHT, &CMainFrame::OnUpdateViewThemeLight)
	ON_MESSAGE(MainFrameMsgOnSelectRecordItem, &CMainFrame::OnSelectRecordItem)
	ON_MESSAGE(MainFrameMsgCanOpenRecordItem, &CMainFrame::OnCanOpenRecordItem)
	ON_MESSAGE(MainFrameMsgOpenRecordItem, &CMainFrame::OnOpenRecordItem)
	ON_MESSAGE(MainFrameMsgViewUpdateSizeScroll, &CMainFrame::OnViewUpdateSizeScroll)
	ON_COMMAND(ID_EDIT_PASTE, &CMainFrame::OnEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, &CMainFrame::OnUpdateEditPaste)
	ON_UPDATE_COMMAND_UI(ID_STATUSBAR_PANE_COLOR_TEXT, &CMainFrame::OnUpdateStatusBarColorText)
END_MESSAGE_MAP()


UINT CMainStatusBar::Indicators[] = {
	ID_SEPARATOR,           // status line indicator
	ID_STATUSBAR_PANE_COLOR,
	ID_STATUSBAR_PANE_COLOR_TEXT,
	ID_STATUSBAR_PANE_COORD,
	ID_STATUSBAR_PANE_ZOOM,
};

BOOL CMainStatusBar::Create(CWnd* pParentWnd)
{
	if (!CMainStatusBarBase::Create(pParentWnd))
		return FALSE;
	SetIndicators(Indicators, sizeof(Indicators)/sizeof(UINT));
	return TRUE;
}

BOOL CMainStatusBar::SetPaneText(int nIndex, LPCTSTR lpszNewText, BOOL bUpdate)
{
	if (!CMainStatusBarBase::SetPaneText(nIndex, lpszNewText, bUpdate))
		return FALSE;
	if (lpszNewText && nIndex >= StatusBarIndexColorText)
	{
		int cx = GetPaneWidth(nIndex);
		CClientDC dcScreen(NULL);
		HFONT hFont = GetCurrentFont();

		HGDIOBJ hOldFont = dcScreen.SelectObject(hFont);
		CRect rectText(0, 0, cx, cx);
		dcScreen.DrawText(lpszNewText, static_cast<int>(_tcslen(lpszNewText)), rectText, 
			DT_CALCRECT | DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX);
		dcScreen.SelectObject(hOldFont);
		if (rectText.Width() > cx)
			SetPaneWidth(nIndex, rectText.Width());
	}
	return TRUE;
}

// CMainFrame construction/destruction

CMainFrame::CMainFrame() noexcept
{
	// TODO: add member initialization code here
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	BOOL bNameValid;

	if (!m_wndMenuBar.Create(this))
	{
		TRACE0("Failed to create menubar\n");
		return -1;      // fail to create
	}

	m_wndMenuBar.SetPaneStyle(m_wndMenuBar.GetPaneStyle() | CBRS_SIZE_DYNAMIC | CBRS_TOOLTIPS | CBRS_FLYBY);

	// prevent the menu bar from taking the focus on activation
	CMFCPopupMenu::SetForceMenuFocus(FALSE);

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(theApp.m_bHiColorIcons ? IDR_MAINFRAME_256 : IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	CString strToolBarName;
	bNameValid = strToolBarName.LoadString(IDS_TOOLBAR_STANDARD);
	ASSERT(bNameValid);
	m_wndToolBar.SetWindowText(strToolBarName);

	CString strCustomize;
	//bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	//ASSERT(bNameValid);
	//m_wndToolBar.EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);

	// Allow user-defined toolbars operations:
	InitUserToolbars(nullptr, uiFirstUserToolBarId, uiLastUserToolBarId);

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// TODO: Delete these five lines if you don't want the toolbar and menubar to be dockable
	m_wndMenuBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndMenuBar);
	DockPane(&m_wndToolBar);


	// enable Visual Studio 2005 style docking window behavior
	CDockingManager::SetDockingMode(DT_SMART);
	// enable Visual Studio 2005 style docking window auto-hide behavior
	EnableAutoHidePanes(CBRS_ALIGN_ANY);

	// Load menu item image (not placed on any standard toolbars):
	CMFCToolBar::AddToolBarForImageCollection(IDR_MENU_IMAGES, theApp.m_bHiColorIcons ? IDB_MENU_IMAGES_24 : 0);

	// create docking windows
	if (!CreateDockingWindows())
	{
		TRACE0("Failed to create docking windows\n");
		return -1;
	}

	m_wndFileView.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndFileView);
	CDockablePane* pTabbedBar = nullptr;
	m_wndProperties.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndProperties);
	m_wndThumbnail.EnableDocking(CBRS_ALIGN_ANY);
	//DockPane(&m_wndThumbnail);
	m_wndThumbnail.DockToWindow(&m_wndProperties, CBRS_ALIGN_BOTTOM);

	// set the visual manager used to draw all user interface elements
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));

	// set the visual style to be used the by the visual manager
	CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);

	// Enable toolbar and docking window menu replacement
	//EnablePaneMenu(TRUE, ID_VIEW_CUSTOMIZE, strCustomize, ID_VIEW_TOOLBAR);
	EnablePaneMenu(TRUE, 0, strCustomize, ID_VIEW_TOOLBAR);

	// enable quick (Alt+drag) toolbar customization
	CMFCToolBar::EnableQuickCustomization();

	if (CMFCToolBar::GetUserImages() == nullptr)
	{
		// load user-defined toolbar images
		if (m_UserImages.Load(_T(".\\UserImages.bmp")))
		{
			CMFCToolBar::SetUserImages(&m_UserImages);
		}
	}

	// enable menu personalization (most-recently used commands)
	// TODO: define your own basic commands, ensuring that each pulldown menu has at least one basic command.
	CList<UINT, UINT> lstBasicCommands;

	//lstBasicCommands.AddTail(ID_FILE_NEW);
	lstBasicCommands.AddTail(ID_FILE_OPEN);
	lstBasicCommands.AddTail(ID_FILE_SAVE);
	lstBasicCommands.AddTail(ID_APP_EXIT);
	//lstBasicCommands.AddTail(ID_EDIT_CUT);
	lstBasicCommands.AddTail(ID_EDIT_PASTE);
	//lstBasicCommands.AddTail(ID_EDIT_UNDO);
	lstBasicCommands.AddTail(ID_APP_ABOUT);
	lstBasicCommands.AddTail(ID_VIEW_STATUS_BAR);
	lstBasicCommands.AddTail(ID_VIEW_TOOLBAR);
	//lstBasicCommands.AddTail(ID_SORTING_SORTALPHABETIC);
	//lstBasicCommands.AddTail(ID_SORTING_SORTBYTYPE);
	//lstBasicCommands.AddTail(ID_SORTING_SORTBYACCESS);
	//lstBasicCommands.AddTail(ID_SORTING_GROUPBYTYPE);

	CMFCToolBar::SetBasicCommands(lstBasicCommands);

	OnChangeTheme();

	return 0;
}

LRESULT CMainFrame::OnChangeVisualManager(WPARAM wParam, LPARAM lParam)
{
	OnChangeTheme();
	return CFrameWndEx::OnChangeVisualManager(wParam, lParam);
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

BOOL CMainFrame::CreateDockingWindows()
{
	BOOL bNameValid;

	// Create file view
	CString strFileView;
	bNameValid = strFileView.LoadString(IDS_FILE_VIEW);
	ASSERT(bNameValid);
	if (!m_wndFileView.Create(strFileView, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_FILEVIEW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT| CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create File View window\n");
		return FALSE; // failed to create
	}

	// Create properties window
	CString str;
	bNameValid = str.LoadString(IDS_PROPERTIES_WND);
	ASSERT(bNameValid);
	if (!m_wndProperties.Create(str, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_PROPERTIESWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create Properties window\n");
		return FALSE; // failed to create
	}

	bNameValid = str.LoadString(IDS_THUMBNAIL_WND);
	ASSERT(bNameValid);
	if (!m_wndThumbnail.Create(str, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_THUMBNAIL_WND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create Thumbnail window\n");
		return FALSE; // failed to create
	}

	SetDockingWindowIcons(theApp.m_bHiColorIcons);
	return TRUE;
}

void CMainFrame::SetDockingWindowIcons(BOOL bHiColorIcons)
{
	HICON hFileViewIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_FILE_VIEW_HC : IDI_FILE_VIEW), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndFileView.SetIcon(hFileViewIcon, FALSE);

	HICON hPropertiesBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_PROPERTIES_WND_HC : IDI_PROPERTIES_WND), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndProperties.SetIcon(hPropertiesBarIcon, FALSE);

}

// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWndEx::Dump(dc);
}
#endif //_DEBUG

size_t CMainFrame::GetDrawToRecordIndex() const
{
	int nSel = m_wndFileView.GetCurSelRecIndex();
	if (nSel < 0)
		return (size_t)-1;
	return nSel;
}

// CMainFrame message handlers

void CMainFrame::OnViewCustomize()
{
	CMFCToolBarsCustomizeDialog* pDlgCust = new CMFCToolBarsCustomizeDialog(this, TRUE /* scan menus */);
	pDlgCust->EnableUserDefinedToolbars();
	pDlgCust->Create();
}

BOOL CMainFrame::CheckClipboardForEMF() const
{
	BOOL bOK = IsClipboardFormatAvailable(CF_ENHMETAFILE);
	return bOK;
}

void CMainFrame::OnUpdateEditPaste(CCmdUI* pCmdUI)
{
	BOOL bEnable = FALSE;
	if (!IsSubEMFFrame())
		bEnable = CheckClipboardForEMF();
	pCmdUI->Enable(bEnable);
}

void CMainFrame::OnEditPaste()
{
	theApp.DoEditPaste();
}

void CMainFrame::OnChangeTheme()
{
	m_wndFileView.OnChangeVisualStyle();
	m_wndProperties.OnChangeVisualStyle();
	m_wndThumbnail.OnChangeVisualStyle();
}

void CMainFrame::SetTheme(BOOL bDark)
{
	CWaitCursor wait;
	
	theApp.SetDarkTheme(bDark);

	CMFCVisualManagerOffice2007::SetStyle((CMFCVisualManagerOffice2007::Style)theApp.m_nStyle);
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));

	RedrawWindow(nullptr, nullptr, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);
}

void CMainFrame::OnViewThemeDark()
{
	SetTheme(TRUE);
}

void CMainFrame::OnViewThemeLight()
{
	SetTheme(FALSE);
}

void CMainFrame::OnUpdateViewThemeDark(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(theApp.IsDarkTheme());
}

void CMainFrame::OnUpdateViewThemeLight(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(!theApp.IsDarkTheme());
}

LRESULT CMainFrame::OnToolbarCreateNew(WPARAM wp,LPARAM lp)
{
	LRESULT lres = CFrameWndEx::OnToolbarCreateNew(wp,lp);
	if (lres == 0)
	{
		return 0;
	}

	CMFCToolBar* pUserToolbar = (CMFCToolBar*)lres;
	ASSERT_VALID(pUserToolbar);

	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
	return lres;
}

bool CMainFrame::UpdateViewOnRecord(int index)
{
	auto pView = CheckGetActiveView();
	pView->UpdateViewOnRecord(index);
	auto pDoc = pView->GetDocument();
	auto pEMF = pDoc->GetEMFAccess();
	auto pRec = pEMF->GetRecord((size_t)index);
	if (!pRec)
		return false;
	CachePropertiesContext ctxt{pEMF.get()};
	auto props = pRec->GetProperties(ctxt);
	m_wndProperties.SetPropList(props);
	return true;
}

LRESULT CMainFrame::OnSelectRecordItem(WPARAM wp, LPARAM lp)
{
	if (m_wndProperties.IsWindowVisible())
	{
		UpdateViewOnRecord((int)wp);
	}
	return 0;
}

class EMFRecordVisualizer
{
public:
	EMFRecordVisualizer() = default;
	virtual ~EMFRecordVisualizer() = default;
public:
	virtual void OpenVisualizer(CMainFrame* pFrame, EMFAccess* pEMF) {}
};

class EMFRecordMetafileVisualizer : public EMFRecordVisualizer
{
public:
	EMFRecordMetafileVisualizer(EMFRecAccessGDIPlusRecObject* pRec)
	{
		m_pRecObj = pRec;
	}
public:
	void OpenVisualizer(CMainFrame* pFrame, EMFAccess* pEMF) override;
protected:
	EMFRecAccessGDIPlusRecObject* m_pRecObj;
};

void EMFRecordMetafileVisualizer::OpenVisualizer(CMainFrame* pFrame, EMFAccess* pEMF)
{
	auto pWrapper = m_pRecObj->GetObjectWrapper();
	if (!pWrapper->CacheGDIPlusObject(pEMF))
	{
		ASSERT(0);
		return;
	}
	CWaitCursor wait;
	theApp.CreateNewFrameForSubEMF(pWrapper->GetEMFAccess());
}

using EMFRecordVisualizerOptional = std::pair<bool, std::shared_ptr<EMFRecordVisualizer>>;

static EMFRecordVisualizerOptional AccessEMRRecordVisualizer(EMFRecAccess* pRec, bool bCheckOnly)
{
	switch (pRec->GetRecordType())
	{
	case emfplus::EmfPlusRecordTypeObject:
	{
		auto pObj = ((EMFRecAccessGDIPlusRecObject*)pRec)->GetObjectWrapper()->GetObject();
		if (pObj)
		{
			switch (pObj->GetObjType())
			{
			case emfplus::OObjType::Image:
				{
					auto pImg = (emfplus::OEmfPlusImage*)pObj;
					switch (pImg->Type)
					{
					case emfplus::OImageDataType::Metafile:
						return std::make_pair(true, bCheckOnly ? nullptr : 
							std::make_shared<EMFRecordMetafileVisualizer>((EMFRecAccessGDIPlusRecObject*)pRec));
					}
				}
				break;
			}
		}
	}
	break;
	case emfplus::EmfPlusRecordTypeDrawImage:
	case emfplus::EmfPlusRecordTypeDrawImagePoints:
		if (pRec->GetLinkedRecordCount())
		{
			pRec = pRec->GetLinkedRecord(EMFRecAccess::LinkedObjTypeImage);
			if (pRec)
				return AccessEMRRecordVisualizer(pRec, bCheckOnly);
		}
		break;
	}
	return std::make_pair(false, nullptr);
}

LRESULT CMainFrame::OnCanOpenRecordItem(WPARAM wp, LPARAM /*lp*/)
{
	auto nIndex = (size_t)wp;
	auto pView = DYNAMIC_DOWNCAST(CEMFExplorerView, GetActiveView());
	auto pDoc = pView->GetDocument();
	auto pEMF = pDoc->GetEMFAccess();
	auto pRec = pEMF->GetRecord((size_t)nIndex);
	return AccessEMRRecordVisualizer(pRec, true).first;
}

LRESULT CMainFrame::OnOpenRecordItem(WPARAM wp, LPARAM /*lp*/)
{
	auto nIndex = (size_t)wp;
	auto pView = DYNAMIC_DOWNCAST(CEMFExplorerView, GetActiveView());
	auto pDoc = pView->GetDocument();
	auto pEMF = pDoc->GetEMFAccess();
	auto pRec = pEMF->GetRecord((size_t)nIndex);
	auto vis = AccessEMRRecordVisualizer(pRec, false);
	if (!vis.second)
		return 0;
	vis.second->OpenVisualizer(this, pEMF.get());
	return 1;
}

LRESULT CMainFrame::OnViewUpdateSizeScroll(WPARAM /*wp*/, LPARAM /*lp*/)
{
	m_wndThumbnail.OnViewUpdateSizeScroll();
	return 0;
}

void CMainFrame::OnUpdateStatusBarColorText(CCmdUI* pCmdUI)
{
	auto pView = DYNAMIC_DOWNCAST(CEMFExplorerView, GetActiveView());
	CString str;
	BOOL bValid = pView->HasValidEMFInDoc();
	pCmdUI->Enable(bValid);
	if (bValid)
	{
		COLORREF clr = pView->GetCursorColor();
		str.Format(_T(" RGB(%d, %d, %d) "), GetRValue(clr), GetGValue(clr), GetBValue(clr));
		m_wndStatusBar.SetPaneBackgroundColor(CMainStatusBar::StatusBarIndexColor, clr, TRUE);
	}
	else
	{
		str.LoadString(ID_STATUSBAR_PANE_COLOR_TEXT);
	}
	pCmdUI->SetText(str);
}

BOOL CMainFrame::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle, CWnd* pParentWnd, CCreateContext* pContext)
{
	// base class does the real work

	// do not show the title with document name (because there's none)
	dwDefaultStyle &= ~FWS_ADDTOTITLE;
	if (!CFrameWndEx::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd, pContext))
	{
		return FALSE;
	}

	CWinApp* pApp = AfxGetApp();
	if (pApp->m_pMainWnd == nullptr)
		pApp->m_pMainWnd = this;

	// enable customization button for all user toolbars
	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	for (int i = 0; i < iMaxUserToolbars; i ++)
	{
		CMFCToolBar* pUserToolbar = GetUserToolBarByIndex(i);
		if (pUserToolbar != nullptr)
		{
			pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
		}
	}

	return TRUE;
}

BOOL CMainFrame::LoadEMFFromData(const std::vector<emfplus::u8t>& data, CEMFExplorerDoc::EMFType type)
{
	CWaitCursor wait;

	LoadEMFDataEvent(true);

	auto pView = DYNAMIC_DOWNCAST(CEMFExplorerView, GetActiveView());
	auto pDoc = pView->GetDocument();

	pDoc->UpdateEMFData(data, type);

	LoadEMFDataEvent(false);

	return TRUE;
}

// This is only necessary during loading of frame window
CEMFExplorerView* CMainFrame::CheckGetActiveView() const
{
	auto pView = DYNAMIC_DOWNCAST(CEMFExplorerView, GetActiveView());
	if (!pView)
	{
		// This can happen for sub EMF frame
		CWnd* pWnd = GetDescendantWindow(AFX_IDW_PANE_FIRST, TRUE);
		if (pWnd != NULL && pWnd->IsKindOf(RUNTIME_CLASS(CView)))
			pView = (CEMFExplorerView*)pWnd;
	}
	ASSERT(pView);
	return pView;
}

void CMainFrame::LoadEMFDataEvent(bool bBefore)
{
	auto pView = CheckGetActiveView();
	if (!pView)
		return;

	if (bBefore && !IsSubEMFFrame())
	{
		// Make a copy for safety
		CList<CFrameWnd*, CFrameWnd*> lstFramesCopy;
		auto& lstFrames = CFrameImpl::GetFrameList();
		for (POSITION pos = lstFrames.GetHeadPosition(); pos != NULL;)
		{
			CFrameWnd* pFrame = lstFrames.GetNext(pos);
			lstFramesCopy.AddTail(pFrame);
		}
		for (POSITION pos = lstFramesCopy.GetHeadPosition(); pos != NULL;)
		{
			CFrameWnd* pFrame = lstFramesCopy.GetNext(pos);

			if (CWnd::FromHandlePermanent(pFrame->m_hWnd) != NULL)
			{
				ASSERT_VALID(pFrame);
				if (pFrame->GetSafeHwnd() != GetSafeHwnd())
				{
					pFrame->SendMessage(WM_CLOSE);
				}
			}
		}
	}

	pView->LoadEMFDataEvent(bBefore);
	if (bBefore)
	{
		m_wndProperties.Reset();
	}
	else
	{
		auto pDoc = pView->GetDocument();
		auto emf = pDoc->GetEMFAccess();
		emf->GetRecords();
		m_wndFileView.SetEMFAccess(emf);
		m_wndThumbnail.SetEMFAccess(emf);
		if (emf->GetNestedPath().empty())
		{
			switch (pDoc->GetEMFType())
			{
			case CEMFExplorerDoc::EMFType::FromFile:
			case CEMFExplorerDoc::EMFType::FromClipboard:
				emf->AddNestedPath(pDoc->GetTitle());
				break;
			case CEMFExplorerDoc::EMFType::FromEMFRecord:
				// Nothing is needed since it's already taken care of
				break;
			}
		}
	}
	m_wndFileView.LoadEMFDataEvent(bBefore);
	m_wndThumbnail.LoadEMFDataEvent(bBefore);
	if (!bBefore)
	{
		UpdateViewOnRecord(0);
		m_wndFileView.SetCurSelRecIndex(0);
	}
}

