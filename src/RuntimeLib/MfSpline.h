// MfSpline (MfActorLib.dll): a spline curve in time. Only what RuntimeLib
// calls is declared.
#ifndef RUNTIMELIB_MFSPLINE_H
#define RUNTIMELIB_MFSPLINE_H
class MfTransform3;
class MfVector3;

class __declspec(dllimport) MfSpline {
public:
    unsigned long LengthInTime();
    int InterpolateAtTime(unsigned long time, MfTransform3 &xform);
    // the time of the point of the curve closest to point
    unsigned long ClosestApproach(const MfVector3 &point, float);
};

#endif
