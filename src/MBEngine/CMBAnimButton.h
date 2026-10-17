// CMBAnimButton.h - declarations for MBEngine.dll CMBAnimButton.obj
// Colour header, geometry, IMBObject/IMBAnim/IMBMouseable/IMBEventManager copied from
// CMBDragObject.h; IMBResourceFile(List) from CMBResourceMgr.h.
#ifndef CMBANIMBUTTON_H
#define CMBANIMBUTTON_H

// evidence: <string> numbers come first here (colour statics are $E14..$E60)
#include <string>

typedef unsigned int size_t;

extern "C" __declspec(dllimport) void *MBmalloc_release(size_t size);
extern "C" __declspec(dllimport) void MBfree_release(void *p);
extern "C" __declspec(dllimport) unsigned long MBHashString(const char *);

inline void *operator new(size_t size) { return MBmalloc_release(size); }
inline void operator delete(void *p) { if (p) MBfree_release(p); }

#include "../MBCom/MBGUID.h"

// Color constants: internal-linkage objects defined in the colour header; each
// TU that includes it gets 16 dynamic initializers ($E1..$E47, PDB ldata).
#include "MBColorMask.h"

const MBColorMask kCM_RGBA32(0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
const MBColorMask kCM_ARGB32(0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
const MBColorMask kCM_ARGB16(0x7c00, 0x03e0, 0x001f, 0x8000);
const MBColorMask kCM_RGB16(0xf800, 0x07e0, 0x001f, 0);
const MBColorMask kCM_BGR16(0x001f, 0x07e0, 0xf800, 0);

#include "../MBEngine/MBColor.h"

const MBColor kWhite(255, 255, 255, 0);
const MBColor kBlack(0, 0, 0, 0);
const MBColor kRed(255, 0, 0, 0);
const MBColor kGreen(0, 255, 0, 0);
const MBColor kBlue(0, 0, 255, 0);
const MBColor kYellow(255, 255, 0, 0);
const MBColor kCyan(0, 255, 255, 0);
const MBColor kMagenta(255, 0, 255, 0);
const MBColor kGrey25(64, 64, 64, 0);
const MBColor kGrey50(128, 128, 128, 0);
const MBColor kGrey75(192, 192, 192, 0);

#include "MBPointStruct.h"

#include "../MBEngine/MBPoint.h"

#include "MBPointStruct3.h"

#include "MBPoint3.h"

#include "../MBEngine/MBRect.h"

#include "../MBCom/IMBUnknown.h"

extern "C" long MBComCreateInstance(IMBUnknown *pOuter, const MBGUID &guid, IMBUnknown **unknown);

// PDB ldata order in CMBAnimButton.obj (0x1008aa68..0x1008ab68)
static const MBGUID MBIID_IMBClassFactory =
    { 0x396c7e0b, 0x172d, 0x4182, { 0xa9, 0x09, 0x48, 0xb5, 0x02, 0x3c, 0x7d, 0xea } };
static const MBGUID MBIID_IMBObject =
    { 0x7ed4fc70, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBObjectGroup =
    { 0x7ed99200, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBAnim =
    { 0x7ec2a630, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_CMBAnim =
    { 0x7ecbd150, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBSurface =
    { 0x7ee752b0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBAnimButton =
    { 0x7f121bf0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBMemoryBlock =
    { 0x7944d1b0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBEventManager =
    { 0x79913910, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBMouseable =
    { 0x79d166f0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBUserData =
    { 0x799ef9c0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBStream =
    { 0x8971e7d0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBResource =
    { 0x89981b80, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBResourceFile =
    { 0x899e3840, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBResourceFileList =
    { 0x898a5ad0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_CMBResourceMgr =
    { 0x8991fec0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };

template <class T> class MBAllocator : public std::allocator<T> {
public:
    typedef unsigned long SizeType;
    typedef T *PointerType;
    T *allocate(unsigned long n, const void *) { return (T *)MBmalloc_release(n * sizeof(T)); }
    char *_Charalloc(unsigned long n) { return (char *)MBmalloc_release(n); }
    void deallocate(void *p, unsigned long) { if (p) MBfree_release(p); }
};

typedef std::basic_string<char, std::char_traits<char>, MBAllocator<char> > MBString;

// own copy: which members are inline and which are only declared differs per unit (the original's inline budget; NOTES.md)
template <class T> class MBComPtr {
public:
    MBComPtr(void) : mInterface(0) {}
    MBComPtr(const MBGUID &clsid) : mInterface(0) { CreateObject(clsid); }
    ~MBComPtr(void) { Release(); }
    operator T *(void) const { return mInterface; }
    T *operator->(void) const { return mInterface; }
    MBComPtr &operator=(T *p) {
        if (mInterface != p) {
            T *old = mInterface;
            mInterface = p;
            if (p) p->AddRef();
            if (old) old->Release();
        }
        return *this;
    }
    void Release(void) {
        do {
            if (mInterface != 0)
                mInterface->Release();
        } while (0);
        mInterface = 0;
    }
    void Attach(T *p) {
        Release();
        mInterface = p;
    }
    long CreateObject(const MBGUID &clsid) {
        IMBUnknown *unk;
        Release();
        MBComCreateInstance(0, clsid, &unk);
        if (unk) {
            long hr = _QueryInterface(unk);
            unk->Release();
            return hr;
        }
        return -2;
    }
private:
    long _QueryInterface(IMBUnknown *const &p) {
        long hr = -3;
        if (p) {
            T *pInterface;
            hr = p->QueryInterface(T::GetGUID(), (void **)&pInterface);
            if (hr) {
                Attach(0);
                return hr;
            }
            Attach(pInterface);
            return hr;
        }
        Attach(0);
        return hr;
    }
    T *mInterface;
};

class IMBSurface;
class IMBObjectGroup;
#include "IMBStream.h"
class MBPalette;

#include "IMBObject.h"

#include "IMBObjectGroup.h"

#include "MBPixelFormat.h"

#include "IMBSurface.h"

#include "MBAnimationProperty.h"
#include "MBSequenceProperty.h"
#include "MBFrameProperty.h"

#include "IMBAnim.h"

class IMBEventListener;

#include "IMBEventManager.h"
#include "IMBMouseable.h"

#include "IMBMemoryBlock.h"

#include "IMBResourceFile.h"

#include "MBResFileListAddLoc.h"

#include "IMBResourceFileList.h"

enum MBButtonState {
    kButtonUp = 0,
    kButtonDown = 1
};

enum MBButtonBehavior {
    kPushButtonBehavior = 0,
    kCheckBoxBehavior = 1,
    kRadioButtonBehavior = 2
};

class IMBAnimButton : public IMBObject {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBAnimButton; }
    virtual void SetAnim(const char *) = 0;
    virtual void GetAnim(IMBAnim **) = 0;
    virtual void SetBroadcastChannel(long) = 0;
    virtual void SetClickMessage(long) = 0;
    virtual void SetRolloverMessage(long) = 0;
    virtual void SetLeaveRolloverMessage(long) = 0;
    virtual long GetBroadcastChannel(void) const = 0;
    virtual long GetClickMessage(void) const = 0;
    virtual long GetRolloverMessage(void) const = 0;
    virtual long GetLeaveRolloverMessage(void) const = 0;
    virtual bool IsEnabled(void) const = 0;
    virtual void Enable(void) = 0;
    virtual void Disable(void) = 0;
    virtual void SetBehavior(MBButtonBehavior) = 0;
    virtual MBButtonBehavior GetBehavior(void) const = 0;
    virtual MBButtonState GetButtonState(void) const = 0;
    virtual void SetButtonState(MBButtonState) = 0;
};

class IMBUserData : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBUserData; }
    virtual void SetUserData(IMBMemoryBlock *) = 0;
    virtual void GetUserData(IMBMemoryBlock **) = 0;
};

enum MBDrawState {
    kDrawUp = 0,
    kDrawDown = 1,
    kDrawRollover = 2,
    kDrawDisabledUp = 3,
    kDrawDisabledDown = 4
};

class CMBAnimButton : public IMBAnimButton, public IMBMouseable, public IMBObjectGroup, public IMBUserData {
public:
    CMBAnimButton(void);
    ~CMBAnimButton(void);
protected:
    unsigned long _mRefCnt;                     // +0x10
public:
    virtual long QueryInterface(const MBGUID &iid, void **obj);
    virtual unsigned long AddRef(void);
    virtual unsigned long Release(void);
    virtual MBPoint3 GetOffset(void) const;
    virtual MBPoint3 GetPosition(void) const;
    virtual void Place(const MBPoint3 &point);
    virtual void Move(const MBPoint3 &point);
    virtual MBRect GetBounds(void) const;
    virtual bool IsVisible(void) const;
    virtual long Show(void);
    virtual long Hide(void);
    virtual bool IsDirty(void) const;
    virtual void SetDirty(void);
    virtual void ClearDirty(void);
    virtual long Draw(IMBSurface *dest, MBRect &srcRect, MBRect &destRect) const;
    virtual void GetParent(IMBObjectGroup **parent) const;
    virtual IMBObjectGroup *SetParent(IMBObjectGroup *newParent);
    virtual void GetBackground(IMBObject **background) const;
    virtual void SetBackground(IMBObject *newBg);
    virtual void AddObject(IMBObject *obj);
    virtual void RemoveObject(IMBObject *obj);
    virtual void ObjectMoved(IMBObject *obj);
    virtual void SetAnim(const char *animName);
    virtual void GetAnim(IMBAnim **anim);
    virtual void SetBroadcastChannel(long channel);
    virtual void SetClickMessage(long message);
    virtual void SetRolloverMessage(long message);
    virtual void SetLeaveRolloverMessage(long message);
    virtual long GetBroadcastChannel(void) const;
    virtual long GetClickMessage(void) const;
    virtual long GetRolloverMessage(void) const;
    virtual long GetLeaveRolloverMessage(void) const;
    virtual bool IsEnabled(void) const;
    virtual void Enable(void);
    virtual void Disable(void);
    virtual void SetBehavior(MBButtonBehavior behavior);
    virtual MBButtonBehavior GetBehavior(void) const;
    virtual MBButtonState GetButtonState(void) const;
    virtual void SetButtonState(MBButtonState buttonState);
    virtual bool IsRolledOver(const MBPoint &world_mouse_pos) const;
    virtual void EnterRollover(const MBPoint &world_mouse_pos);
    virtual void WithinRollover(const MBPoint &world_mouse_pos);
    virtual void LeaveRollover(void);
    virtual bool IsClicked(const MBPoint &world_mouse_pos, short mouseButton) const;
    virtual bool IsTrackingClick(void) const;
    virtual void StartTrackingClick(const MBPoint &world_mouse_pos, short button);
    virtual void TrackClick(const MBPoint &world_mouse_pos);
    virtual void StopTrackingClick(const MBPoint &world_mouse_pos, short button);
    virtual bool IsActive(void) const;
    virtual void Activate(void);
    virtual void Deactivate(void);
    virtual void SetPixelTest(bool set);
    virtual bool GetPixelTest(void);
    virtual void SetUserData(IMBMemoryBlock *userData);
    virtual void GetUserData(IMBMemoryBlock **userData);
protected:
    bool IsWorldPointInAnim(const MBPoint &worldPoint) const;
    long GetUpSequenceIndex(void) const;
    long GetDownSequenceIndex(void) const;
    MBComPtr<IMBAnim> mAnimation;               // +0x14
    IMBObjectGroup *mParent;                    // +0x18
    IMBMemoryBlock *mUserData;                  // +0x1c
    long mDrawStateSequences[5];                // +0x20
    long mBroadcastChannel;                     // +0x34
    long mClickMessage;                         // +0x38
    long mRolloverMessage;                      // +0x3c
    long mLeaveRolloverMessage;                 // +0x40
    MBButtonBehavior mBehavior;                 // +0x44
    MBButtonState mButtonState;                 // +0x48
    MBPoint3 mOffset;                           // +0x4c
    bool mIsActive;                             // +0x58
    bool mPixelTest;                            // +0x59
    bool mIsTrackingClick;                      // +0x5a
    bool mIsEnabled;                            // +0x5b
    bool mIsOn;                                 // +0x5c
    // Stand-in (see NOTES): the PDB numbering (ctype $E67/$E68) proves one more
    // numbered item than this source has; declared only, never defined.
    static long sUnknownAnimButtonNumbering;
};

IMBUnknown *MakeCMBAnimButton(void);

#endif
