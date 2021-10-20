
// EMFExplorer.cpp : Defines the class behaviors for the application.
//

#include "pch.h"
#include "framework.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "EMFExplorer.h"
#include "MainFrm.h"

#include "EMFExplorerDoc.h"
#include "EMFExplorerView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CEMFExplorerApp

BEGIN_MESSAGE_MAP(CEMFExplorerApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CEMFExplorerApp::OnAppAbout)
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
	ON_COMMAND(ID_EDIT_PASTE, &CEMFExplorerApp::OnEditPaste)
END_MESSAGE_MAP()


// CEMFExplorerApp construction

CEMFExplorerApp::CEMFExplorerApp() noexcept
{
	m_bHiColorIcons = TRUE;


	// TODO: replace application ID string below with unique ID string; recommended
	// format for string is CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("EMFExplorer.AppID.NoVersion"));

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

CEMFExplorerApp::~CEMFExplorerApp()
{
	delete m_pSubDocTemplate;
}

// The one and only CEMFExplorerApp object

CEMFExplorerApp theApp;


// CEMFExplorerApp initialization

class CEMFDocTemplate : public CSingleDocTemplate
{
public:
	using CSingleDocTemplate::CSingleDocTemplate;
public:
	bool OpenFromClipboardData(const std::vector<emfplus::u8t>& data);
public:
	CDocument* OpenDocumentFile(LPCTSTR lpszPathName, BOOL bAddToMRU, BOOL bMakeVisible) override;
};

bool CEMFDocTemplate::OpenFromClipboardData(const std::vector<emfplus::u8t>& data)
{
	ASSERT(m_pOnlyDoc);
	CEMFExplorerDoc* pDoc = DYNAMIC_DOWNCAST(CEMFExplorerDoc, m_pOnlyDoc);
	if (!pDoc->OnNewDocument())
	{
		TRACE(traceAppMsg, 0, "CEMFDocTemplate::OpenFromClipboardData returned FALSE.\n");
		return false;
	}
	pDoc->SetTitle(_T("Clipboard"));
	auto pMainFrame = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	if (!pMainFrame->LoadEMFFromData(data, CEMFExplorerDoc::EMFType::FromClipboard))
		return false;
	InitialUpdateFrame(pMainFrame, pDoc);
	return true;
}

CDocument* CEMFDocTemplate::OpenDocumentFile(LPCTSTR lpszPathName, BOOL bAddToMRU, BOOL bMakeVisible)
{
	auto pMainFrame = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	if (pMainFrame)
		pMainFrame->LoadEMFDataEvent(true);
	auto pDoc = CSingleDocTemplate::OpenDocumentFile(lpszPathName, bAddToMRU, bMakeVisible);
	if (pMainFrame)
		pMainFrame->LoadEMFDataEvent(false);
	return pDoc;
}

BOOL CEMFExplorerApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();


	EnableTaskbarInteraction(FALSE);

	// AfxInitRichEdit2() is required to use RichEdit control
	// AfxInitRichEdit2();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));
	LoadStdProfileSettings(4);  // Load standard INI file options (including MRU)

	LoadCustomSettings();

	InitContextMenuManager();

	InitKeyboardManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views
	CEMFDocTemplate* pDocTemplate;
	pDocTemplate = new CEMFDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CEMFExplorerDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CEMFExplorerView));
	if (!pDocTemplate)
		return FALSE;

	AddDocTemplate(pDocTemplate);


	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Enable DDE Execute open
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);

	// Dispatch commands specified on the command line.  Will return FALSE if
	// app was launched with /RegServer, /Register, /Unregserver or /Unregister.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// Always show all menu items
	CMFCMenuBar::SetRecentlyUsedMenus(FALSE);

	// The one and only window has been initialized, so show and update it
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	// call DragAcceptFiles only if there's a suffix
	//  In an SDI app, this should occur after ProcessShellCommand
	// Enable drag/drop open
	m_pMainWnd->DragAcceptFiles();
	return TRUE;
}

int CEMFExplorerApp::ExitInstance()
{
	SaveCustomSettings();
	return CWinAppEx::ExitInstance();
}

BOOL CEMFExplorerApp::IsDarkTheme() const
{
	return m_nStyle == CMFCVisualManagerOffice2007::Office2007_ObsidianBlack;
}

void CEMFExplorerApp::SetDarkTheme(BOOL bDark)
{
	m_nStyle = bDark ? CMFCVisualManagerOffice2007::Office2007_ObsidianBlack : CMFCVisualManagerOffice2007::Office2007_Silver;
}

// CEMFExplorerApp message handlers


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg() noexcept;

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() noexcept : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// App command to run the dialog
void CEMFExplorerApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

