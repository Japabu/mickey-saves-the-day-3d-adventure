#include "Gfx_ViewMatrix.h"

// 0x1010f940 ??0ViewMatrix@Gfx@@QAE@XZ
Gfx::ViewMatrix::ViewMatrix()
{
}

// TODO 0x1010f970 ?SetView@ViewMatrix@Gfx@@QAEXABV?$Vec3@M@@00@Z: inline budget: the original inlines the Vec3 operator-=/operator- and translate bodies and calls only mag2, multScal, the SquareMatrix constructor, composeByCols and multRight; VC6 here calls five more of the inline helpers out of line; normalize divides with fld 1.0 / fdiv st(1) in the original (as View::CalcViewFrustumPlanes) (84% similar)
void Gfx::ViewMatrix::SetView(const Vec3<float> &from, const Vec3<float> &at, const Vec3<float> &up)
{
    Vec3<float> dir = from - at;
    float distance = -dir.mag();
    dir.normalize();
    Vec3<float> side = up.cross(dir);
    side.normalize();
    Vec3<float> newUp = dir.cross(side);
    newUp.normalize();

    identity();
    translate(-at);

    TMatrix4<float> rotation;
    rotation.identity();
    rotation.composeByCols(side, newUp, dir, Vec3<float>(0.0f, 0.0f, 0.0f));
    multRight(rotation);

    TMatrix4<float> translation;
    translation.identity();
    translation.translate(Vec3<float>(0.0f, 0.0f, distance));
    multRight(translation);
}

// 0x1010f960 ??1ViewMatrix@Gfx@@QAE@XZ
Gfx::ViewMatrix::~ViewMatrix()
{
}
