// MfPath3: a 3D path for Mf3dPathActivity (RuntimeLib.dll 1004a890-1004c230).
#include "MfPath3.h"
#include "MfSkeletonNode.h"
#include "MfSpline.h"
#include "MfSplineNode.h"

// 0x1004a890 ??0MfPath3@@QAE@XZ
MfPath3::MfPath3()
{
    mNumSamples = 0;
    mKeys = 0;
    mSpline = 0;
    mKind = KEYS;
}

// 0x1004aa40 ??1MfPath3@@QAE@XZ
MfPath3::~MfPath3()
{
    if (mKeys)
        delete [] mKeys;
}

// 0x1004aaa0 ?Allocate@MfPath3@@QAEXH@Z
void MfPath3::Allocate(int numKeys)
{
    if (mKeys)
        delete [] mKeys;
    mNumSamples = numKeys;
    mKeys = numKeys ? new MfTransform3[numKeys] : 0;
}

// TODO 0x1004ab50 ?Transform@MfPath3@@QAE?AVMfTransform3@@M@Z: the zero register (edi, ours
// ebx) and the inline budget: the original copies the last key vectors inline where ours calls the
// imported MfVector3 copy constructor; the key wrap i - i / 36 * 36 divides by 36 and
// subtracts in the original, ours divides by -36 and adds (same value; i %= ARC_KEYS gives
// an idiv, i = i - i / ARC_KEYS * ARC_KEYS breaks the budget further) (84% similar)
MfTransform3 MfPath3::Transform(float frame)
{
    MfTransform3 result;
    if (mKind == ARC) {
        if (frame <= 0.0f)
            return mKeys[0];
        if (mNumArcKeys - 1 <= frame)
            return mKeys[mNumSamples - 1];
        int i = (int)frame;
        frame -= i;
        if (i >= ARC_KEYS)
            i -= i / ARC_KEYS * ARC_KEYS;
        int j = i == ARC_KEYS - 1 ? 0 : i + 1;
        result.interpolate(mKeys[i], mKeys[j], frame);
        return result;
    }
    if (mKind == MOVE) {
        MfTransform3 xform;
        if (frame <= 0.0f) {
            xform.translation() = mFrame.translation();
            xform.rotation() = mFromRotation;
            return xform;
        }
        float time;
        if (frame == mNumSamples - 1)
            time = mDurationSeconds;
        else
            time = frame * 0.033f;
        MfVector3 position;
        position = mFrame * (mVelocity * time + mGravity * time * time * 0.5f + mVertical * time);
        xform.translation() = position;
        xform.rotation().interpolate(mFromRotation, mToRotation, frame / (mNumSamples - 1), 0);
        return xform;
    }
    if (mKind == SPLINE_KEYS) {
        MfTransform3 xform;
        if (frame <= 0.0f)
            return mKey0;
        if (frame >= mNumSamples - 1)
            return mKey3;
        xform.interpolate(mKey0, mKey1, mKey2, mKey3, frame / (mNumSamples - 1));
        return xform;
    }
    if (mKind == SPLINE) {
        MfTransform3 xform;
        if (frame <= 0.0f)
            frame = 0.0f;
        if (frame >= mNumSamples - 1)
            frame = mNumSamples - 1;
        int time = (int)(frame / (mNumSamples - 1) * (mCurveEnd - mCurveStart) + 0.5f);
        if (mReverse)
            time = mCurveEnd - time;
        else
            time += mCurveStart;
        mCurve->InterpolateAtTime(time, xform);
        if (mReverse) {
            MfRotation3 turn(3.1415927f, MfVector3(0.0f, 1.0f, 0.0f));
            xform.rotation() = turn * xform.rotation();
        }
        xform = mSpline->globalTransform() * xform;
        return xform;
    }
    if (frame <= 0.0f)
        return mKeys[0];
    if (mNumSamples - 1 <= frame)
        return mKeys[mNumSamples - 1];
    int i = (int)frame;
    result.interpolate(mKeys[i], mKeys[i + 1], frame - i);
    return result;
}

