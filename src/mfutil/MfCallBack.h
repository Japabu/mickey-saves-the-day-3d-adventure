// MfCallBack: the callback base of the mfutil containers (mfutil.dll). Every member is
// inline; the first object of mfutil.dll (MfCopyFolder.cpp) keeps the exported copies.
// vtable 0x100172d4: ??_E, CallBack(a,b,c) +4, CallBack(a,b) +8, CallBack(a) +0xc
#ifndef MFUTIL_MFCALLBACK_H
#define MFUTIL_MFCALLBACK_H

#include "mfutil_api.h"

class MFUTIL_API MfCallBack {
public:
    MfCallBack() {}
    virtual ~MfCallBack() {}
    virtual int CallBack(void *a) { return 0; }
    virtual int CallBack(void *a, void *b) { return 0; }
    virtual int CallBack(void *a, void *b, void *c) { return 0; }
};

#endif
