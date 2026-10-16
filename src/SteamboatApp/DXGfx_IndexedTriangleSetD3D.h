#pragma once
#include "Gfx_IndexedTriangleSet.h"
#include "Graph_NodeTraverser.h"
#include "StdBase.h"
#include "d3d7.h"
namespace Gfx { class BoundingBox; }
namespace Gfx { class BoundingSphere; }
namespace Gfx { class PackedTriStripSet; }
namespace Gfx { class RenderTraverser; }
namespace Graph { class BasicRefreshTraverser; }
namespace Graph { class Stream; }

namespace DXGfx {
class IndexedTriangleSetD3D : public Gfx::IndexedTriangleSet {
public:
    virtual StdBase::TypeInfo const * GetTypeInfo() const;
    virtual char const * GetTypeName() const;
    virtual ~IndexedTriangleSetD3D();
protected:
    virtual void Refresh(Graph::BasicRefreshTraverser &);
public:
    virtual Graph::NodeTraverser::Response Render(Gfx::RenderTraverser *);
    virtual void Read(Graph::Stream *);
    virtual void SetDrawFunction();
    IndexedTriangleSetD3D();
    IndexedTriangleSetD3D(Gfx::BoundingBox const &);
    IndexedTriangleSetD3D(Gfx::BoundingSphere const &);
    IndexedTriangleSetD3D(Gfx::PackedTriStripSet &);
protected:
    bool RenderDrawPrimitive(Gfx::RenderTraverser *);
    void BuildVertexData();
    void Clean();
    void CopyVertices();
    static int s_TotalTypeCount;
    static int s_CurrentTypeCount;
    static StdBase::TypeInfo const s_TypeInfo;
    // (no PDB type: layout from the constructors, Clean, BuildVertexData and
    // RenderDrawPrimitive)
    int m_Unused3a8;                                    // +0x3a8 never written
    bool m_Textured;                                    // +0x3ac texturing state the vertex data was built for
    int m_nTriangles;                                   // +0x3b0
    Vec<unsigned short> m_D3DIndices;                   // +0x3b4 16-bit index list for Direct3D
    DWORD m_FVF;                                        // +0x3cc D3DFVF_* flags of the strided data
    DWORD m_DrawFlags;                                  // +0x3d0 D3DDP_* flags of the draw call
    D3DDRAWPRIMITIVESTRIDEDDATA m_StridedData;          // +0x3d4
};
}
