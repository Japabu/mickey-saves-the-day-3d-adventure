// MfNewMTPath: a new move path, empty, whose configurations are freed with it
// (MfManipPlanner::PlanMove and ReadMTPath create one). The name is ours: the
// original zeroes and frees these members inline where it uses an MfMTPath.
#ifndef RUNTIMELIB_MFNEWMTPATH_H
#define RUNTIMELIB_MFNEWMTPATH_H
#include "MfMsgData.h"

struct MfNewMTPath : public MfMTPath {
    MfNewMTPath()
    {
        mFrom = 0;
        mTo = 0;
    }
    ~MfNewMTPath()
    {
        if (mFrom)
            delete[] mFrom;
        if (mTo)
            delete[] mTo;
    }
};

#endif
