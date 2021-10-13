// dllmain.h : Declaration of module class.

class CEMFExplorerHandlersModule : public ATL::CAtlDllModuleT<CEMFExplorerHandlersModule>
{
public :
	DECLARE_LIBID(LIBID_EMFExplorerHandlersLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_EMFEXPLORERHANDLERS, "{1054cbf4-6e39-4711-8a3c-87ebcf9e176b}")
};

extern class CEMFExplorerHandlersModule _AtlModule;
