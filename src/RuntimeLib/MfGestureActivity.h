#pragma once
#include "MfAtomicActivity.h"
#include "enums.h"
class MfActor;
class MfSkill;
class MfStream;
class MfString;

class __declspec(dllexport) MfGestureActivity : public MfAtomicActivity {
public:
    enum { TYPE = 'GesA' };
    virtual int Type() { return TYPE; }
    virtual int IsType(int type)
    {
        if (type == TYPE)
            return 1;
        return MfAtomicActivity::IsType(type);
    }
    virtual ~MfGestureActivity();
    virtual MfString PrintName() const { return MfString("Gesture"); }
protected:
    virtual void _CheckBubble(unsigned long &);
public:
    virtual MfActivityStatus Update(unsigned long);
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;
    virtual int AnimateSubtree(MfString const &);
    virtual int IgnoreSubtree(MfString const &);
    virtual int WillFinish() { return mRepeat != -1; }
    virtual long StreamType() const;
    MfGestureActivity & operator=(MfGestureActivity const &);
    MfGestureActivity();
    MfGestureActivity(MfActor *, unsigned long, MfSkill *, int, int);
    MfGestureActivity(MfGestureActivity const &);
    static long const STREAM_TYPE;
protected:
    void _FixTrace();
    void _FixXform();
public:
    int mRepeat;                                            // 0xac -1: forever
    MfTransform3 mTrace;                                    // 0xb0
    int mcc;                                                // 0xcc
    int md0;                                                // 0xd0 request option 0x4000
    int md4;                                                // 0xd4 request option 0x1000
    int md8;                                                // 0xd8
    int mdc;                                                // 0xdc
};