// 0x1004b340 ?Key@MfPath3@@QAEAAVMfTransform3@@H@Z
MfTransform3 &MfPath3::Key(int i)
{
    if (mKind == ARC) {
        if (i < ARC_KEYS)
            return mKeys[i];
        return mKeys[i - i / ARC_KEYS * ARC_KEYS];
    }
    return mKeys[i];
}

// 0x1004b3b0 ?SetArc@MfPath3@@QAEXABVMfTransform3@@HMM@Z
// Keys every 10 degrees around a center radius away (flags 8, 0x10: turning
// about the up axis, else about the side axis; 0x20 and 8 put the center on
// the other side).
void MfPath3::SetArc(const MfTransform3 &start, int flags, float radius, float angle)
{
    MfVector3 offset;
    MfVector3 center;
    MfVector3 point;
    MfVector3 axis;
    int negative = 0;
    if (angle < 0.0f) {
        angle = -angle;
        negative = 1;
    }
    if (radius < 0.0f)
        radius = -radius;
    mKind = ARC;
    mAngle = angle;
    mRadius = radius;
    mFlags = flags;
    mStart = start;
    int numKeys;
    if (angle > 6.2831855f)
        numKeys = ARC_KEYS;
    else
        numKeys = (int)(angle * 5.7295775f);
    mNumArcKeys = (int)(angle * 5.7295775f);
    if (numKeys < 1)
        numKeys = 1;
    if (mNumArcKeys < 1)
        mNumArcKeys = 1;
    Allocate(numKeys);
    MfRotation3 turn;
    if ((flags & 8) || (flags & 0x10)) {
        if (flags & 8)
            radius = -radius;
        offset = start.rotation() * MfVector3(-radius, 0.0f, 0.0f);
        center = start.translation() + offset;
        axis = start.rotation() * MfVector3(0.0f, 1.0f, 0.0f);
        for (int i = 0; i < numKeys; i++) {
            float a = i * 0.17453294f;
            if (negative)
                a = -a;
            if (radius < 0.0f)
                turn = MfRotation3(a, axis);
            else
                turn = MfRotation3(-a, axis);
            point = center - turn * offset;
            Key(i).rotation() = turn * start.rotation();
            Key(i).translation() = point;
        }
    } else {
        if (flags & 0x20)
            radius = -radius;
        offset = start.rotation() * MfVector3(0.0f, -radius, 0.0f);
        center = start.translation() + offset;
        axis = start.rotation() * MfVector3(1.0f, 0.0f, 0.0f);
        for (int i = 0; i < numKeys; i++) {
            float a = i * 0.17453294f;
            if (negative)
                a = -a;
            if (radius < 0.0f)
                turn = MfRotation3(-a, axis);
            else
                turn = MfRotation3(a, axis);
            point = center - turn * offset;
            Key(i).rotation() = turn * start.rotation();
            Key(i).translation() = point;
        }
    }
}

// 0x1004ba30 ?SetLine@MfPath3@@QAEXABVMfTransform3@@ABVMfVector3@@1H@Z
void MfPath3::SetLine(const MfTransform3 &start, const MfVector3 &goal, const MfVector3 &direction, int flags)
{
    mKind = MOVE;
    mFlags = flags;
    mStart = start;
    mGoal = goal;
    mDirection = direction;
    mToEnd = 0;
    mNumSamples = 0x7fffffff;
    MfVector3 v;
    mFrame.rotation().setValue(MfVector3(0.0f, -1.0f, 0.0f), goal.unit());
    mFrame.translation() = start.translation();
    v = ~mFrame.rotation() * direction;
    mVelocity.mX = v.mX;
    mVelocity.mY = 0.0f;
    mVelocity.mZ = v.mZ;
    mGravity = ~mFrame.rotation() * goal;
    mVertical.mX = 0.0f;
    mVertical.mY = v.mY;
    mVertical.mZ = 0.0f;
    mFromRotation = start.rotation();
    mToRotation = start.rotation();
}

