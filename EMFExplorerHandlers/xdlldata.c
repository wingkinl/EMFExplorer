// wrapper for dlldata.c

#define REGISTER_PROXY_DLL //DllRegisterServer, etc.

#define USE_STUBLESS_PROXY	//defined only with MIDL switch /Oicf

#pragma comment(lib, "rpcns4.lib")
#pragma comment(lib, "rpcrt4.lib")

#define ENTRY_PREFIX	Prx

#include "dlldata.c"
#include "EMFExplorerHandlers_p.c"
