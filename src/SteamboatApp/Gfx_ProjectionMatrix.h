#pragma once
#include "TMatrix4.h"

namespace Gfx {
// Perspective or orthographic projection.
class ProjectionMatrix : public TMatrix4<float> {
public:
    ProjectionMatrix();
    void SetOrthographic(float, float, float, float, float, float);
    void SetPerspective(float, float, float, float);
    ~ProjectionMatrix();
};
}
