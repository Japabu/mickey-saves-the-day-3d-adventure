#include "DXGfx_TextureAttributeD3D.h"
#include "Gfx_TextureAttribute.h"
#include "Graph_PassiveTraverser.h"
#include "StdBase.h"
#include "MBNew.h"
#include "DXGfx_ViewD3D.h"
#include "Gfx_RenderTraverser.h"
#include "DX_Device.h"
#include "DX_Direct3D.h"
#include "DX_Exception.h"
#include "Gfx_TextureException.h"
#include "Gfx_RefreshTraverser.h"
#include "DX_TextureMapManager.h"
#include "DX_DirectDrawSurface.h"
#include "StdString.h"
#include <string.h>
#include <locale> // evidence: the original unit ends with the std::ctype<unsigned short>::id and std::collate<char>::id initializers

// 0x100d9bd0 ?GetTypeInfo@TextureAttributeD3D@DXGfx@@UBEPBUTypeInfo@StdBase@@XZ
StdBase::TypeInfo const * DXGfx::TextureAttributeD3D::GetTypeInfo() const
{
    return &s_TypeInfo;
}

// 0x100d9be0 ?GetTypeName@TextureAttributeD3D@DXGfx@@UBEPBDXZ
char const * DXGfx::TextureAttributeD3D::GetTypeName() const
{
    return s_TypeInfo.typeName;
}

// TODO 0x100d9bf0 ??0TextureAttributeD3D@DXGfx@@QAE@XZ: inline budget: the original calls the string's _Tidy and ssasn out of line, VC6 here inlines both (same stores otherwise)
DXGfx::TextureAttributeD3D::TextureAttributeD3D()
    : m_ColorOp(D3DTOP_MODULATE), m_ColorArg1(D3DTA_TEXTURE), m_ColorArg2(D3DTA_DIFFUSE),
      m_AlphaOp(D3DTOP_DISABLE), m_AlphaArg1(D3DTA_TEXTURE), m_AlphaArg2(D3DTA_DIFFUSE), m_Unknownfc(2)
{
    m_SurfaceAttrib.fileName = GetFilename();
    m_SurfaceAttrib.isMipMap = m_MipMap;
    m_SurfaceAttrib.hasColorKey = m_HasColorKey;
    m_SurfaceAttrib.isAlpha = m_Alpha;
    m_SurfaceAttrib.render = 1;
    if (m_HasColorKey || m_Alpha) {
        m_AlphaOp = D3DTOP_MODULATE;
        m_AlphaArg1 = D3DTA_TEXTURE;
        m_AlphaArg2 = D3DTA_DIFFUSE;
    }
}

// 0x100d9d00 ??_GTextureAttributeD3D@DXGfx@@MAEPAXI@Z

// 0x100d9d30 ??1DirectDrawSurfaceAttrib@DX@@QAE@XZ

// 0x100d9d80 ??1TextureAttributeD3D@DXGfx@@MAE@XZ
DXGfx::TextureAttributeD3D::~TextureAttributeD3D()
{
}

// 0x100d9de0 ?SetTexture@TextureAttributeD3D@DXGfx@@UAEXXZ
void DXGfx::TextureAttributeD3D::SetTexture()
{
    m_SurfaceAttrib.fileName = GetFilename();
    m_SurfaceAttrib.hasColorKey = m_HasColorKey;
    m_SurfaceAttrib.isMipMap = m_MipMap;
    m_SurfaceAttrib.isAlpha = m_Alpha;
    m_SurfaceAttrib.render = 1;
    SetTextureStageCompositeMode(m_TextureStageCompositeMode);
}

