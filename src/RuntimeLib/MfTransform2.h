// MfTransform2 (mflinalg.dll); declaration as in src/mflinalg.
#ifndef RUNTIMELIB_MFTRANSFORM2_H
#define RUNTIMELIB_MFTRANSFORM2_H
#include "MfVector2.h"
#include "MfRotation2.h"

class __declspec(dllimport) MfTransform2 {
public:
    MfTransform2() {}
    MfTransform2(const MfRotation2 &r, const MfVector2 &t) : _translation(t), _rotation(r) {}
    MfTransform2(const MfTransform2 &t)
    {
        _rotation = t._rotation;
        _translation = t._translation;
    }
    ~MfTransform2() {}
    MfTransform2 &operator=(const MfTransform2 &t)
    {
        _rotation = t._rotation;
        _translation = t._translation;
        return *this;
    }
    MfVector2 &translation() { return _translation; }
    const MfVector2 &translation() const { return _translation; }
    MfRotation2 &rotation() { return _rotation; }
    const MfRotation2 &rotation() const { return _rotation; }
    float x() { return _translation.x(); }
    float y() { return _translation.y(); }
    static MfTransform2 identity() { return MfTransform2(MfRotation2::identity(), MfVector2()); }
    MfTransform2 operator~();
    MfTransform2 operator*(const MfTransform2 &);
    MfVector2 operator*(const MfVector2 &);
    void interpolate(const MfTransform2 &, const MfTransform2 &, float);
private:
    MfVector2 _translation;
    MfRotation2 _rotation;
};

#endif
