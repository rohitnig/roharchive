#include "ArchiveExtractor.h"
#include <strsafe.h>

// DLL Entry Point
BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
    UNREFERENCED_PARAMETER(lpReserved);

    switch (dwReason)
    {
    case DLL_PROCESS_ATTACH:
        // Store the module instance handle for later use
        g_hInstance = hModule;
        
        // Disable thread library calls for performance
        DisableThreadLibraryCalls(hModule);
        break;

    case DLL_PROCESS_DETACH:
        break;

    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
        // We disabled thread library calls, so these should not occur
        break;
    }

    return TRUE;
}

// DLL Exported Functions for COM Registration

// DllCanUnloadNow - Called by COM to determine if the DLL can be unloaded
STDAPI DllCanUnloadNow()
{
    // Return S_OK if no objects are currently in use, S_FALSE otherwise
    return (g_refCount == 0) ? S_OK : S_FALSE;
}

// DllGetClassObject - Called by COM to get a class factory for the requested class
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, void** ppv)
{
    if (ppv == nullptr)
    {
        return E_POINTER;
    }

    *ppv = nullptr;

    // Check if the requested class ID matches our ArchiveExtractor
    if (IsEqualCLSID(rclsid, CLSID_ArchiveExtractor))
    {
        // Create and return our class factory
        ClassFactory* pClassFactory = new(std::nothrow) ClassFactory();
        if (pClassFactory == nullptr)
        {
            return E_OUTOFMEMORY;
        }

        HRESULT hr = pClassFactory->QueryInterface(riid, ppv);
        pClassFactory->Release();
        return hr;
    }

    return CLASS_E_CLASSNOTAVAILABLE;
}

// DllRegisterServer - Called to register the DLL with the system
STDAPI DllRegisterServer()
{
    return ArchiveExtractor::RegisterServer();
}

// DllUnregisterServer - Called to unregister the DLL from the system
STDAPI DllUnregisterServer()
{
    return ArchiveExtractor::UnregisterServer();
}