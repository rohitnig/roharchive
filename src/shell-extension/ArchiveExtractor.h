#pragma once

#include <windows.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <comdef.h>
#include <vector>
#include <string>
#include <memory>

// Forward declarations
class ClassFactory;

// Archive Extractor Shell Extension Class
// Implements IContextMenu and IShellExtInit interfaces for right-click context menu integration
class ArchiveExtractor : public IContextMenu, public IShellExtInit
{
public:
    // Constructor/Destructor
    ArchiveExtractor();
    virtual ~ArchiveExtractor();

    // IUnknown methods
    STDMETHOD(QueryInterface)(REFIID riid, void** ppv);
    STDMETHOD_(ULONG, AddRef)();
    STDMETHOD_(ULONG, Release)();

    // IShellExtInit methods
    STDMETHOD(Initialize)(LPCITEMIDLIST pidlFolder, IDataObject* pDataObj, HKEY hKeyProgID);

    // IContextMenu methods
    STDMETHOD(QueryContextMenu)(HMENU hmenu, UINT indexMenu, UINT idCmdFirst, UINT idCmdLast, UINT uFlags);
    STDMETHOD(InvokeCommand)(LPCMINVOKECOMMANDINFO lpici);
    STDMETHOD(GetCommandString)(UINT_PTR idCmd, UINT uType, UINT* pReserved, LPSTR pszName, UINT cchMax);

    // Static methods for COM registration
    static HRESULT RegisterServer();
    static HRESULT UnregisterServer();

private:
    // Reference counting
    ULONG m_refCount;

    // Selected files from Windows Explorer
    std::vector<std::wstring> m_selectedFiles;

    // Context menu command IDs
    enum MenuCommands {
        MENU_EXTRACT_HERE = 0,
        MENU_EXTRACT_TO_FOLDER = 1,
        MENU_EXTRACT_TO_SUBFOLDER = 2,
        MENU_EXTRACT_AND_DELETE = 3,
        MENU_TEST_ARCHIVE = 4,
        MENU_COMMAND_COUNT = 5
    };

    // Helper methods
    bool IsArchiveFile(const std::wstring& fileName) const;
    bool IsSupportedFormat(const std::wstring& extension) const;
    std::wstring GetFileExtension(const std::wstring& fileName) const;
    void ExtractArchive(const std::wstring& archivePath, const std::wstring& destinationPath, bool showProgress = true);
    std::wstring GetDefaultExtractionPath(const std::wstring& archivePath) const;
    bool TestArchive(const std::wstring& archivePath);
    bool ShouldOverwriteFiles(HWND hwnd, const std::wstring& destinationPath);
    void ShowExtractionComplete(HWND hwnd, int fileCount, const std::wstring& destinationPath);
    
    // Menu creation helpers
    void AddContextMenuItem(HMENU hmenu, UINT id, UINT position, const wchar_t* text, const wchar_t* help);
    HMENU CreateArchiveSubmenu(UINT idCmdFirst);
};

// Class Factory for COM object creation
class ClassFactory : public IClassFactory
{
public:
    ClassFactory();
    virtual ~ClassFactory();

    // IUnknown methods
    STDMETHOD(QueryInterface)(REFIID riid, void** ppv);
    STDMETHOD_(ULONG, AddRef)();
    STDMETHOD_(ULONG, Release)();

    // IClassFactory methods
    STDMETHOD(CreateInstance)(IUnknown* pUnkOuter, REFIID riid, void** ppv);
    STDMETHOD(LockServer)(BOOL fLock);

private:
    ULONG m_refCount;
};

// Global variables and functions
extern HINSTANCE g_hInstance;
extern ULONG g_refCount;

// COM registration GUIDs
// {B4F4A7E0-8F4D-4B5E-9C3A-1F2E3D4C5B6A}
extern const GUID CLSID_ArchiveExtractor;

// Registry helper functions
HRESULT RegisterInprocServer(PCWSTR pszModule, const CLSID& clsid, PCWSTR pszFriendlyName);
HRESULT UnregisterInprocServer(const CLSID& clsid);
HRESULT RegisterShellExtContextMenuHandler(PCWSTR pszFileType, const CLSID& clsid, PCWSTR pszFriendlyName);
HRESULT UnregisterShellExtContextMenuHandler(PCWSTR pszFileType, const CLSID& clsid);