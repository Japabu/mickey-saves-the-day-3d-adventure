// PORT override: 64-bit: a texel's colour is read as 4 bytes (the original reads an unsigned long, 8 bytes here)
#include "Gfx_TextureAttribute.h"
#include "StdBase.h"
#include <string.h>
#include "Gfx_DisplayQuality.h"
#include "Gfx_AttributeStack.h"
#include "Gfx_WriteTraverser.h"
#include "Graph_FileStream.h"
#include "VisMisc.h"
#pragma optimize("a", on) // evidence: Push and Pop keep the stack's _Last in a register across the inlined vector copies (no-alias code)

// 0x100f84b0 ?GetTypeInfo@TextureAttribute@Gfx@@UBEPBUTypeInfo@StdBase@@XZ
StdBase::TypeInfo const * Gfx::TextureAttribute::GetTypeInfo() const
{
    return &s_TypeInfo;
}

// 0x100f84c0 ?GetTypeName@TextureAttribute@Gfx@@UBEPBDXZ
char const * Gfx::TextureAttribute::GetTypeName() const
{
    return s_TypeInfo.typeName;
}

// 0x100f84d0 ??0TextureAttribute@Gfx@@QAE@XZ
Gfx::TextureAttribute::TextureAttribute()
    : m_BlendMode(c_Normal), m_ColorKey(0), m_HasColorKey(false), m_Filter(c_Bilinear), m_MipMap(false),
      m_Alpha(false), m_WrapU(true), m_WrapV(true), m_TextureStage(0), m_TextureStageCompositeMode(c_Default),
      m_MultiPassOverride(false), m_Enable(true), m_CachedSurface(0), m_CachedSurfaceWidth(0),
      m_CachedSurfaceHeight(0), m_CachedSurfaceDepth(0)
{
}

// 0x100f85c0 ?Push@TextureAttribute@Gfx@@UBEXPAVPassiveTraverser@Graph@@@Z
void Gfx::TextureAttribute::Push(Graph::PassiveTraverser *) const
{
    s_Stack[m_TextureStage].push(this);
}

// 0x100f8780 ?Pop@TextureAttribute@Gfx@@UBEXPAVPassiveTraverser@Graph@@@Z
void Gfx::TextureAttribute::Pop(Graph::PassiveTraverser *) const
{
    s_Stack[m_TextureStage].pop();
}

// 0x100f87d0 ?Write@TextureAttribute@Gfx@@UAEXPAVWriteTraverser@2@@Z
void Gfx::TextureAttribute::Write(Gfx::WriteTraverser *traverser)
{
    Text name("GfxTextureAttribute");
    writeText(name, traverser->GetStream());
    WriteInternals(traverser);
}

// 0x100f8850 ?ReadVis@TextureAttribute@Gfx@@UAEXPAVStream@Graph@@@Z
void Gfx::TextureAttribute::ReadVis(Graph::Stream *stream)
{
    Attribute::ReadVis(stream);
    ReadInternals(stream, 0);
}

// 0x100f8870 ?Read@TextureAttribute@Gfx@@UAEXPAVStream@Graph@@@Z
void Gfx::TextureAttribute::Read(Graph::Stream *stream)
{
    int version;
    Attribute::Read(stream);
    stream->Read(&version);
    ReadInternals(stream, version);
}

// 0x100f88a0 ??4TextureAttribute@Gfx@@QAEXABV01@@Z
void Gfx::TextureAttribute::operator=(const Gfx::TextureAttribute &texture)
{
    SetFilename(texture.GetFilename());
    SetHasColorKey(texture.HasColorKey());
    SetColorKey(texture.GetColorKey());
    SetFilter(texture.GetFilter());
    SetMipMap(texture.GetMipMap());
    SetAlpha(texture.HasAlpha());
    SetBlendMode(texture.GetBlendMode());
    SetWrapU(texture.GetWrapU());
    SetWrapV(texture.GetWrapV());
    SetTextureStage(texture.GetTextureStage());
}

