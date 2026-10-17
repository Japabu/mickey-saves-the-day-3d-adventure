// MBMouseUtils.h - declarations for MBEngine.dll MBMouseUtils.obj
#ifndef MBMOUSEUTILS_H
#define MBMOUSEUTILS_H

#include "MBScreenUtils.h"

extern "C" long MBComCreateInstance(IMBUnknown *pOuter, const MBGUID &guid, IMBUnknown **unknown);

// own copy: which members are inline and which are only declared differs per unit (the original's inline budget; NOTES.md)
template <class T>
class MBComPtr {
public:
    MBComPtr(void) : mInterface(0) {}
    MBComPtr(const MBGUID &clsid) : mInterface(0) { CreateObject(clsid); }
    ~MBComPtr(void) { if (mInterface) mInterface->Release(); }
    operator T *(void) const { return mInterface; }
    T *operator->(void) const { return mInterface; }
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
        Attach(0);
        return hr;
    }
    T *mInterface;
};

#include "IMBView.h"

#include "IMBViewManager.h"

extern "C" {
bool MBGetGlobalMouseLocation(MBPoint &globalPoint);
bool MBGetScreenMouseLocation(MBPoint &screenPoint, IMBScreen **screen);
bool MBGetWorldMouseLocation(MBPoint &worldPoint, IMBWorld **world);
}

#endif
