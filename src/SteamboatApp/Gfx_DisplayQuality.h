#pragma once
#include "Gfx_Attribute.h"
#include "StdBase.h"
namespace Gfx { class AttributeStack; }
namespace Gfx { class WriteTraverser; }
namespace Graph { class PassiveTraverser; }
namespace Graph { class Stream; }

namespace Gfx {
// Rendering quality of the nodes below it (m_Quality 1: only blended
// textures, 2: no textures); the instances share one attribute stack (no PDB
// declaration; member names chosen from the use).
class DisplayQuality : public Gfx::Attribute {
public:
    virtual StdBase::TypeInfo const * GetTypeInfo() const;
    virtual char const * GetTypeName() const;
    virtual ~DisplayQuality();
    virtual void Write(Gfx::WriteTraverser *);
    virtual void ReadVis(Graph::Stream *);
    virtual void Read(Graph::Stream *);
    virtual void Push(Graph::PassiveTraverser *) const;
    virtual void Pop(Graph::PassiveTraverser *) const;
    DisplayQuality();
    static Gfx::AttributeStack *GetStack() { return s_Stack; }
    int GetQuality() const { return m_Quality; }
protected:
    void ReadInternals(Graph::Stream *);
    void WriteInternals(Gfx::WriteTraverser *);
    static int s_TotalTypeCount;
    static int s_CurrentTypeCount;
    static StdBase::TypeInfo const s_TypeInfo;
private:
    static int s_InstanceCount;
    static Gfx::AttributeStack * s_Stack;
    int m_TypeHandle;
    int m_Quality;
};
}
