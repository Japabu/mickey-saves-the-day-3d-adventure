// MfManipActivity: an actor manipulates (grasps, places, touches ...) a target
// actor along a planned path. Layout from the copy constructor
// ??0MfManipActivity@@QAE@ABV0@@Z; the inline members are emitted by every
// unit that includes this header (the class is exported).
#pragma once
#include "MfAtomicActivity.h"
#include "MfTransform3.h"
#include "enums.h"
class MfActor;
class MfActorPath;
struct MfGCPath;
class MfHandle;
struct MfMTPath;
class MfManipSkill;
class MfSkeletonNode;
class MfStream;

class __declspec(dllexport) MfManipActivity : public MfAtomicActivity {
public:
    enum { TYPE = 'MnpA' };
    enum MfMotionType { MfMotionType_unknown };

    MfManipActivity();
    MfManipActivity(MfActor *, unsigned long, MfManipSkill *, MfGCPath *, MfMTPath *, int, MfActor *, int,
                    MfHandle *, MfSkeletonNode *, int);
    virtual ~MfManipActivity();
    virtual int Type() { return TYPE; }
    virtual int IsType(int type)
    {
        if (type == TYPE)
            return 1;
        return MfAtomicActivity::IsType(type);
    }
protected:
    virtual void _CheckBubble(unsigned long &);
public:
    virtual MfActivityStatus Update(unsigned long);
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;
    virtual int HasMotion();
    virtual long StreamType() const;

    MfMotionType getMotionType() { return mMotionType; }
    MfHandle *getHandle() { return mHandle; }
    MfActor *getTarget() { return mTarget; }

    static long const STREAM_TYPE;

    int mac;                                                // 0xac
    MfMotionType mMotionType;                               // 0xb0
    MfSkeletonNode *mb4;                                    // 0xb4
    MfActor *mTarget;                                       // 0xb8
    MfActorPath *mPath;                                     // 0xbc the sampled motion
    MfGCPath *mGCPath;                                      // 0xc0
    MfMTPath *mMTPath;                                      // 0xc4
    int mc8;                                                // 0xc8
    unsigned int mNumSamples;                               // 0xcc of mPath
    MfTransform3 mGrasp;                                    // 0xd0 from mGCPath
    MfHandle *mHandle;                                      // 0xec
    int mf0;                                                // 0xf0
};
