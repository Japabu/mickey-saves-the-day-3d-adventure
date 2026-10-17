// CMBWinClock.h - declarations for MBEngine.dll CMBWinClock.obj
#ifndef CMBWINCLOCK_H
#define CMBWINCLOCK_H

typedef unsigned int size_t;

extern "C" __declspec(dllimport) void *MBmalloc_release(size_t size);
extern "C" __declspec(dllimport) void MBfree_release(void *p);
extern "C" __declspec(dllimport) unsigned long __stdcall timeGetTime(void);

inline void *operator new(size_t size) { return MBmalloc_release(size); }
inline void operator delete(void *p) { MBfree_release(p); }

struct MBGUID {
    unsigned long data1;
    unsigned short data2;
    unsigned short data3;
    unsigned char data4[8];
    unsigned char operator==(const MBGUID &o) const {
        return data1 == o.data1 && data2 == o.data2 && data3 == o.data3 &&
               ((const unsigned long *)data4)[0] == ((const unsigned long *)o.data4)[0] &&
               ((const unsigned long *)data4)[1] == ((const unsigned long *)o.data4)[1];
    }
};

static const MBGUID MBIID_IMBUnknown =
    { 0x59d231f0, 0x9c6c, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBClassFactory =
    { 0x396c7e0b, 0x172d, 0x4182, { 0xa9, 0x09, 0x48, 0xb5, 0x02, 0x3c, 0x7d, 0xea } };
static const MBGUID MBIID_IMBGameClock =
    { 0x04e3c980, 0x9ea1, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };

class IMBUnknown {
public:
    virtual long QueryInterface(const MBGUID &, void **) = 0;
    virtual unsigned long AddRef(void) = 0;
    virtual unsigned long Release(void) = 0;
};

extern "C" IMBUnknown *MBComUtilAddSingleton(IMBUnknown *unk);

#include "IMBGameClock.h"

class CMBWinClock : public IMBGameClock {
public:
    CMBWinClock(void);
    ~CMBWinClock(void);
protected:
    unsigned long _mRefCnt;
public:
    virtual long QueryInterface(const MBGUID &iid, void **obj);
    virtual unsigned long AddRef(void);
    virtual unsigned long Release(void);
    virtual unsigned long GetRealTime(void);
    virtual unsigned long GetGameTime(void);
    virtual long Pause(void);
    virtual long Unpause(void);
    virtual void SetTime(unsigned long newtime);
    virtual void AddTime(unsigned long addtime);
    virtual void SetMSPerSecond(long mspersec);
protected:
    void UpdateTime(void);
    unsigned long mLastTime;
    unsigned long mCurrentGameTime;
    unsigned long mCurrentRealTime;
    long mMSperSec;
    long mPauseCount;
};

IMBUnknown *MakeCMBWinClock(void);

#endif
