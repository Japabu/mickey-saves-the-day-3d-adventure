// MfPath2: a 2D path of transforms for a locomotion, the MfAiPath2 the
// application passes (RuntimeLib.dll 10049bf0-1004a890).
#include "MfPath2.h"
#include <string.h>
#include <windows.h>
#include "MfSkeletonNode.h"
#include "MfTransform3.h"

// 0x10049bf0 ??0MfPath2@@QAE@XZ
MfPath2::MfPath2()
{
    mCount = 0;
    mPoints = 0;
    mRefCount = 1;
    mParent = 0;
    mOpen = 2;
}

// 0x10049c20 ?GetNumConfigs@MfPath2@@UAEHXZ
int MfPath2::GetNumConfigs()
{
    if (!mOpen)
        return m28;
    return mCount;
}

// 0x10049c30 ??0MfPath2@@QAE@ABV0@@Z
MfPath2::MfPath2(const MfPath2 &path)
{
    mRefCount = 1;
    mStart = path.mStart;
    mFlags = path.mFlags;
    mRadius = path.mRadius;
    mAngle = path.mAngle;
    mPoints = 0;
    SetNumPoints(path.mCount);
    for (int i = 0; i < mCount; i++)
        mPoints[i] = path.mPoints[i];
    mArc = path.mArc;
    m28 = path.m28;
    mOpen = path.mOpen;
    mParent = path.mParent;
}

// 0x10049d00 ??1MfPath2@@QAE@XZ
MfPath2::~MfPath2()
{
    if (mPoints)
        delete [] mPoints;
}

// 0x10049d60 ?SetNumPoints@MfPath2@@QAEXH@Z
void MfPath2::SetNumPoints(int count)
{
    if (mPoints)
        delete [] mPoints;
    mCount = count;
    mPoints = count ? new MfTransform2[count] : 0;
}

// 0x10049e10 ?PointAt@MfPath2@@QAE?AVMfTransform2@@M@Z
MfTransform2 MfPath2::PointAt(float pos)
{
    MfTransform2 xf;
    if (!mOpen) {
        if (pos <= 0)
            return mPoints[0];
        if (m28 - 1 <= pos)
            return mPoints[mCount - 1];
        int i = (int)pos;
        pos -= i;
        int k = i; if (i >= RING_SIZE) k = i - i / RING_SIZE * RING_SIZE;
        int j = 0;
        if (k != RING_SIZE - 1)
            j = k + 1;
        xf.interpolate(mPoints[k], mPoints[j], pos);
        return xf;
    }
    if (pos <= 0)
        return mPoints[0];
    if (mCount - 1 <= pos)
        return mPoints[mCount - 1];
    int i = (int)pos;
    xf.interpolate(mPoints[i], mPoints[i + 1], pos - i);
    return xf;
}

// 0x1004a070 ?Update@MfPath2@@QAEXXZ
void MfPath2::Update()
{
    MfVector2 next, prev, dir;
    if (mPoints && mCount >= 2) {
        dir = mPoints[1].translation() - mPoints[0].translation();
        mPoints[0].rotation() = dir.angle();
        for (int i = 1; i < mCount - 1; i++) {
            next = mPoints[i + 1].translation() - mPoints[i].translation();
            prev = mPoints[i].translation() - mPoints[i - 1].translation();
            dir = next + prev;
            if (dir.x() == 0 && dir.y() == 0)
                mPoints[i].rotation() = mPoints[i - 1].rotation();
            else
                mPoints[i].rotation() = dir.angle();
        }
        dir = mPoints[mCount - 1].translation() - mPoints[mCount - 2].translation();
        if (dir.x() == 0 && dir.y() == 0)
            mPoints[mCount - 1].rotation() = mPoints[mCount - 2].rotation();
        else
            mPoints[mCount - 1].rotation() = dir.angle();
    }
}

// 0x1004a280 ?SetArc@MfPath2@@QAEXABVMfTransform2@@HMM@Z
void MfPath2::SetArc(const MfTransform2 &start, int flags, float radius, float angle)
{
    MfVector2 offset, center, point;
    mStart = start;
    mFlags = flags;
    mRadius = radius;
    mAngle = angle;
    if (angle < 0)
        angle = -angle;
    if (radius < 0)
        radius = -radius;
    if (flags & 8)
        radius = -radius;
    float step = 0.17453294f;
    mOpen = 0;
    mArc = angle;
    int count;
    if (angle > 6.2831855f)
        count = RING_SIZE;
    else
        count = (int)(angle * 5.7295775f);
    m28 = (int)(angle * 5.7295775f);
    if (count < 2) {
        step = angle;
        count = 2;
        m28 = 2;
    }
    SetNumPoints(count);
    offset = start.rotation() * MfVector2(0, -radius);
    center = start.translation() + offset;
    for (int i = 0; i < count; i++) {
        float a = i * step;
        if (flags & 2)
            a = -a;
        if (radius < 0)
            point = center - MfRotation2(a) * offset;
        else
            point = center - MfRotation2(-a) * offset;
        Point(i).translation() = point;
    }
    Update();
}

// 0x1004a540 ?GetInterfaceName@MfPath2@@UAEXPADHAAH1@Z
void MfPath2::GetInterfaceName(char *name, int size, int &major, int &minor)
{
    strncpy(name, "MfAiPath2", size - 1);
    name[size - 1] = 0;
    major = 1;
    minor = 0;
}

// 0x1004a580 ?AddRef@MfPath2@@UAEHXZ
int MfPath2::AddRef()
{
    return InterlockedIncrement(&mRefCount);
}

// 0x1004a590 ?Release@MfPath2@@UAEHXZ
int MfPath2::Release()
{
    long count = InterlockedDecrement(&mRefCount);
    if (!count)
        delete this;
    return count;
}

// 0x1004a5c0 ?GetParentNode@MfPath2@@UAEPAVMfAiActorNode@@XZ
MfAiActorNode *MfPath2::GetParentNode()
{
    return mParent;
}

// 0x1004a5d0 ?GetTransform@MfPath2@@UAE?AVMfTransform3@@M@Z
MfTransform3 MfPath2::GetTransform(float index)
{
    MfTransform2 point = PointAt(index);
    MfTransform3 xform;
    xform.translation() = MfVector3(point.y(), 0, point.x());
    xform.rotation().setValue(point.rotation().angle(), MfVector3(0, 1, 0));
    if (mParent)
        xform = mParent->globalTransform() * xform;
    return xform;
}

// 0x1004a710 ?SetTransform@MfPath2@@UAEXHABVMfTransform3@@@Z
void MfPath2::SetTransform(int index, const MfTransform3 &xform)
{
    MfTransform3 local(xform);
    if (mParent)
        local = ~mParent->globalTransform() * local;
    MfTransform2 point;
    float angle;
    MfVector3 axis;
    point.translation().x() = local.translation().z();
    point.translation().y() = local.translation().x();
    local.rotation().getValue(angle, axis);
    if (axis.y() < 0)
        angle = -angle;
    point.rotation() = angle;
    Point(index) = point;
}
