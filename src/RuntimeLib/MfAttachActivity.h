// MfAttachActivity: moves the actor onto a transform with a manipulation
// skill. RuntimeLib's own class ('AttA', unit 100196a0-1001ab90; vtables
// 100b34ac, 100b33ec, 100b33d4).
#ifndef RUNTIMELIB_MFATTACHACTIVITY_H
#define RUNTIMELIB_MFATTACHACTIVITY_H
#include "MfAtomicActivity.h"
class MfStream;
class MfString;
class MfTransform3;

class MfAttachActivity : public MfAtomicActivity {
public:
    enum { TYPE = 'AttA' };
    MfAttachActivity();
    MfAttachActivity(MfActor *actor, unsigned long startTime, MfSkill *skill,
                     const MfTransform3 &target, int flags);
    virtual int Type() { return TYPE; }
    virtual int IsType(int type)
    {
        if (type == TYPE)
            return 1;
        return MfAtomicActivity::IsType(type);
    }
    virtual ~MfAttachActivity();
    virtual MfString PrintName() const { return MfString("Attach"); }
    virtual void PrepareForExecution();
protected:
    virtual void _CheckBubble(unsigned long &);
public:
    virtual MfActivityStatus Update(unsigned long time);
    virtual int HasMotion() { return 1; }
    virtual long StreamType() const;
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;
    static long const STREAM_TYPE;

    MfTransform3 mTarget;                                   // 0xac
    MfTransform3 mStart;                                    // 0xc8 of the skill's root, relative to the parent
    int me4;                                                // 0xe4 flag 0x1000
};

#endif
