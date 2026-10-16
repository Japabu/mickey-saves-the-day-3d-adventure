// MfXformMatrix3 (mflinalg.dll): a 3x4 affine matrix. Only what RuntimeLib
// uses; as in src/mflinalg/MfXformMatrix3.h.
#ifndef RUNTIMELIB_MFXFORMMATRIX3_H
#define RUNTIMELIB_MFXFORMMATRIX3_H
#include "MfRotation3.h"
#include "MfVector3.h"

class __declspec(dllimport) MfXformMatrix3 {
public:
    MfXformMatrix3();
    MfXformMatrix3(const MfXformMatrix3 &);
    MfXformMatrix3(const MfRotation3 &r, const MfVector3 &t) { setValue(r, t); }
    ~MfXformMatrix3() {}
    void setValue(const MfRotation3 &, const MfVector3 &);
    void multiply(const MfVector3 &, MfVector3 &) const;

    float _m[3][4];
};

#endif