// TODO 0x100d9e40 ?Apply@TextureAttributeD3D@DXGfx@@UBEXPAVPassiveTraverser@Graph@@@Z: 68% - the statements and calls are recovered; the original calls operator[] of the device list out of line and lays the !WantTexturing exit out after the try block
void DXGfx::TextureAttributeD3D::Apply(Graph::PassiveTraverser *traverser) const
{
    Gfx::RenderTraverser *renderTraverser = StdBaseCast(traverser, (Gfx::RenderTraverser *)0);
    if (!renderTraverser || !GetEnable())
        return;
    DX::Device *device = ((DXGfx::ViewD3D *)renderTraverser->GetView())->GetDevice();
    int stage = device->GetDirect3D()->DriverInfo().bCanDoMultiTexture ? m_TextureStage : 0;
    TextureAttributeD3D *self = (TextureAttributeD3D *)this;
    if (!WantTexturing()) {
        self->m_SurfaceAttrib.render = 0;
        device->SetTexture(self->m_SurfaceAttrib, m_TextureStage);
        return;
    }
    device->SetTextureStageState(stage + 1, D3DTSS_COLOROP, D3DTOP_DISABLE);
    device->SetTextureStageState(stage + 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
    device->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, m_Alpha);
    self->m_SurfaceAttrib.render = 1;
    try {
        device->SetRenderState(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATE);
        device->SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, TRUE);
        device->SetTextureStageState(stage, D3DTSS_ADDRESSU, m_WrapU ? D3DTADDRESS_WRAP : D3DTADDRESS_CLAMP);
        device->SetTextureStageState(stage, D3DTSS_ADDRESSV, m_WrapV ? D3DTADDRESS_WRAP : D3DTADDRESS_CLAMP);
        device->SetTexture(self->m_SurfaceAttrib, stage);
        if (m_BlendMode != 1)
            device->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
        device->SetRenderState(D3DRENDERSTATE_DESTBLEND, m_BlendMode == 1 ? D3DBLEND_ONE : D3DBLEND_INVSRCALPHA);
        device->SetTextureStageState(stage, D3DTSS_TEXCOORDINDEX, stage);
        device->SetTextureStageState(stage, D3DTSS_COLOROP, m_ColorOp);
        device->SetTextureStageState(stage, D3DTSS_ALPHAOP, m_AlphaOp);
        if (m_TextureStage == stage) {
            device->SetTextureStageState(stage, D3DTSS_COLORARG1, m_ColorArg1);
            device->SetTextureStageState(stage, D3DTSS_COLORARG2, m_ColorArg2);
            device->SetTextureStageState(stage, D3DTSS_ALPHAARG1, m_AlphaArg1);
            device->SetTextureStageState(stage, D3DTSS_ALPHAARG2, m_AlphaArg2);
        } else {
            device->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, TRUE);
            device->SetRenderState(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_COPY);
            switch (m_ColorOp) {
            case D3DTOP_MODULATE:
                device->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ZERO);
                device->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_SRCCOLOR);
                device->SetTextureStageState(stage, D3DTSS_COLORARG1, D3DTA_TEXTURE);
                device->SetTextureStageState(stage, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
                device->SetTextureStageState(stage, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
                device->SetTextureStageState(stage, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
                break;
            case D3DTOP_ADD:
                device->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ONE);
                device->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE);
                device->SetTextureStageState(stage, D3DTSS_COLOROP, D3DTOP_MODULATE);
                device->SetTextureStageState(stage, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
                device->SetTextureStageState(stage, D3DTSS_COLORARG1, D3DTA_TEXTURE);
                device->SetTextureStageState(stage, D3DTSS_COLORARG2, D3DTA_CURRENT);
                device->SetTextureStageState(stage, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
                device->SetTextureStageState(stage, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
                break;
            }
        }
        if (m_Alpha) {
            device->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE, TRUE);
            device->SetRenderState(D3DRENDERSTATE_ALPHAFUNC, D3DCMP_NOTEQUAL);
        } else {
            device->SetRenderState(D3DRENDERSTATE_COLORKEYENABLE, m_HasColorKey);
            device->SetRenderState(D3DRENDERSTATE_ALPHAFUNC, D3DCMP_ALWAYS);
            device->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE, m_HasColorKey ? TRUE : FALSE);
        }
        if (m_MipMap && !device->GetDirect3D()->IsEmulation())
            device->SetTextureStageState(stage, D3DTSS_MIPFILTER, m_Filter == 2 ? D3DTFP_LINEAR : D3DTFP_POINT);
        else
            device->SetTextureStageState(stage, D3DTSS_MIPFILTER, D3DTFP_NONE);
        device->SetTextureStageState(stage, D3DTSS_MINFILTER, D3DTFN_LINEAR);
        device->SetTextureStageState(stage, D3DTSS_MAGFILTER, D3DTFG_LINEAR);
    } catch (DX::Exception &e) {
        throw Gfx::TextureException(*this, e.GetResult());
    }
}

