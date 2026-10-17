// MBAnimCast.h - one cast member of an animation: a surface decoded on demand from a Squish frame.
// Declared first by the includer (still one copy per unit in wave 1): IMBUnknown, MBComPtr, MBGUID.
#ifndef MBENGINE_MBANIMCAST_H
#define MBENGINE_MBANIMCAST_H

#include "../MBEngine/MBPoint.h"
#include "MBSize.h"

class IMBStream;
class IMBSurface;
struct MBPixelFormat;
class SquishImageFrame;

class MBAnimCast : public IMBUnknown {
public:
    MBAnimCast(void);
protected:
    unsigned long _mRefCnt;
public:
    virtual long QueryInterface(const MBGUID &iid, void **obj);
    virtual unsigned long AddRef(void);
    virtual unsigned long Release(void);
    MBSize GetSize(void) const;
    void GetSurface(unsigned long currentTime, IMBSurface **surface);
    void GetSurface(IMBSurface **surface);
    void SetSurface(IMBSurface *surface);
    bool IsSurfaceResident(void) const;
    MBPoint GetRegistrationPoint(void) const;
    void SetRegistrationPoint(const MBPoint &registrationPoint);
    MBPoint GetAttachmentPoint(void) const;
    void SetAttachmentPoint(const MBPoint &attachmentPoint);
    void ClearAttachmentPoint(void);
    bool HasAttachmentPoint(void) const;
    bool Init(IMBStream *stream, MBPixelFormat pf);
    unsigned long GetLastAccess(void) const;
    unsigned long Purge(void);
    unsigned long GetApproxSize(void) const;
    void Lock(void);
protected:
    bool ReloadSquishFrame(bool needToSeek);
    bool SquishFrameToSurface(void);
    MBPoint mRegistrationPoint;     // +0x08
    MBPoint mAttachmentPoint;       // +0x10
    MBSize mSize;                   // +0x18
    MBComPtr<IMBSurface> mSurface;  // +0x20
    MBComPtr<IMBStream> mSource;    // +0x24
    unsigned long mSourceOffset;    // +0x28
    class SquishImageFrame *mFrame;       // +0x2c
    unsigned long mDepth;           // +0x30
    unsigned long mSquishPixelFmt;  // +0x34
    unsigned long mLastAccess;      // +0x38
    bool mCanBePurged;              // +0x3c
    bool mHasAttachmentPoint;       // +0x3d
private:
    ~MBAnimCast(void);
};

#endif
