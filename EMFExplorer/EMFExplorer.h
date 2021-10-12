
// EMFExplorer.h : main header file for the EMFExplorer application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CEMFExplorerApp:
// See EMFExplorer.cpp for the implementation of this class
//

class CEMFExplorerApp : public CWinAppEx
{
public:
	CEMFExplorerApp() noexcept;


// Overrides
public:
	virtual BOOL InitInstance();

// Implementation
	BOOL  m_bHiColorIcons;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CEMFExplorerApp theApp;
