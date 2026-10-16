#pragma once
#include <vector>
#include "Gfx_Attribute.h"
#include "StdBase.h"
namespace Gfx { class Shape; }
namespace Gfx { class WriteTraverser; }
namespace Graph { class PassiveTraverser; }

namespace Gfx {
// Shapes sorted by their render state, so each state is set once per frame
// (no PDB declaration, member names chosen from the use).
class RenderList : public Gfx::Attribute {
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
    virtual ~RenderList();
    virtual void Write(Gfx::WriteTraverser *);
    virtual void Push(Graph::PassiveTraverser *) const;
    virtual void Pop(Graph::PassiveTraverser *) const;
    RenderList();
    static StdBase::TypeInfo const * GetStaticTypeInfo() { return &s_TypeInfo; }
    void AddShape(Gfx::Shape *);
    void Clear();
protected:
    static int s_TotalTypeCount;
    static int s_CurrentTypeCount;
    static StdBase::TypeInfo const s_TypeInfo;
    int m_TypeHandle;
private:
    std::vector<std::vector<Gfx::Shape *> > m_ShapeLists;
};
}