// TODO 0x1004bc10 ?SetMove@MfPath3@@QAEXABVMfTransform3@@ABVMfVector3@@0MH@Z: the original calls the
// imported MfVector3::lengthSquared for the normalizations, which this MfVector3.h expands (69% similar)
void MfPath3::SetMove(const MfTransform3 &from, const MfVector3 &velocity, const MfTransform3 &to, float duration,
                      int flags)
{
    mKind = MOVE;
    mFlags = flags;
    mStart = from;
    mEnd = to;
    mDurationSeconds = duration;
    mGoal = velocity;
    mToEnd = 1;
    mFlags = flags;
    if (flags & 0x80)
        mNumSamples = 0x7fffffff;
    else
        mNumSamples = 1 - (int)(duration * -30.30303f);
    MfVector3 delta;
    MfVector3 direction = velocity;
    if (direction.length() > 0.0f) {
        float length = (float)sqrt(direction.lengthSquared());
        direction.mX /= length;
        direction.mY /= length;
        direction.mZ /= length;
        mFrame.rotation().setValue(MfVector3(0.0f, -1.0f, 0.0f), direction);
    }
    mFrame.translation() = from.translation();
    delta = ~mFrame * to.translation();
    mVelocity.mX = delta.mX;
    mVelocity.mY = 0.0f;
    mVelocity.mZ = delta.mZ;
    float speed = mVelocity.length() / duration;
    if (speed > 0.0f) {
        float length = (float)sqrt(mVelocity.lengthSquared());
        mVelocity.mX /= length;
        mVelocity.mY /= length;
        mVelocity.mZ /= length;
        mVelocity = mVelocity * speed;
    }
    mGravity = ~mFrame.rotation() * velocity;
    float gravity = mGravity.length();
    mVertical.mX = 0.0f;
    mVertical.mZ = 0.0f;
    mVertical.mY = (delta.mY - gravity * duration * duration * -0.5f) / duration;
    mFromRotation = from.rotation();
    mToRotation = to.rotation();
}

// 0x1004bf20 ?SetSpline@MfPath3@@QAEXPAVMfSplineNode@@HVMfVector3@@H@Z
void MfPath3::SetSpline(MfSplineNode *spline, int reverse, MfVector3 start, int duration)
{
    mKind = SPLINE;
    mDuration = duration;
    mSpline = spline;
    MfProtoNode *proto = spline->mProto;
    mCurve = proto->mStyle == MfProtoNode::SPLINE ? (MfSpline *)proto->mInfo : 0;
    mReverse = reverse;
    mSplineStart = start;
    start = ~spline->globalTransform() * mSplineStart;
    mCurveStart = mCurve->ClosestApproach(start, 0);
    if (duration < 0) {
        mLooped = 1;
        mNumSamples = 300;
        mCurveEnd = mCurve->LengthInTime();
        if (mReverse) {
            mCurveEnd = mCurveStart;
            mCurveStart = 0;
        }
        if (mCurveEnd == 0)
            mNumSamples = 1;
        else
            mNumSamples = (mCurveEnd - mCurveStart) * mNumSamples / mCurveEnd;
    } else {
        mLooped = 0;
        mNumSamples = duration / 33;
        mCurveEnd = mCurve->LengthInTime();
        if (mReverse) {
            mCurveEnd = mCurveStart;
            mCurveStart = 0;
        }
        if (mCurveEnd == 0)
            mNumSamples = 1;
        else
            mNumSamples = (mCurveEnd - mCurveStart) * mNumSamples / mCurveEnd;
    }
}

// 0x1004c080 ?SetKeys@MfPath3@@QAEXABVMfTransform3@@000H@Z
void MfPath3::SetKeys(const MfTransform3 &key0, const MfTransform3 &key1, const MfTransform3 &key2,
                      const MfTransform3 &key3, int duration)
{
    mKind = SPLINE_KEYS;
    mDuration = duration;
    if (duration < 0) {
        mNumSamples = 120;
        mLooped = 1;
    } else {
        mLooped = 0;
        mNumSamples = duration / 33;
    }
    mKey0 = key0;
    mKey1 = key1;
    mKey2 = key2;
    mKey3 = key3;
}

// 0x1004c1d0 ?SetKeys@MfPath3@@QAEXPAVMfTransform3@@HH@Z
void MfPath3::SetKeys(MfTransform3 *keys, int count, int duration)
{
    mKind = KEYS;
    mDuration = duration;
    if (duration < 0.0f)
        mLooped = 1;
    else
        mLooped = 0;
    mTotalTime = duration;
    mNumSamples = count;
    mKeys = keys;
}
