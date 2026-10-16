#pragma once
// MfJointKeyframe: one key of a joint track ('JKey'): the joint's transform
// relative to its parent and in the actor, each with two spline control
// points, and how the track runs on to the next key.
#include "MfKeyframe.h"
#include "MfTransform3.h"
#include "MfVector3.h"
class MfRotation3;
class MfStream;

// How the track runs from a key to the next one.
enum { MF_JOINT_LINEAR = 0, MF_JOINT_SPLINE = 1, MF_JOINT_HOLD = 2 };

// size 0xd4
class __declspec(dllexport) MfJointKeyframe : public MfKeyframe {
public:
    MfJointKeyframe();
    MfJointKeyframe(const MfJointKeyframe &);
    virtual ~MfJointKeyframe();
    virtual long StreamType() const;
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;
    virtual MfKeyframe *CreateCopy();
    static void ComputeControlPoint(const MfTransform3 *prev, const MfTransform3 *cur,
                                    const MfTransform3 *next, MfRotation3 &in,
                                    MfRotation3 &out, MfVector3 &tangent);
    void ComputeBlends(const MfJointKeyframe *prev, const MfJointKeyframe *next, int interpolation);
    void Interpolate(MfJointKeyframe *from, MfJointKeyframe *to, float t);
    void Scale(const MfVector3 &);
    void Transform(const MfTransform3 &);
    static const long STREAM_TYPE;

    int mFlags;                         // +0x0c
    MfTransform3 mXform;                // +0x10 relative to the parent joint
    MfTransform3 mXformCtrl[2];         // +0x2c its spline control points
    MfTransform3 mWorld;                // +0x64 in the actor
    MfTransform3 mWorldCtrl[2];         // +0x80
    MfVector3 mOffset;                  // +0xb8
    int mUserData;                      // +0xc4
    float mBlend[2];                    // +0xc8 ease out of the key / into the next one
    int mInterpolation;                 // +0xd0 MF_JOINT_LINEAR ...
};
