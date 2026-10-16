// MBMsgBoxWin.h - declarations for MBEngine.dll MBMsgBoxWin.obj
#ifndef MBMSGBOXWIN_H
#define MBMSGBOXWIN_H

#include <windows.h>
#include <string>
extern "C" __declspec(dllimport) void *__cdecl MBmalloc_release(unsigned int size);
extern "C" __declspec(dllimport) void __cdecl MBfree_release(void *p);

template <class T> class MBAllocator : public std::allocator<T> {
public:
    T *allocate(unsigned long n, const void *) { return (T *)MBmalloc_release(n * sizeof(T)); }
    char *_Charalloc(unsigned long n) { return (char *)MBmalloc_release(n); }
    void deallocate(void *p, unsigned long) { if (p) MBfree_release(p); }
};

typedef std::basic_string<unsigned short, std::char_traits<unsigned short>, MBAllocator<unsigned short> > MBString16;

enum MBMsgBoxButtonID {
    kMBMsgBoxIDOK = 0,
    kMBMsgBoxIDCancel = -1,
    kMBMsgBoxIDRetry = 1,
    kMBMsgBoxIDYes = 0,
    kMBMsgBoxIDNo = -1
};

// the default title instantiates MBString16 here, ahead of the colour statics ($E numbering)
extern "C" MBMsgBoxButtonID MBMsgBox(const MBString16 &text, unsigned long flags = 0, const MBString16 &title = MBString16());

#include "MBScreenUtils.h"

extern "C" long MBComCreateInstance(IMBUnknown *pOuter, const MBGUID &guid, IMBUnknown **unknown);

template <class T>
class MBComPtr {
public:
    MBComPtr(void) : mInterface(0) {}
    template <class U> MBComPtr(U *const &p) : mInterface(0) { _QueryInterface(p); }
    ~MBComPtr(void) { if (mInterface) mInterface->Release(); }
    operator T *(void) const { return mInterface; }
    T *operator->(void) const { return mInterface; }
    MBComPtr &operator=(T *p) {
        if (mInterface != p) {
            T *old = mInterface;
            mInterface = p;
            if (p) p->AddRef();
            if (old) old->Release();
        }
        return *this;
    }
    void Release(void) {
        do {
            if (mInterface != 0)
                mInterface->Release();
        } while (0);
        mInterface = 0;
    }
    void Attach(T *p) {
        Release();
        mInterface = p;
    }
    long CreateObject(const MBGUID &clsid);
private:
    template <class U> long _QueryInterface(U *const &p) {
        long hr;
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

class IMBEventListener;

class IMBEventManager : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBEventManager; }
    virtual void BroadcastEvent(long, long, void *) = 0;
    virtual void AddListener(long, IMBEventListener *) = 0;
    virtual void RemoveListener(long, IMBEventListener *) = 0;
    virtual void RemoveListener(IMBEventListener *) = 0;
};

class IMBWinScreen : public IMBScreen {
public:
    virtual void SetHWND(HWND) = 0;
    virtual HWND GetHWND(void) = 0;
};

class IMBDirectXScreen : public IMBWinScreen {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBDirectXScreen; }
};

#endif
