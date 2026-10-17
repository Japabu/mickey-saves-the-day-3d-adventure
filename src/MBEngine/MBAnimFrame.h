// MBAnimFrame.h - one frame of an animation sequence: cast member, offset, delay, sounds and scripts it starts.
// Declared first by the includer (still one copy per unit in wave 1): IMBUnknown, MBGUID, MBString.
#ifndef MBENGINE_MBANIMFRAME_H
#define MBENGINE_MBANIMFRAME_H

#include "../MBEngine/MBPoint.h"

#include "MBFrameProperty.h"

class IMBScriptData;

class MBAnimFrame : public IMBUnknown {
public:
    MBAnimFrame(void);
protected:
    unsigned long _mRefCnt;
public:
    virtual long QueryInterface(const MBGUID &iid, void **obj);
    virtual unsigned long AddRef(void);
    virtual unsigned long Release(void);
    long SetFrameProperty(MBFrameProperty property, bool value);
    long SetFrameProperty(MBFrameProperty property, unsigned short value);
    long SetFrameProperty(MBFrameProperty property, unsigned long value);
    long SetFrameProperty(MBFrameProperty property, const MBString &value);
    long SetFrameProperty(MBFrameProperty property, const MBPoint &value);
    long GetFrameProperty(MBFrameProperty property, bool &value) const;
    long GetFrameProperty(MBFrameProperty property, unsigned short &value) const;
    long GetFrameProperty(MBFrameProperty property, unsigned long &value) const;
    long GetFrameProperty(MBFrameProperty property, MBPoint &value) const;
    long UseProperty(MBFrameProperty property, bool use);
    bool HasProperty(MBFrameProperty property) const;
    long GetCastID(void) const;
    void SetCastID(long castID);
protected:
    long mCastID;
    unsigned long mProperties;
    MBPoint mCastMemberOffset;
    unsigned long mStartSound;
    unsigned long mStartAnim;
    unsigned long mStopAnim;
    unsigned long mStartScript;
    unsigned long mStopScript;
    unsigned short mFrameDelay;
    unsigned short mStartAnimSequence;
    bool mNotify;
    IMBScriptData *mScriptData;
private:
    ~MBAnimFrame(void);
};

#endif
