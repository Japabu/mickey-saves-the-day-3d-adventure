// CMBDirectDrawScreenManager.h - declarations for MBEngine.dll CMBDirectDrawScreenManager.obj
#ifndef CMBDIRECTDRAWSCREENMANAGER_H
#define CMBDIRECTDRAWSCREENMANAGER_H

#include "CMBDirect3DScreen.h" // evidence: the color statics take $E1..$E48, before any STL number
#include <string> // evidence: $E66/$E67 (ctype id) = 48 + the <string> baseline 18/19
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

// own copy: which members are inline and which are only declared differs per unit (the original's inline budget; NOTES.md)
template <class T> class MBComPtr {
public:
    MBComPtr(void) : mInterface(0) {}
    MBComPtr(const MBGUID &clsid) : mInterface(0) { CreateObject(clsid); }
    ~MBComPtr() { Release(); }
    operator T *(void) const { return mInterface; }
    T *operator->(void) const { return mInterface; }
    void Attach(T *p) {
        if (mInterface) mInterface->Release();
        mInterface = p;
    }
    void Release(void) {
        do {
            if (mInterface != 0)
                mInterface->Release();
        } while (0);
        mInterface = 0;
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
    long _QueryInterface(IMBUnknown *const &p);
    T *mInterface;
};

template <class T>
long MBComPtr<T>::_QueryInterface(IMBUnknown *const &p)
{
    long hr;
    if (p) {
        T *pInterface;
        hr = p->QueryInterface(T::GetGUID(), (void **)&pInterface);
        if (hr) {
            Release();
            return hr;
        }
        Attach(pInterface);
    } else {
        if (mInterface) {
            T *old = mInterface;
            mInterface = 0;
            old->Release();
        }
        hr = -3;
    }
    return hr;
}

struct ScreenData {
    IMBWinScreen *screen;
    long screenID;
};

#include "IMBScreenManager.h"

#include "IMBDirectDrawScreenManager.h"

class CMBDirectDrawScreenManager : public IMBDirectDrawScreenManager {
public:
    CMBDirectDrawScreenManager(void);
    ~CMBDirectDrawScreenManager(void);
protected:
    unsigned long _mRefCnt;
public:
    virtual long QueryInterface(const MBGUID &iid, void **obj);
    virtual unsigned long AddRef(void);
    virtual unsigned long Release(void);
    virtual void EnterWindowedMode(void);
    virtual void EnterFullScreenMode(void);
    virtual void SetHWND(HWND__ *hWnd);
    virtual bool EnableDirect3D(bool on);
    virtual void SetDirectDrawEnumerator(MBDDEnumCallback func);
    virtual bool RequestScreen(long screenID, MBScreenAttributes &attributes);
    virtual void Activate(void);
    virtual void Deactivate(void);
    virtual MBPoint GetGlobalMouseLocation(void);
    virtual bool ProcessSystemEvent(void *event);
    virtual void RemoveScreen(long screenID);
    virtual void RemoveAllScreens(void);
    virtual unsigned short GetNumScreens(void);
    virtual void GetScreenByIndex(unsigned short index, IMBScreen **screen);
    virtual void GetScreenByID(long screenID, IMBScreen **screen);
    virtual void FindScreenContainingPoint(const MBPoint &location, IMBScreen **screen);
protected:
    void AddScreen(IMBWinScreen *screen, long screenID);
    MBVector<ScreenData> mScreens;
    HWND__ *mWindow;
    bool mIsFullScreen;
    bool mUseDirect3D;
    MBDDEnumCallback mDDrawEnumeratorFunc;
};

IMBUnknown *MakeCMBDirectDrawScreenManager(void);

#endif
