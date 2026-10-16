#pragma once
#include "MfLock.h"
class MfScDataHandle;
class MfScLinkData;
class MfString;

// Base of everything a data handle can point to: a lockable, named object.
class __declspec(dllexport) MfScData {
public:
    MfScData();
    virtual ~MfScData();
    virtual MfScLinkData * CountCircLinks(MfScDataHandle *);
    virtual void ReleaseCircLinks(MfScLinkData *);
    virtual int GetThroughCount(MfScDataHandle *);
    virtual void AddRef();
    virtual void Release();
    MfString & Name() { return *m_name; }
    void Lock() { m_lock.Lock(); }
    void Unlock() { m_lock.Unlock(); }
protected:
    MfLock m_lock;      // +4
    MfString *m_name;   // +8
};
