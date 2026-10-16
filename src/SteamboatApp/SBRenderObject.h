#pragma once
// SBRenderObject.h: the world object that stands for the 3D scene: drawing it asks the node to
// render its characters (declaration from SteamboatApp.pdb).
#include "MBInterfaces.h"

class SteamboatNode;

class SBRenderObject : public IMBObject {
public:
    SBRenderObject(const SBRenderObject &);
    SBRenderObject();
protected:
    unsigned long _mRefCnt;                             // +0x0004 size 0x4
public:
    virtual long QueryInterface(const MBGUID &iid, void **obj); // vftable+0x00
    virtual unsigned long AddRef();                     // vftable+0x04
    virtual unsigned long Release();                    // vftable+0x08
    void SetParentNode(SteamboatNode *node) { mParentNode = node; }
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
    virtual long Draw(IMBSurface *dest, MBRect &worldRect, MBRect &destRect) const; // vftable+0x38
    virtual void GetParent(IMBObjectGroup **parent) const; // vftable+0x3c
protected:
    virtual IMBObjectGroup *SetParent(IMBObjectGroup *newParent); // vftable+0x40
    IMBObjectGroup *mParent;                            // +0x0008 size 0x4
    SteamboatNode *mParentNode;                         // +0x000c size 0x4
    MBPoint3 mPos;                                      // +0x0010 size 0xc
    long mHideCount;                                    // +0x001c size 0x4
    bool mDirty;                                        // +0x0020 size 0x1
};
