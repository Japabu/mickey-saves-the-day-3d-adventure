// MfPoVector3 (MfPoLibrary.dll): the Piccolo object of a vector. Only what
// RuntimeLib uses.
#ifndef RUNTIMELIB_MFPOVECTOR3_H
#define RUNTIMELIB_MFPOVECTOR3_H
#include "MfPoBase.h"
#include "MfVector3.h"

class __declspec(dllimport) MfPoVector3 : public MfPoBase {
public:
    MfPoVector3(const MfVector3 &);
    virtual ~MfPoVector3();

    MfVector3 *_v3;                                         // 0x14
    int bOwner;                                             // 0x18
};

#endif
