#pragma optimize("a", on) // evidence: the constructor, operator= and Pop store and load out of source order (no-alias code)
#include "Gfx_MaterialAttribute.h"
#include "MathConstants.h"
#include "Graph_BasicAttribute.h"
#include "StdBase.h"
#include "Gfx_WriteTraverser.h"
#include "Graph_FileStream.h"
#include "VisMisc.h"
#include "Gfx_TextureAttribute.h"
#include "Gfx_NodeFactory.h"
#include "Graph_DataBase.h"
#include "Graph_BasicNode.h"
#include "Text.h"
#include "StdString.h"
#include <locale> // evidence: the original unit ends with the std::ctype<unsigned short>::id and std::collate<char>::id initializers

// 0x100f76a0 ?GetTypeInfo@MaterialAttribute@Gfx@@UBEPBUTypeInfo@StdBase@@XZ
StdBase::TypeInfo const * Gfx::MaterialAttribute::GetTypeInfo() const
{
    return &s_TypeInfo;
}

// 0x100f76b0 ?GetTypeName@MaterialAttribute@Gfx@@UBEPBDXZ
char const * Gfx::MaterialAttribute::GetTypeName() const
{
    return s_TypeInfo.typeName;
}

// 0x100f76c0 ??0MaterialAttribute@Gfx@@QAE@XZ
Gfx::MaterialAttribute::MaterialAttribute()
    : m_Ambient(0.2f, 0.2f, 0.2f, 1.0f), m_Diffuse(0.6f, 0.6f, 0.6f, 1.0f), m_Specular(1.0f, 1.0f, 1.0f, 1.0f),
      m_Emissive(0.0f, 0.0f, 0.0f, 1.0f), m_Power(0.0f), m_ShadeMode(2), m_BackfaceCulling(1), field_c2(true),
      m_ZBuffer(true), m_Textured(false), field_c5(false)
{
}

// 0x100f77a0 ?Push@MaterialAttribute@Gfx@@UBEXPAVPassiveTraverser@Graph@@@Z
void Gfx::MaterialAttribute::Push(Graph::PassiveTraverser *) const
{
    s_Stack.push(this);
}

// 0x100f7990 ?Pop@MaterialAttribute@Gfx@@UBEXPAVPassiveTraverser@Graph@@@Z
void Gfx::MaterialAttribute::Pop(Graph::PassiveTraverser *) const
{
    s_Stack.pop();
}

// 0x100f79d0 ?Write@MaterialAttribute@Gfx@@UAEXPAVWriteTraverser@2@@Z
void Gfx::MaterialAttribute::Write(Gfx::WriteTraverser *traverser)
{
    Text name("GfxMaterialAttribute");
    writeText(name, traverser->GetStream());
    WriteInternals(traverser);
}

// 0x100f7a50 ?ReadVis@MaterialAttribute@Gfx@@UAEXPAVStream@Graph@@@Z
void Gfx::MaterialAttribute::ReadVis(Graph::Stream *stream)
{
    Attribute::ReadVis(stream);
    ReadInternals(stream, 0);
}

// 0x100f7a70 ?Read@MaterialAttribute@Gfx@@UAEXPAVStream@Graph@@@Z
void Gfx::MaterialAttribute::Read(Graph::Stream *stream)
{
    int version;
    Attribute::Read(stream);
    stream->Read(&version);
    ReadInternals(stream, version);
}

// 0x100f7aa0 ??4MaterialAttribute@Gfx@@UAEAAV01@ABV01@@Z
Gfx::MaterialAttribute &Gfx::MaterialAttribute::operator=(const Gfx::MaterialAttribute &material)
{
    m_Ambient = material.m_Ambient;
    m_Diffuse = material.m_Diffuse;
    m_Specular = material.m_Specular;
    m_Emissive = material.m_Emissive;
    m_Power = material.GetPower();
    return *this;
}

// 0x100f7b40 ??8MaterialAttribute@Gfx@@QBE_NABV01@@Z
bool Gfx::MaterialAttribute::operator==(const Gfx::MaterialAttribute &material) const
{
    if (m_Ambient == material.m_Ambient && m_Diffuse == material.m_Diffuse && m_Specular == material.m_Specular &&
        m_Emissive == material.m_Emissive && GetPower() == material.GetPower() && m_ShadeMode == material.m_ShadeMode &&
        m_BackfaceCulling == material.m_BackfaceCulling && field_c2 == material.field_c2 &&
        m_ZBuffer == material.m_ZBuffer)
        return true;
    return false;
}

