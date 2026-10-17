// MBAnimSequence.h - one sequence of an animation: its frames and playback properties.
// Declared first by the includer (still one copy per unit in wave 1): IMBUnknown, MBGUID, MBString, MBVector.
#ifndef MBENGINE_MBANIMSEQUENCE_H
#define MBENGINE_MBANIMSEQUENCE_H

#include "MBFrameProperty.h"
#include "MBSequenceProperty.h"

class MBAnimFrame;
class MBPoint;

class MBAnimSequence : public IMBUnknown {
public:
    MBAnimSequence(void);
protected:
    unsigned long _mRefCnt;
public:
    virtual long QueryInterface(const MBGUID &iid, void **obj);
    virtual unsigned long AddRef(void);
    virtual unsigned long Release(void);
    void SetCastID(long frameIndex, long castID);
    long GetCastID(long frameIndex) const;
    long SetSequenceProperty(MBSequenceProperty property, bool value);
    long SetSequenceProperty(MBSequenceProperty property, unsigned short value);
    long SetFrameProperty(long frameIndex, MBFrameProperty property, bool value);
    long SetFrameProperty(long frameIndex, MBFrameProperty property, unsigned short value);
    long SetFrameProperty(long frameIndex, MBFrameProperty property, unsigned long value);
    long SetFrameProperty(long frameIndex, MBFrameProperty property, const MBString &value);
    long SetFrameProperty(long frameIndex, MBFrameProperty property, const MBPoint &value);
    long GetSequenceProperty(MBSequenceProperty property, bool &value) const;
    long GetSequenceProperty(MBSequenceProperty property, unsigned short &value) const;
    long GetFrameProperty(long frameIndex, MBFrameProperty property, bool &value) const;
    long GetFrameProperty(long frameIndex, MBFrameProperty property, unsigned short &value) const;
    long GetFrameProperty(long frameIndex, MBFrameProperty property, unsigned long &value) const;
    long GetFrameProperty(long frameIndex, MBFrameProperty property, MBPoint &value) const;
    long UseProperty(MBSequenceProperty property, bool use);
    bool HasProperty(MBSequenceProperty property) const;
    long UseFrameProperty(long frameIndex, MBFrameProperty property, bool use);
    bool HasFrameProperty(long frameIndex, MBFrameProperty property) const;
    long GetNumFrames(void) const;
    long AddFrame(void);
protected:
    class MBAnimFrame *GetAnimFrame(long frameIndex) const;
    MBVector<class MBAnimFrame *> mFrames;
    unsigned long mProperties;
    unsigned short mZPlane;
    unsigned short mDelay;
    unsigned short mSequenceEndAction;
    unsigned short mJumpSequence;
    unsigned short mJumpRandomSequenceMin;
    unsigned short mJumpRandomSequenceMax;
    unsigned short mTileWidth;
    unsigned short mTileHeight;
    bool mInSync;
private:
    ~MBAnimSequence(void);
};

#endif
