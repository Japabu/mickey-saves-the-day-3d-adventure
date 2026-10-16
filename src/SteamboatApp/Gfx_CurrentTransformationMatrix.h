#pragma once
#include <stack>
#include <vector>
#include "Gfx_ProjectionMatrix.h"
#include "Gfx_ViewMatrix.h"

namespace Gfx { class MatrixTransform; }
namespace DXGfx { class IndexedTriangleSetD3D; class PolyLineSetD3D; class PackedTriStripSetD3D; }

namespace Gfx {
// The projection, the view and a stack of model matrices; the singleton the
// traversals push node transformations onto (no PDB declaration, member
// names chosen from the use).
class CurrentTransformationMatrix : public Gfx::ProjectionMatrix {
public:
    static void Create();
    static void Destroy()
    {
        delete s_pInstance;
        s_pInstance = 0;
    }
    static Gfx::CurrentTransformationMatrix *GetInstance() { return s_pInstance; }
    const Gfx::ViewMatrix &GetViewMatrix() { return m_ViewMatrix; }
    const TMatrix4<float> &GetModelMatrix() { return m_ModelMatrixStack.top(); }
    const TMatrix4<float> &GetMatrix() { return m_Matrix; } // the sphere maps turn the reflection with it (set to identity)
    void Pop();
    void PushLeft(const TMatrix4<float> &);
    void PushRight(const TMatrix4<float> &);
    void SetOrthographic(float, float, float, float, float, float);
    void SetPerspective(float, float, float, float);
    void SetView(const Vec3<float> &, const Vec3<float> &, const Vec3<float> &);
protected:
    friend class Gfx::MatrixTransform;
    friend class DXGfx::IndexedTriangleSetD3D; // Render hands the changed matrices to Direct3D
    friend class DXGfx::PolyLineSetD3D;
    friend class DXGfx::PackedTriStripSetD3D;
    CurrentTransformationMatrix();
    ~CurrentTransformationMatrix();
    static Gfx::CurrentTransformationMatrix *s_pInstance;
private:
    Gfx::ViewMatrix m_ViewMatrix;
    std::stack<TMatrix4<float>, std::vector<TMatrix4<float> > > m_ModelMatrixStack;
    TMatrix4<float> m_Matrix;
    bool m_ProjectionChanged;
    bool m_ViewChanged;
    bool m_ModelChanged;
};
}
