#pragma optimize("a", on) // evidence: the constructor loads s_InstanceCount before the member and vftable stores (no-alias code)
#include "Gfx_DisplayQuality.h"
#include "Gfx_Attribute.h"
#include "Gfx_AttributeStack.h"
#include "Graph_PassiveTraverser.h"
#include "StdBase.h"
#include "Gfx_WriteTraverser.h"
#include "Graph_FileStream.h"
#include "VisMisc.h"

// 0x10110d30 ?GetTypeInfo@DisplayQuality@Gfx@@UBEPBUTypeInfo@StdBase@@XZ
StdBase::TypeInfo const * Gfx::DisplayQuality::GetTypeInfo() const
{
    return &s_TypeInfo;
}

// 0x10110d40 ?GetTypeName@DisplayQuality@Gfx@@UBEPBDXZ
char const * Gfx::DisplayQuality::GetTypeName() const
{
    return s_TypeInfo.typeName;
}

// 0x10110d50 ??0DisplayQuality@Gfx@@QAE@XZ
Gfx::DisplayQuality::DisplayQuality() : m_Quality(0)
{
    if (++s_InstanceCount == 1)
        s_Stack = new Gfx::AttributeStack(0);
}

// 0x10110e00 ??1DisplayQuality@Gfx@@UAE@XZ
Gfx::DisplayQuality::~DisplayQuality()
{
    if (--s_InstanceCount == 0) {
        delete s_Stack;
        s_Stack = 0;
    }
}

// 0x10110e80 ?Push@DisplayQuality@Gfx@@UBEXPAVPassiveTraverser@Graph@@@Z
void Gfx::DisplayQuality::Push(Graph::PassiveTraverser *) const
{
    s_Stack->Push(this);
}

// 0x10110e90 ?Pop@DisplayQuality@Gfx@@UBEXPAVPassiveTraverser@Graph@@@Z
void Gfx::DisplayQuality::Pop(Graph::PassiveTraverser *) const
{
    s_Stack->Pop();
}

// 0x10110ea0 ?Write@DisplayQuality@Gfx@@UAEXPAVWriteTraverser@2@@Z
void Gfx::DisplayQuality::Write(Gfx::WriteTraverser *traverser)
{
    Text name("GfxDisplayQuality");
    writeText(name, traverser->GetStream());
    WriteInternals(traverser);
}

// 0x10110f20 ?ReadVis@DisplayQuality@Gfx@@UAEXPAVStream@Graph@@@Z
void Gfx::DisplayQuality::ReadVis(Graph::Stream *stream)
{
    Attribute::ReadVis(stream);
    ReadInternals(stream);
}

// 0x10110f40 ?Read@DisplayQuality@Gfx@@UAEXPAVStream@Graph@@@Z
void Gfx::DisplayQuality::Read(Graph::Stream *stream)
{
    Attribute::Read(stream);
    ReadInternals(stream);
}

// 0x10110f60 ?WriteInternals@DisplayQuality@Gfx@@IAEXPAVWriteTraverser@2@@Z
void Gfx::DisplayQuality::WriteInternals(Gfx::WriteTraverser *traverser)
{
    Attribute::WriteInternals(traverser);
    traverser->GetStream()->Write((short)m_Quality);
}

// 0x10110f80 ?ReadInternals@DisplayQuality@Gfx@@IAEXPAVStream@Graph@@@Z
void Gfx::DisplayQuality::ReadInternals(Graph::Stream *stream)
{
    short quality;
    stream->Read(&quality);
    m_Quality = quality;
}

// 0x10110de0 ??_GDisplayQuality@Gfx@@UAEPAXI@Z

// the run-time type of Gfx::DisplayQuality (values from the original's data section)
StdBase::TypeInfo const Gfx::DisplayQuality::s_TypeInfo = { "Gfx::DisplayQuality", &Gfx::Attribute::s_TypeInfo };

// static data (zero in the original, .bss)
int Gfx::DisplayQuality::s_InstanceCount = 0;
Gfx::AttributeStack *Gfx::DisplayQuality::s_Stack = 0;
