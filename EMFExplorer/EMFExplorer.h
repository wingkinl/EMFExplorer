
// EMFExplorer.h : main header file for the EMFExplorer application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols

class CEMFExplorerDoc;

// CEMFExplorerApp:
// See EMFExplorer.cpp for the implementation of this class
//

class CEMFExplorerApp : public CWinAppEx
{
public:
	CEMFExplorerApp() noexcept;

protected:
	CMultiDocTemplate* m_pSubDocTemplate;
public:
	CEMFExplorerDoc* CreateNewFrameForSubEMF();
// Overrides
public:
	virtual BOOL InitInstance();
	int ExitInstance() override;

// Implementation
	BOOL m_bHiColorIcons;
	BOOL m_bBackgroundDark = TRUE;
	BOOL m_bShowTransparentBkGrid = TRUE;
	const COLORREF m_crfBkColorDark = RGB(0x53, 0x53, 0x53);

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CEMFExplorerApp theApp;
