
// MainFrm.h : interface of the CMainFrame class
//

#pragma once
#include "FileView.h"
#include "PropertiesWnd.h"
#include "EMFExplorerDoc.h"

#define _ENABLE_STATUS_BAR

class CMainFrame : public CFrameWndEx
{
	
protected: // create from serialization only
	CMainFrame() noexcept;
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:

// Operations
public:
	void SetViewBackgroundDark(BOOL bDark);
// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = nullptr, CCreateContext* pContext = nullptr);

	BOOL LoadEMFFromData(const std::vector<emfplus::u8t>& data, CEMFExplorerDoc::EMFType type);

	virtual void LoadEMFDataEvent(bool bBefore);

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	void OnChangeTheme();

protected:  // control bar embedded members
	CMFCMenuBar       m_wndMenuBar;
	CMFCToolBar       m_wndToolBar;
	CMFCStatusBar     m_wndStatusBar;
	CMFCToolBarImages m_UserImages;
	CFileView         m_wndFileView;
	CPropertiesWnd    m_wndProperties;

// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnViewCustomize();
	afx_msg void OnViewBackgroundDark();
	afx_msg void OnViewBackgroundLight();
	afx_msg void OnUpdateViewBackgroundDark(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewBackgroundLight(CCmdUI* pCmdUI);
	afx_msg void OnViewTransparentBkGrid();
	afx_msg void OnUpdateViewTransparentBkGrid(CCmdUI* pCmdUI);
	afx_msg LRESULT OnToolbarCreateNew(WPARAM wp, LPARAM lp);
	DECLARE_MESSAGE_MAP()

	BOOL CreateDockingWindows();
	void SetDockingWindowIcons(BOOL bHiColorIcons);
};


