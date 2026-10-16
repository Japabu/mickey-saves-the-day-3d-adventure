#include <float.h>
#include "MfRect.h"
#include "MfVector2.h"
#include "MfVector3.h"
#include "MfTriangleSet.h"

// 0x10019800 ??0MfRect@@QAE@XZ
MfRect::MfRect()
{
}

// 0x10019810 ?maxx@MfRect@@QAEAAMXZ
float & MfRect::maxx()
{
    return mMaxx;
}

// 0x10019820 ??4MfRect@@QAEAAV0@ABV0@@Z (compiler-generated)

// 0x10019850 ??0MfRect@@QAE@MMMM@Z
MfRect::MfRect(float minx, float maxx, float miny, float maxy)
{
    setValue(minx, maxx, miny, maxy);
}

// 0x10019880 ?setValue@MfRect@@QAEXMMMM@Z
void MfRect::setValue(float minx, float maxx, float miny, float maxy)
{
    mMinx = minx;
    mMaxx = maxx;
    mMiny = miny;
    mMaxy = maxy;
}

// 0x100198a0 ?area@MfRect@@QBEMXZ
float MfRect::area() const
{
    return (mMaxx - mMinx) * (mMaxy - mMiny);
}

// 0x100198b0 ?contains@MfRect@@QBEHABVMfVector2@@@Z
int MfRect::contains(MfVector2 const &p) const
{
    if (p.x() >= mMinx && p.x() <= mMaxx && p.y() >= mMiny && p.y() <= mMaxy)
        return 1;
    return 0;
}

// 0x10019900 ?projectTriSet@MfRect@@QAEXPAVMfTriangleSet@@@Z
// The rectangle the triangle set covers seen from above (z across, x up).
void MfRect::projectTriSet(MfTriangleSet *set)
{
    mMinx = FLT_MAX;
    mMiny = FLT_MAX;
    mMaxx = -FLT_MAX;
    mMaxy = -FLT_MAX;
    int n = set->GetNumVertices();
    MfVector3 v;
    for (int i = 0; i < n; i++) {
        v = set->Vertex(i);
        if (v.z() < mMinx)
            mMinx = v.z();
        if (v.x() < mMiny)
            mMiny = v.x();
        if (v.z() > mMaxx)
            mMaxx = v.z();
        if (v.x() > mMaxy)
            mMaxy = v.x();
    }
}
