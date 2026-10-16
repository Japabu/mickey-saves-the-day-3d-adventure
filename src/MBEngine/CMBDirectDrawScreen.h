// CMBDirectDrawScreen.h - declarations for MBEngine.dll CMBDirectDrawScreen.obj
// (the class itself and the DirectDraw 7 pieces live in CMBDirect3DScreen.h)
#ifndef CMBDIRECTDRAWSCREEN_H
#define CMBDIRECTDRAWSCREEN_H

#include "CMBDirect3DScreen.h"

// DirectDraw 7 entry points (static ddraw.lib thunks, not in the DirectX 5 headers)
extern "C" const GUID IID_IDirectDraw7;
extern "C" HRESULT __stdcall DirectDrawCreateEx(GUID *, void **, const IID &, IUnknown *);
extern "C" HRESULT __stdcall DirectDrawEnumerateExA(MBDDEnumCallback, void *, DWORD);

extern "C" unsigned char MBShowSystemCursor(unsigned char show);

class IMBEventManager : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBEventManager; }
    virtual void BroadcastEvent(long, long, void *) = 0;
    virtual void AddListener(long, IMBEventListener *) = 0;
    // VC6 lays out overloaded virtuals in reverse declaration order
    virtual void RemoveListener(long, IMBEventListener *) = 0; // vftable+0x18
    virtual void RemoveListener(IMBEventListener *) = 0;       // vftable+0x14
};

template <class T> class MBComPtr {
public:
    MBComPtr(void) : mInterface(0) {}
    MBComPtr(const MBGUID &clsid) : mInterface(0) { CreateObject(clsid); }
    ~MBComPtr(void) { if (mInterface) mInterface->Release(); }
    operator T *(void) const { return mInterface; }
    T *operator->(void) const { return mInterface; }
    void Release(void) {
        if (mInterface) {
            mInterface->Release();
            mInterface = 0;
        }
    }
    MBComPtr &operator=(T *p) {
        if (mInterface != p) {
            T *old = mInterface;
            mInterface = p;
            if (p) p->AddRef();
            if (old) old->Release();
        }
        return *this;
    }
    void Attach(T *p) {
        Release();
        mInterface = p;
    }
    long CreateObject(const MBGUID &clsid) {
        IMBUnknown *unk;
        Release();
        MBComCreateInstance(0, clsid, &unk);
        if (unk) {
            long hr = _QueryInterface(unk);
            unk->Release();
            return hr;
        }
        return -2;
    }
private:
    long _QueryInterface(IMBUnknown *const &p) {
        long hr = -3;
        if (p) {
            T *pInterface;
            hr = p->QueryInterface(T::GetGUID(), (void **)&pInterface);
            if (hr) {
                Attach(0);
                return hr;
            }
            Attach(pInterface);
            return hr;
        }
        *this = 0;
        return hr;
    }
    T *mInterface;
};

IMBUnknown *MakeCMBDirectDrawScreen(void);

#endif
