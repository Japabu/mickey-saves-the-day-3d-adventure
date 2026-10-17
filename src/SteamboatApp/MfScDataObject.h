#pragma once
// MfScDataObject.h: the object classes of Motivate's Piccolo script VM (PiccoloRT.dll) that the
// game's script objects derive from, as the game declares them (SteamboatApp.pdb). The virtual
// functions are declared in vftable order; members the game never calls are left out.
#include "MfString.h"

class MfScDataHandle;
class MfScLinkData;
class MfScHandleDict;
class MfScPropertyExtList;
class MfStringList;

// own copy: the game calls PiccoloRT and MfPoLibrary through its import thunks (no dllimport) and emits its own copies of
// inline members; the owner's header declares the class PICCOLORT_API / MFPO_API, dllimport here,
// and they open PiccoloRT's and MfPoLibrary's MfString copies.
class MfScData {
public:
    MfScData();
    virtual ~MfScData();                                // vftable+0x00
    virtual MfScLinkData *CountCircLinks(MfScDataHandle *); // vftable+0x04
    virtual void ReleaseCircLinks(MfScLinkData *);      // vftable+0x08
    virtual int GetThroughCount(MfScDataHandle *);      // vftable+0x0c
    virtual void AddRef();                              // vftable+0x10
    virtual void Release();                             // vftable+0x14
    MfString &Name();
private:
    long lLock;                                         // +0x0004 (MfLock)
    MfString *pfsName;                                  // +0x0008
};

// own copy: as above.
class MfScDataObject : public MfScData {
public:
    enum MfPropertyErrorCode {
        kNoError = 0
    };
    MfScDataObject(unsigned int);
    virtual ~MfScDataObject();
    virtual MfPropertyErrorCode GetProperty(const MfString &, MfScDataHandle **); // vftable+0x18
    virtual MfPropertyErrorCode SetProperty(MfScDataHandle *, const MfString &, MfScDataHandle *, int); // vftable+0x1c
    virtual MfString GetClassNameA();                   // vftable+0x20
    virtual void GetPropertyNames(MfStringList *, int); // vftable+0x24
    virtual void GetInstanceProperties(MfStringList *, int); // vftable+0x28
    virtual void FilloutPropertyNameList(MfStringList *, void *); // vftable+0x2c
    virtual void FilloutPropertyNameList(MfStringList *); // vftable+0x30
    virtual MfScPropertyExtList *GetPropertyExtensions(); // vftable+0x34
protected:
    MfScHandleDict *pdictProperties;                    // +0x000c
    unsigned int uiFlags;                               // +0x0010
};

// own copy: as above.
class MfScObject : public MfScDataObject {
public:
    MfScObject(unsigned int);
};

// own copy: as above.
class MfPoBase : public MfScObject {
public:
    MfPoBase(unsigned int);
};
