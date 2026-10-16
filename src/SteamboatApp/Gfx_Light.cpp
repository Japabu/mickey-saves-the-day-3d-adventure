#include "Gfx_Light.h"
#include "MathConstants.h"
#include "StdBase.h"
#include "Gfx_WriteTraverser.h"
#include "Graph_FileStream.h"
#include "VisMisc.h"

// 0x100f33f0 ?GetTypeInfo@Light@Gfx@@UBEPBUTypeInfo@StdBase@@XZ
StdBase::TypeInfo const * Gfx::Light::GetTypeInfo() const
{
    return &s_TypeInfo;
}

// 0x100f3400 ?GetTypeName@Light@Gfx@@UBEPBDXZ
char const * Gfx::Light::GetTypeName() const
{
    return s_TypeInfo.typeName;
}

// 0x100f3410 ?Write@Light@Gfx@@UAEXPAVWriteTraverser@2@@Z
void Gfx::Light::Write(Gfx::WriteTraverser *traverser)
{
    unsigned char header[10] = "GfxLight";
    if (WriteHeaderInfo(traverser, header))
        WriteInternals(traverser);
}

// 0x100f3460 ?Read@Light@Gfx@@UAEXPAVStream@Graph@@@Z
void Gfx::Light::Read(Graph::Stream *stream)
{
    int version;
    Node::Read(stream);
    stream->Read(&version);
    ReadInternals(stream, version);
}

// 0x100f3490 ?WriteInternals@Light@Gfx@@IAEXPAVWriteTraverser@2@@Z
void Gfx::Light::WriteInternals(Gfx::WriteTraverser *traverser)
{
    Node::WriteInternals(traverser);
    Graph::FileStream *stream = traverser->GetStream();
    stream->Write(Version);
    stream->Write((short)GetType());
    writeVec3f(GetPosition(), stream);
    writeVec3f(GetDirection(), stream);
    writeVec4f(GetColor(), stream);
}

// 0x100f3510 ?ReadInternals@Light@Gfx@@IAEXPAVStream@Graph@@H@Z
void Gfx::Light::ReadInternals(Graph::Stream *stream, int version)
{
    short type;
    stream->Read(&type);
    SetType((LightType)type);
    Vec3<float> vector;
    readVec3f(vector, stream);
    SetPosition(vector);
    readVec3f(vector, stream);
    SetDirection(vector);
    Vec4<float> color;
    readVec4f(color, stream);
    SetColor(color);
}

int const Gfx::Light::Version = 0;

// 0x100f3300 _$E37
// 0x100f3310 _$E36
// 0x100f3330 _$E40
// 0x100f3340 _$E39
// 0x100f3360 _$E43
// 0x100f3370 _$E42
// 0x100f3390 _$E46
// 0x100f33a0 _$E45
// 0x100f33c0 _$E49
// 0x100f33d0 _$E48

// the run-time type of Gfx::Light (values from the original's data section)
StdBase::TypeInfo const Gfx::Light::s_TypeInfo = { "Gfx::Light", &Gfx::Node::s_TypeInfo };
