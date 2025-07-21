#include "ArchiveExtractor.h"
#include <strsafe.h>
#include <comdef.h>

// Registry helper functions implementation

// Register the COM server in the registry
HRESULT RegisterInprocServer(PCWSTR pszModule, const CLSID& clsid, PCWSTR pszFriendlyName)
{
    HRESULT hr = S_OK;
    HKEY hkey = nullptr;
    WCHAR szCLSID[MAX_PATH];
    WCHAR szSubkey[MAX_PATH];

    // Convert CLSID to string
    if (FAILED(StringFromGUID2(clsid, szCLSID, ARRAYSIZE(szCLSID))))
    {
        return E_FAIL;
    }

    // Create the HKCR\CLSID\{CLSID} key
    hr = StringCchPrintf(szSubkey, ARRAYSIZE(szSubkey), L"CLSID\\%s", szCLSID);
    if (SUCCEEDED(hr))
    {
        hr = HRESULT_FROM_WIN32(RegCreateKeyEx(HKEY_CLASSES_ROOT, szSubkey, 0, 
            nullptr, REG_OPTION_NON_VOLATILE, KEY_WRITE, nullptr, &hkey, nullptr));
        
        if (SUCCEEDED(hr))
        {
            // Set the default value to the friendly name
            hr = HRESULT_FROM_WIN32(RegSetValueEx(hkey, nullptr, 0, REG_SZ, 
                reinterpret_cast<const BYTE*>(pszFriendlyName), 
                (lstrlen(pszFriendlyName) + 1) * sizeof(WCHAR)));

            RegCloseKey(hkey);
        }
    }

    // Create the HKCR\CLSID\{CLSID}\InProcServer32 key
    if (SUCCEEDED(hr))
    {
        hr = StringCchPrintf(szSubkey, ARRAYSIZE(szSubkey), L"CLSID\\%s\\InProcServer32", szCLSID);
        if (SUCCEEDED(hr))
        {
            hr = HRESULT_FROM_WIN32(RegCreateKeyEx(HKEY_CLASSES_ROOT, szSubkey, 0, 
                nullptr, REG_OPTION_NON_VOLATILE, KEY_WRITE, nullptr, &hkey, nullptr));
            
            if (SUCCEEDED(hr))
            {
                // Set the default value to the DLL path
                hr = HRESULT_FROM_WIN32(RegSetValueEx(hkey, nullptr, 0, REG_SZ, 
                    reinterpret_cast<const BYTE*>(pszModule), 
                    (lstrlen(pszModule) + 1) * sizeof(WCHAR)));

                if (SUCCEEDED(hr))
                {
                    // Set the threading model
                    PCWSTR pszThreadingModel = L"Apartment";
                    hr = HRESULT_FROM_WIN32(RegSetValueEx(hkey, L"ThreadingModel", 0, REG_SZ, 
                        reinterpret_cast<const BYTE*>(pszThreadingModel), 
                        (lstrlen(pszThreadingModel) + 1) * sizeof(WCHAR)));
                }

                RegCloseKey(hkey);
            }
        }
    }

    return hr;
}

