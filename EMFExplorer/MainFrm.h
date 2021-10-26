
// MainFrm.h : interface of the CMainFrame class
//

#pragma once
#include "FileView.h"
#include "PropertiesWnd.h"
#include "EMFExplorerDoc.h"
#include "ThumbnailWnd.h"

#define _ENABLE_STATUS_BAR

enum MainFrameMsg
{
	// wParam = record index, lParam = Hover (non-zero)
	MainFrameMsgOnSelectRecordItem = WM_APP + 100,
	// wParam = record index
	MainFrameMsgCanOpenRecordItem,
	// wParam = record index
	MainFrameMsgOpenRecordItem,
	// wParam = Size changed (non-zero) or scroll only (zero)
	MainFrameMsgViewUpdateSizeScroll,
};

class CEMFExplorerView;

using CMainStatusBarBase = CMFCStatusBar;

class CMainStatusBar : public CMainStatusBarBase
{
public:
	enum StatusBarIndex
	{
		StatusBarIndexInfo,
		StatusBarIndexColor,
		StatusBarIndexColorText,
		StatusBarIndexCoord,
		StatusBarIndexZoom,
	};

	static UINT Indicators[];

	BOOL Create(CWnd* pParentWnd);
protected:
	BOOL SetPaneText(int nIndex, LPCTSTR lpszNewText, BOOL bUpdate = TRUE) override;
};

class CMainFrame : public CFrameWndEx
{
	
protected: // create from serialization only
	CMainFrame() noexcept;
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:
	
// Operations
public:
	void SetTheme(BOOL bDark);
// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = nullptr, CCreateContext* pContext = nullptr);

	BOOL LoadEMFFromData(const std::vector<emfplus::u8t>& data, CEMFExplorerDoc::EMFType type);

	virtual void LoadEMFDataEvent(bool bBefore);

	virtual bool IsSubEMFFrame() const { return false; }

	size_t GetDrawToRecordIndex() const;
// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	void OnChangeTheme();

	BOOL CheckClipboardForEMF() const;
private:
	bool UpdateViewOnSelRecord(int index, BOOL bHover = FALSE);

	CEMFExplorerView* CheckGetActiveView() const;
protected:  // control bar embedded members
	CMFCMenuBar       m_wndMenuBar;
	CMFCToolBar       m_wndToolBar;
	CMainStatusBar    m_wndStatusBar;
	CMFCToolBarImages m_UserImages;
	CFileView         m_wndFileView;
	CPropertiesWnd    m_wndProperties;
	CThumbnailWnd     m_wndThumbnail;

	enum DrawToType
	{
		DrawToAll,
		DrawToSelection,
		DrawToHover,
	};

	DrawToType GetDrawToSelection() const {return m_nDrawToType;}

	DrawToType		m_nDrawToType = DrawToAll;
	BOOL			m_bUpdatePropOnHover = FALSE;

// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT OnChangeVisualManager(WPARAM wParam, LPARAM lParam);
	afx_msg void OnViewCustomize();
	afx_msg void OnEditPaste();
	afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
	afx_msg void OnViewThemeDark();
	afx_msg void OnViewThemeLight();
	afx_msg void OnUpdateViewThemeDark(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewThemeLight(CCmdUI* pCmdUI);

	afx_msg LRESULT OnToolbarCreateNew(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnSelectRecordItem(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnCanOpenRecordItem(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnOpenRecordItem(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnViewUpdateSizeScroll(WPARAM wp, LPARAM lp);

	afx_msg void OnUpdateStatusBarColorText(CCmdUI* pCmdUI);

	afx_msg void OnViewDrawToSelection();
	afx_msg void OnUpdateViewDrawToSelection(CCmdUI* pCmdUI);
	afx_msg void OnViewDrawToHover();
	afx_msg void OnUpdateViewDrawToHover(CCmdUI* pCmdUI);
	afx_msg void OnViewUpdatePropOnHover();
	afx_msg void OnUpdateViewUpdatePropOnHover(CCmdUI* pCmdUI);
	DECLARE_MESSAGE_MAP()

	BOOL CreateDockingWindows();
	void SetDockingWindowIcons(BOOL bHiColorIcons);
};


