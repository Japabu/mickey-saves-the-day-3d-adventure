#pragma optimize("a", on) // evidence: Refresh keeps m_Coords' pointer across the BoundingBox::operator+= calls and GetAtomBoundingBox stores only the BoundingBox vftable (no-alias code)
#include "Gfx_PackedTriStripSet.h"
#include "MathConstants.h"
#include "Gfx_RenderTraverser.h"
#include "Gfx_VertexClusterAttribute.h"
#include "Gfx_WriteTraverser.h"
#include "Graph_FileStream.h"
#include "Graph_Stream.h"

// 0x100f3680 ?GetTypeInfo@PackedTriStripSet@Gfx@@UBEPBUTypeInfo@StdBase@@XZ
StdBase::TypeInfo const * Gfx::PackedTriStripSet::GetTypeInfo() const
{
    return &s_TypeInfo;
}

// 0x100f3690 ?GetTypeName@PackedTriStripSet@Gfx@@UBEPBDXZ
char const * Gfx::PackedTriStripSet::GetTypeName() const
{
    return s_TypeInfo.typeName;
}

// 0x100f36a0 ??0PackedTriStripSet@Gfx@@QAE@XZ
Gfx::PackedTriStripSet::PackedTriStripSet()
    : m_CullCW(1), m_nVerts(0), m_nTris(0), m_nStrips(0), m_nIndices(0), m_EnvironmentMapping(false),
      DrawFunction(0)
{
}

// 0x100f3780 ?Render@PackedTriStripSet@Gfx@@UAE?AW4Response@NodeTraverser@Graph@@PAVRenderTraverser@2@@Z
Graph::NodeTraverser::Response Gfx::PackedTriStripSet::Render(Gfx::RenderTraverser *traverser)
{
    VertexClusterAttribute::HandleClusterStack(this, traverser->GetFrameTime());
    return Graph::NodeTraverser::Continue;
}

// 0x100f37a0 ?Write@PackedTriStripSet@Gfx@@UAEXPAVWriteTraverser@2@@Z
void Gfx::PackedTriStripSet::Write(Gfx::WriteTraverser *traverser)
{
    if (WriteHeaderInfo(traverser, (unsigned char *)"GfxPackedTriStripSet"))
        WriteInternals(traverser);
}

// 0x100f37d0 ?ReadVis@PackedTriStripSet@Gfx@@UAEXPAVStream@Graph@@@Z
void Gfx::PackedTriStripSet::ReadVis(Graph::Stream *stream)
{
    PackedVertexShape::ReadVis(stream);
    ReadInternals(stream, 0);
}

// 0x100f37f0 ?ReadVis2@PackedTriStripSet@Gfx@@UAEXPAVStream@Graph@@@Z
void Gfx::PackedTriStripSet::ReadVis2(Graph::Stream *stream)
{
    int version;
    PackedVertexShape::ReadVis(stream);
    stream->Read(&version);
    ReadInternals(stream, version);
}

// 0x100f3820 ?Read@PackedTriStripSet@Gfx@@UAEXPAVStream@Graph@@@Z
void Gfx::PackedTriStripSet::Read(Graph::Stream *stream)
{
    int version;
    PackedVertexShape::Read(stream);
    stream->Read(&version);
    ReadInternals(stream, version);
}

// TODO 0x100f3850 ?GetAtom@PackedTriStripSet@Gfx@@UAEHHPAPAV?$Vec3@M@@@Z: register allocation (the original keeps diff in edx and pushes the registers after the pointer tests)
int Gfx::PackedTriStripSet::GetAtom(int atom, Vec3<float> **coords)
{
    // binary search for the strip that holds triangle atom
    int *prim = m_Prim;
    int *start = m_TriIndexStart;
    int hi = m_TriIndexStart.length() - 1;
    int mid = 0;
    int lo = 0;
    int diff = 0;
    do {
        if (diff < 0) {
            hi = mid - 1;
            if (hi <= lo)
                hi = lo;
        } else if (diff > 0) {
            lo = mid + 1;
            if (lo >= hi)
                lo = hi;
        }
        mid = (lo + hi) >> 1;
        diff = atom - start[mid];
    } while (lo < hi && diff != 0);
    int strip = (diff < 0) ? mid - 1 : mid;
    int first = prim[strip] - start[strip] + atom;
    for (int i = 0; i < 3; i++)
        coords[i] = &m_Coords[first + i];
    return 3;
}

// 0x100f3910 ?GetNumberOfAtoms@PackedTriStripSet@Gfx@@UBEHXZ
int Gfx::PackedTriStripSet::GetNumberOfAtoms() const
{
    return m_AtomBBoxes.length();
}

// 0x100f3920 ?GetNumberOfAtomBoundingBoxes@PackedTriStripSet@Gfx@@UBEHXZ
int Gfx::PackedTriStripSet::GetNumberOfAtomBoundingBoxes() const
{
    return m_AtomBBoxes.length();
}