// 0x100f7c80 ?SetMaterial@MaterialAttribute@Gfx@@UAEXXZ
void Gfx::MaterialAttribute::SetMaterial()
{
    ScheduleRefresh();
}

// 0x100f7c90 ??1MaterialAttribute@Gfx@@MAE@XZ
Gfx::MaterialAttribute::~MaterialAttribute()
{
}

// 0x100f7ca0 ?WriteInternals@MaterialAttribute@Gfx@@IAEXPAVWriteTraverser@2@@Z
void Gfx::MaterialAttribute::WriteInternals(Gfx::WriteTraverser *traverser)
{
    Attribute::WriteInternals(traverser);
    Graph::FileStream *stream = traverser->GetStream();
    stream->Write(Version);
    writeVec4f(m_Ambient, stream);
    writeVec4f(m_Diffuse, stream);
    writeVec4f(m_Specular, stream);
    writeVec4f(m_Emissive, stream);
    stream->Write(m_Power);
    stream->Write(m_BackfaceCulling);
    stream->Write((short)field_c2);
    stream->Write((short)m_ZBuffer);
}

// EQUIV 0x100f7d40 ?ReadInternals@MaterialAttribute@Gfx@@IAEXPAVStream@Graph@@H@Z: reviewed: 66% by bytes; the original keeps the _SS temporary 4 bytes lower (its frame is 4 bytes smaller), its inlined _Tidy forms the count pointer with lea instead of dec, and the m_Power = 0 store is scheduled past two independent loads; calls, arguments, conditions and their order agree
void Gfx::MaterialAttribute::ReadInternals(Graph::Stream *stream, int version)
{
    if (version < 6) {
        // old files keep the texture in the material
        Gfx::TextureAttribute *texture = (Gfx::TextureAttribute *)((Gfx::NodeFactory *)Graph::DataBase::GetInstance()->GetFactory())->MakeTextureAttribute();
        ReadInternals(stream, version, texture);
        Graph::BasicNode *parent = Graph::DataBase::GetInstance()->GetCurrentParentNode();
        if (parent) {
            if (_SS<char>(texture->GetFilename()).GetLength() > 0) {
                texture->SetTexture();
                parent->AddAttribute(texture);
            } else {
                texture->Unref();
            }
        }
        return;
    }
    readVec4f(m_Ambient, stream);
    readVec4f(m_Diffuse, stream);
    readVec4f(m_Specular, stream);
    readVec4f(m_Emissive, stream);
    stream->Read(&m_Power);
    m_Power = 0.0f;
    stream->Read(&m_BackfaceCulling);
    short flag;
    stream->Read(&flag);
    field_c2 = flag != 0;
    if (version > 6) {
        short zBuffer;
        stream->Read(&zBuffer);
        m_ZBuffer = zBuffer != 0;
    }
}

