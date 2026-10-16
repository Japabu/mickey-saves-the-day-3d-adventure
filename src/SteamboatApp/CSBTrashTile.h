#pragma once
// CSBTrashTile.h: a tile of the trash-bash activity, an animation button that flies to its square
// (declaration from SteamboatApp.pdb).
#include "CSBAnimButton.h"

class SBTrashSquare;

class CSBTrashTile : public CSBAnimButton, public ISBTrashTile, public IMBEventListener, public IMBPeriodical {
public:
    CSBTrashTile(const CSBTrashTile &);
    CSBTrashTile();
protected:
    unsigned long _mRefCnt;                             // +0x0098 size 0x4
public:
    virtual long QueryInterface(const MBGUID &iid, void **obj); // vftable+0x00
    virtual unsigned long AddRef();                     // vftable+0x04
    virtual unsigned long Release();                    // vftable+0x08
    virtual void SetAnim(const char *animName);         // vftable+0x48
    virtual void SetAnim(IMBAnim *anim);                // vftable+0x44
    virtual void Disable();                             // vftable+0x94
    virtual void Enable();                              // vftable+0x90
    virtual bool Compare(const ISBTrashTile *otherTile) const; // vftable+0x0c (subobject +0x8c), this adjust +0x8c
    virtual long GetID() const;                         // vftable+0x10 (subobject +0x8c), this adjust +0x8c
    virtual void Translate(SBTrashSquare *square, const long time); // vftable+0x14 (subobject +0x8c), this adjust +0x8c
    virtual void ListenToEvent(long channel, long event, void *data); // vftable+0x0c (subobject +0x90), this adjust +0x90
    virtual void Idle();                                // vftable+0x0c (subobject +0x94), this adjust +0x94
protected:
    virtual ~CSBTrashTile();                            // deleting destructor at vftable+0xb0
    long myID;                                          // +0x009c size 0x4
    MBPoint3 mLaunchPoint;                              // +0x00a0 size 0xc
    long mLaunchTime;                                   // +0x00ac size 0x4
    long mETA;                                          // +0x00b0 size 0x4
    float mVelocityX;                                   // +0x00b4 size 0x4
    float mVelocityY;                                   // +0x00b8 size 0x4
    bool mDoneTranslating;                              // +0x00bc size 0x1
    SBTrashSquare *mDestSquare;                         // +0x00c0 size 0x4
};