// 0x100f8950 ??8TextureAttribute@Gfx@@QBE_NABV01@@Z
bool Gfx::TextureAttribute::operator==(const Gfx::TextureAttribute &texture) const
{
    bool equal = strcmp(GetFilename(), texture.GetFilename()) == 0; return equal;
}

// 0x100f89b0 ?GetEnable@TextureAttribute@Gfx@@QBE_NXZ
bool Gfx::TextureAttribute::GetEnable() const
{
    if (m_TextureAttributeEnableContext)
        return m_TextureAttributeEnableContext->GetValue((Gfx::TextureAttribute *)this);
    return m_Enable;
}

// 0x100f89f0 ?SetEnable@TextureAttribute@Gfx@@QAEX_N@Z
void Gfx::TextureAttribute::SetEnable(bool enable)
{
    if (GetEnable() != enable) {
        if (m_TextureAttributeEnableContext)
            m_TextureAttributeEnableContext->GetValue(this) = enable;
        else
            m_Enable = enable;
    }
}

// The texel at (u, v) of the cached 24-bit surface, with alpha 0xff;
// coordinates are wrapped or clamped to [0, 1].
// EQUIV 0x100f8a40 ?GetTextureColor@TextureAttribute@Gfx@@QAEKAAVRefreshTraverser@2@MM@Z: reviewed: one EH state store is scheduled past an independent instruction; nothing else differs. Was: instruction order only (91% similar): the original restores edi before the 0xff byte store, VC6 here after; same code otherwise
unsigned long Gfx::TextureAttribute::GetTextureColor(Gfx::RefreshTraverser &traverser, float u, float v)
{
    if (m_WrapU) {
        if (u > 1.0f || u < -1.0f)
            u -= (int)u;
        if (u < 0.0f)
            u += 1.0f;
    } else if (u > 1.0f) {
        u = 1.0f;
    } else if (u < 0.0f) {
        u = 0.0f;
    }
    if (m_WrapV) {
        if (v > 1.0f || v < -1.0f)
            v -= (int)v;
        if (v < 0.0f)
            v += 1.0f;
    } else if (v > 1.0f) {
        v = 1.0f;
    } else if (v < 0.0f) {
        v = 0.0f;
    }
    unsigned char *bits = GetTextureCache(traverser);
    int texel = (int)((m_CachedSurfaceHeight - 1) * v) * m_CachedSurfaceWidth + (int)((m_CachedSurfaceWidth - 1) * u);
    unsigned char *p = &bits[texel * 3];
    unsigned char color[4];
    color[3] = 0xff;
    color[0] = p[0];
    color[1] = p[1];
    color[2] = p[2];
    return *(unsigned int *)color; // PORT: the four colour bytes, not eight
}

// 0x100f8bd0 ??1TextureAttribute@Gfx@@MAE@XZ
Gfx::TextureAttribute::~TextureAttribute()
{
    if (m_CachedSurface) {
        delete [] m_CachedSurface;
        m_CachedSurface = 0;
    }
}

// 0x100f8c50 ?WriteInternals@TextureAttribute@Gfx@@IAEXPAVWriteTraverser@2@@Z
void Gfx::TextureAttribute::WriteInternals(Gfx::WriteTraverser *traverser)
{
    Attribute::WriteInternals(traverser);
    Graph::FileStream *stream = traverser->GetStream();
    stream->Write(Version);
    writeText(m_Filename, stream);
    stream->Write((short)m_HasColorKey);
    stream->Write((unsigned char *)&m_ColorKey, 4);
    stream->Write((int)m_Filter);
    stream->Write((short)m_Alpha);
    stream->Write((short)m_MipMap);
    stream->Write((short)m_WrapU);
    stream->Write((short)m_WrapV);
    stream->Write((int)m_BlendMode);
    stream->Write((int)m_TextureStageCompositeMode);
    stream->Write(m_TextureStage);
}

