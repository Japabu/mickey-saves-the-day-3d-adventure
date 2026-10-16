#pragma once
#include <memory>
#include <stack>
#include <vector>
#include "Gfx_Attribute.h"
#include "StdBase.h"
#include "Vec4.h"
namespace Gfx { class TextureAttribute; }
namespace Gfx { class WriteTraverser; }
namespace Graph { class PassiveTraverser; }
namespace Graph { class Stream; }

namespace DXGfx { class IndexedTriangleSetD3D; class PackedTriStripSetD3D; class PolyLineSetD3D; }

namespace Gfx {
// Surface material: the colours and power of a Direct3D material plus the
// shading, culling and depth-buffer settings of the nodes below it (no PDB
// declaration; member names chosen from the use).
class MaterialAttribute : public Gfx::Attribute {
public:
    virtual StdBase::TypeInfo const * GetTypeInfo() const;
    virtual char const * GetTypeName() const;
protected:
    virtual ~MaterialAttribute();
public:
    virtual void Write(Gfx::WriteTraverser *);
    virtual void ReadVis(Graph::Stream *);
    virtual void Read(Graph::Stream *);
    virtual void Push(Graph::PassiveTraverser *) const;
    virtual void Pop(Graph::PassiveTraverser *) const;
    virtual Gfx::MaterialAttribute & operator=(Gfx::MaterialAttribute const &);
    virtual void SetMaterial();
    virtual void Apply(Graph::PassiveTraverser *) const = 0;
    MaterialAttribute();
    bool operator==(Gfx::MaterialAttribute const &) const;
    float GetPower() const { return m_Power; }
protected:
    void ReadInternals(Graph::Stream *, int);
    void ReadInternals(Graph::Stream *, int, Gfx::TextureAttribute *);
    void WriteInternals(Gfx::WriteTraverser *);
    static StdBase::TypeInfo const s_TypeInfo;
    static int s_TotalTypeCount;
    static int s_CurrentTypeCount;
    int m_TypeHandle;
private:
    // the Direct3D shapes read the current material (VC6 did not check access here)
    friend class DXGfx::IndexedTriangleSetD3D;
    friend class DXGfx::PackedTriStripSetD3D;
    friend class DXGfx::PolyLineSetD3D;
    static std::stack<Gfx::MaterialAttribute const *, std::vector<Gfx::MaterialAttribute const *, std::allocator<Gfx::MaterialAttribute const *> > > s_Stack;
    static int const Version;
public:
    Vec4<float> m_Ambient;
    Vec4<float> m_Diffuse;
    Vec4<float> m_Specular;
    Vec4<float> m_Emissive;
    float m_Power;
    int m_ShadeMode;
    short m_BackfaceCulling;
    bool field_c2;
    bool m_ZBuffer;
    bool m_Textured;
    bool field_c5;
};
}
