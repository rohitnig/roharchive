#include "ArchiveExtractor.h"
#include "../extraction-engine/ArchiveExtractor.h" // Extraction engine header
#include <strsafe.h>
#include <shlwapi.h>
#include <shellapi.h>
#include <algorithm>
#include <filesystem>

#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "shell32.lib")

// Global variables
HINSTANCE g_hInstance = NULL;
ULONG g_refCount = 0;

// Define the CLSID
// {B4F4A7E0-8F4D-4B5E-9C3A-1F2E3D4C5B6A}
const GUID CLSID_ArchiveExtractor = 
{ 0xB4F4A7E0, 0x8F4D, 0x4B5E, { 0x9C, 0x3A, 0x1F, 0x2E, 0x3D, 0x4C, 0x5B, 0x6A } };

// Constructor
ArchiveExtractor::ArchiveExtractor() : m_refCount(1)
{
    InterlockedIncrement(&g_refCount);
}

// Destructor
ArchiveExtractor::~ArchiveExtractor()
{
    InterlockedDecrement(&g_refCount);
}

// IUnknown implementation
STDMETHODIMP ArchiveExtractor::QueryInterface(REFIID riid, void** ppv)
{
    if (ppv == nullptr)
    {
        return E_POINTER;
    }

    *ppv = nullptr;

    if (IsEqualIID(riid, IID_IUnknown))
    {
        *ppv = static_cast<IUnknown*>(static_cast<IContextMenu*>(this));
    }
    else if (IsEqualIID(riid, IID_IContextMenu))
    {
        *ppv = static_cast<IContextMenu*>(this);
    }
    else if (IsEqualIID(riid, IID_IShellExtInit))
    {
        *ppv = static_cast<IShellExtInit*>(this);
    }
    else
    {
        return E_NOINTERFACE;
    }

    AddRef();
    return S_OK;
}

STDMETHODIMP_(ULONG) ArchiveExtractor::AddRef()
{
    return InterlockedIncrement(&m_refCount);
}

STDMETHODIMP_(ULONG) ArchiveExtractor::Release()
{
    ULONG refCount = InterlockedDecrement(&m_refCount);
    if (refCount == 0)
    {
        delete this;
    }
    return refCount;
}

// IShellExtInit implementation
STDMETHODIMP ArchiveExtractor::Initialize(LPCITEMIDLIST pidlFolder, IDataObject* pDataObj, HKEY hKeyProgID)
{
    if (pDataObj == nullptr)
    {
        return E_INVALIDARG;
    }

    HRESULT hr = S_OK;
    FORMATETC formatetc = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
    STGMEDIUM stgmedium = { TYMED_HGLOBAL };

    // Get the file list from the data object
    hr = pDataObj->GetData(&formatetc, &stgmedium);
    if (SUCCEEDED(hr))
    {
        HDROP hdrop = static_cast<HDROP>(GlobalLock(stgmedium.hGlobal));
        if (hdrop != nullptr)
        {
            UINT fileCount = DragQueryFile(hdrop, 0xFFFFFFFF, NULL, 0);
            m_selectedFiles.clear();
            m_selectedFiles.reserve(fileCount);

            for (UINT i = 0; i < fileCount; i++)
            {
                UINT pathLength = DragQueryFile(hdrop, i, NULL, 0);
                if (pathLength > 0)
                {
                    std::vector<wchar_t> buffer(pathLength + 1);
                    if (DragQueryFile(hdrop, i, buffer.data(), pathLength + 1) > 0)
                    {
                        std::wstring filePath(buffer.data());
                        if (IsArchiveFile(filePath))
                        {
                            m_selectedFiles.push_back(filePath);
                        }
                    }
                }
            }

            GlobalUnlock(stgmedium.hGlobal);
        }

        ReleaseStgMedium(&stgmedium);
    }

    // Only show context menu if we have at least one archive file
    return m_selectedFiles.empty() ? S_FALSE : S_OK;
}

