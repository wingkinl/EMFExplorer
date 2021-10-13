// dllmain.cpp : Implementation of DllMain.

#include "pch.h"
#include "framework.h"
#include "resource.h"
#include "EMFExplorerHandlers_i.h"
#include "dllmain.h"
#include "xdlldata.h"

CEMFExplorerHandlersModule _AtlModule;

class CEMFExplorerHandlersApp : public CWinApp
{
public:

// Overrides
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(CEMFExplorerHandlersApp, CWinApp)
END_MESSAGE_MAP()

CEMFExplorerHandlersApp theApp;

BOOL CEMFExplorerHandlersApp::InitInstance()
{
	if (!PrxDllMain(m_hInstance, DLL_PROCESS_ATTACH, nullptr))
		return FALSE;
	return CWinApp::InitInstance();
}

int CEMFExplorerHandlersApp::ExitInstance()
{
	return CWinApp::ExitInstance();
}
