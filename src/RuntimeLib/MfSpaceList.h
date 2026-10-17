// MfSpaceList: the world's list of spaces.
#ifndef RUNTIMELIB_MFSPACELIST_H
#define RUNTIMELIB_MFSPACELIST_H
#include "MfDblList.h"
#include "RuntimeLib_api.h"
class MfSpace;
class MfString;

class RUNTIME_API MfSpaceList : public MfDblList {
public:
    MfSpaceList();
    virtual ~MfSpaceList() {}
    void Prepend(const MfSpace *space) { Insert((void *)space, LP_BEFORE, pliFirst); }
    int Contains(MfSpace *space);
    MfSpace *Contains(MfString &fileName);
    int Remove(MfSpace *space, int destroy);
};

#endif
