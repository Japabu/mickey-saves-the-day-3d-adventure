#pragma once
#include "Gfx_IndexedVertexShape.h"
#include "Graph_NodeTraverser.h"
#include "StdBase.h"
#include "Array.h"
#include "Gfx_BoundingVolume.h"
namespace Gfx { class PackedTriStripSet; }
namespace Gfx { class RenderTraverser; }
namespace Gfx { class WriteTraverser; }
namespace Graph { class BasicRefreshTraverser; }
namespace Graph { class Stream; }

namespace Gfx {
// Indexed shape of triangles: m_Prim holds where each triangle's indices
// start; the per-triangle bounding boxes are rebuilt by Refresh.
class IndexedTriangleSet : public Gfx::IndexedVertexShape {
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
    IndexedTriangleSet(const Gfx::IndexedTriangleSet &);
    IndexedTriangleSet(const Gfx::BoundingSphere &);
    IndexedTriangleSet(const Gfx::BoundingBox &);
    IndexedTriangleSet(Gfx::PackedTriStripSet &);
    IndexedTriangleSet();
    virtual int GetAtom(int, Vec3<float> **);
    virtual int GetNumberOfAtoms() const;
    virtual int GetNumberOfAtomBoundingBoxes() const;
    virtual Gfx::BoundingBox GetAtomBoundingBox(int);
    short GetCullCW() { return m_CullCW; }
    void SetEnvironmentMapping(bool mapping) { m_EnvironmentMapping = mapping; }
    bool GetEnvironmentMapping() { return m_EnvironmentMapping; }
    virtual Graph::NodeTraverser::Response Render(Gfx::RenderTraverser *);
    virtual void Write(Gfx::WriteTraverser *);
    virtual void Read(Graph::Stream *);
protected:
    virtual ~IndexedTriangleSet();
    void WriteInternals(Gfx::WriteTraverser *);
    void ReadInternals(Graph::Stream *, int);
    virtual void Refresh(Graph::BasicRefreshTraverser &);
    virtual void SetDrawFunction();
    void ConvertPackedTriStripSetToIndexedTriangleSet(Gfx::PackedTriStripSet *);
    short m_CullCW;
    int m_nTris;
    bool m_EnvironmentMapping;
private:
    static int const Version;
    void (*DrawFunction)();
    Array<int> m_TriIndexStart;
    Array<Gfx::BoundingBox> m_AtomBBoxes;
};
}
