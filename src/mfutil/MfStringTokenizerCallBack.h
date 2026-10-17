// MfStringTokenizerCallBack: what MfString::Tokenize calls for every token
// (mfutil.dll; not derived from MfCallBack). All members inline; MfCopyFolder.cpp
// keeps the exported copies. vtable 0x100172f4: ??_E, CallBack
#ifndef MFUTIL_MFSTRINGTOKENIZERCALLBACK_H
#define MFUTIL_MFSTRINGTOKENIZERCALLBACK_H

#include "mfutil_api.h"

class MfString;

class MFUTIL_API MfStringTokenizerCallBack {
public:
    MfStringTokenizerCallBack() {}
    virtual ~MfStringTokenizerCallBack() {}
    virtual int CallBack(const MfString &token, MfString &rest) { return 1; }
};

#endif
