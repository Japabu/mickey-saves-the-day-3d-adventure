// MfTransform3 (mflinalg.dll): a rigid transform, translation then rotation.
// The constructors, assignment and destructor are inline in the original header.
#ifndef RUNTIMELIB_MFTRANSFORM3_H
#define RUNTIMELIB_MFTRANSFORM3_H
#include "MfRotation3.h"
#include "MfVector3.h"
class MfXformMatrix3;

class __declspec(dllimport) MfTransform3 {
public:
    MfRotation3 &rotation() { return mRotation; }
    const MfRotation3 &rotation() const { return mRotation; }
    MfTransform3 &operator=(const MfTransform3 &t) { mRotation = t.mRotation; mTranslation = t.mTranslation; return *this; }
    MfTransform3 operator*(MfTransform3 const &) const;
    MfTransform3 operator~() const;
    MfTransform3() {}
    MfTransform3(const MfRotation3 &r, const MfVector3 &v) : mTranslation(v), mRotation(r) {}
    MfTransform3(const MfTransform3 &t) : mTranslation(t.mTranslation), mRotation(t.mRotation) {}
    MfTransform3(MfXformMatrix3 const &);
    MfVector3 &translation() { return mTranslation; }
    const MfVector3 &translation() const { return mTranslation; }
    MfVector3 operator*(MfVector3 const &) const;
    float &x() { return mTranslation.mX; }
    float &y() { return mTranslation.mY; }
    float &z() { return mTranslation.mZ; }
    static MfTransform3 __cdecl identity();
    void Multiply(MfTransform3 const &, MfTransform3 const &);
    void Multiply(MfVector3 &, MfVector3 const &);
    void PremultiplyInverse(MfTransform3 const &);
    void interpolate(MfTransform3 const &, MfTransform3 const &, MfTransform3 const &, MfTransform3 const &, float);
    void interpolate(MfTransform3 const &, MfTransform3 const &, float);
    void setValue(MfRotation3 const &, MfVector3 const &);
    ~MfTransform3() {}
    MfVector3 mTranslation;
    MfRotation3 mRotation;
};

#endif
