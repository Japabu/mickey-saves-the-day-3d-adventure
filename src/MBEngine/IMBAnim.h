// IMBAnim.h - an animation object (sequences of frames of cast members).
// Declared first by the includer (still one copy per unit in wave 1): IMBUnknown, MBGUID, MBIID_IMBAnim, MBString.
#ifndef MBENGINE_IMBANIM_H
#define MBENGINE_IMBANIM_H

#include "IMBObject.h"

#include "MBAnimationProperty.h"
#include "MBFrameProperty.h"
#include "MBSequenceProperty.h"

class IMBStream;
class IMBSurface;
class MBPoint;

class IMBAnim : public IMBObject {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBAnim; }
    virtual long Load(unsigned long, IMBStream *, bool) = 0;
    virtual long Save(IMBStream *) = 0;
    virtual void Clone(IMBAnim **) = 0;
    // VC6 lays out overloaded virtuals in reverse declaration order
    virtual long SetAnimationProperty(MBAnimationProperty, const MBString &) = 0;
    virtual long SetAnimationProperty(MBAnimationProperty, unsigned long &) = 0;
    virtual long SetAnimationProperty(MBAnimationProperty, unsigned short) = 0;
    virtual long SetAnimationProperty(MBAnimationProperty, bool) = 0;
    virtual long SetSequenceProperty(long, MBSequenceProperty, unsigned short) = 0;
    virtual long SetSequenceProperty(long, MBSequenceProperty, bool) = 0;
    virtual long SetFrameProperty(long, long, MBFrameProperty, const MBPoint &) = 0;
    virtual long SetFrameProperty(long, long, MBFrameProperty, const MBString &) = 0;
    virtual long SetFrameProperty(long, long, MBFrameProperty, unsigned long) = 0;
    virtual long SetFrameProperty(long, long, MBFrameProperty, unsigned short) = 0;
    virtual long SetFrameProperty(long, long, MBFrameProperty, bool) = 0;
    virtual long GetAnimationProperty(MBAnimationProperty, unsigned long &) const = 0;
    virtual long GetAnimationProperty(MBAnimationProperty, unsigned short &) const = 0;
    virtual long GetAnimationProperty(MBAnimationProperty, bool &) const = 0;
    virtual long GetSequenceProperty(long, MBSequenceProperty, unsigned short &) const = 0;
    virtual long GetSequenceProperty(long, MBSequenceProperty, bool &) const = 0;
    virtual long GetFrameProperty(long, long, MBFrameProperty, MBPoint &) const = 0;
    virtual long GetFrameProperty(long, long, MBFrameProperty, unsigned long &) const = 0;
    virtual long GetFrameProperty(long, long, MBFrameProperty, unsigned short &) const = 0;
    virtual long GetFrameProperty(long, long, MBFrameProperty, bool &) const = 0;
    virtual long UseAnimationProperty(MBAnimationProperty, bool) = 0;
    virtual bool HasAnimationProperty(MBAnimationProperty) const = 0;
    virtual long UseSequenceProperty(long, MBSequenceProperty, bool) = 0;
    virtual bool HasSequenceProperty(long, MBSequenceProperty) const = 0;
    virtual long UseFrameProperty(long, long, MBFrameProperty, bool) = 0;
    virtual bool HasFrameProperty(long, long, MBFrameProperty) const = 0;
    virtual void SetCastID(long, long, long) = 0;
    virtual long GetCastID(long, long) const = 0;
    virtual void Play(void) = 0;
    virtual void Stop(void) = 0;
    virtual long Pause(void) = 0;
    virtual long Resume(void) = 0;
    virtual bool IsPaused(void) const = 0;
    virtual bool IsStalled(void) const = 0;
    virtual bool IsRendering(void) const = 0;
    virtual bool IsPlaying(void) const = 0;
    virtual void Scale(float, float, float) = 0;
    virtual unsigned long GetNumSequences(void) const = 0;
    virtual long AddSequence(void) = 0;
    virtual bool SetCurrentSequence(long) = 0;
    virtual long GetCurrentSequenceIndex(void) const = 0;
    virtual unsigned long GetNumFrames(long) const = 0;
    virtual long AddFrame(long) = 0;
    virtual bool SetCurrentFrame(long) = 0;
    virtual long GetCurrentFrameIndex(void) const = 0;
    virtual unsigned long GetNumCasts(void) const = 0;
    virtual long AddCastMember(IMBSurface *, const MBPoint &) = 0;
    virtual void GetCastMemberSurface(long, IMBSurface **) = 0;
    virtual void SetCastMemberSurface(long, IMBSurface *) = 0;
    virtual MBPoint GetCastMemberRegistrationPoint(long) const = 0;
    virtual void SetCastMemberRegistrationPoint(long, const MBPoint &) = 0;
    virtual MBPoint GetCastMemberAttachmentPoint(long) const = 0;
    virtual void SetCastMemberAttachmentPoint(long, const MBPoint &) = 0;
};

#endif
