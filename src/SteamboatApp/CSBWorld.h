#pragma once
// CSBWorld.h: the game's world (declaration from SteamboatApp.pdb).
#include "SBInterfaces.h"
#include "SBObjectGroup.h"

class SBObjectGroup;

class CSBWorld : public IMBWorld, public ISBWorld {
public:
    CSBWorld(const CSBWorld &);
    CSBWorld();
    ~CSBWorld();
protected:
    unsigned long _mRefCnt;                             // +0x0008 size 0x4
public:
    virtual long QueryInterface(const MBGUID &iid, void **obj); // vftable+0x00
    virtual unsigned long AddRef();                     // vftable+0x04
    virtual unsigned long Release();                    // vftable+0x08
    virtual MBRect GetBounds() const;                   // vftable+0x0c
    virtual void SetBounds(const MBRect &bounds);       // vftable+0x10
    virtual long Draw(IMBSurface *dest, MBRect &worldRect, MBRect &destRect); // vftable+0x14
    virtual IMBObjectGroup &GetRoot();                  // vftable+0x18
    virtual const IMBObjectGroup &GetConstRoot() const; // vftable+0x1c
    virtual unsigned long GetNObjects();                // vftable+0x0c (subobject +0x4), this adjust +0x4
    virtual MBComPtr<IMBObject> GetObjectA(unsigned long index); // vftable+0x10 (subobject +0x4), this adjust +0x4
protected:
    MBRect mBounds;                                     // +0x000c size 0x10
    SBObjectGroup *mRoot;                               // +0x001c size 0x4
};