// IContextMenu implementation
STDMETHODIMP ArchiveExtractor::QueryContextMenu(HMENU hmenu, UINT indexMenu, UINT idCmdFirst, UINT idCmdLast, UINT uFlags)
{
    // Don't add menu items if no archive files are selected
    if (m_selectedFiles.empty())
    {
        return MAKE_HRESULT(SEVERITY_SUCCESS, 0, 0);
    }

    // Don't add items during a default verb query or when asked to be quiet
    if (uFlags & (CMF_DEFAULTONLY | CMF_VERBSONLY))
    {
        return MAKE_HRESULT(SEVERITY_SUCCESS, 0, 0);
    }

    UINT currentIndex = indexMenu;
    UINT commandCount = 0;

    // Add separator before our menu items
    if (indexMenu > 0)
    {
        InsertMenu(hmenu, currentIndex++, MF_BYPOSITION | MF_SEPARATOR, 0, NULL);
    }

    // Add "Extract Here" menu item
    AddContextMenuItem(hmenu, idCmdFirst + MENU_EXTRACT_HERE, currentIndex++, 
                      L"Extract Here", L"Extract archive to current folder");
    commandCount++;

    // Add "Extract to Folder" menu item
    AddContextMenuItem(hmenu, idCmdFirst + MENU_EXTRACT_TO_FOLDER, currentIndex++, 
                      L"Extract to Folder...", L"Extract archive to a new folder");
    commandCount++;

    // Add separator after our menu items
    InsertMenu(hmenu, currentIndex++, MF_BYPOSITION | MF_SEPARATOR, 0, NULL);

    return MAKE_HRESULT(SEVERITY_SUCCESS, 0, commandCount);
}

STDMETHODIMP ArchiveExtractor::InvokeCommand(LPCMINVOKECOMMANDINFO lpici)
{
    if (lpici == nullptr)
    {
        return E_INVALIDARG;
    }

    // Check if the command is specified by string or index
    if (HIWORD(lpici->lpVerb) != 0)
    {
        // Command specified by string - not currently supported
        return E_FAIL;
    }

    // Command specified by index
    UINT commandIndex = LOWORD(lpici->lpVerb);
    
    try
    {
        switch (commandIndex)
        {
        case MENU_EXTRACT_HERE:
            // Extract all selected archives to their current directories
            for (const auto& archivePath : m_selectedFiles)
            {
                std::wstring extractPath = std::filesystem::path(archivePath).parent_path();
                ExtractArchive(archivePath, extractPath);
            }
            break;

        case MENU_EXTRACT_TO_FOLDER:
            // Extract all selected archives to new folders with archive names
            for (const auto& archivePath : m_selectedFiles)
            {
                std::wstring extractPath = GetDefaultExtractionPath(archivePath);
                ExtractArchive(archivePath, extractPath);
            }
            break;

        default:
            return E_INVALIDARG;
        }
    }
    catch (...)
    {
        // Log error and show user-friendly message
        MessageBox(lpici->hwnd, L"An error occurred while extracting the archive.", 
                  L"Archive Extractor", MB_OK | MB_ICONERROR);
        return E_FAIL;
    }

    return S_OK;
}

STDMETHODIMP ArchiveExtractor::GetCommandString(UINT_PTR idCmd, UINT uType, UINT* pReserved, LPSTR pszName, UINT cchMax)
{
    if (uType == GCS_HELPTEXTW)
    {
        LPCWSTR helpText = nullptr;
        
        switch (idCmd)
        {
        case MENU_EXTRACT_HERE:
            helpText = L"Extract the selected archive(s) to the current folder";
            break;
        case MENU_EXTRACT_TO_FOLDER:
            helpText = L"Extract the selected archive(s) to new folder(s)";
            break;
        default:
            return E_INVALIDARG;
        }

        return StringCchCopyW(reinterpret_cast<LPWSTR>(pszName), cchMax, helpText);
    }

    return E_INVALIDARG;
}

// Helper method implementations
bool ArchiveExtractor::IsArchiveFile(const std::wstring& fileName) const
{
    std::wstring extension = GetFileExtension(fileName);
    return IsSupportedFormat(extension);
}

bool ArchiveExtractor::IsSupportedFormat(const std::wstring& extension) const
{
    // Convert to lowercase for comparison
    std::wstring lowerExt = extension;
    std::transform(lowerExt.begin(), lowerExt.end(), lowerExt.begin(), ::towlower);
    
    return (lowerExt == L".gz" || 
            lowerExt == L".bz2" || 
            lowerExt == L".tar" ||
            lowerExt == L".tar.gz" ||
            lowerExt == L".tar.bz2" ||
            lowerExt == L".tgz" ||
            lowerExt == L".tbz2");
}

std::wstring ArchiveExtractor::GetFileExtension(const std::wstring& fileName) const
{
    std::filesystem::path filePath(fileName);
    
    // Handle compound extensions like .tar.gz
    std::wstring extension = filePath.extension().wstring();
    if (extension == L".gz" || extension == L".bz2")
    {
        std::wstring stem = filePath.stem().wstring();
        // C++17 compatible ends_with check
        const std::wstring tarExt = L".tar";
        if (stem.length() >= tarExt.length() && 
            stem.compare(stem.length() - tarExt.length(), tarExt.length(), tarExt) == 0)
        {
            extension = L".tar" + extension;
        }
    }
    
    return extension;
}