// Unregister the COM server from the registry
HRESULT UnregisterInprocServer(const CLSID& clsid)
{
    HRESULT hr = S_OK;
    WCHAR szCLSID[MAX_PATH];
    WCHAR szSubkey[MAX_PATH];

    // Convert CLSID to string
    if (FAILED(StringFromGUID2(clsid, szCLSID, ARRAYSIZE(szCLSID))))
    {
        return E_FAIL;
    }

    // Delete the HKCR\CLSID\{CLSID} key and all its subkeys
    hr = StringCchPrintf(szSubkey, ARRAYSIZE(szSubkey), L"CLSID\\%s", szCLSID);
    if (SUCCEEDED(hr))
    {
        hr = HRESULT_FROM_WIN32(RegDeleteTree(HKEY_CLASSES_ROOT, szSubkey));
        if (hr == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
        {
            // If the key doesn't exist, consider it a success
            hr = S_OK;
        }
    }

    return hr;
}

// Register the shell extension for specific file types
HRESULT RegisterShellExtContextMenuHandler(PCWSTR pszFileType, const CLSID& clsid, PCWSTR pszFriendlyName)
{
    HRESULT hr = S_OK;
    HKEY hkey = nullptr;
    WCHAR szCLSID[MAX_PATH];
    WCHAR szSubkey[MAX_PATH];

    // Convert CLSID to string
    if (FAILED(StringFromGUID2(clsid, szCLSID, ARRAYSIZE(szCLSID))))
    {
        return E_FAIL;
    }

    // Register for the specified file type
    // HKCR\<filetype>\shellex\ContextMenuHandlers\<friendly name> = {CLSID}
    hr = StringCchPrintf(szSubkey, ARRAYSIZE(szSubkey), 
        L"%s\\shellex\\ContextMenuHandlers\\%s", pszFileType, pszFriendlyName);
    
    if (SUCCEEDED(hr))
    {
        hr = HRESULT_FROM_WIN32(RegCreateKeyEx(HKEY_CLASSES_ROOT, szSubkey, 0, 
            nullptr, REG_OPTION_NON_VOLATILE, KEY_WRITE, nullptr, &hkey, nullptr));
        
        if (SUCCEEDED(hr))
        {
            // Set the default value to our CLSID
            hr = HRESULT_FROM_WIN32(RegSetValueEx(hkey, nullptr, 0, REG_SZ, 
                reinterpret_cast<const BYTE*>(szCLSID), 
                (lstrlen(szCLSID) + 1) * sizeof(WCHAR)));

            RegCloseKey(hkey);
        }
    }

    return hr;
}

// Unregister the shell extension for specific file types
HRESULT UnregisterShellExtContextMenuHandler(PCWSTR pszFileType, const CLSID& clsid)
{
    UNREFERENCED_PARAMETER(clsid);
    
    HRESULT hr = S_OK;
    WCHAR szSubkey[MAX_PATH];

    // Delete the context menu handler registration
    hr = StringCchPrintf(szSubkey, ARRAYSIZE(szSubkey), 
        L"%s\\shellex\\ContextMenuHandlers\\ArchiveExtractor", pszFileType);
    
    if (SUCCEEDED(hr))
    {
        hr = HRESULT_FROM_WIN32(RegDeleteKey(HKEY_CLASSES_ROOT, szSubkey));
        if (hr == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
        {
            // If the key doesn't exist, consider it a success
            hr = S_OK;
        }
    }

    return hr;
}

// ArchiveExtractor registration implementation
HRESULT ArchiveExtractor::RegisterServer()
{
    HRESULT hr = S_OK;
    WCHAR szModule[MAX_PATH];

    // Get the path to this DLL
    if (GetModuleFileName(g_hInstance, szModule, ARRAYSIZE(szModule)) == 0)
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    // Register the COM server
    hr = RegisterInprocServer(szModule, CLSID_ArchiveExtractor, L"Archive Extractor Shell Extension");
    
    if (SUCCEEDED(hr))
    {
        // Register for all supported file types
        const PCWSTR fileTypes[] = {
            L"*",           // All files (we'll filter in Initialize)
            L".gz",
            L".bz2", 
            L".tar",
            L".tar.gz",
            L".tar.bz2",
            L".tgz",
            L".tbz2"
        };

        for (size_t i = 0; i < ARRAYSIZE(fileTypes) && SUCCEEDED(hr); i++)
        {
            hr = RegisterShellExtContextMenuHandler(fileTypes[i], CLSID_ArchiveExtractor, L"ArchiveExtractor");
        }
    }

    return hr;
}

HRESULT ArchiveExtractor::UnregisterServer()
{
    HRESULT hr = S_OK;

    // Unregister from all supported file types
    const PCWSTR fileTypes[] = {
        L"*",
        L".gz",
        L".bz2",
        L".tar", 
        L".tar.gz",
        L".tar.bz2",
        L".tgz",
        L".tbz2"
    };

    for (size_t i = 0; i < ARRAYSIZE(fileTypes); i++)
    {
        HRESULT hrTemp = UnregisterShellExtContextMenuHandler(fileTypes[i], CLSID_ArchiveExtractor);
        if (FAILED(hrTemp) && SUCCEEDED(hr))
        {
            hr = hrTemp; // Remember the first failure
        }
    }

    // Unregister the COM server
    HRESULT hrTemp = UnregisterInprocServer(CLSID_ArchiveExtractor);
    if (FAILED(hrTemp) && SUCCEEDED(hr))
    {
        hr = hrTemp;
    }

    return hr;
}