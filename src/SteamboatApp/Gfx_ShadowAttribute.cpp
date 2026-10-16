#pragma optimize("a", on) // evidence: the constructor and destructor load s_InstanceCount before the vftable store (no-alias code)
#include "Gfx_ShadowAttribute.h"
#include "Gfx_Attribute.h"
#include "Gfx_AttributeStack.h"
#include "Graph_PassiveTraverser.h"
#include "StdBase.h"
#include "Gfx_WriteTraverser.h"
#include "Graph_FileStream.h"
#include "VisMisc.h"

// 0x100fc6d0 ?GetTypeInfo@ShadowAttribute@Gfx@@UBEPBUTypeInfo@StdBase@@XZ
StdBase::TypeInfo const * Gfx::ShadowAttribute::GetTypeInfo() const
{
    return &s_TypeInfo;
}

// 0x100fc6e0 ?GetTypeName@ShadowAttribute@Gfx@@UBEPBDXZ
char const * Gfx::ShadowAttribute::GetTypeName() const
{
    return s_TypeInfo.typeName;
}

// 0x100fc6f0 ??0ShadowAttribute@Gfx@@QAE@XZ
Gfx::ShadowAttribute::ShadowAttribute() : m_Plane(Vec3<float>(0.0f, 0.0f, 1.0f), 0.0f)
{
    if (++s_InstanceCount == 1)
        s_Stack = new Gfx::AttributeStack(0);
}

// 0x100fc7c0 ?Push@ShadowAttribute@Gfx@@UBEXPAVPassiveTraverser@Graph@@@Z
void Gfx::ShadowAttribute::Push(Graph::PassiveTraverser *) const
{
    s_Stack->Push(this);
}

// 0x100fc7d0 ?Pop@ShadowAttribute@Gfx@@UBEXPAVPassiveTraverser@Graph@@@Z
void Gfx::ShadowAttribute::Pop(Graph::PassiveTraverser *) const
{
    s_Stack->Pop();
}

// 0x100fc7e0 ?Write@ShadowAttribute@Gfx@@UAEXPAVWriteTraverser@2@@Z
void Gfx::ShadowAttribute::Write(Gfx::WriteTraverser *traverser)
{
    Text name("GfxShadowAttribute");
    writeText(name, traverser->GetStream());
    WriteInternals(traverser);
}

// 0x100fc860 ?Read@ShadowAttribute@Gfx@@UAEXPAVStream@Graph@@@Z
void Gfx::ShadowAttribute::Read(Graph::Stream *stream)
{
    int version;
    Attribute::Read(stream);
    stream->Read(&version);
    ReadInternals(stream, version);
}

// 0x100fc890 ?WriteInternals@ShadowAttribute@Gfx@@IAEXPAVWriteTraverser@2@@Z
void Gfx::ShadowAttribute::WriteInternals(Gfx::WriteTraverser *traverser)
{
    Attribute::WriteInternals(traverser);
    Graph::FileStream *stream = traverser->GetStream();
    stream->Write(Version);
    writeVec3f(m_Plane.GetNormal(), stream);
    stream->Write(m_Plane.GetOffset());
}

// 0x100fc8e0 ?ReadInternals@ShadowAttribute@Gfx@@IAEXPAVStream@Graph@@H@Z
void Gfx::ShadowAttribute::ReadInternals(Graph::Stream *stream, int)
{
    Vec3<float> normal;
    float offset;
    readVec3f(normal, stream);
    stream->Read(&offset);
    m_Plane.Set(normal, offset);
}

// 0x100fc920 ??1ShadowAttribute@Gfx@@MAE@XZ
Gfx::ShadowAttribute::~ShadowAttribute()
{
    if (--s_InstanceCount == 0) {
        delete s_Stack;
        s_Stack = 0;
    }
}

// the run-time type of Gfx::ShadowAttribute (values from the original's data section)
StdBase::TypeInfo const Gfx::ShadowAttribute::s_TypeInfo = { "Gfx::ShadowAttribute", &Gfx::Attribute::s_TypeInfo };
int const Gfx::ShadowAttribute::Version = 0;

// 0x100fc7a0 ??_GShadowAttribute@Gfx@@MAEPAXI@Z

// static data (zero in the original, .bss)
Gfx::AttributeStack *Gfx::ShadowAttribute::s_Stack = 0;
int Gfx::ShadowAttribute::s_InstanceCount = 0;
