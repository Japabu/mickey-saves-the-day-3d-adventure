#pragma once
#include "Graph_BasicAttribute.h"
#include "StdBase.h"
namespace Gfx { class WriteTraverser; }
namespace Graph { class PassiveTraverser; }
namespace Graph { class Stream; }

namespace Gfx {
// Base of the graphics attributes (material, texture, shadow ...): pushed
// onto the traverser's attribute stacks while the owning node is visited.
class Attribute : public Graph::BasicAttribute {
protected:
    void IncrementTypeCount();
    void DecrementTypeCount();
    static StdBase::TypeInfo const s_TypeInfo;
    static int s_CurrentTypeCount;
    static int s_TotalTypeCount;
    int m_TypeHandle;
public:
    virtual StdBase::TypeInfo const * GetTypeInfo() const;
    virtual char const * GetTypeName() const;
    static StdBase::TypeInfo const * GetStaticTypeInfo() { return &s_TypeInfo; }
    static int GetCurrentTypeCount();
    static int GetTotalTypeCount();
    int GetTypeHandle() const;
    short IsIgnored() { return m_Ignore; }
    void SetIgnore(short ignore) { m_Ignore = ignore; }
    virtual void Write(Gfx::WriteTraverser *) = 0;
    virtual void ReadVis(Graph::Stream *);
    virtual void Read(Graph::Stream *);
    virtual void Push(Graph::PassiveTraverser *) const = 0;
    virtual void Pop(Graph::PassiveTraverser *) const = 0;
protected:
    Attribute(const Gfx::Attribute &);
    Attribute();
    virtual ~Attribute();
    int WriteHeaderInfo(Gfx::WriteTraverser *, unsigned char *);
    void WriteInternals(Gfx::WriteTraverser *);
    void ReadInternals(Graph::Stream *, int);
    short m_Ignore;
private:
    static int const Version;
};
}