// 0x100f8d20 ?ReadInternals@TextureAttribute@Gfx@@IAEXPAVStream@Graph@@H@Z
void Gfx::TextureAttribute::ReadInternals(Graph::Stream *stream, int version)
{
    m_Filename = readText(stream);
    short hasColorKey;
    stream->Read(&hasColorKey);
    m_HasColorKey = hasColorKey != 0;
    stream->Read((unsigned char *)&m_ColorKey, 4);
    stream->Read((int *)&m_Filter);
    short alpha;
    stream->Read(&alpha);
    m_Alpha = alpha != 0;
    short mipMap;
    stream->Read(&mipMap);
    m_MipMap = mipMap != 0;
    if (version > 0) {
        short wrap;
        stream->Read(&wrap);
        m_WrapU = wrap != 0;
        stream->Read(&wrap);
        m_WrapV = wrap != 0;
    }
    if (version > 1)
        stream->Read((int *)&m_BlendMode);
    if (version > 2)
        stream->Read((int *)&m_TextureStageCompositeMode);
    if (version > 3)
        stream->Read(&m_TextureStage);
}

// 0x100f8e90 ?WantTexturing@TextureAttribute@Gfx@@QBE_NXZ
bool Gfx::TextureAttribute::WantTexturing() const
{
    bool want = true;
    if (Gfx::DisplayQuality::GetStack()) {
        const Gfx::DisplayQuality *quality = (const Gfx::DisplayQuality *)Gfx::DisplayQuality::GetStack()->GetTop();
        if (quality) {
            switch (quality->GetQuality()) {
            case 1:
                want = m_Alpha;
                break;
            case 2:
                want = false;
                break;
            }
        }
    }
    return want;
}

// the run-time type of Gfx::TextureAttribute (values from the original's data section)
StdBase::TypeInfo const Gfx::TextureAttribute::s_TypeInfo = { "Gfx::TextureAttribute", &Gfx::Attribute::s_TypeInfo };
int const Gfx::TextureAttribute::Version = 4;
std::stack<Gfx::TextureAttribute const *, std::vector<Gfx::TextureAttribute const *, std::allocator<Gfx::TextureAttribute const *> > > Gfx::TextureAttribute::s_Stack[8];
Graph::NodeContext<Gfx::TextureAttribute, bool> *Gfx::TextureAttribute::m_TextureAttributeEnableContext = 0;

// 0x100f8ed0 _$E38
// 0x100f8ee0 _$E34
// 0x100f8f00 _$E37
// 0x100f8f10 _$E35

// Compiler-generated and template instances this unit also emits:
// 0x100f85a0 ??_GTextureAttribute@Gfx@@MAEPAXI@Z
// 0x100f8f40 ??_F?$stack@PBVTextureAttribute@Gfx@@V?$vector@PBVTextureAttribute@Gfx@@V?$allocator@PBVTextureAttribute@Gfx@@@std@@@std@@@std@@QAEXXZ
// 0x100f8f50 ??1?$stack@PBVTextureAttribute@Gfx@@V?$vector@PBVTextureAttribute@Gfx@@V?$allocator@PBVTextureAttribute@Gfx@@@std@@@std@@@std@@QAE@XZ
// 0x100f8f80 ??0?$stack@PBVTextureAttribute@Gfx@@V?$vector@PBVTextureAttribute@Gfx@@V?$allocator@PBVTextureAttribute@Gfx@@@std@@@std@@@std@@QAE@ABV?$allocator@PBVTextureAttribute@Gfx@@@1@@Z
// 0x100f8fa0 ?_Destroy@?$vector@PBVTextureAttribute@Gfx@@V?$allocator@PBVTextureAttribute@Gfx@@@std@@@std@@IAEXPAPBVTextureAttribute@Gfx@@0@Z
// 0x100f8fb0 ?_Ucopy@?$vector@PBVTextureAttribute@Gfx@@V?$allocator@PBVTextureAttribute@Gfx@@@std@@@std@@IAEPAPBVTextureAttribute@Gfx@@PBQBV34@0PAPBV34@@Z
// 0x100f8fe0 ?_Ufill@?$vector@PBVTextureAttribute@Gfx@@V?$allocator@PBVTextureAttribute@Gfx@@@std@@@std@@IAEXPAPBVTextureAttribute@Gfx@@IABQBV34@@Z
// 0x100f9010 ?_Construct@std@@YAXPAPBVTextureAttribute@Gfx@@ABQBV23@@Z
