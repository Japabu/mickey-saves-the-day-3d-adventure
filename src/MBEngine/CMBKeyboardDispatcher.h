// CMBKeyboardDispatcher.h - declarations for MBEngine.dll CMBKeyboardDispatcher.obj
#ifndef CMBKEYBOARDDISPATCHER_H
#define CMBKEYBOARDDISPATCHER_H

typedef unsigned int size_t;

extern "C" __declspec(dllimport) void *MBmalloc_release(size_t size);
extern "C" __declspec(dllimport) void MBfree_release(void *p);

inline void *operator new(size_t size) { return MBmalloc_release(size); }
inline void operator delete(void *p) { MBfree_release(p); }

#include "../MBCom/MBGUID.h"

// header-defined interface IDs (.rdata order 0x10089c58..0x10089ca8)
#include "../MBCom/IMBUnknown.h"
static const MBGUID MBIID_IMBClassFactory =
    { 0x396c7e0b, 0x172d, 0x4182, { 0xa9, 0x09, 0x48, 0xb5, 0x02, 0x3c, 0x7d, 0xea } };
static const MBGUID MBIID_IMBEventManager =
    { 0x79913910, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBEventListener =
    { 0x799755d0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBKeyboardDispatcher =
    { 0x7a2880a0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBKeyboardTarget =
    { 0x7a364150, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };


extern "C" IMBUnknown *MBComUtilAddSingleton(IMBUnknown *unk);
extern "C" long MBComCreateInstance(IMBUnknown *pOuter, const MBGUID &guid, IMBUnknown **unknown);

#include "IMBEventListener.h"

#include "IMBEventManager.h"

class IMBKeyboardTarget : public IMBUnknown {
public:
    virtual bool AllowDontBeTarget(IMBKeyboardTarget *) = 0;
    virtual bool AllowBeTarget(IMBKeyboardTarget *) = 0;
};

class IMBKeyboardDispatcher : public IMBEventListener {
public:
    virtual void GetTarget(IMBKeyboardTarget **) = 0;
    virtual bool SwitchToTarget(IMBKeyboardTarget *) = 0;
};

// own copy: which members are inline and which are only declared differs per unit (the original's inline budget; NOTES.md)
template <class T>
class MBComPtr {
public:
    MBComPtr(void) : mInterface(0) {}
    MBComPtr(const MBGUID &clsid) : mInterface(0) { CreateObject(clsid); }
    ~MBComPtr(void) { if (mInterface) mInterface->Release(); }
    operator T *(void) const { return mInterface; }
    T *operator->(void) const { return mInterface; }
    operator bool(void) const { return mInterface != 0; }
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
    long _QueryInterface(IMBUnknown *const &p); // 0x100039a0, emitted in another object
    T *mInterface;
};

class CMBKeyboardDispatcher : public IMBKeyboardDispatcher {
public:
    CMBKeyboardDispatcher(void);
    ~CMBKeyboardDispatcher(void);
protected:
    unsigned long _mRefCnt;
public:
    virtual long QueryInterface(const MBGUID &iid, void **obj);
    virtual unsigned long AddRef(void);
    virtual unsigned long Release(void);
    virtual void ListenToEvent(long channel, long event, void *data);
    virtual void GetTarget(IMBKeyboardTarget **target);
    virtual bool SwitchToTarget(IMBKeyboardTarget *newTarget);
protected:
    void ForceTargetSwitch(IMBKeyboardTarget *newTarget);
    IMBKeyboardTarget *mTarget;
};

IMBUnknown *MakeCMBKeyboardDispatcher(void);

#endif
