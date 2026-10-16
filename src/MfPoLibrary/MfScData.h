// MfScData (PiccoloRT.dll): the base of everything a MfScDataHandle can
// point to, a lockable, named object. Members from the SteamboatApp PDB.
#pragma once
#include "MfLock.h"
#include "MfString.h"
class MfScDataHandle;
class MfScLinkData;

class __declspec(dllimport) MfScData {
public:
    MfScData();
    virtual ~MfScData();
    // the tag of the constructor that builds a class's exemplar (the object
    // the script sees as the class itself)
    class Exemplar {
    public:
        Exemplar() {}
        ~Exemplar() {}
    };
    MfString &Name() { return *pfsName; }
    virtual MfScLinkData *CountCircLinks(MfScDataHandle *);
    virtual void ReleaseCircLinks(MfScLinkData *);
    virtual int GetThroughCount(MfScDataHandle *);
    void Lock() { lLock.Lock(-1); }
    void Unlock() { lLock.Unlock(); }
    virtual void AddRef();
    virtual void Release();
private:
    MfLock lLock;                                           // 0x04
    MfString *pfsName;                                      // 0x08
};
