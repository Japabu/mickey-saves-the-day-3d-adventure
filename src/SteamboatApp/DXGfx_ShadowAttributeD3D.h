#pragma once
#include "Gfx_ShadowAttribute.h"
#include "StdBase.h"
#include "Vec4.h"
#include "TMatrix4.h"
namespace Graph { class PassiveTraverser; }

namespace DXGfx {
// Shadow attribute of the Direct3D renderer: pushes the projection onto the
// shadow plane and draws with a translucent black material (no PDB type
// beyond the virtuals; member names chosen from the use).
class ShadowAttributeD3D : public Gfx::ShadowAttribute {
public:
    virtual StdBase::TypeInfo const * GetTypeInfo() const;
    virtual char const * GetTypeName() const;
protected:
    virtual ~ShadowAttributeD3D();
public:
    virtual void Push(Graph::PassiveTraverser *) const;
    virtual void Pop(Graph::PassiveTraverser *) const;
    ShadowAttributeD3D();
protected:
    static int s_TotalTypeCount;
    static int s_CurrentTypeCount;
    static StdBase::TypeInfo const s_TypeInfo;
public:
    int m_Unused88;
    // a D3DMATERIAL7 (diffuse, ambient, specular, emissive, power)
    Vec4<float> m_Diffuse;
    Vec4<float> m_Ambient;
    Vec4<float> m_Specular;
    Vec4<float> m_Emissive;
    float m_Power;
    TMatrix4<float> m_Matrix;
};
}
