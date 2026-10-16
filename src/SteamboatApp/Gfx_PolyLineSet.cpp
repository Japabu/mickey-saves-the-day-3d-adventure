#pragma optimize("a", on) // evidence: GetAtomBoundingBox stores only the BoundingBox vftable (no-alias code), as in IndexedTriangleSet
#include "Gfx_PolyLineSet.h"
#include "MathConstants.h"
#include "Gfx_RenderTraverser.h"
#include "Gfx_WriteTraverser.h"
#include "Graph_FileStream.h"
#include "Graph_NodeTraverser.h"
#include "Graph_Stream.h"
#include "StdBase.h"

// 0x100fb860 ?GetTypeInfo@PolyLineSet@Gfx@@UBEPBUTypeInfo@StdBase@@XZ
StdBase::TypeInfo const * Gfx::PolyLineSet::GetTypeInfo() const
{
    return &s_TypeInfo;
}

// 0x100fb870 ?GetTypeName@PolyLineSet@Gfx@@UBEPBDXZ
char const * Gfx::PolyLineSet::GetTypeName() const
{
    return s_TypeInfo.typeName;
}

// 0x100fb880 ??0PolyLineSet@Gfx@@QAE@XZ
Gfx::PolyLineSet::PolyLineSet()
{
}

// 0x100fb910 ?SetDirty@PolyLineSet@Gfx@@UAEXXZ
void Gfx::PolyLineSet::SetDirty()
{
}

// 0x100fb940 ?Render@PolyLineSet@Gfx@@UAE?AW4Response@NodeTraverser@Graph@@PAVRenderTraverser@2@@Z
Graph::NodeTraverser::Response Gfx::PolyLineSet::Render(Gfx::RenderTraverser *)
{
    return Graph::NodeTraverser::Continue;
}

// 0x100fb950 ?Write@PolyLineSet@Gfx@@UAEXPAVWriteTraverser@2@@Z
void Gfx::PolyLineSet::Write(Gfx::WriteTraverser *traverser)
{
    if (WriteHeaderInfo(traverser, (unsigned char *)"GfxPolyLineSet"))
        WriteInternals(traverser);
}

// 0x100fb980 ?ReadVis@PolyLineSet@Gfx@@UAEXPAVStream@Graph@@@Z
void Gfx::PolyLineSet::ReadVis(Graph::Stream *stream)
{
    PackedVertexShape::ReadVis(stream);
    ReadInternals(stream, 0);
}

// 0x100fb9a0 ?ReadVis2@PolyLineSet@Gfx@@UAEXPAVStream@Graph@@@Z
void Gfx::PolyLineSet::ReadVis2(Graph::Stream *stream)
{
    int version;
    PackedVertexShape::ReadVis(stream);
    stream->Read(&version);
    ReadInternals(stream, version);
}

// 0x100fb9d0 ?Read@PolyLineSet@Gfx@@UAEXPAVStream@Graph@@@Z
void Gfx::PolyLineSet::Read(Graph::Stream *stream)
{
    int version;
    PackedVertexShape::Read(stream);
    stream->Read(&version);
    ReadInternals(stream, version);
}

// 0x100fba00 ?GetAtom@PolyLineSet@Gfx@@UAEHHPAPAV?$Vec3@M@@@Z
int Gfx::PolyLineSet::GetAtom(int, Vec3<float> **)
{
    return 3;
}

// 0x100fba10 ?GetNumberOfAtoms@PolyLineSet@Gfx@@UBEHXZ
int Gfx::PolyLineSet::GetNumberOfAtoms() const
{
    return m_AtomBBoxes.length();
}

// 0x100fba20 ?GetNumberOfAtomBoundingBoxes@PolyLineSet@Gfx@@UBEHXZ
int Gfx::PolyLineSet::GetNumberOfAtomBoundingBoxes() const
{
    return m_AtomBBoxes.length();
}

// 0x100fba30 ?GetAtomBoundingBox@PolyLineSet@Gfx@@UAE?AVBoundingBox@2@H@Z
Gfx::BoundingBox Gfx::PolyLineSet::GetAtomBoundingBox(int atom)
{
    return m_AtomBBoxes[atom];
}

#pragma optimize("a", off) // evidence: the destructor stores the BasicObject vftable before loading m_LineIndexStart's pointer
// 0x100fba90 ??1PolyLineSet@Gfx@@MAE@XZ
Gfx::PolyLineSet::~PolyLineSet()
{
}
#pragma optimize("a", on) // back to no-alias code

// 0x100fbb30 ?WriteInternals@PolyLineSet@Gfx@@IAEXPAVWriteTraverser@2@@Z
void Gfx::PolyLineSet::WriteInternals(Gfx::WriteTraverser *traverser)
{
    PackedVertexShape::WriteInternals(traverser);
    traverser->GetStream()->Write(Version);
}

// 0x100fbb50 ?ReadInternals@PolyLineSet@Gfx@@IAEXPAVStream@Graph@@H@Z
void Gfx::PolyLineSet::ReadInternals(Graph::Stream *, int)
{
    SetDrawFunction();
}

// 0x100fbb60 ?Refresh@PolyLineSet@Gfx@@MAEXAAVBasicRefreshTraverser@Graph@@@Z
void Gfx::PolyLineSet::Refresh(Graph::BasicRefreshTraverser &traverser)
{
    SetDrawFunction();
    Node::Refresh(traverser);
}

// 0x100fbb80 ?SetDrawFunction@PolyLineSet@Gfx@@MAEXXZ
void Gfx::PolyLineSet::SetDrawFunction()
{
}

// defined after the code: the original loads it from memory
int const Gfx::PolyLineSet::Version = 0;

// 0x100fb920 ??_GPolyLineSet@Gfx@@MAEPAXI@Z
// 0x100fb770 _$E47
// 0x100fb780 _$E46
// 0x100fb7a0 _$E50
// 0x100fb7b0 _$E49
// 0x100fb7d0 _$E53
// 0x100fb7e0 _$E52
// 0x100fb800 _$E56
// 0x100fb810 _$E55
// 0x100fb830 _$E59
// 0x100fb840 _$E58