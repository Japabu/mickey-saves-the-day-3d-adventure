#pragma once
// CMBScreenPoly.h: a polygonal hotspot on the screen (declaration from SteamboatApp.pdb).
#include "SBInterfaces.h"

class CMBScreenPoly : public IMBScreenPoly, public IMBMouseable, public IMBObject {
public:
    CMBScreenPoly(const CMBScreenPoly &);
    CMBScreenPoly(const MBVector<MBPointStruct> vertices, long nZPlane);
    CMBScreenPoly();
    ~CMBScreenPoly();
protected:
    unsigned long _mRefCnt;                             // +0x000c size 0x4
public:
    virtual long QueryInterface(const MBGUID &iid, void **obj); // vftable+0x00
    virtual unsigned long AddRef();                     // vftable+0x04
    virtual unsigned long Release();                    // vftable+0x08
    virtual void SetBroadcastChannel(long channel);     // vftable+0x0c
    virtual void SetClickMessage(long message, void *data); // vftable+0x10
    virtual void SetClickMessage(long message);         // vftable+0x14
    virtual void SetEnterMessage(long message, void *data); // vftable+0x18
    virtual void SetEnterMessage(long message);         // vftable+0x1c
    virtual void SetLeaveMessage(long message, void *data); // vftable+0x20
    virtual void SetLeaveMessage(long message);         // vftable+0x24
    virtual long GetBroadcastChannel();                 // vftable+0x28
    virtual long GetClickMessage();                     // vftable+0x2c
    virtual long GetEnterMessage();                     // vftable+0x30
    virtual long GetLeaveMessage();                     // vftable+0x34
    virtual void *GetClickData();                       // vftable+0x38
    virtual void *GetEnterData();                       // vftable+0x3c
    virtual void *GetLeaveData();                       // vftable+0x40
    virtual void SetRect(const MBRect &rect, long nZPlane); // vftable+0x44
    virtual void SetVertices(const MBVector<MBPointStruct> vertices, long nZPlane); // vftable+0x48
    virtual MBVector<MBPointStruct> GetVertices();      // vftable+0x4c
    virtual int LoadFromStream(const MBComPtr<IMBStream> stream, const MBComPtr<IMBPref> &pref, const char *key); // vftable+0x50
    virtual void SetZ(const long z);                    // vftable+0x54
    virtual long GetZ();                                // vftable+0x58
    virtual bool IsRolledOver(const MBPoint &world_mouse_pos) const; // vftable+0x0c (subobject +0x4), this adjust +0x4
    virtual void EnterRollover(const MBPoint &world_mouse_pos); // vftable+0x10 (subobject +0x4), this adjust +0x4
    virtual void WithinRollover(const MBPoint &world_mouse_pos); // vftable+0x14 (subobject +0x4), this adjust +0x4
    virtual void LeaveRollover();                       // vftable+0x18 (subobject +0x4), this adjust +0x4
    virtual bool IsClicked(const MBPoint &world_mouse_pos, short mouseButton) const; // vftable+0x1c (subobject +0x4), this adjust +0x4
    virtual bool IsTrackingClick() const;               // vftable+0x20 (subobject +0x4), this adjust +0x4
    virtual void StartTrackingClick(const MBPoint &world_mouse_pos, short button); // vftable+0x24 (subobject +0x4), this adjust +0x4
    virtual void TrackClick(const MBPoint &world_mouse_pos); // vftable+0x28 (subobject +0x4), this adjust +0x4
    virtual void StopTrackingClick(const MBPoint &world_mouse_pos, short button); // vftable+0x2c (subobject +0x4), this adjust +0x4
    virtual bool IsActive() const;                      // vftable+0x5c
    virtual void Activate();                            // vftable+0x60
    virtual void Deactivate();                          // vftable+0x64
    virtual void SetPixelTest(bool set);                // vftable+0x3c (subobject +0x4), this adjust +0x4
    virtual bool GetPixelTest();                        // vftable+0x40 (subobject +0x4), this adjust +0x4
    virtual MBPoint3 GetOffset() const;                 // vftable+0x0c (subobject +0x8), this adjust +0x8
    virtual MBPoint3 GetPosition() const;               // vftable+0x10 (subobject +0x8), this adjust +0x8
    virtual void Place(const MBPoint3 &point);          // vftable+0x14 (subobject +0x8), this adjust +0x8
    virtual void Move(const MBPoint3 &point);           // vftable+0x18 (subobject +0x8), this adjust +0x8
    virtual MBRect GetBounds() const;                   // vftable+0x1c (subobject +0x8), this adjust +0x8
    virtual bool IsVisible() const;                     // vftable+0x20 (subobject +0x8), this adjust +0x8
    virtual long Show();                                // vftable+0x24 (subobject +0x8), this adjust +0x8
    virtual long Hide();                                // vftable+0x28 (subobject +0x8), this adjust +0x8
    virtual bool IsDirty() const;                       // vftable+0x2c (subobject +0x8), this adjust +0x8
    virtual void SetDirty();                            // vftable+0x30 (subobject +0x8), this adjust +0x8
    virtual void ClearDirty();                          // vftable+0x34 (subobject +0x8), this adjust +0x8
    virtual long Draw(IMBSurface *dest, MBRect &srcRect, MBRect &destRect) const; // vftable+0x38 (subobject +0x8), this adjust +0x8
    virtual void GetParent(IMBObjectGroup **parent) const; // vftable+0x3c (subobject +0x8), this adjust +0x8
    virtual IMBObjectGroup *SetParent(IMBObjectGroup *newParent); // vftable+0x40 (subobject +0x8), this adjust +0x8
    virtual MBString &GetIfProperty();                  // vftable+0x68
    virtual MBString &GetIfNotProperty();               // vftable+0x6c
protected:
    bool IsPointInPoly(const MBPoint &point) const;
    long mBroadcastChannel;                             // +0x0010 size 0x4
    long mClickMessage;                                 // +0x0014 size 0x4
    long mEnterMessage;                                 // +0x0018 size 0x4
    long mLeaveMessage;                                 // +0x001c size 0x4
    bool mIsActive;                                     // +0x0020 size 0x1
    bool mIsTrackingClick;                              // +0x0021 size 0x1
    bool mPixelTest;                                    // +0x0022 size 0x1
    MBString mIfProperty;                               // +0x0024 size 0x10
    MBString mIfNotProperty;                            // +0x0034 size 0x10
    MBVector<MBPointStruct> mVertices;                  // +0x0044 size 0x10
    long mZPlane;                                       // +0x0054 size 0x4
    void *mClickData;                                   // +0x0058 size 0x4
    void *mEnterData;                                   // +0x005c size 0x4
    void *mLeaveData;                                   // +0x0060 size 0x4
    IMBObjectGroup *mParent;                            // +0x0064 size 0x4
};

