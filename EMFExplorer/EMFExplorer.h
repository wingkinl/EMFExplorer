
// EMFExplorer.h : main header file for the EMFExplorer application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols
#include <memory>

class CEMFExplorerDoc;
class EMFAccess;
class CSubEMFDocTemplate;

// CEMFExplorerApp:
// See EMFExplorer.cpp for the implementation of this class
//

class CEMFExplorerApp : public CWinAppEx
{
public:
	CEMFExplorerApp() noexcept;
	~CEMFExplorerApp();

protected:
	CSubEMFDocTemplate* m_pSubDocTemplate = nullptr;
public:
	bool CreateNewFrameForSubEMF(std::shared_ptr<EMFAccess> emf);
// Overrides
public:
	virtual BOOL InitInstance();
	int ExitInstance() override;

	BOOL IsDarkTheme() const;
	void SetDarkTheme(BOOL bDark);

// Implementation
	BOOL m_bHiColorIcons;
	int m_nStyle = 0;
	int m_nImgBackgroundType = 0;
	BOOL m_bViewCenter = TRUE;

	const COLORREF m_crfDarkThemeBkColor = RGB(0x53, 0x53, 0x53);
	const COLORREF m_crfDarkThemeTxtColor = RGB(0xf1,0xf1,0xf1);

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	void LoadCustomSettings();
	void SaveCustomSettings();

	CDocument* OpenDocumentFile(LPCTSTR lpszFileName) override;

	afx_msg void OnAppAbout();
	afx_msg void OnEditPaste();
	DECLARE_MESSAGE_MAP()
};

extern CEMFExplorerApp theApp;
