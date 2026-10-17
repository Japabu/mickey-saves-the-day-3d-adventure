#pragma once
// MfTransform3.h: Motivate's rigid transform, a translation and a rotation (MfActorLib/mflinalg),
// as the game declares it (SteamboatApp.pdb). Constructors, destructor and accessors are inline.
#include "MfVector3.h"
#include "MfRotation3.h"

class MfXformMatrix3;

// own copy: the game calls mflinalg through its import thunks (no dllimport) and emits its own copies of
// inline members; the owner's header declares the class MFLINALG_API, dllimport here.
class MfTransform3 {
public:
    MfTransform3(const MfRotation3 &r, const MfVector3 &t) : _translation(t), _rotation(r) {}
    MfTransform3(const MfXformMatrix3 &);
    MfTransform3(const MfTransform3 &t) : _translation(t._translation), _rotation(t._rotation) {}
    MfTransform3() {}
    ~MfTransform3() {}
    MfTransform3 &operator=(const MfTransform3 &t)
    {
        _rotation = t._rotation;
        _translation = t._translation;
        return *this;
    }
    void Multiply(MfVector3 &, const MfVector3 &);
    void Multiply(const MfTransform3 &, const MfTransform3 &);
    MfTransform3 operator~() const;
    MfVector3 operator*(const MfVector3 &) const;
    MfTransform3 operator*(const MfTransform3 &) const;
    MfVector3 &translation() { return _translation; }
    const MfVector3 &translation() const { return _translation; }
    MfRotation3 &rotation() { return _rotation; }
    const MfRotation3 &rotation() const { return _rotation; }
    void setValue(const MfRotation3 &r, const MfVector3 &t)
    {
        _rotation = r;
        _translation = t;
    }
    void interpolate(const MfTransform3 &, const MfTransform3 &, const MfTransform3 &, const MfTransform3 &, float);
    void interpolate(const MfTransform3 &, const MfTransform3 &, float);
    static MfTransform3 identity() { return MfTransform3(MfRotation3::identity(), MfVector3()); }
    float &x() { return _translation.x(); }
    float &y() { return _translation.y(); }
    float &z() { return _translation.z(); }
    void PremultiplyInverse(const MfTransform3 &);
private:
    MfVector3 _translation;                             // +0x0000
    MfRotation3 _rotation;                              // +0x000c
};
