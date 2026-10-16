#include "Gfx_ProjectionMatrix.h"
#include "MathConstants.h"

// 0x1010fd40 ??0ProjectionMatrix@Gfx@@QAE@XZ
Gfx::ProjectionMatrix::ProjectionMatrix()
{
}

// 0x1010fd60 ??1ProjectionMatrix@Gfx@@QAE@XZ
Gfx::ProjectionMatrix::~ProjectionMatrix()
{
}

// TODO 0x1010fd70 ?SetPerspective@ProjectionMatrix@Gfx@@QAEXMMMM@Z: FPU code identical; the original keeps the constant element stores in statement order between the FPU instructions, VC6 here hoists them to the top (57% similar)
void Gfx::ProjectionMatrix::SetPerspective(float fov, float aspect, float zNear, float zFar)
{
    float f = 1.0f / (float)tan(fov * DEG_TO_RAD * 0.5);

    elements[0][0] = f;
    elements[0][1] = 0.0f;
    elements[0][2] = 0.0f;
    elements[0][3] = 0.0f;
    elements[1][0] = 0.0f;
    elements[1][1] = f * aspect;
    elements[1][2] = 0.0f;
    elements[1][3] = 0.0f;
    elements[2][0] = 0.0f;
    elements[2][1] = 0.0f;
    elements[2][2] = -(zFar / (zFar - zNear));
    elements[2][3] = -1.0f;
    elements[3][0] = 0.0f;
    elements[3][1] = 0.0f;
    elements[3][2] = -(zNear * zFar / (zFar - zNear));
    elements[3][3] = 0.0f;
}

// TODO 0x1010fdf0 ?SetOrthographic@ProjectionMatrix@Gfx@@QAEXMMMMMM@Z: as SetPerspective (constant stores hoisted here) (70% similar)
void Gfx::ProjectionMatrix::SetOrthographic(float left, float right, float bottom, float top, float zNear, float zFar)
{
    elements[0][0] = 2.0f / (right - left);
    elements[0][1] = 0.0f;
    elements[0][2] = 0.0f;
    elements[0][3] = -((left + right) / (right - left));
    elements[1][0] = 0.0f;
    elements[1][1] = 2.0f / (top - bottom);
    elements[1][2] = 0.0f;
    elements[1][3] = -((bottom + top) / (top - bottom));
    elements[2][0] = 0.0f;
    elements[2][1] = 0.0f;
    elements[2][3] = -((zNear + zFar) / (zFar - zNear));
    elements[2][2] = 2.0f / (zFar - zNear);
    elements[3][0] = 0.0f;
    elements[3][1] = 0.0f;
    elements[3][2] = 0.0f;
    elements[3][3] = 1.0f;
    transpose();
}

// 0x1010fc50 _$E2
// 0x1010fc60 _$E1
// 0x1010fc80 _$E5
// 0x1010fc90 _$E4
// 0x1010fcb0 _$E8
// 0x1010fcc0 _$E7
// 0x1010fce0 _$E11
// 0x1010fcf0 _$E10
// 0x1010fd10 _$E14
// 0x1010fd20 _$E13
