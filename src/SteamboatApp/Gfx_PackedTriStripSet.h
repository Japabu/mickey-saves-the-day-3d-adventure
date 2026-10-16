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
// Triangle strips packed into one vertex list; m_Prim holds the strip lengths.
class PackedTriStripSet : public Gfx::PackedVertexShape {
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
    static int GetCurrentTypeCount();
    static int GetTotalTypeCount();
    int GetTypeHandle() const { return m_TypeHandle; }
    PackedTriStripSet(const Gfx::PackedTriStripSet &);
    PackedTriStripSet();
    virtual int GetAtom(int, Vec3<float> **);
    virtual int GetNumberOfAtoms() const;
    virtual int GetNumberOfAtomBoundingBoxes() const;
    virtual Gfx::BoundingBox GetAtomBoundingBox(int);
    short GetCullCW() { return m_CullCW; }
    void SetEnvironmentMapping(bool mapping) { m_EnvironmentMapping = mapping; }
    bool GetEnvironmentMapping() { return m_EnvironmentMapping; }
    virtual Graph::NodeTraverser::Response Render(Gfx::RenderTraverser *);
    virtual void Write(Gfx::WriteTraverser *);
    virtual void ReadVis2(Graph::Stream *);
    virtual void ReadVis(Graph::Stream *);
    virtual void Read(Graph::Stream *);
    virtual int GetNumVerts() const { return m_nVerts; }
    virtual int GetNumTris() const { return m_nTris; }
    virtual int GetNumStrips() const { return m_nStrips; }
    virtual int GetNumIndices() const { return m_nIndices; }
protected:
    virtual ~PackedTriStripSet();
    void WriteInternals(Gfx::WriteTraverser *);
    void ReadInternals(Graph::Stream *, int);
    virtual void Refresh(Graph::BasicRefreshTraverser &);
    virtual void SetDrawFunction();
    short m_CullCW;
    int m_nVerts;
    int m_nTris;
    int m_nStrips;
    int m_nIndices;
    bool m_EnvironmentMapping;
private:
    static int const Version;
    void (*DrawFunction)();
    Array<int> m_TriIndexStart;
    Array<Gfx::BoundingBox> m_AtomBBoxes;
};
}
