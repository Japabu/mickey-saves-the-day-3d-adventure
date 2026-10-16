#pragma once
#include "Gfx_Attribute.h"
#include "StdBase.h"
#include "Gfx_Plane3f.h"
namespace Gfx { class AttributeStack; }
namespace Gfx { class WriteTraverser; }
namespace Graph { class PassiveTraverser; }
namespace Graph { class Stream; }

namespace Gfx {
// Projects the shapes below it onto a plane as their shadow; the instances
// share one attribute stack (no PDB declaration; member names chosen from
// the use).
class ShadowAttribute : public Gfx::Attribute {
public:
    virtual StdBase::TypeInfo const * GetTypeInfo() const;
    virtual char const * GetTypeName() const;
protected:
    virtual ~ShadowAttribute();
public:
    virtual void Write(Gfx::WriteTraverser *);
    virtual void Read(Graph::Stream *);
    virtual void Push(Graph::PassiveTraverser *) const;
    virtual void Pop(Graph::PassiveTraverser *) const;
    virtual bool operator==(Gfx::ShadowAttribute const &shadow) const
    {
        // the inlined Plane3f comparison (no out-of-line Plane3f::operator== in the binary)
        const Gfx::Plane3f &a = m_Plane;
        const Gfx::Plane3f &b = shadow.m_Plane;
        return a.GetNormal() == b.GetNormal() && a.GetOffset() == b.GetOffset();
    }
    virtual bool operator!=(Gfx::ShadowAttribute const &shadow) { return !(*this == shadow); }
    ShadowAttribute();
protected:
    void ReadInternals(Graph::Stream *, int);
    void WriteInternals(Gfx::WriteTraverser *);
    static StdBase::TypeInfo const s_TypeInfo;
    static int s_TotalTypeCount;
    static int s_CurrentTypeCount;
private:
    static Gfx::AttributeStack * s_Stack;
    static int s_InstanceCount;
    static int const Version;
    int m_TypeHandle;
public:
    Gfx::Plane3f m_Plane;
};
}
