#pragma once
// What floor and spline nodes hold (MfProtoNode::FloorInfo(), SplineInfo()).
#include "MfSpline.h"

// What a floor node holds.
struct MfProtoFloorInfo {
    MfProtoFloorInfo() { m00 = 0; }
    int m00;    // nonzero: the floor takes no manipulation targets
    int m04;
};

// What a spline node holds.
struct MfProtoSplineInfo {
    MfSpline spline;
};
