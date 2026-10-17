// MfLocoPlanCache: what an MfLiveActor remembers of its last locomotion plan
// (exported struct), and MfRoadEdge, which it points to. Layout from the
// constructor 10002dd0 and from ??0MfLiveActor@@QAE@W4MF_THREAD_NAME@@@Z.
#ifndef RUNTIMELIB_MFLOCOPLANCACHE_H
#define RUNTIMELIB_MFLOCOPLANCACHE_H
// the includes and forward declarations it needs, in the order the two structs
// had when they shared one header (MfRoadEdge's note below: this compiler's
// output depends on the declarations a unit sees)
#include "MfManipPlanCache.h"
#include "RuntimeLib_api.h"

struct MfRoadPortal;

// an edge between two portals of a road map cell (MfRoadMap.h; the actor
// sets mBlocked). Declared here: its full declaration in this header keeps
// MfLocoSkill::CycleLength identical, a forward declaration flips it.
struct MfRoadEdge {
    MfRoadEdge();                                           // 1005ec90
    ~MfRoadEdge();                                          // 1005ecb0
    // the portal at the other end, or 0
    MfRoadPortal *Other(MfRoadPortal *portal)
    {
        return portal == mFrom ? mTo : (portal == mTo ? mFrom : 0);
    }

    int mCost;                                              // 0x00
    MfRoadPortal *mFrom;                                    // 0x04
    MfRoadPortal *mTo;                                      // 0x08
    int m0c;                                                // 0x0c
    int mBlocked;                                           // 0x10
};

// the last locomotion request (MfLiveActor::RequestLocoPlan stores its
// arguments here so that the request can be repeated, RespondToClearPath)
struct RUNTIME_API MfLocoPlanCache {
    MfLocoPlanCache()
        : mLocoSkill(0), mSkill(0), m1c(0), m20(0), mIndex1(0x35), mIndex2(0x35), mIndex3(0x35)
    {
        ma4 = 0;
        ma8 = 0;
        mac = 0;
        mb0 = 0;
        mPath = 0;
    }
    ~MfLocoPlanCache() {}
    int m00;                                                // 0x00 request argument 7
    MfTransform2 mGoal;                                     // 0x04
    MfFloor *mFloor;                                        // 0x10
    MfLocoSkill *mLocoSkill;                                // 0x14
    MfSkill *mSkill;                                        // 0x18
    int m1c;                                                // 0x1c request argument 12
    int m20;                                                // 0x20 request argument 8
    MfLiveActor *mTarget;                                   // 0x24
    float m28, m2c;                                         // 0x28 request arguments 10, 11
    float m30, m34;                                         // 0x30 request arguments 13, 14
    MfRoadEdge *mBlockedEdge;                               // 0x38 where the path was blocked:
    int mBlockedFrom, mBlockedTo;                           // 0x3c an edge of the road map, a
    int mBlockedCell;                                       // 0x44 pair of nodes or a cell
    MfIndex mIndex1;                                        // 0x48
    MfIndex mIndex2;                                        // 0x5c
    MfIndex mIndex3;                                        // 0x70
    int mEventID;                                           // 0x84
    int m88;                                                // 0x88 request argument 1
    MfTransform2 m8c;                                       // 0x8c
    MfTransform2 mLast;                                     // 0x98
    int ma4, ma8, mac, mb0;                                 // 0xa4
    int mb4, mb8;                                           // 0xb4
    int mbc;                                                // 0xbc request argument 6
    MfDblList *mPath;                                       // 0xc0 MfPathPoints, the planned way
    MfDblListItem *mPathPos;                                // 0xc4
    int mc8;                                                // 0xc8
};

#endif