// EQUIV 0x100f7f70 ?ReadInternals@MaterialAttribute@Gfx@@IAEXPAVStream@Graph@@HPAVTextureAttribute@2@@Z: reviewed: 66% by bytes; the original keeps 0 and 1 in registers (VC6 uses immediates) and its short locals in other slots (frame 8 bytes smaller), and inlines Text's unref in the version > 1 branch where VC6 inlines it in the version <= 0 branch (the same BasicObject::unref); calls, arguments, conditions and their order agree
void Gfx::MaterialAttribute::ReadInternals(Graph::Stream *stream, int version, Gfx::TextureAttribute *texture)
{
    Text filename;
    readVec4f(m_Ambient, stream);
    readVec4f(m_Diffuse, stream);
    readVec4f(m_Specular, stream);
    readVec4f(m_Emissive, stream);
    stream->Read(&m_Power);
    m_Power = 0.0f;
    if (m_Textured) {
        if (version > 0) {
            stream->Read(&m_BackfaceCulling);
            short flag;
            stream->Read(&flag);
            field_c2 = flag != 0;
            int ignored;
            stream->Read(&ignored);
            filename = readText(stream);
            texture->SetFilename(filename);
            short colorKeyed;
            stream->Read(&colorKeyed);
            texture->SetHasColorKey(colorKeyed != 0);
            unsigned long colorKey;
            stream->Read((unsigned char *)&colorKey, 4);
            texture->SetColorKey(colorKey);
            int filter;
            stream->Read(&filter);
            texture->SetFilter((Gfx::TextureAttribute::FilterType)filter);
        } else {
            filename = readText(stream);
            texture->SetFilename(filename);
        }
    } else {
        stream->Read(&m_BackfaceCulling);
        short flag;
        stream->Read(&flag);
        field_c2 = flag != 0;
        if (version > 1) {
            short ignored;
            stream->Read(&ignored);
            filename = readText(stream);
            texture->SetFilename(filename);
            short colorKeyed;
            stream->Read(&colorKeyed);
            texture->SetHasColorKey(colorKeyed != 0);
            unsigned long colorKey;
            stream->Read((unsigned char *)&colorKey, 4);
            texture->SetColorKey(colorKey);
            int filter;
            stream->Read(&filter);
            texture->SetFilter((Gfx::TextureAttribute::FilterType)filter);
            if (version > 2) {
                short alpha;
                stream->Read(&alpha);
                texture->SetAlpha(alpha != 0);
                if (version > 3) {
                    short mipMap;
                    stream->Read(&mipMap);
                    texture->SetMipMap(mipMap != 0);
                    if (version > 4) {
                        short blendMode;
                        stream->Read(&blendMode);
                        texture->SetBlendMode((Gfx::TextureAttribute::BlendMode)blendMode);
                    }
                }
            }
        }
    }
}

// the run-time type of Gfx::MaterialAttribute (values from the original's data section)
StdBase::TypeInfo const Gfx::MaterialAttribute::s_TypeInfo = { "Gfx::MaterialAttribute", &Gfx::Attribute::s_TypeInfo };
int const Gfx::MaterialAttribute::Version = 7;
std::stack<Gfx::MaterialAttribute const *, std::vector<Gfx::MaterialAttribute const *, std::allocator<Gfx::MaterialAttribute const *> > > Gfx::MaterialAttribute::s_Stack;

// 0x100f75b0 _$E19
// 0x100f75c0 _$E18
// 0x100f75e0 _$E22
// 0x100f75f0 _$E21
// 0x100f7610 _$E25
// 0x100f7620 _$E24
// 0x100f7640 _$E28
// 0x100f7650 _$E27
// 0x100f7670 _$E31
// 0x100f7680 _$E30

// 0x100f8320 _$E67
// 0x100f8330 _$E63
// 0x100f8350 _$E66
// 0x100f8360 _$E64

// Compiler-generated and template instances this unit also emits:
// 0x100f7780 ??_GMaterialAttribute@Gfx@@MAEPAXI@Z
// 0x100f8380 ??1?$stack@PBVMaterialAttribute@Gfx@@V?$vector@PBVMaterialAttribute@Gfx@@V?$allocator@PBVMaterialAttribute@Gfx@@@std@@@std@@@std@@QAE@XZ
// 0x100f83b0 ??0?$stack@PBVMaterialAttribute@Gfx@@V?$vector@PBVMaterialAttribute@Gfx@@V?$allocator@PBVMaterialAttribute@Gfx@@@std@@@std@@@std@@QAE@ABV?$allocator@PBVMaterialAttribute@Gfx@@@1@@Z
// 0x100f83d0 ?_Destroy@?$vector@PBVMaterialAttribute@Gfx@@V?$allocator@PBVMaterialAttribute@Gfx@@@std@@@std@@IAEXPAPBVMaterialAttribute@Gfx@@0@Z
// 0x100f83e0 ?_Ucopy@?$vector@PBVMaterialAttribute@Gfx@@V?$allocator@PBVMaterialAttribute@Gfx@@@std@@@std@@IAEPAPBVMaterialAttribute@Gfx@@PBQBV34@0PAPBV34@@Z
// 0x100f8410 ?_Ufill@?$vector@PBVMaterialAttribute@Gfx@@V?$allocator@PBVMaterialAttribute@Gfx@@@std@@@std@@IAEXPAPBVMaterialAttribute@Gfx@@IABQBV34@@Z
// 0x100f8440 ?_Construct@std@@YAXPAPBVMaterialAttribute@Gfx@@ABQBV23@@Z

// 0x100f8460 _$E78
// 0x100f8490 _$E74
// 0x100f84a0 _$E77
