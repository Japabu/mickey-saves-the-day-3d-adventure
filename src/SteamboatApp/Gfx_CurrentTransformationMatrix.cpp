// evidence: Pop keeps the erase result and _Last in registers (no-alias code, as in the other
// Graphics.lib units)
#pragma optimize("a", on)
#include "Gfx_CurrentTransformationMatrix.h"

// 0x100f2b70 ?Create@CurrentTransformationMatrix@Gfx@@SAXXZ
void Gfx::CurrentTransformationMatrix::Create()
{
    if (!s_pInstance)
        s_pInstance = new Gfx::CurrentTransformationMatrix;
}

// 0x100f2be0 ?SetView@CurrentTransformationMatrix@Gfx@@QAEXABV?$Vec3@M@@00@Z
void Gfx::CurrentTransformationMatrix::SetView(const Vec3<float> &eye, const Vec3<float> &at, const Vec3<float> &up)
{
    m_ViewMatrix.SetView(eye, at, up);
    m_ViewChanged = true;
}

// 0x100f2c10 ?SetPerspective@CurrentTransformationMatrix@Gfx@@QAEXMMMM@Z
void Gfx::CurrentTransformationMatrix::SetPerspective(float fov, float aspect, float nearPlane, float farPlane)
{
    ProjectionMatrix::SetPerspective(fov, aspect, nearPlane, farPlane);
    m_ProjectionChanged = true;
}

// 0x100f2c40 ?SetOrthographic@CurrentTransformationMatrix@Gfx@@QAEXMMMMMM@Z
void Gfx::CurrentTransformationMatrix::SetOrthographic(float left, float right, float bottom, float top, float nearPlane,
                                                        float farPlane)
{
    ProjectionMatrix::SetOrthographic(left, right, bottom, top, nearPlane, farPlane);
    m_ProjectionChanged = true;
}

// TODO 0x100f2c80 ?PushLeft@CurrentTransformationMatrix@Gfx@@QAEXABV?$TMatrix4@M@@@Z: product loop identical; the original copies the left factor with an out-of-line setValues() and calls vector::insert out of line
void Gfx::CurrentTransformationMatrix::PushLeft(const TMatrix4<float> &m)
{
    m_ModelMatrixStack.push(m * m_ModelMatrixStack.top());
    m_ModelChanged = true;
}

// TODO 0x100f2d50 ?PushRight@CurrentTransformationMatrix@Gfx@@QAEXABV?$TMatrix4@M@@@Z: as PushLeft
void Gfx::CurrentTransformationMatrix::PushRight(const TMatrix4<float> &m)
{
    m_ModelMatrixStack.push(m_ModelMatrixStack.top() * m);
    m_ModelChanged = true;
}

// 0x100f2e10 ?Pop@CurrentTransformationMatrix@Gfx@@QAEXXZ
void Gfx::CurrentTransformationMatrix::Pop()
{
    m_ModelMatrixStack.pop();
    m_ModelChanged = true;
}

// 0x100f2e80 ??0CurrentTransformationMatrix@Gfx@@IAE@XZ
Gfx::CurrentTransformationMatrix::CurrentTransformationMatrix()
    : m_ProjectionChanged(false), m_ViewChanged(false), m_ModelChanged(false)
{
    TMatrix4<float> identity;
    identity.identity();
    m_ModelMatrixStack.push(identity);
    m_ViewMatrix.identity();
    ProjectionMatrix::identity();
    m_Matrix.identity();
}

// 0x100f3000 ??1CurrentTransformationMatrix@Gfx@@IAE@XZ
Gfx::CurrentTransformationMatrix::~CurrentTransformationMatrix()
{
}

// 0x100f2fd0 ??1?$stack@V?$TMatrix4@M@@V?$vector@V?$TMatrix4@M@@V?$allocator@V?$TMatrix4@M@@@std@@@std@@@std@@QAE@XZ
// TODO 0x100f3080 ?insert@?$vector@V?$TMatrix4@M@@V?$allocator@V?$TMatrix4@M@@@std@@@std@@QAEXPAV?$TMatrix4@M@@IABV3@@Z: bytes identical; the two element copies call SquareMatrix<float, 4>::operator= out of line where the original calls setValues(const float *) (same arguments; the original's operator= is setValues(m) with setValues not inlined, and no unit emits an operator= with setValues' bytes: ours returns *this, 22 bytes against 20) (100% similar)
// 0x100f32e0 ?_Construct@std@@YAXPAV?$TMatrix4@M@@ABV2@@Z

// static data (zero in the original, .bss)
Gfx::CurrentTransformationMatrix *Gfx::CurrentTransformationMatrix::s_pInstance = 0;
