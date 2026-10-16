#pragma once
// CSBRenderManager.h: the world object that draws the 3D scene (declaration from SteamboatApp.pdb).
#include "Gfx_Node.h"
#include "SBInterfaces.h"

class CSBRenderManager : public IMBObject {
public:
    static const MBGUID &GetGUID() { return MBIID_CSBRenderManager; }
protected:
    unsigned long _mRefCnt;                             // +0x0004 size 0x4
public:
    virtual long QueryInterface(const MBGUID &iid, void **obj); // vftable+0x00
    virtual unsigned long AddRef();                     // vftable+0x04
    virtual unsigned long Release();                    // vftable+0x08
    CSBRenderManager(const CSBRenderManager &);
    CSBRenderManager();
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
    virtual long Draw(IMBSurface *, MBRect &, MBRect &) const; // vftable+0x38
    virtual void GetParent(IMBObjectGroup **parent) const; // vftable+0x3c
    virtual IMBObjectGroup *SetParent(IMBObjectGroup *newParent); // vftable+0x40
    virtual void SetRenderFlags(unsigned long flags);   // vftable+0x44
    virtual void SetRootNode(Gfx::Node *root);          // vftable+0x48
protected:
    virtual ~CSBRenderManager();                        // deleting destructor at vftable+0x4c
    MBPoint3 mOffset;                                   // +0x0008 size 0xc
    MBPoint3 mPosition;                                 // +0x0014 size 0xc
    MBRect mBounds;                                     // +0x0020 size 0x10
    long mHideCount;                                    // +0x0030 size 0x4
    bool mIsDirty;                                      // +0x0034 size 0x1
    MBComPtr<IMBObjectGroup> mParent;                   // +0x0038 size 0x4
    unsigned long mRenderFlags;                         // +0x003c size 0x4
    Gfx::Node *mRootNode;                               // +0x0040 size 0x4
};

