// MBMouseUtils.h - declarations for MBEngine.dll MBMouseUtils.obj
#ifndef MBMOUSEUTILS_H
#define MBMOUSEUTILS_H

#include "MBScreenUtils.h"

extern "C" long MBComCreateInstance(IMBUnknown *pOuter, const MBGUID &guid, IMBUnknown **unknown);

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

class IMBView : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBView; }
    virtual unsigned char IsDirty(void) = 0;
    virtual void Draw(IMBSurface *) = 0;
    virtual void SetWorldPosition(MBPoint) = 0;
    virtual MBPoint GetWorldPosition(void) = 0;
    virtual MBRect GetWorldBounds(void) = 0;
    virtual void SetScreenPosition(MBPoint) = 0;
    virtual MBPoint GetScreenPosition(void) = 0;
    virtual MBRect GetScreenBounds(void) = 0;
    virtual void SetSize(unsigned short, unsigned short) = 0;
    virtual unsigned short GetWidth(void) = 0;
    virtual unsigned short GetHeight(void) = 0;
    virtual void AttachToWorld(IMBWorld *) = 0;
    virtual void AttachToScreen(IMBScreen *) = 0;
    virtual unsigned char IsAttachedToScreen(IMBScreen *) = 0;
    virtual void GetWorld(IMBWorld **) = 0;
    virtual MBPoint ScreenToWorldPoint(const MBPoint &) = 0;
};

class IMBViewManager : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBViewManager; }
    virtual void AddView(IMBView *) = 0;
    virtual void RemoveView(IMBView *) = 0;
    virtual void FindViewContaingPoint(IMBScreen *, const MBPoint &, IMBView **) = 0;
};

extern "C" {
bool MBGetGlobalMouseLocation(MBPoint &globalPoint);
bool MBGetScreenMouseLocation(MBPoint &screenPoint, IMBScreen **screen);
bool MBGetWorldMouseLocation(MBPoint &worldPoint, IMBWorld **world);
}

#endif
