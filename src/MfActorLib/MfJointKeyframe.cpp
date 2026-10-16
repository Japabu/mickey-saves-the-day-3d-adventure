// MfActorLib.dll 0x10009dd0-0x1000ad80: MfJointKeyframe.
#include "MfJointKeyframe.h"
#include "MfRotation3.h"
#include "MfStream.h"

// 0x10009dd0 ?CreateCopy@MfJointKeyframe@@UAEPAVMfKeyframe@@XZ
MfKeyframe *MfJointKeyframe::CreateCopy()
{
    return new MfJointKeyframe(*this);
}

// 0x10009e30 ??4MfJointKeyframe@@QAEAAV0@ABV0@@Z (compiler-generated)

const long MfJointKeyframe::STREAM_TYPE = 'yeKJ';

// 0x10009f80 ?CreateJointKeyframe@@YAPAVMfStreamObject@@XZ (stream constructor for 'JKey')
static MfStreamObject *CreateJointKeyframe()
{
    return new MfJointKeyframe;
}

// 0x10009fe0 _$E8 (jumps to _$E7)
// 0x10009ff0 _$E7
static MfStreamConstructorEntry s_entry(MfJointKeyframe::STREAM_TYPE, CreateJointKeyframe);

// 0x1000a010 ?StreamType@MfJointKeyframe@@UBEJXZ
long MfJointKeyframe::StreamType() const
{
    return STREAM_TYPE;
}

// 0x1000a020 ??0MfJointKeyframe@@QAE@XZ
MfJointKeyframe::MfJointKeyframe()
    : mFlags(0), mUserData(0), mInterpolation(MF_JOINT_LINEAR)
{
    mBlend[0] = 2.0f / 3.0f;
    mBlend[1] = 1.0f / 3.0f;
}

// 0x1000a110 ??_EMfJointKeyframe@@UAEPAXI@Z (compiler-generated)

// 0x1000a170 ??0MfJointKeyframe@@QAE@ABV0@@Z
MfJointKeyframe::MfJointKeyframe(const MfJointKeyframe &key)
    : MfKeyframe(key), mFlags(key.mFlags), mXform(key.mXform), mWorld(key.mWorld),
      mOffset(key.mOffset), mUserData(key.mUserData), mInterpolation(key.mInterpolation)
{
    for (int i = 0; i < 2; i++) {
        mXformCtrl[i] = key.mXformCtrl[i];
        mWorldCtrl[i] = key.mWorldCtrl[i];
        mBlend[i] = key.mBlend[i];
    }
}

// 0x1000a310 ??1MfJointKeyframe@@UAE@XZ
MfJointKeyframe::~MfJointKeyframe()
{
}

// 0x1000a390 ?ComputeBlends@MfJointKeyframe@@QAEXPBV1@0H@Z
// Sets the control points of the key from its neighbours.
void MfJointKeyframe::ComputeBlends(const MfJointKeyframe *prev, const MfJointKeyframe *next,
                                    int interpolation)
{
    MfRotation3 out, in;
    MfVector3 tangent;
    mInterpolation = interpolation;
    const MfTransform3 *prevWorld = prev ? &prev->mWorld : 0;
    const MfTransform3 *nextWorld = next ? &next->mWorld : 0;
    MfTransform3 *world = &mWorld;
    const MfTransform3 *prevXform = prev ? &prev->mXform : 0;
    const MfTransform3 *nextXform = next ? &next->mXform : 0;
    MfTransform3 *xform = &mXform;
    switch (interpolation) {
    case MF_JOINT_SPLINE:
        ComputeControlPoint(prevWorld, world, nextWorld, in, out, tangent);
        mWorldCtrl[0].setValue(in, world->translation() - tangent);
        mWorldCtrl[1].setValue(out, world->translation() + tangent);
        ComputeControlPoint(prevXform, xform, nextXform, in, out, tangent);
        mXformCtrl[0].setValue(in, xform->translation() - tangent);
        mXformCtrl[1].setValue(out, xform->translation() + tangent);
        break;
    case MF_JOINT_LINEAR:
        if (prevWorld)
            mWorldCtrl[0].interpolate(*prevWorld, *world, 2.0f / 3.0f);
        else
            mWorldCtrl[0] = *world;
        if (nextWorld)
            mWorldCtrl[1].interpolate(*world, *nextWorld, 1.0f / 3.0f);
        else
            mWorldCtrl[1] = *world;
        if (prevXform)
            mXformCtrl[0].interpolate(*prevXform, *xform, 2.0f / 3.0f);
        else
            mXformCtrl[0] = *xform;
        if (nextXform)
            mXformCtrl[1].interpolate(*xform, *nextXform, 1.0f / 3.0f);
        else
            mXformCtrl[1] = *xform;
        break;
    }
}

