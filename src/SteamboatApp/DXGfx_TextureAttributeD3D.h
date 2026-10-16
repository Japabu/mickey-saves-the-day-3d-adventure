#pragma once
#include "Gfx_TextureAttribute.h"
#include "StdBase.h"
#include "DX_TextureMapManager.h"
#include "d3d7.h"
namespace Gfx { class RefreshTraverser; }
namespace Graph { class PassiveTraverser; }
namespace Graph { class Stream; }

namespace DXGfx {
class TextureAttributeD3D : public Gfx::TextureAttribute {
public:
    virtual StdBase::TypeInfo const * GetTypeInfo() const;
    virtual char const * GetTypeName() const;
protected:
    virtual ~TextureAttributeD3D();
public:
    virtual void ReadVis(Graph::Stream *);
    virtual void Read(Graph::Stream *);
    virtual void Push(Graph::PassiveTraverser *) const;
    virtual void Pop(Graph::PassiveTraverser *) const;
    virtual void SetTexture();
    virtual void SetTextureStageCompositeMode(Gfx::TextureAttribute::TextureStageCompositeMode);
    virtual void Apply(Graph::PassiveTraverser *) const;
protected:
    virtual unsigned char * GetTextureCache(Gfx::RefreshTraverser &);
public:
    TextureAttributeD3D();
protected:
    static int s_TotalTypeCount;
    static int s_CurrentTypeCount;
    static StdBase::TypeInfo const s_TypeInfo;
    // (no PDB type: layout from the constructor and SetTexture)
    int m_Unknownb4;                                    // +0xb4 never set by the constructor
    DWORD m_ColorOp;                                    // +0xb8 texture stage state values
    DWORD m_ColorArg1;                                  // +0xbc
    DWORD m_ColorArg2;                                  // +0xc0
    DWORD m_AlphaOp;                                    // +0xc4
    DWORD m_AlphaArg1;                                  // +0xc8
    DWORD m_AlphaArg2;                                  // +0xcc
    DX::DirectDrawSurfaceAttrib m_SurfaceAttrib;        // +0xd0 what the texture map manager loads
    int m_Unknownf8;                                    // +0xf8
    int m_Unknownfc;                                    // +0xfc 2 after construction
    int m_Unknown100;                                   // +0x100
};
}
