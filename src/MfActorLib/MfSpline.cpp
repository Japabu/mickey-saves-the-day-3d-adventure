// MfActorLib.dll 0x1001a7b0-0x1001baf0: MfSpline.
#include <math.h>
#include "MfSpline.h"
#include "MfVector3.h"
#include "MfRotation3.h"
#include "MfStream.h"
#include "MfStreamConstructorEntry.h"

// 0x1001a7b0 ??0MfSpline@@QAE@XZ
MfSpline::MfSpline()
    : mLength(0), mAxis(MF_SPLINE_NO_AXIS)
{
}

// 0x1001a7e0 ??0MfSpline@@QAE@ABV0@@Z
MfSpline::MfSpline(const MfSpline &spline)
    : mLength(spline.mLength), mAxis(spline.mAxis)
{
    MfDblListItem *pli;
    for (MfSplineControlPoint *cp = (MfSplineControlPoint *)spline.mControlPoints.GetFirst(pli); cp;
         cp = (MfSplineControlPoint *)spline.mControlPoints.GetNext(pli))
        mControlPoints.Add(new MfSplineControlPoint(*cp));
}

// 0x1001a960 ??1MfSpline@@QAE@XZ
MfSpline::~MfSpline()
{
    MfDblListItem *pli;
    for (MfSplineControlPoint *cp = (MfSplineControlPoint *)mControlPoints.GetFirst(pli); cp;
         cp = (MfSplineControlPoint *)mControlPoints.GetNext(pli))
        delete cp;
}

// 0x1001aa00 ??4MfSpline@@QAEAAV0@ABV0@@Z
MfSpline &MfSpline::operator=(const MfSpline &spline)
{
    MfDblListItem *pli;
    MfSplineControlPoint *cp;
    for (cp = (MfSplineControlPoint *)mControlPoints.GetFirst(pli); cp;
         cp = (MfSplineControlPoint *)mControlPoints.GetNext(pli))
        delete cp;
    mControlPoints.Clear();
    mLength = spline.mLength;
    for (cp = (MfSplineControlPoint *)spline.mControlPoints.GetFirst(pli); cp;
         cp = (MfSplineControlPoint *)spline.mControlPoints.GetNext(pli))
        mControlPoints.Add(new MfSplineControlPoint(*cp));
    return *this;
}

// 0x1001aba0 ?SetLinear@MfSpline@@QAEXABVMfTransform3@@0K@Z
// A straight path from from to to in duration.
void MfSpline::SetLinear(const MfTransform3 &from, const MfTransform3 &to, unsigned long duration)
{
    MfDblListItem *pli;
    MfSplineControlPoint *cp;
    for (cp = (MfSplineControlPoint *)mControlPoints.GetFirst(pli); cp;
         cp = (MfSplineControlPoint *)mControlPoints.GetNext(pli))
        delete cp;
    mControlPoints.Clear();

    cp = new MfSplineControlPoint(0, from);
    cp->ctrl[1].interpolate(from, to, 1.0f / 3.0f);
    mControlPoints.Add(cp);

    cp = new MfSplineControlPoint(duration, to);
    cp->ctrl[0].interpolate(from, to, 2.0f / 3.0f);
    mControlPoints.Add(cp);
}

// EQUIV 0x1001ae00 ?InsertControlPoint@MfSpline@@QAEPAUMfSplineControlPoint@@K@Z: reviewed: MfTransform3() and one MfTransform3::operator= expanded in one, called in the other; stack slots
// Splits the segment around time into two without changing the path.
MfSplineControlPoint *MfSpline::InsertControlPoint(unsigned long time)
{
    if (mControlPoints.GetCount() < 2)
        return 0;
    MfDblListItem *pli;
    MfSplineControlPoint *last = (MfSplineControlPoint *)mControlPoints.GetLast(pli);
    if (time <= 0 || time >= last->time)
        return 0;
    MfSplineControlPoint *a = (MfSplineControlPoint *)mControlPoints.GetFirst(pli);
    MfSplineControlPoint *b;
    for (b = (MfSplineControlPoint *)mControlPoints.GetNext(pli); b && time > b->time;
         b = (MfSplineControlPoint *)mControlPoints.GetNext(pli))
        a = b;
    if (time == b->time)
        return b;

    // de Casteljau at t
    MfTransform3 p0, p1, p2;
    float t = (float)(time - a->time) / (float)(b->time - a->time);
    p0.interpolate(a->xform, a->ctrl[1], b->ctrl[0], b->xform, t);
    MfSplineControlPoint *cp = new MfSplineControlPoint(time, p0);
    p0.interpolate(a->xform, a->ctrl[1], t);
    p1.interpolate(a->ctrl[1], b->ctrl[0], t);
    p2.interpolate(b->ctrl[0], b->xform, t);
    a->ctrl[1] = p0;
    cp->ctrl[0].interpolate(p0, p1, t);
    cp->ctrl[1].interpolate(p1, p2, t);
    b->ctrl[0] = p2;
    mControlPoints.Insert(cp, MfDblList::LP_BEFORE, pli);
    return cp;
}

