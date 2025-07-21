#include "ArchiveExtractor.h"

// ClassFactory Constructor
ClassFactory::ClassFactory() : m_refCount(1)
{
    InterlockedIncrement(&g_refCount);
}

// ClassFactory Destructor
ClassFactory::~ClassFactory()
{
    InterlockedDecrement(&g_refCount);
}

// IUnknown implementation for ClassFactory
STDMETHODIMP ClassFactory::QueryInterface(REFIID riid, void** ppv)
{
    if (ppv == nullptr)
    {
        return E_POINTER;
    }

    *ppv = nullptr;

    if (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IClassFactory))
    {
        *ppv = static_cast<IClassFactory*>(this);
        AddRef();
        return S_OK;
    }

    return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) ClassFactory::AddRef()
{
    return InterlockedIncrement(&m_refCount);
}

STDMETHODIMP_(ULONG) ClassFactory::Release()
{
    ULONG refCount = InterlockedDecrement(&m_refCount);
    if (refCount == 0)
    {
        delete this;
    }
    return refCount;
}

// IClassFactory implementation
STDMETHODIMP ClassFactory::CreateInstance(IUnknown* pUnkOuter, REFIID riid, void** ppv)
{
    if (ppv == nullptr)
    {
        return E_POINTER;
    }

    *ppv = nullptr;

    // This class does not support aggregation
    if (pUnkOuter != nullptr)
    {
        return CLASS_E_NOAGGREGATION;
    }

    // Create the ArchiveExtractor instance
    ArchiveExtractor* pArchiveExtractor = new(std::nothrow) ArchiveExtractor();
    if (pArchiveExtractor == nullptr)
    {
        return E_OUTOFMEMORY;
    }

    // Query for the requested interface
    HRESULT hr = pArchiveExtractor->QueryInterface(riid, ppv);
    pArchiveExtractor->Release();

    return hr;
}

STDMETHODIMP ClassFactory::LockServer(BOOL fLock)
{
    if (fLock)
    {
        InterlockedIncrement(&g_refCount);
    }
    else
    {
        InterlockedDecrement(&g_refCount);
    }
    
    return S_OK;
}