void ArchiveExtractor::ExtractArchive(const std::wstring& archivePath, const std::wstring& destinationPath)
{
    try {
        // Create extraction engine
        auto extractor = ArchiveEngine::ArchiveExtractorFactory::CreateExtractor(archivePath);
        
        if (!extractor) {
            std::wstring extension = GetFileExtension(archivePath);
            if (extension == L".gz" || extension == L".bz2" || extension == L".tar.gz" || extension == L".tar.bz2") {
                MessageBox(NULL, 
                    L"Compressed archive formats (.gz, .bz2, .tar.gz, .tar.bz2) are not yet implemented.\n"
                    L"They will be available in the next update when compression library integration is complete.\n\n"
                    L"Currently supported: .tar archives only.",
                    L"Archive Extractor - Format Not Yet Available", MB_OK | MB_ICONINFORMATION);
            } else {
                MessageBox(NULL, 
                    (L"Unsupported archive format: " + extension + 
                     L"\n\nSupported formats: .tar\nComing soon: .gz, .bz2, .tar.gz, .tar.bz2").c_str(),
                    L"Archive Extractor - Unsupported Format", MB_OK | MB_ICONWARNING);
            }
            return;
        }

        // Progress callback for updating user
        auto progressCallback = [](uint64_t current, uint64_t total, const std::wstring& fileName, const std::wstring& operation) -> bool {
            // For now, we'll just continue without showing progress dialog
            // TODO: Implement proper progress dialog in Phase 5
            return true; // Continue extraction
        };

        // Perform extraction
        auto result = extractor->Extract(archivePath, destinationPath, progressCallback);

        if (result.success) {
            std::wstring message = L"Successfully extracted " + std::to_wstring(result.extractedFiles.size()) + 
                                  L" files from: " + archivePath + 
                                  L"\nTo: " + destinationPath + 
                                  L"\n\nProcessed: " + ArchiveEngine::Utils::FormatFileSize(result.bytesProcessed) +
                                  L"\nTime: " + ArchiveEngine::Utils::FormatDuration(result.timeElapsed);
            
            MessageBox(NULL, message.c_str(), L"Archive Extractor - Success", MB_OK | MB_ICONINFORMATION);
            
            // Open destination folder in Explorer
            ShellExecute(NULL, L"explore", destinationPath.c_str(), NULL, NULL, SW_SHOWNORMAL);
        } else {
            std::wstring message = L"Failed to extract archive: " + archivePath + 
                                  L"\n\nError: " + result.errorMessage;
            MessageBox(NULL, message.c_str(), L"Archive Extractor - Error", MB_OK | MB_ICONERROR);
        }
        
    } catch (const ArchiveEngine::ArchiveException& e) {
        std::wstring errorMsg = std::wstring(e.what(), e.what() + strlen(e.what()));
        std::wstring message = L"Archive extraction error: " + errorMsg + 
                              L"\nArchive: " + archivePath;
        MessageBox(NULL, message.c_str(), L"Archive Extractor - Error", MB_OK | MB_ICONERROR);
    } catch (const std::exception& e) {
        std::wstring errorMsg = std::wstring(e.what(), e.what() + strlen(e.what()));
        std::wstring message = L"Unexpected error: " + errorMsg + 
                              L"\nArchive: " + archivePath;
        MessageBox(NULL, message.c_str(), L"Archive Extractor - Error", MB_OK | MB_ICONERROR);
    }
}

std::wstring ArchiveExtractor::GetDefaultExtractionPath(const std::wstring& archivePath) const
{
    std::filesystem::path filePath(archivePath);
    std::filesystem::path parentPath = filePath.parent_path();
    std::wstring baseName = filePath.stem().wstring();
    
    // Remove .tar from compound extensions (C++17 compatible)
    const std::wstring tarExt = L".tar";
    if (baseName.length() >= tarExt.length() && 
        baseName.compare(baseName.length() - tarExt.length(), tarExt.length(), tarExt) == 0)
    {
        baseName = baseName.substr(0, baseName.length() - 4);
    }
    
    return (parentPath / baseName).wstring();
}

void ArchiveExtractor::AddContextMenuItem(HMENU hmenu, UINT id, UINT index, const wchar_t* text, const wchar_t* help)
{
    InsertMenu(hmenu, index, MF_BYPOSITION | MF_STRING, id, text);
}