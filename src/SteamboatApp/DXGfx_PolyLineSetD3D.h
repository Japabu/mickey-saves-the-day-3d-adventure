#pragma once
#include "Gfx_PolyLineSet.h"
#include "Graph_NodeTraverser.h"
#include "StdBase.h"
#include "Vec.h"
#include "Vec4.h"
#include "d3d7.h"
namespace Gfx { class RenderTraverser; }
namespace Graph { class BasicRefreshTraverser; }
namespace Graph { class Stream; }

#ifndef D3DFVF_VERTEX
#define D3DFVF_VERTEX (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1)
#define D3DFVF_LVERTEX (D3DFVF_XYZ | D3DFVF_RESERVED1 | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1)
#endif

namespace DXGfx {
// The Direct3D poly line set: one line strip per primitive, drawn from
// unlit vertices (with normals) when the shape has no colours, else from
// lit vertices coloured by the current material (no PDB type: layout from
// the constructor and the vertex-building functions).
class PolyLineSetD3D : public Gfx::PolyLineSet {
public:
    virtual StdBase::TypeInfo const * GetTypeInfo() const;
    virtual char const * GetTypeName() const;
    virtual ~PolyLineSetD3D();
protected:
    virtual void Refresh(Graph::BasicRefreshTraverser &);
public:
    virtual Graph::NodeTraverser::Response Render(Gfx::RenderTraverser *);
    virtual void Read(Graph::Stream *);
    virtual void ReadVis(Graph::Stream *);
    virtual void SetDrawFunction();
    virtual void SetDirty();
    PolyLineSetD3D();
protected:
    bool RenderDrawPrimitive(Gfx::RenderTraverser *);
    void AppendVertices(int);
    void BuildVertexData();
    void ChopVertices(int);
    void Clean();
    void CopyLitVertex(int, Vec4<float> const &);
    void CopyUnlitVertex(int);
    void CopyVertices();
    void Rebuild();
    void SetupStrips();
    static int s_TotalTypeCount;
    static int s_CurrentTypeCount;
    static StdBase::TypeInfo const s_TypeInfo;
    int m_Unused404;                                    // +0x404 never written
    bool m_VerticesChanged;                             // +0x408 copy the vertices again before drawing
    bool m_Dirty;                                       // +0x409 rebuild the vertex data before drawing
    bool m_IgnoreMaterial;                              // +0x40a lit vertices take the colours unmodulated
    int m_nVertices;                                    // +0x40c
    int m_nLineSegments;                                // +0x410
    int m_nStrips;                                      // +0x414
    int m_Unused418;                                    // +0x418
    Vec<int> m_StripLengths;                            // +0x41c vertices per strip
    Vec<D3DVERTEX> m_UnlitVertices;                     // +0x434
    Vec<D3DLVERTEX> m_LitVertices;                      // +0x44c
};
}
