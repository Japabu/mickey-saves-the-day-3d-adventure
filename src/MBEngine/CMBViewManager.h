// CMBViewManager.h - declarations for MBEngine.dll CMBViewManager.obj
#ifndef CMBVIEWMANAGER_H
#define CMBVIEWMANAGER_H

#include "CMBView.h" // evidence: the color statics take $E1..$E48, before any STL number
#include <string>    // evidence: ctype id $E67/$E68 (see NOTES)
#include <vector>

// allocator without construct/destroy: insert calls the out-of-line std::_Construct
template <class T> class MBAllocator : public std::allocator<T> {
public:
    typedef unsigned long SizeType;
    typedef T *PointerType;
    T *allocate(unsigned long n, const void *) { return (T *)MBmalloc_release(n * sizeof(T)); }
    char *_Charalloc(unsigned long n) { return (char *)MBmalloc_release(n); }
    void deallocate(void *p, unsigned long) { if (p) MBfree_release(p); }
};

template <class T> class MBVector : public std::vector<T, MBAllocator<T> > {
};

extern "C" long MBComCreateInstance(IMBUnknown *pOuter, const MBGUID &guid, IMBUnknown **unknown);
extern "C" IMBUnknown *MBComUtilAddSingleton(IMBUnknown *unk);

template <class T> class MBComPtr {
public:
    MBComPtr(void) : mInterface(0) {}
    MBComPtr(const MBGUID &clsid) : mInterface(0) { CreateObject(clsid); }
    ~MBComPtr() { if (mInterface) mInterface->Release(); mInterface = 0; }
    operator T *(void) const { return mInterface; }
    T *operator->(void) const { return mInterface; }
    MBComPtr &operator=(T *p) {
        Release();
        mInterface = p;
        return *this;
    }
    void Release(void) {
        if (mInterface) {
            mInterface->Release();
            mInterface = 0;
        }
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
    long _QueryInterface(IMBUnknown *const &p); // emitted in another object
    T *mInterface;
};

class IMBPeriodical : public IMBUnknown {
public:
    virtual void Idle(void) = 0;
};

class IMBPeriodicalManager : public IMBUnknown {
public:
    virtual void AddPeriodical(IMBPeriodical *) = 0;
    virtual void RemovePeriodical(IMBPeriodical *) = 0;
    virtual void DevoteTimeToPeriodicals(void) = 0;
};

class IMBEventListener;

class IMBEventManager : public IMBUnknown {
public:
    virtual void BroadcastEvent(long, long, void *) = 0;
    virtual void AddListener(long, IMBEventListener *) = 0;
    virtual void RemoveListener(long, IMBEventListener *) = 0;
    virtual void RemoveListener(IMBEventListener *) = 0;
};

struct MBScreenAttributes;

class IMBScreenManager : public IMBUnknown {
public:
    virtual bool RequestScreen(long, MBScreenAttributes &) = 0;
    virtual void Activate(void) = 0;
    virtual void Deactivate(void) = 0;
    virtual MBPoint GetGlobalMouseLocation(void) = 0;
    virtual bool ProcessSystemEvent(void *) = 0;
    virtual void RemoveScreen(long) = 0;
    virtual void RemoveAllScreens(void) = 0;
    virtual unsigned short GetNumScreens(void) = 0;
    virtual void GetScreenByIndex(unsigned short, IMBScreen **) = 0;
    virtual void GetScreenByID(long, IMBScreen **) = 0;
    virtual void FindScreenContainingPoint(const MBPoint &, IMBScreen **) = 0;
};

extern "C" void MBGetScreenManager(IMBScreenManager **screenManager);

class IMBViewManager : public IMBUnknown {
public:
    virtual void AddView(IMBView *) = 0;
    virtual void RemoveView(IMBView *) = 0;
    virtual void FindViewContaingPoint(IMBScreen *, const MBPoint &, IMBView **) = 0;
};

class CMBViewManager : public IMBViewManager, public IMBPeriodical {
public:
    CMBViewManager(void);
    ~CMBViewManager(void);
protected:
    unsigned long _mRefCnt;      // +0x08
public:
    virtual long QueryInterface(const MBGUID &iid, void **obj);
    virtual unsigned long AddRef(void);
    virtual unsigned long Release(void);
    virtual void AddView(IMBView *view);
    virtual void RemoveView(IMBView *view);
    virtual void FindViewContaingPoint(IMBScreen *screen, const MBPoint &point, IMBView **view);
    virtual void Idle(void);
protected:
    MBVector<IMBView *> mViews;  // +0x0c
};

IMBUnknown *MakeCMBViewManager(void);

#endif
