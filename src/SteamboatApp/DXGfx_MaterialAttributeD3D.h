#pragma once
#include "Gfx_MaterialAttribute.h"
#include "StdBase.h"
#include "d3d7.h"
namespace Graph { class PassiveTraverser; }
namespace Graph { class Stream; }

namespace DXGfx {
// Material attribute of the Direct3D renderer (no PDB type beyond the
// virtuals; member names chosen from the use).
class MaterialAttributeD3D : public Gfx::MaterialAttribute {
public:
    virtual StdBase::TypeInfo const * GetTypeInfo() const;
    virtual char const * GetTypeName() const;
protected:
    virtual ~MaterialAttributeD3D();
public:
    virtual void ReadVis(Graph::Stream *);
    virtual void Read(Graph::Stream *);
    virtual void Push(Graph::PassiveTraverser *) const;
    virtual void Pop(Graph::PassiveTraverser *) const;
    virtual void SetMaterial();
    virtual void Apply(Graph::PassiveTraverser *) const;
    MaterialAttributeD3D();
protected:
    static int s_TotalTypeCount;
    static int s_CurrentTypeCount;
    static StdBase::TypeInfo const s_TypeInfo;
public:
    int m_UnusedC8;
    // the Direct3D copy of the colours and the power (SetMaterial)
    D3DMATERIAL7 m_Material;
    char m_Unknown110[32]; // not used by the code found so far; NodeFactory allocates 0x130 bytes
};
}
