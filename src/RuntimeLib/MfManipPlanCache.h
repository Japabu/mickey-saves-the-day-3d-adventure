// MfManipPlanCache: what an MfLiveActor remembers of its last manipulation
// plan (exported struct). Layout from the constructor 10002bb0 and from
// ??0MfLiveActor@@QAE@W4MF_THREAD_NAME@@@Z. The includes and forward
// declarations serve MfLocoPlanCache.h too, which includes this header (the
// two structs shared one header; the order of the declarations is kept).
#ifndef RUNTIMELIB_MFMANIPPLANCACHE_H
#define RUNTIMELIB_MFMANIPPLANCACHE_H
#include "MfDblList.h"
#include "MfIndex.h"
#include "MfTransform2.h"
#include "MfTransform3.h"
#include "MfVector3.h"
#include "RuntimeLib_api.h"
class MfFloor;
class MfHandle;
class MfLiveActor;
class MfLocoSkill;
class MfManipSkill;
struct MfPathPoint;
class MfSkeletonNode;
class MfSkill;

// the last manipulation request (MfLiveActor::RequestManipGCPlan stores its
// arguments here; HandleCheckReachable continues with them)
struct RUNTIME_API MfManipPlanCache {
    MfManipPlanCache()
        : mTarget(0), mFloor(0), mLocoSkill(0), mSkill(0), mOtherSkill(0), mManipSkill(0),
          mHandle(0), mTargetConfig(0)
    {
    }
    ~MfManipPlanCache() {}
    MfLiveActor *mTarget;                                   // 0x00
    MfFloor *mFloor;                                        // 0x04
    MfLocoSkill *mLocoSkill;                                // 0x08
    MfSkill *mSkill;                                        // 0x0c
    MfManipSkill *mOtherSkill;                              // 0x10 request argument 5
    MfManipSkill *mManipSkill;                              // 0x14
    MfHandle *mHandle;                                      // 0x18
    MfSkeletonNode *mNode;                                  // 0x1c the node of the manipulation
    int m20, m24, m28;                                      // 0x20
    MfTransform3 mFrom;                                     // 0x2c the target's parent transform
    MfTransform3 *mTargetConfig;                            // 0x48 one transform per node of mTarget
    int m4c;                                                // 0x4c request argument 10
    MfTransform3 mTo;                                       // 0x50 request argument 11
    int m6c;                                                // 0x6c
    int mEventID;                                           // 0x70
    int m74;                                                // 0x74 request argument 1
    MfVector3 mOffset;                                      // 0x78
};

#endif
