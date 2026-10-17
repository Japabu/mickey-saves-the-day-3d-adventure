// Gfx::BVolHierCenterRef: an atom's centre and index, sorted along an axis while
// a bounding-volume hierarchy is built (Gfx_BVolHier.h).
#ifndef BVOLHIERCENTERREF_H
#define BVOLHIERCENTERREF_H
#include "Vec3.h"

namespace Gfx {
struct BVolHierCenterRef {
    Vec3<float> vCenter;
    int atomIndex;
};
}

#endif
