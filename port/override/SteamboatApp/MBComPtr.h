// PORT override: adds operator< for maps keyed by MBComPtr (libc++ finds the T *, T & and bool conversions ambiguous); otherwise a copy of src/SteamboatApp/MBComPtr.h with port/rewrite.py's 32-bit long (int), keep in sync.
#pragma once
// muffin/include/Com/MBComPtr.h: the engine's COM-style smart pointer (members from
// SteamboatApp.pdb; bodies from the out-of-line copies in SteamboatNode.obj and MBEngine).
#include "MBGUID.h"

template <class T> class MBComPtr {
public:
    MBComPtr(const MBGUID &clsid) : mInterface(0) { CreateObject(clsid); }
    MBComPtr(T *p, bool addRef) : mInterface(p) { if (addRef && mInterface) mInterface->AddRef(); }
    MBComPtr(T *p) : mInterface(p) { if (mInterface) mInterface->AddRef(); }
    MBComPtr(int) : mInterface(0) {}
    MBComPtr() : mInterface(0) {}
    template <class U> MBComPtr(const MBComPtr<U> &p) : mInterface(0) { _QueryInterface(p); }
    template <class U> MBComPtr(U *const &p) : mInterface(0) { _QueryInterface(p); }
    MBComPtr(const MBComPtr &ptr) : mInterface(ptr.mInterface) { if (mInterface) mInterface->AddRef(); }
    MBComPtr<T> &operator=(int) { return *this = (T *)0; }
    // Assigning another interface queries for this one (CMBCursor.obj calls _QueryInterface on the
    // static pointers directly). VC6 takes the MBComPtr<U> template only in front of the copy
    // assignment (C2535 after it).
    template <class U> MBComPtr<T> &operator=(const MBComPtr<U> &p)
    {
        _QueryInterface(p);
        return *this;
    }
    MBComPtr<T> &operator=(const MBComPtr<T> &ptr) { return *this = ptr.mInterface; }
    template <class U> MBComPtr<T> &operator=(U *const &p)
    {
        _QueryInterface(p);
        return *this;
    }
    // AddRef() is the member, not pInterface->AddRef(): SBMovieNode::ListenToEvent calls the
    // out-of-line MBComPtr<IMBMovie>::AddRef from its inlined assignment (and DrawCursor matches).
    MBComPtr<T> &operator=(T *pInterface)
    {
        if (mInterface != pInterface) {
            T *old = mInterface;
            mInterface = pInterface;
            AddRef();
            if (old)
                old->Release();
        }
        return *this;
    }
    ~MBComPtr() { Release(); }
    void Attach(T *p, bool addRef)
    {
        Release();
        mInterface = p;
        if (addRef && mInterface)
            mInterface->AddRef();
    }
    void Attach(T *p)
    {
        Release();
        mInterface = p;
    }
    T *Detach()
    {
        T *p = mInterface;
        mInterface = 0;
        return p;
    }
    operator T *() const { return mInterface; }
    operator T &() const { return *mInterface; }
    T &operator*() const { return *mInterface; }
    T *operator->() const { return mInterface; }
    operator bool() const { return mInterface != 0; }
    bool operator==(int) { return mInterface == 0; }
    bool operator==(MBComPtr<T> &p) { return mInterface == p.mInterface; }
    bool operator==(T *p) { return mInterface == p; }
    bool operator!=(int) { return mInterface != 0; }
    bool operator!=(MBComPtr<T> &p) { return mInterface != p.mInterface; }
    bool operator!=(T *p) { return mInterface != p; }
    // PORT: maps keyed by MBComPtr compare the interface pointers (VC6 took the T * conversion)
    bool operator<(const MBComPtr<T> &p) const { return mInterface < p.mInterface; }
    // The do/while(0) block (a release macro in the original, it seems) compiles to the same code
    // as a plain if, but its extra nodes change VC6's inlining budget: with it the callers inline
    // CreateObject, fold Release and call _QueryInterface out of line as the originals do.
    void Release()
    {
        do {
            if (mInterface != 0)
                mInterface->Release();
        } while (0);
        mInterface = 0;
    }
    void AddRef()
    {
        if (mInterface)
            mInterface->AddRef();
    }
    T *GetInterfacePtr() const { return mInterface; }
    // Asks the held interface for another one; -2 when nothing is held (the result goes through hr:
    // the instances copy it into a second register). A MBComPtr<T> built from a
    // pointer to another MBComPtr comes here (SteamboatNode::ClearHotspots passes its iterators).
    int QueryInterface(const MBGUID &iid, void **ppv) const
    {
        int hr;
        if (mInterface)
            hr = mInterface->QueryInterface(iid, ppv);
        else
            hr = -2;
        return hr;
    }
    int CreateObject(const MBGUID &clsid)
    {
        IMBUnknown *unk;
        Release();
        MBComCreateInstance(0, clsid, &unk);
        if (unk) {
            int hr = _QueryInterface(unk);
            unk->Release();
            return hr;
        }
        return -2;
    }
private:
    template <class U> int _QueryInterface(U *const &p)
    {
        int hr;
        if (p) {
            T *pInterface;
            hr = p->QueryInterface(T::GetGUID(), (void **)&pInterface);
            if (hr) {
                Attach(0);
                return hr;
            }
            Attach(pInterface);
        } else {
            *this = 0;
            hr = -3;
        }
        return hr;
    }
    // Same shape as the raw-pointer form (the out-of-line copy in CMBCursor.obj).
    template <class U> int _QueryInterface(const MBComPtr<U> &p)
    {
        int hr;
        if (p) {
            T *pInterface;
            hr = p->QueryInterface(T::GetGUID(), (void **)&pInterface);
            if (hr) {
                Attach(0);
                return hr;
            }
            Attach(pInterface);
        } else {
            *this = 0;
            hr = -3;
        }
        return hr;
    }
    T *mInterface;
};
