// MfNewGCPath: a new grasp path, no configurations yet (MfManipPlanner::PlanGrasp
// and ReadGCPath create one). The name is ours: the original zeroes these
// members inline where it news an MfGCPath.
#ifndef RUNTIMELIB_MFNEWGCPATH_H
#define RUNTIMELIB_MFNEWGCPATH_H
#include "MfGCPath.h"

struct MfNewGCPath : public MfGCPath {
    MfNewGCPath()
    {
        m00 = 0;
        m04 = 0;
        m08 = 0;
        m0c = 0;
        m10 = 0;
        m38 = 0;
        m3c = 0;
    }
};

#endif