// 0x100da210 ?Push@TextureAttributeD3D@DXGfx@@UBEXPAVPassiveTraverser@Graph@@@Z
void DXGfx::TextureAttributeD3D::Push(Graph::PassiveTraverser *traverser) const
{
    Apply(traverser);
    TextureAttribute::Push(traverser);
}

// 0x100da230 ?Pop@TextureAttributeD3D@DXGfx@@UBEXPAVPassiveTraverser@Graph@@@Z
void DXGfx::TextureAttributeD3D::Pop(Graph::PassiveTraverser *arg1) const
{
    TextureAttribute::Pop(arg1);
}

// 0x100da240 ?ReadVis@TextureAttributeD3D@DXGfx@@UAEXPAVStream@Graph@@@Z
void DXGfx::TextureAttributeD3D::ReadVis(Graph::Stream *stream)
{
    TextureAttribute::ReadVis(stream);
    SetTexture();
}

// 0x100da260 ?Read@TextureAttributeD3D@DXGfx@@UAEXPAVStream@Graph@@@Z
void DXGfx::TextureAttributeD3D::Read(Graph::Stream *stream)
{
    TextureAttribute::Read(stream);
    SetTexture();
}

// The stage's colour and alpha operations for the compositing mode.
// 0x100da280 ?SetTextureStageCompositeMode@TextureAttributeD3D@DXGfx@@UAEXW4TextureStageCompositeMode@TextureAttribute@Gfx@@@Z
void DXGfx::TextureAttributeD3D::SetTextureStageCompositeMode(Gfx::TextureAttribute::TextureStageCompositeMode mode)
{
    m_TextureStageCompositeMode = mode;
    m_ColorArg1 = D3DTA_TEXTURE;
    m_ColorArg2 = D3DTA_CURRENT;
    m_AlphaArg1 = D3DTA_TEXTURE;
    m_AlphaArg2 = D3DTA_CURRENT;
    m_AlphaOp = D3DTOP_DISABLE;
    if (mode == c_Default) {
        m_ColorOp = D3DTOP_MODULATE;
        m_ColorArg1 = D3DTA_TEXTURE;
        m_ColorArg2 = D3DTA_DIFFUSE;
        if (m_HasColorKey || m_Alpha) {
            m_AlphaOp = D3DTOP_MODULATE;
            m_AlphaArg1 = D3DTA_TEXTURE;
            m_AlphaArg2 = D3DTA_DIFFUSE;
        } else {
            m_AlphaOp = D3DTOP_SELECTARG1;
            m_AlphaArg1 = D3DTA_TEXTURE;
            m_AlphaArg2 = D3DTA_DIFFUSE;
        }
    } else if (mode == c_ModulateTextureWithPreviousStage) {
        m_ColorOp = D3DTOP_MODULATE;
        if (m_HasColorKey || m_Alpha)
            m_AlphaOp = D3DTOP_BLENDCURRENTALPHA;
    } else if (mode == c_ModulateTextureWithPreviousStageX2) {
        m_ColorOp = D3DTOP_MODULATE2X;
        m_AlphaOp = D3DTOP_BLENDCURRENTALPHA;
    } else if (mode == c_ModulateTextureWithPreviousStageX4) {
        m_ColorOp = D3DTOP_MODULATE4X;
        m_AlphaOp = D3DTOP_BLENDCURRENTALPHA;
    } else if (mode == c_AddTextureToPreviousStage) {
        m_ColorOp = D3DTOP_ADD;
        m_AlphaOp = D3DTOP_SELECTARG1;
    } else if (mode == c_AddTextureToPreviousStageSigned) {
        m_ColorOp = D3DTOP_ADDSIGNED;
        m_AlphaOp = D3DTOP_BLENDCURRENTALPHA;
    } else if (mode == c_AddTextureToPreviousStageSignedX2) {
        m_ColorOp = D3DTOP_ADDSIGNED2X;
        m_AlphaOp = D3DTOP_BLENDCURRENTALPHA;
    } else if (mode == c_AddTextureToPreviousStageSmooth) {
        m_ColorOp = D3DTOP_ADDSMOOTH;
        m_AlphaOp = D3DTOP_BLENDCURRENTALPHA;
    }
}