// 0x1001b150 ?LengthInTime@MfSpline@@QAEKXZ
unsigned long MfSpline::LengthInTime()
{
    MfDblListItem *pli;
    MfSplineControlPoint *last = (MfSplineControlPoint *)mControlPoints.GetLast(pli);
    return last ? last->time : 0;
}

// 0x1001b170 ?InterpolateAtTime@MfSpline@@QAEHKAAVMfTransform3@@@Z
int MfSpline::InterpolateAtTime(unsigned long time, MfTransform3 &result)
{
    MfDblListItem *pli;
    MfSplineControlPoint *a = (MfSplineControlPoint *)mControlPoints.GetFirst(pli);
    MfSplineControlPoint *b;
    for (b = (MfSplineControlPoint *)mControlPoints.GetNext(pli); b && b->time < time;
         b = (MfSplineControlPoint *)mControlPoints.GetNext(pli))
        a = b;
    if (!b)
        return 0;
    float t = (float)(time - a->time) / (float)(b->time - a->time);
    result.interpolate(a->xform, a->ctrl[1], b->ctrl[0], b->xform, t);
    if (mAxis != MF_SPLINE_NO_AXIS) {
        MfVector3 axis;
        if (mAxis == MF_SPLINE_Z)
            axis.z() = 1.0f;
        else if (mAxis == MF_SPLINE_NEG_Z)
            axis.z() = -1.0f;
        else if (mAxis == MF_SPLINE_Y)
            axis.y() = 1.0f;
        else if (mAxis == MF_SPLINE_NEG_Y)
            axis.y() = -1.0f;
        else if (mAxis == MF_SPLINE_X)
            axis.x() = 1.0f;
        else if (mAxis == MF_SPLINE_NEG_X)
            axis.x() = -1.0f;
        MfVector3 direction;
        direction.interpolate(a->xform.translation(), a->ctrl[1].translation(),
                              b->ctrl[0].translation(), b->xform.translation(), t);
        axis = result.rotation() * axis;
        MfRotation3 turn(axis, direction);
        result.rotation() = turn * result.rotation();
    }
    return 1;
}

// 0x1001b360 ?Integrate@MfSpline@@QAEHXZ
// Measures each segment at MF_SPLINE_SAMPLES points.
int MfSpline::Integrate()
{
    if (mControlPoints.GetCount() < 2)
        return 0;
    mLength = 0;
    MfDblListItem *pli;
    MfSplineControlPoint *a = (MfSplineControlPoint *)mControlPoints.GetFirst(pli);
    for (MfSplineControlPoint *b = (MfSplineControlPoint *)mControlPoints.GetNext(pli); b;
         b = (MfSplineControlPoint *)mControlPoints.GetNext(pli)) {
        a->length = 0;
        MfVector3 p;
        MfVector3 prev = a->xform.translation();
        for (int i = 0; i < MF_SPLINE_SAMPLES; i++) {
            p.interpolate(a->xform.translation(), a->ctrl[1].translation(),
                          b->ctrl[0].translation(), b->xform.translation(),
                          (i + 1) / 11.0f);
            a->distance[i] = a->length += (p - prev).length();
            prev = p;
        }
        mLength += a->length += (b->xform.translation() - p).length();
        a = b;
    }
    return 1;
}

// 0x1001b520 ?InterpolateAtDistance@MfSpline@@QAEHMAAVMfTransform3@@@Z
int MfSpline::InterpolateAtDistance(float distance, MfTransform3 &result)
{
    MfDblListItem *pli;
    MfSplineControlPoint *a = (MfSplineControlPoint *)mControlPoints.GetFirst(pli);
    MfSplineControlPoint *b;
    for (b = (MfSplineControlPoint *)mControlPoints.GetNext(pli); b && distance > a->length;
         b = (MfSplineControlPoint *)mControlPoints.GetNext(pli)) {
        distance -= a->length;
        a = b;
    }
    if (!b)
        return 0;
    int i;
    for (i = 0; i < MF_SPLINE_SAMPLES; i++)
        if (!(a->distance[i] < distance))
            break;
    float from = i == 0 ? 0.0f : a->distance[i - 1];
    float to = i == MF_SPLINE_SAMPLES ? a->length : a->distance[i];
    result.interpolate(a->xform, a->ctrl[1], b->ctrl[0], b->xform,
                       (distance - from) / (to - from) + i / 11.0f);
    return 1;
}

