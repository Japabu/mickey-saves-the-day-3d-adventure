// MfPoTransform (MfPoLibrary.dll): the Piccolo object of a transform. Only
// what RuntimeLib uses (dynamic_cast target, Transform()).
#ifndef RUNTIMELIB_MFPOTRANSFORM_H
#define RUNTIMELIB_MFPOTRANSFORM_H
#include "MfPoBase.h"
#include "MfTransform3.h"

class __declspec(dllimport) MfPoTransform : public MfPoBase {
public:
    virtual ~MfPoTransform();
    MfTransform3 Transform();

    MfTransform3 *_t3;                                      // 0x14
};

#endif