// 0x1000a6d0 ?ComputeControlPoint@MfJointKeyframe@@SAXPBVMfTransform3@@00AAVMfRotation3@@1AAVMfVector3@@@Z
// The rotations a third of the way into and out of cur and the tangent of
// the translation at cur (Catmull-Rom like; one sided at the ends).
void MfJointKeyframe::ComputeControlPoint(const MfTransform3 *prev, const MfTransform3 *cur,
                                          const MfTransform3 *next, MfRotation3 &in,
                                          MfRotation3 &out, MfVector3 &tangent)
{
    MfVector3 toNext, fromPrev;
    MfRotation3 mid, leap;
    if (prev && next) {
        toNext = next->translation() - cur->translation();
        fromPrev = cur->translation() - prev->translation();
        tangent = (toNext + fromPrev) / 6.0f;
        leap.leap(prev->rotation(), cur->rotation());
        mid.bisect(leap, next->rotation());
        out.interpolate(cur->rotation(), mid, 1.0f / 3.0f, 0);
        in.leap(out, cur->rotation());
    } else if (next) {
        tangent = (next->translation() - cur->translation()) / 3.0f;
        out.interpolate(cur->rotation(), next->rotation(), 1.0f / 3.0f, 0);
        in.leap(out, cur->rotation());
    } else if (prev) {
        tangent = (cur->translation() - prev->translation()) / 3.0f;
        in.interpolate(cur->rotation(), prev->rotation(), 1.0f / 3.0f, 0);
        out.leap(in, cur->rotation());
    }
}

// 0x1000a970 ?Scale@MfJointKeyframe@@QAEXABVMfVector3@@@Z
void MfJointKeyframe::Scale(const MfVector3 &scale)
{
    int i;
    mWorld.translation() *= scale;
    for (i = 0; i < 2; i++)
        mWorldCtrl[i].translation() *= scale;
    mOffset *= scale;
    mXform.translation() *= scale;
    for (i = 0; i < 2; i++)
        mXformCtrl[i].translation() *= scale;
}

// 0x1000a9d0 ?Transform@MfJointKeyframe@@QAEXABVMfTransform3@@@Z
void MfJointKeyframe::Transform(const MfTransform3 &t)
{
    int i;
    mWorld = t * mWorld;
    for (i = 0; i < 2; i++)
        mWorldCtrl[i] = t * mWorldCtrl[i];
    mOffset = t * mOffset;
    mXform = t * mXform;
    for (i = 0; i < 2; i++)
        mXformCtrl[i] = t * mXformCtrl[i];
}

// 0x1000ab60 ?Interpolate@MfJointKeyframe@@QAEXPAV1@0M@Z
// The key at t (0..1) between from and to; to 0 (or a hold) gives from.
void MfJointKeyframe::Interpolate(MfJointKeyframe *from, MfJointKeyframe *to, float t)
{
    mFlags = from->mFlags;
    mUserData = from->mUserData;
    if (to && from->mInterpolation != MF_JOINT_HOLD) {
        float s = 1.0f - t;
        float out = from->mBlend[1] * s * s * t * 3.0f;
        float in = to->mBlend[0] * s * t * t * 3.0f;
        t = out + in + t * t * t;
        mFlags &= to->mFlags;
        if (from->mInterpolation == MF_JOINT_LINEAR && to->mInterpolation == MF_JOINT_LINEAR) {
            mXform.interpolate(from->mXform, to->mXform, t);
            mWorld.interpolate(from->mWorld, to->mWorld, t);
        } else {
            mXform.interpolate(from->mXform, to->mXform, t);
            mWorld.interpolate(from->mWorld, from->mWorldCtrl[1], to->mWorldCtrl[0], to->mWorld, t);
        }
        mOffset.interpolate(from->mOffset, to->mOffset, t);
    } else {
        mXform = from->mXform;
        mWorld = from->mWorld;
        mOffset = from->mOffset;
    }
}

// 0x1000ad20 ?ReadFromStream@MfJointKeyframe@@UAE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfJointKeyframe::ReadFromStream(MfStream &s)
{
    MfKeyframe::ReadFromStream(s);
    s.ReadBytes(sizeof(MfJointKeyframe) - 0xc, &mFlags);
    return s.Status();
}

// 0x1000ad50 ?WriteToStream@MfJointKeyframe@@UBE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfJointKeyframe::WriteToStream(MfStream &s) const
{
    MfKeyframe::WriteToStream(s);
    s.WriteBytes(sizeof(MfJointKeyframe) - 0xc, &mFlags);
    return s.Status();
}