// 0x1001b610 ?ClosestApproach@MfSpline@@QAEKABVMfVector3@@M@Z
// The time at which the path passes closest to radius from point (checked at
// 100 times).
unsigned long MfSpline::ClosestApproach(const MfVector3 &point, float radius)
{
    float step = LengthInTime() / 99.0f;
    MfTransform3 xform;
    InterpolateAtTime(0, xform);
    unsigned long best = 0;
    float bestDist = (float)fabs((xform.translation() - point).length() - radius);
    for (int i = 1; i < 100; i++) {
        unsigned long time = (unsigned long)(i * step);
        InterpolateAtTime(time, xform);
        float dist = (float)fabs((xform.translation() - point).length() - radius);
        if (dist < bestDist) {
            bestDist = dist;
            best = time;
        }
    }
    return best;
}

// 0x1001b740 ?Transform@MfSpline@@QAEXABVMfTransform3@@@Z
void MfSpline::Transform(const MfTransform3 &t)
{
    MfDblListItem *pli;
    for (MfSplineControlPoint *cp = (MfSplineControlPoint *)mControlPoints.GetFirst(pli); cp;
         cp = (MfSplineControlPoint *)mControlPoints.GetNext(pli)) {
        cp->xform = t * cp->xform;
        cp->ctrl[0] = t * cp->ctrl[0];
        cp->ctrl[1] = t * cp->ctrl[1];
    }
}

// 0x1001b830 ?Scale@MfSpline@@QAEXABVMfVector3@@@Z
void MfSpline::Scale(const MfVector3 &scale)
{
    MfDblListItem *pli;
    for (MfSplineControlPoint *cp = (MfSplineControlPoint *)mControlPoints.GetFirst(pli); cp;
         cp = (MfSplineControlPoint *)mControlPoints.GetNext(pli)) {
        cp->xform.translation() *= scale;
        cp->ctrl[0].translation() *= scale;
        cp->ctrl[1].translation() *= scale;
    }
}

// 0x1001b890 ?ScaleDuration@MfSpline@@QAEXM@Z
void MfSpline::ScaleDuration(float scale)
{
    MfDblListItem *pli;
    for (MfSplineControlPoint *cp = (MfSplineControlPoint *)mControlPoints.GetFirst(pli); cp;
         cp = (MfSplineControlPoint *)mControlPoints.GetNext(pli))
        cp->time = (unsigned long)(cp->time * scale);
}

const long MfSpline::STREAM_TYPE = 'Spli';

// 0x1001b8f0 ?CreateSpline@@YAPAVMfStreamObject@@XZ (stream constructor for 'Spli')
static MfStreamObject *CreateSpline()
{
    return new MfSpline;
}

// 0x1001b950 _$E7 (jumps to _$E6)
// 0x1001b960 _$E6
static MfStreamConstructorEntry s_entry(MfSpline::STREAM_TYPE, CreateSpline);

// 0x1001b980 ?StreamType@MfSpline@@UBEJXZ
long MfSpline::StreamType() const
{
    return STREAM_TYPE;
}

// 0x1001b990 ?ReadFromStream@MfSpline@@UAE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfSpline::ReadFromStream(MfStream &s)
{
    int n;
    s >> n;
    for (int i = 0; i < n; i++) {
        MfSplineControlPoint *cp = new MfSplineControlPoint;
        s.ReadBytes(sizeof(MfSplineControlPoint), cp);
        mControlPoints.Add(cp);
    }
    s >> mLength;
    return s.Status();
}

// 0x1001ba90 ?WriteToStream@MfSpline@@UBE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfSpline::WriteToStream(MfStream &s) const
{
    s << mControlPoints.GetCount();
    MfDblListItem *pli;
    for (MfSplineControlPoint *cp = (MfSplineControlPoint *)mControlPoints.GetFirst(pli); cp;
         cp = (MfSplineControlPoint *)mControlPoints.GetNext(pli))
        s.WriteBytes(sizeof(MfSplineControlPoint), cp);
    s << mLength;
    return s.Status();
}