// 0x100f3930 ?GetAtomBoundingBox@PackedTriStripSet@Gfx@@UAE?AVBoundingBox@2@H@Z
Gfx::BoundingBox Gfx::PackedTriStripSet::GetAtomBoundingBox(int atom)
{
    return m_AtomBBoxes[atom];
}

#pragma optimize("a", off) // evidence: the destructor stores the BasicObject vftable before loading m_TriIndexStart's pointer
// 0x100f3990 ??1PackedTriStripSet@Gfx@@MAE@XZ
Gfx::PackedTriStripSet::~PackedTriStripSet()
{
}
#pragma optimize("a", on) // back to no-alias code

// 0x100f3a30 ?WriteInternals@PackedTriStripSet@Gfx@@IAEXPAVWriteTraverser@2@@Z
void Gfx::PackedTriStripSet::WriteInternals(Gfx::WriteTraverser *traverser)
{
    PackedVertexShape::WriteInternals(traverser);
    Graph::FileStream *stream = traverser->GetStream();
    stream->Write(Version);
    stream->Write(m_CullCW);
    stream->Write((short)m_EnvironmentMapping);
}

// TODO 0x100f3a80 ?ReadInternals@PackedTriStripSet@Gfx@@IAEXPAVStream@Graph@@H@Z: register allocation (stream in edi, this in esi in the original)
void Gfx::PackedTriStripSet::ReadInternals(Graph::Stream *stream, int version)
{
    if (version > 0) {
        stream->Read(&m_CullCW);
        if (version > 1) {
            short mapping;
            stream->Read(&mapping);
            m_EnvironmentMapping = mapping != 0;
        }
    }
    SetDrawFunction();
}

// TODO 0x100f3ad0 ?Refresh@PackedTriStripSet@Gfx@@MAEXAAVBasicRefreshTraverser@Graph@@@Z: the original turns the strip loop into a counted loop with the coordinate pointers in registers
void Gfx::PackedTriStripSet::Refresh(Graph::BasicRefreshTraverser &traverser)
{
    int *prim = m_Prim;
    m_AtomBBoxes.setLength(m_Prim[m_Prim.length() - 1] - 2 * (m_Prim.length() - 1));
    Gfx::BoundingBox *box = m_AtomBBoxes;
    int i;
    for (i = 0; i < m_Prim.length() - 1; i++) {
        Vec3<float> *a = &m_Coords[prim[i]];
        Vec3<float> *b = &m_Coords[prim[i] + 1];
        for (int j = prim[i] + 2; j < prim[i + 1]; j++) {
            Vec3<float> *c = &m_Coords[j];
            *box += *a;
            *box += *b;
            *box += *c;
            box++;
            a = b;
            b = c;
        }
    }
    m_TriIndexStart.setLength(m_Prim.length());
    int *start = m_TriIndexStart;
    int count = 0;
    for (i = 0; i < m_Prim.length() - 1; i++) {
        start[i] = count;
        count += prim[i + 1] - prim[i] - 2;
    }
    start[m_Prim.length() - 1] = count;
    SetDrawFunction();
    Node::Refresh(traverser);
}

// 0x100f3c80 ?SetDrawFunction@PackedTriStripSet@Gfx@@MAEXXZ
void Gfx::PackedTriStripSet::SetDrawFunction()
{
}

// defined after the code: the original loads it from memory
// the run-time type of Gfx::PackedTriStripSet (values from the original's data section)
StdBase::TypeInfo const Gfx::PackedTriStripSet::s_TypeInfo = { "Gfx::PackedTriStripSet", &Gfx::PackedVertexShape::s_TypeInfo };
int const Gfx::PackedTriStripSet::Version = 2;

// 0x100f3760 ??_GPackedTriStripSet@Gfx@@MAEPAXI@Z
// 0x100f3c90 ??1BoundingVolume@Gfx@@UAE@XZ
// 0x100f3590 _$E47
// 0x100f35a0 _$E46
// 0x100f35c0 _$E50
// 0x100f35d0 _$E49
// 0x100f35f0 _$E53
// 0x100f3600 _$E52
// 0x100f3620 _$E56
// 0x100f3630 _$E55
// 0x100f3650 _$E59
// 0x100f3660 _$E58
// 0x100d68b0 ?GetNumVerts@PackedTriStripSet@Gfx@@UBEHXZ
// 0x100d68c0 ?GetNumTris@PackedTriStripSet@Gfx@@UBEHXZ
// 0x100d68d0 ?GetNumStrips@PackedTriStripSet@Gfx@@UBEHXZ
// 0x100d68e0 ?GetNumIndices@PackedTriStripSet@Gfx@@UBEHXZ
