#pragma once
#include "Gfx_PackedTriStripSet.h"
#include "Graph_NodeTraverser.h"
#include "StdBase.h"
#include "Vec.h"
#include "d3d7.h"
namespace Gfx { class RenderTraverser; }
namespace Graph { class BasicRefreshTraverser; }
namespace Graph { class Stream; }

namespace DXGfx {
// Direct3D drawing of a PackedTriStripSet: one strided vertex description per
// strip (pointers into the shape's coordinate, normal, colour and texture lists).
class PackedTriStripSetD3D : public Gfx::PackedTriStripSet {
public:
    virtual StdBase::TypeInfo const * GetTypeInfo() const;
    virtual char const * GetTypeName() const;
    virtual ~PackedTriStripSetD3D();
protected:
    virtual void Refresh(Graph::BasicRefreshTraverser &);
public:
    virtual Graph::NodeTraverser::Response Render(Gfx::RenderTraverser *);
    virtual void Read(Graph::Stream *);
    virtual void ReadVis(Graph::Stream *);
    virtual void SetDrawFunction();
    PackedTriStripSetD3D();
protected:
    bool RenderDrawPrimitive(Gfx::RenderTraverser *);
    void BuildVertexData();
    void Clean();
    void SetupStrips();
    static int s_TotalTypeCount;
    static int s_CurrentTypeCount;
    static StdBase::TypeInfo const s_TypeInfo;
public:
    char unknown_420[4];
    bool m_Textured;                                    // +0x424 texturing state the vertex data was built for
    int m_nTriangles;                                   // +0x428
    DWORD m_FVF;                                        // +0x42c D3DFVF_* flags of the strided data
    DWORD m_DrawFlags;                                  // +0x430 D3DDP_* flags of the draw calls
    Vec<D3DDRAWPRIMITIVESTRIDEDDATA> m_StridedData;     // +0x434 one per strip
};
}
