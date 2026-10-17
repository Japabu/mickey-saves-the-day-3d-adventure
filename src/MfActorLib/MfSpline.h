#pragma once
// MfSpline: a path of transforms through timed control points ('Spli'),
// interpolated as a cubic Bezier between neighbouring points; Integrate
// measures it so it can also be sampled by distance.
#include "MfActorLib_api.h"
#include "MfStreamObject.h"
#include "MfDblList.h"
#include "MfTransform3.h"
class MfStream;
class MfVector3;

#define MF_SPLINE_SAMPLES 10

// size 0x88, written to streams as it is
struct MfSplineControlPoint {
    MfSplineControlPoint() {}
    MfSplineControlPoint(unsigned long t, const MfTransform3 &x)
    {
        time = t;
        xform = ctrl[0] = ctrl[1] = x;
    }
    unsigned long time;                     // +0x00
    MfTransform3 xform;                     // +0x04
    MfTransform3 ctrl[2];                   // +0x20 Bezier points before and after
    float length;                           // +0x58 of the segment to the next point
    float distance[MF_SPLINE_SAMPLES];      // +0x5c along the segment at its samples
    int flags;                              // +0x84
};

// Axis of the control point transforms that InterpolateAtTime turns into the
// direction of travel; MF_SPLINE_NO_AXIS keeps the interpolated rotation.
enum { MF_SPLINE_X, MF_SPLINE_NEG_X, MF_SPLINE_Y, MF_SPLINE_NEG_Y, MF_SPLINE_Z,
       MF_SPLINE_NEG_Z, MF_SPLINE_NO_AXIS };

class MFACTOR_API MfSpline : public MfStreamObject {
public:
    MfSpline();
    MfSpline(const MfSpline &);
    ~MfSpline();
    MfSpline &operator=(const MfSpline &);
    virtual long StreamType() const;
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;
    void SetLinear(const MfTransform3 &from, const MfTransform3 &to, unsigned long duration);
    MfSplineControlPoint *InsertControlPoint(unsigned long time);
    unsigned long LengthInTime();
    int InterpolateAtTime(unsigned long time, MfTransform3 &result);
    int Integrate();
    int InterpolateAtDistance(float distance, MfTransform3 &result);
    unsigned long ClosestApproach(const MfVector3 &point, float radius);
    void Transform(const MfTransform3 &);
    void Scale(const MfVector3 &);
    void ScaleDuration(float);
    static const long STREAM_TYPE;

    MfDblList mControlPoints;               // +0x04
    float mLength;                          // +0x14
    int mAxis;                              // +0x18
};