void CEMFExplorerApp::OnEditPaste()
{
	if (!m_pMainWnd->OpenClipboard())
	{
		AfxMessageBox(_T("Failed to open clipboard!"), MB_ICONERROR);
		return;
	}
	std::vector<emfplus::u8t> vBuffer;
	BOOL bOK = FALSE;
	auto hEMF = (HENHMETAFILE)GetClipboardData(CF_ENHMETAFILE);
	if (hEMF)
	{
		UINT nSize = GetEnhMetaFileBits(hEMF, 0, nullptr);
		vBuffer.resize((size_t)nSize);
		auto nRead = GetEnhMetaFileBits(hEMF, nSize, vBuffer.data());
		bOK = nRead == nSize;
	}
	else
	{
		AfxMessageBox(_T("Not an EMF format!"), MB_ICONERROR);
	}
	CloseClipboard();
	if (bOK)
	{
		// now we want to show the document name on the title
		m_pMainWnd->ModifyStyle(0, FWS_ADDTOTITLE);
		ASSERT(m_pDocManager);
		POSITION pos = m_pDocManager->GetFirstDocTemplatePosition();
		auto pDocTemp = DYNAMIC_DOWNCAST(CEMFDocTemplate, m_pDocManager->GetNextDocTemplate(pos));
		pDocTemp->OpenFromClipboardData(vBuffer);
	}
}

// CEMFExplorerApp customization load/save methods

void CEMFExplorerApp::PreLoadState()
{
	BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
	bNameValid = strName.LoadString(IDS_EXPLORER);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EXPLORER);
}

void CEMFExplorerApp::LoadCustomState()
{
	
}

void CEMFExplorerApp::SaveCustomState()
{
	
}

const TCHAR cszThemeStyle[] = _T("ThemeStyle");
const TCHAR cszImgBackgroundType[] = _T("BackgroundType");
const TCHAR cszViewCenter[] = _T("ViewCenter");
const TCHAR cszFitWinType[] = _T("FitWinType");

void CEMFExplorerApp::LoadCustomSettings()
{
	m_nStyle = GetInt(cszThemeStyle, CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);
	m_nImgBackgroundType = GetInt(cszImgBackgroundType, CEMFExplorerView::ImgBackgroundTypeTransparentGrid);
	m_bViewCenter = GetInt(cszViewCenter, TRUE);
	m_nFitWinType = GetInt(cszFitWinType, CScrollZoomView::FitToBoth);
}

void CEMFExplorerApp::SaveCustomSettings()
{
	WriteInt(cszThemeStyle, m_nStyle);
	WriteInt(cszImgBackgroundType, m_nImgBackgroundType);
	WriteInt(cszViewCenter, m_bViewCenter);
	WriteInt(cszFitWinType, m_nFitWinType);
}

CDocument* CEMFExplorerApp::OpenDocumentFile(LPCTSTR lpszFileName)
{
	ASSERT(m_pMainWnd && m_pMainWnd->IsKindOf(RUNTIME_CLASS(CMainFrame)));
	// now we want to show the document name on the title
	m_pMainWnd->ModifyStyle(0, FWS_ADDTOTITLE);
	return CWinAppEx::OpenDocumentFile(lpszFileName);
}

// CEMFExplorerApp message handlers

class CSubEMFDocTemplate : public CMultiDocTemplate
{
public:
	using CMultiDocTemplate::CMultiDocTemplate;
public:
	void SetDefaultTitle(CDocument* pDocument) override
	{
		CEMFExplorerDoc* pDoc = DYNAMIC_DOWNCAST(CEMFExplorerDoc, pDocument);
		if (pDoc)
		{
			ASSERT(pDoc->GetEMFType() == CEMFExplorerDoc::EMFType::FromEMFRecord);
			// TODO
			pDocument->SetTitle(_T("Sub EMF"));
			return;
		}
		CMultiDocTemplate::SetDefaultTitle(pDocument);
	}
private:
};

CEMFExplorerDoc* CEMFExplorerApp::CreateNewFrameForSubEMF()
{
	if (!m_pSubDocTemplate)
	{
		m_pSubDocTemplate = new CSubEMFDocTemplate(
			IDR_MAINFRAME,
			RUNTIME_CLASS(CEMFExplorerDoc),
			RUNTIME_CLASS(CMainFrame),       // main SDI frame window
			RUNTIME_CLASS(CEMFExplorerView));
		if (!m_pSubDocTemplate)
			return nullptr;
		m_pSubDocTemplate->LoadTemplate();
	}

	CDocument* pDoc = nullptr;
	CFrameWnd* pFrame = nullptr;

	// Create a new instance of the document referenced
	// by the m_pDocTemplate member.
	if (m_pSubDocTemplate != nullptr)
		pDoc = m_pSubDocTemplate->CreateNewDocument();

	if (pDoc != nullptr)
	{
		// If creation worked, use create a new frame for
		// that document.
		pFrame = m_pSubDocTemplate->CreateNewFrame(pDoc, nullptr);
		if (pFrame != nullptr)
		{
			// Set the title, and initialize the document.
			// If document initialization fails, clean-up
			// the frame window and document.

			m_pSubDocTemplate->SetDefaultTitle(pDoc);
			if (!pDoc->OnNewDocument())
			{
				pFrame->DestroyWindow();
				pFrame = nullptr;
			}
			else
			{
				// Otherwise, update the frame
				m_pSubDocTemplate->InitialUpdateFrame(pFrame, pDoc, TRUE);
			}
		}
	}

	// If we failed, clean up the document and show a
	// message to the user.

	if (pFrame == nullptr || pDoc == nullptr)
	{
		delete pDoc;
		pDoc = nullptr;
		AfxMessageBox(AFX_IDP_FAILED_TO_CREATE_DOC);
	}
	return DYNAMIC_DOWNCAST(CEMFExplorerDoc, pDoc);
}
