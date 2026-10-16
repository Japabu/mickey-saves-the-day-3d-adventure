#pragma once
#include "Gfx_PackedVertexShape.h"
#include "Graph_NodeTraverser.h"
#include "StdBase.h"
#include "Array.h"
#include "Gfx_BoundingVolume.h"
namespace Gfx { class RenderTraverser; }
namespace Gfx { class WriteTraverser; }
namespace Graph { class BasicRefreshTraverser; }
namespace Graph { class Stream; }

namespace Gfx {
// Lines packed into one vertex list; m_Prim holds the line lengths. Not in
// the PDB: the members follow IndexedTriangleSet/PackedTriStripSet.
class PolyLineSet : public Gfx::PackedVertexShape {
protected:
    void IncrementTypeCount()
    {
        s_CurrentTypeCount++;
        m_TypeHandle = s_TotalTypeCount;
        s_TotalTypeCount++;
    }
    void DecrementTypeCount() { s_CurrentTypeCount--; }
    static StdBase::TypeInfo const s_TypeInfo;
    static int s_CurrentTypeCount;
    static int s_TotalTypeCount;
    int m_TypeHandle;
public:
    virtual StdBase::TypeInfo const * GetTypeInfo() const;
    virtual char const * GetTypeName() const;
    static StdBase::TypeInfo const * GetStaticTypeInfo() { return &s_TypeInfo; }
    PolyLineSet();
    virtual int GetAtom(int, Vec3<float> **);
    virtual int GetNumberOfAtoms() const;
    virtual int GetNumberOfAtomBoundingBoxes() const;
    virtual Gfx::BoundingBox GetAtomBoundingBox(int);
    virtual Graph::NodeTraverser::Response Render(Gfx::RenderTraverser *);
    virtual void Write(Gfx::WriteTraverser *);
    virtual void ReadVis2(Graph::Stream *);
    virtual void ReadVis(Graph::Stream *);
    virtual void Read(Graph::Stream *);
    virtual void SetDirty();
protected:
    virtual ~PolyLineSet();
    void WriteInternals(Gfx::WriteTraverser *);
    void ReadInternals(Graph::Stream *, int);
    virtual void Refresh(Graph::BasicRefreshTraverser &);
    virtual void SetDrawFunction();
private:
    static int const Version;
    Array<int> m_LineIndexStart;
    Array<Gfx::BoundingBox> m_AtomBBoxes;
};
}
