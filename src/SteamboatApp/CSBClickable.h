#pragma once
// CSBClickable.h: an animation that reacts to the mouse (declaration from SteamboatApp.pdb).
#include "SBInterfaces.h"
#include "SBMouseEvent.h"

struct SBClickableSoundSpec {
    unsigned long soundID;
    long soundChannel;
};

class CSBClickable : public ISBClickable, public IMBMouseable, public IMBObjectGroup {
public:
    CSBClickable(const CSBClickable &);
    CSBClickable();
    ~CSBClickable();
protected:
    unsigned long _mRefCnt;                             // +0x000c size 0x4
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
    virtual void GetBackground(IMBObject **background) const; // vftable+0x44 (subobject +0x8), this adjust +0x8
    virtual void SetBackground(IMBObject *newBg);       // vftable+0x48 (subobject +0x8), this adjust +0x8
    virtual void AddObject(IMBObject *obj);             // vftable+0x4c (subobject +0x8), this adjust +0x8
    virtual void RemoveObject(IMBObject *obj);          // vftable+0x50 (subobject +0x8), this adjust +0x8
    virtual void ObjectMoved(IMBObject *obj);           // vftable+0x54 (subobject +0x8), this adjust +0x8
    virtual void SetAnim(const char *animName);         // vftable+0x48
    virtual void SetAnim(IMBAnim *anim);                // vftable+0x44
    virtual MBComPtr<IMBAnim> GetAnim();                // vftable+0x4c
    virtual void SetBroadcastChannel(long channel);     // vftable+0x50
    virtual void SetClickMessage(long message);         // vftable+0x58
    virtual void SetClickMessage(long message, void *data); // vftable+0x54
    virtual void SetRolloverMessage(long message);      // vftable+0x60
    virtual void SetRolloverMessage(long message, void *data); // vftable+0x5c
    virtual void SetLeaveMessage(long message);         // vftable+0x68
    virtual void SetLeaveMessage(long message, void *data); // vftable+0x64
    virtual void PlaceCentered(MBPoint3 &point);        // vftable+0x6c
    virtual long GetBroadcastChannel();                 // vftable+0x70
    virtual long GetClickMessage();                     // vftable+0x74
    virtual long GetRolloverMessage();                  // vftable+0x78
    virtual long GetLeaveMessage();                     // vftable+0x7c
    virtual void *GetClickData();                       // vftable+0x80
    virtual void *GetRolloverData();                    // vftable+0x84
    virtual void *GetLeaveData();                       // vftable+0x88
    virtual bool IsEnabled() const;                     // vftable+0x8c
    virtual void Enable();                              // vftable+0x90
    virtual void Disable();                             // vftable+0x94
    virtual void SetBehavior(MBButtonBehavior behavior); // vftable+0x98
    virtual MBButtonBehavior GetBehavior() const;       // vftable+0x9c
    virtual MBButtonState GetButtonState() const;       // vftable+0xa0
    virtual void SetButtonState(MBButtonState buttonState); // vftable+0xa4
    virtual void SetSound(MBButtonDrawState drawState, const char *soundName, long soundChannel); // vftable+0xa8
    virtual bool IsRolledOver(const MBPoint &world_mouse_pos) const; // vftable+0x0c (subobject +0x4), this adjust +0x4
    virtual void EnterRollover(const MBPoint &world_mouse_pos); // vftable+0x10 (subobject +0x4), this adjust +0x4
    virtual void WithinRollover(const MBPoint &world_mouse_pos); // vftable+0x14 (subobject +0x4), this adjust +0x4
    virtual void LeaveRollover();                       // vftable+0x18 (subobject +0x4), this adjust +0x4
    virtual bool IsClicked(const MBPoint &world_mouse_pos, short mouseButton) const; // vftable+0x1c (subobject +0x4), this adjust +0x4
    virtual bool IsTrackingClick() const;               // vftable+0x20 (subobject +0x4), this adjust +0x4
    virtual void StartTrackingClick(const MBPoint &world_mouse_pos, short button); // vftable+0x24 (subobject +0x4), this adjust +0x4
    virtual void TrackClick(const MBPoint &world_mouse_pos); // vftable+0x28 (subobject +0x4), this adjust +0x4
    virtual void StopTrackingClick(const MBPoint &world_mouse_pos, short button); // vftable+0x2c (subobject +0x4), this adjust +0x4
    virtual bool IsActive() const;                      // vftable+0x30 (subobject +0x4), this adjust +0x4
    virtual void Activate();                            // vftable+0x34 (subobject +0x4), this adjust +0x4
    virtual void Deactivate();                          // vftable+0x38 (subobject +0x4), this adjust +0x4
    virtual void SetPixelTest(bool set);                // vftable+0x3c (subobject +0x4), this adjust +0x4
    virtual bool GetPixelTest();                        // vftable+0x40 (subobject +0x4), this adjust +0x4
    virtual int LoadFromStream(const MBComPtr<IMBStream> stream, const MBComPtr<IMBPref> &pref, const char *key); // vftable+0xac
    virtual MBString &GetIfProperty();                  // vftable+0xb0
    virtual MBString &GetIfNotProperty();               // vftable+0xb4
protected:
    bool IsWorldPointInAnim(const MBPoint &worldPoint) const;
    long GetUpSequenceIndex() const;
    long GetDownSequenceIndex() const;
    MBComPtr<IMBAnim> mAnimation;                       // +0x0010 size 0x4
    IMBObjectGroup *mParent;                            // +0x0014 size 0x4
    long mDrawStateSequences[5];                        // +0x0018 size 0x14
    SBClickableSoundSpec mDrawStateSounds[5];           // +0x002c size 0x28
    long mBroadcastChannel;                             // +0x0054 size 0x4
    long mClickMessage;                                 // +0x0058 size 0x4
    long mRolloverMessage;                              // +0x005c size 0x4
    long mLeaveMessage;                                 // +0x0060 size 0x4
    MBButtonBehavior mBehavior;                         // +0x0064 size 0x4
    MBButtonState mButtonState;                         // +0x0068 size 0x4
    MBPoint3 mOffset;                                   // +0x006c size 0xc
    bool mIsActive;                                     // +0x0078 size 0x1
    bool mPixelTest;                                    // +0x0079 size 0x1
    bool mIsTrackingClick;                              // +0x007a size 0x1
    bool mIsEnabled;                                    // +0x007b size 0x1
    bool mIsOn;                                         // +0x007c size 0x1
    void *mRolloverData;                                // +0x0080 size 0x4
    void *mClickData;                                   // +0x0084 size 0x4
    void *mLeaveData;                                   // +0x0088 size 0x4
    SBMouseEvent mEventData;                            // +0x008c size 0x24
    MBString mIfProperty;                               // +0x00b0 size 0x10
    MBString mIfNotProperty;                            // +0x00c0 size 0x10
};

