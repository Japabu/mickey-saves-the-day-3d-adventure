#pragma once
#include <memory>
#include <stack>
#include <vector>
#include "Gfx_Attribute.h"
#include "StdBase.h"
namespace Gfx { class WriteTraverser; }
namespace Graph { class PassiveTraverser; }
namespace Gfx { class MaterialAttribute; }
namespace Gfx { class TextureAttribute; }

namespace Gfx {
// A captured set of texture and material attributes (no PDB declaration;
// member names chosen from the use).
class RenderState : public Gfx::Attribute {
protected:
    void IncrementTypeCount()
    {
        s_CurrentTypeCount++;
        m_TypeHandle = s_TotalTypeCount;
        s_TotalTypeCount++;
    }
    void DecrementTypeCount() { s_CurrentTypeCount--; }
public:
    virtual StdBase::TypeInfo const * GetTypeInfo() const;
    virtual char const * GetTypeName() const;
    virtual ~RenderState();
    virtual void Write(Gfx::WriteTraverser *);
    virtual void Push(Graph::PassiveTraverser *) const;
    virtual void Pop(Graph::PassiveTraverser *) const;
    RenderState();
    static StdBase::TypeInfo const * GetStaticTypeInfo() { return &s_TypeInfo; }
    static int GetTotalTypeCount() { return s_TotalTypeCount; }
    int GetTypeHandle() const { return m_TypeHandle; }
    bool operator==(Gfx::RenderState const &) const;
protected:
    static int s_TotalTypeCount;
    static int s_CurrentTypeCount;
    static StdBase::TypeInfo const s_TypeInfo;
private:
    static std::stack<Gfx::RenderState const *,std::vector<Gfx::RenderState const *,std::allocator<Gfx::RenderState const *> > > s_Stack;
    int m_TypeHandle;
public:
    std::vector<const Gfx::TextureAttribute *> m_TextureAttributes;
    const Gfx::MaterialAttribute *m_MaterialAttribute;
};
}
