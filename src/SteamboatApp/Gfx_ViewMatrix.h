#pragma once
#include "TMatrix4.h"

namespace Gfx {
// Viewing transformation from an eye point, a point looked at and an up vector.
class ViewMatrix : public TMatrix4<float> {
public:
    ViewMatrix();
    void SetView(const Vec3<float> &, const Vec3<float> &, const Vec3<float> &);
    ~ViewMatrix();
};
}
