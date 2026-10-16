// MfSpaceList: the world's list of spaces.
#ifndef RUNTIMELIB_MFSPACELIST_H
#define RUNTIMELIB_MFSPACELIST_H
#include "MfDblList.h"
class MfSpace;
class MfString;

class __declspec(dllexport) MfSpaceList : public MfDblList {
public:
    MfSpaceList();
    virtual ~MfSpaceList() {}
    void Prepend(const MfSpace *space) { Insert((void *)space, LP_BEFORE, pliFirst); }
    int Contains(MfSpace *space);
    MfSpace *Contains(MfString &fileName);
    int Remove(MfSpace *space, int destroy);
};

#endif
