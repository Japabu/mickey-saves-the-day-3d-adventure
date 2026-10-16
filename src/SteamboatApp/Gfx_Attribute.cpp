#include "Gfx_Attribute.h"
#include "MathConstants.h"
#include "Graph_DataBase.h"
#include "Graph_FileStream.h"
#include "Gfx_WriteTraverser.h"
#include "VisMisc.h"
#include "StdBase.h"
#include <locale> // evidence: the original unit ends with the std::ctype<unsigned short>::id and std::collate<char>::id initializers

// 0x100e93a0 ?GetTypeInfo@Attribute@Gfx@@UBEPBUTypeInfo@StdBase@@XZ
StdBase::TypeInfo const * Gfx::Attribute::GetTypeInfo() const
{
    return &s_TypeInfo;
}

// 0x100e93b0 ?GetTypeName@Attribute@Gfx@@UBEPBDXZ
char const * Gfx::Attribute::GetTypeName() const
{
    return s_TypeInfo.typeName;
}

// 0x100e93c0 ?ReadVis@Attribute@Gfx@@UAEXPAVStream@Graph@@@Z
void Gfx::Attribute::ReadVis(Graph::Stream *stream)
{
    Graph::BasicNode *parent = Graph::DataBase::GetInstance()->GetCurrentParentNode();
    if (parent)
        parent->AddAttribute(this);
    ReadInternals(stream, 0);
}

// 0x100e93f0 ?Read@Attribute@Gfx@@UAEXPAVStream@Graph@@@Z
void Gfx::Attribute::Read(Graph::Stream *stream)
{
    int version;
    Graph::BasicNode *parent = Graph::DataBase::GetInstance()->GetCurrentParentNode();
    if (parent)
        parent->AddAttribute(this);
    stream->Read(&version);
    ReadInternals(stream, version);
}

// 0x100e9430 ??0Attribute@Gfx@@IAE@XZ
Gfx::Attribute::Attribute() : m_Ignore(0)
{
}

// 0x100e9470 ??1Attribute@Gfx@@MAE@XZ
Gfx::Attribute::~Attribute()
{
}

// 0x100e9480 ?WriteInternals@Attribute@Gfx@@IAEXPAVWriteTraverser@2@@Z
void Gfx::Attribute::WriteInternals(Gfx::WriteTraverser *traverser)
{
    traverser->GetStream()->Write(Version);
    writeText(GetName(), traverser->GetStream());
    traverser->GetStream()->Write(m_Ignore);
}

// 0x100e94c0 ?ReadInternals@Attribute@Gfx@@IAEXPAVStream@Graph@@H@Z
void Gfx::Attribute::ReadInternals(Graph::Stream *stream, int version)
{
    SetName(readText(stream));
    stream->Read(&m_Ignore);
}

int const Gfx::Attribute::Version = 0;

// 0x100e9450 ??_GAttribute@Gfx@@MAEPAXI@Z
// 0x100e92b0 _$E14
// 0x100e92c0 _$E13
// 0x100e92e0 _$E17
// 0x100e92f0 _$E16
// 0x100e9310 _$E20
// 0x100e9320 _$E19
// 0x100e9340 _$E23
// 0x100e9350 _$E22
// 0x100e9370 _$E26
// 0x100e9380 _$E25
// 0x100e9560 _$E61
// 0x100e9590 _$E57
// 0x100e95a0 _$E60
