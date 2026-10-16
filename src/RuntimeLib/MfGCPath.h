// MfGCPath (RuntimeLib's own struct, layout from its destructor 10032590 and
// MfManipActivity, which copies m14).
#ifndef RUNTIMELIB_MFGCPATH_H
#define RUNTIMELIB_MFGCPATH_H
#include "MfTransform3.h"
class MfActor;

// a planned grasp (the planner's GC path; MfManipActivity, MfConfigRecord):
// the configurations along the way and the grasp transform
struct MfGCPath {
    ~MfGCPath();

    MfTransform3 *m00, *m04, *m08, *m0c, *m10;              // 0x00
    MfTransform3 m14;                                       // 0x14
    int m30, m34;                                           // 0x30
    MfTransform3 *m38, *m3c;                                // 0x38
    MfActor *mTarget;                                       // 0x40
};

#endif