// TODO 0x100da400 ?GetTextureCache@TextureAttributeD3D@DXGfx@@MAEPAEAAVRefreshTraverser@Gfx@@@Z: 67% - the original keeps the device and paths in stack slots and inlines the string helpers differently; same calls
unsigned char *DXGfx::TextureAttributeD3D::GetTextureCache(Gfx::RefreshTraverser &traverser)
{
    if (!m_CachedSurface) {
        DX::Device *device = ((DXGfx::ViewD3D *)traverser.GetView())->GetDevice();
        std::list<std::string> &paths = device->GetDirect3D()->GetTextureMapManager()->GetPaths();
        _SS<char> name(m_Filename.getConstPtr());
        if (!name.empty())
            _strlwr(name.Buffer());
        int pos = name.find(".png");
        if (pos != -1 && pos >= 0) {
            DX::PNGFileFormat *png = 0;
            DX::DirectDrawSurface *surface = new DX::DirectDrawSurface(device->GetDirect3D()->GetTextureMapManager()->GetDirectDraw());
            std::list<std::string>::iterator path;
            for (path = paths.begin(); path != paths.end(); ++path) {
                try {
                    png = surface->LoadPNG((*path + name.c_str()).c_str());
                    break;
                } catch (DX::Exception &e) {
                    if (e.GetResult() != 110)
                        throw;
                }
            }
            if (path == paths.end())
                throw DX::Exception(110, "Could not locate texture map");
            m_CachedSurface = new unsigned char[png->width * png->height * 3];
            if (png->lpDDPal) {
                PALETTEENTRY *palette = new PALETTEENTRY[256];
                png->lpDDPal->GetEntries(0, 0, 256, palette);
                m_CachedSurfaceWidth = png->width;
                m_CachedSurfaceHeight = png->height;
                m_CachedSurfaceDepth = 3;
                int row = png->height - 1;
                for (int y = 0; y < png->height; y++, row--) {
                    unsigned char *src = png->pixels + png->rowBytes * y;
                    int dst = png->width * row * 3;
                    for (int x = 0; x < png->width; x++, dst += 3) {
                        m_CachedSurface[dst] = palette[src[x]].peRed;
                        m_CachedSurface[dst + 1] = palette[src[x]].peGreen;
                        m_CachedSurface[dst + 2] = palette[src[x]].peBlue;
                    }
                }
                delete[] palette;
            }
            if (png) {
                if (png->pixels)
                    delete png->pixels;
                delete png;
            }
            delete surface;
        }
    }
    return m_CachedSurface;
}

// 0x100da820 _$E70
// 0x100da850 _$E66
// 0x100da860 _$E69

// Template and inline instances this unit also emits:
// EQUIV 0x100da7a0 ?find@?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@QBEIPBDII@Z: reviewed: npos is the constant -1 in the original and loaded from memory by VC6 (as Actor.cpp rfind); nothing else differs. Was: 98% - the inline instance compiles differently here

// the run-time type of DXGfx::TextureAttributeD3D (values from the original's data section)
StdBase::TypeInfo const DXGfx::TextureAttributeD3D::s_TypeInfo = { "DXGfx::TextureAttributeD3D", &Gfx::TextureAttribute::s_TypeInfo };
