#pragma once
// SBObjectGroup.h: the game's object group, the root of the world (declaration from SteamboatApp.pdb).
#include "SBInterfaces.h"

class SBObjectGroup : public IMBObjectGroup {
    typedef MBMultimap<long, IMBObject *, std::less<long> > ObjectMap;
public:
    SBObjectGroup(const SBObjectGroup &);
    SBObjectGroup();
    ~SBObjectGroup();
protected:
    unsigned long _mRefCnt;                             // +0x0004 size 0x4
public:
    virtual long QueryInterface(const MBGUID &iid, void **obj); // vftable+0x00
    virtual unsigned long AddRef();                     // vftable+0x04
    virtual unsigned long Release();                    // vftable+0x08
    virtual MBPoint3 GetOffset() const;                 // vftable+0x0c
    virtual MBPoint3 GetPosition() const;               // vftable+0x10
    virtual void Place(const MBPoint3 &point);          // vftable+0x14
    virtual void Move(const MBPoint3 &point);           // vftable+0x18
    virtual MBRect GetBounds() const;                   // vftable+0x1c
    virtual bool IsVisible() const;                     // vftable+0x20
    virtual long Show();                                // vftable+0x24
    virtual long Hide();                                // vftable+0x28
    virtual bool IsDirty() const;                       // vftable+0x2c
    virtual void SetDirty();                            // vftable+0x30
    virtual void ClearDirty();                          // vftable+0x34
    virtual long Draw(IMBSurface *dest, MBRect &srcRect, MBRect &destRect) const; // vftable+0x38
    virtual void GetParent(IMBObjectGroup **parent) const; // vftable+0x3c
    virtual IMBObjectGroup *SetParent(IMBObjectGroup *newParent); // vftable+0x40
    virtual void GetBackground(IMBObject **background) const; // vftable+0x44
    virtual void SetBackground(IMBObject *newBg);       // vftable+0x48
    virtual void AddObject(IMBObject *obj);             // vftable+0x4c
    virtual void RemoveObject(IMBObject *obj);          // vftable+0x50
    virtual void ObjectMoved(IMBObject *obj);           // vftable+0x54
    virtual unsigned long GetNObjects();                // vftable+0x58
    virtual MBComPtr<IMBObject> GetObjectA(unsigned long index); // vftable+0x5c
private:
    bool ClipRectsToObject(IMBObject *obj, MBRect &srcRect, MBRect &destRect) const;
    ObjectMap::iterator FindObject(IMBObject *obj);
    IMBObjectGroup *mParent;                            // +0x0008 size 0x4
    IMBObject *mBackground;                             // +0x000c size 0x4
    MBRect mCachedBounds;                               // +0x0010 size 0x10
    bool mCachedBoundsValid;                            // +0x0020 size 0x1
    MBPoint3 mOffset;                                   // +0x0024 size 0xc
    long mHideCount;                                    // +0x0030 size 0x4
    bool mDirty;                                        // +0x0034 size 0x1
    ObjectMap mObjects;            // +0x0038 size 0x10
};

