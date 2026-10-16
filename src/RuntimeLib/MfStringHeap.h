// MfStringHeap (mfutil.dll): a list of heap MfStrings handed out by
// AllocString; declaration as in src/mfutil.
#ifndef RUNTIMELIB_MFSTRINGHEAP_H
#define RUNTIMELIB_MFSTRINGHEAP_H
#include "MfStringList.h"

class __declspec(dllimport) MfStringHeap : public MfStringList {
public:
    MfStringHeap();
    virtual ~MfStringHeap();
    MfString *AllocString();
    void ReleaseString(MfString *s);
    void ReleaseAll();
};

#endif
