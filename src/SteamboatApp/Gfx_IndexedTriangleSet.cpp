#pragma optimize("a", on) // evidence: GetAtomBoundingBox stores only the BoundingBox vftable (no-alias code)
#include "Gfx_IndexedTriangleSet.h"
#include "MathConstants.h"
#include "StdBase.h"
#include "Graph_Stream.h"
#include "Graph_FileStream.h"
#include "Gfx_WriteTraverser.h"
#include "Gfx_RenderTraverser.h"
#include "Gfx_VertexClusterAttribute.h"
#include "Gfx_PackedTriStripSet.h"
#include "Geodesic.h"

// The unit's own copy of the geodesic calculator's constant (Geodesic.cpp
// has another one).
const double DEG2RAD = 0.0174532925199433;

// 0x100e96a0 ?GetTypeInfo@IndexedTriangleSet@Gfx@@UBEPBUTypeInfo@StdBase@@XZ
StdBase::TypeInfo const * Gfx::IndexedTriangleSet::GetTypeInfo() const
{
    return &s_TypeInfo;
}

// 0x100e96b0 ?GetTypeName@IndexedTriangleSet@Gfx@@UBEPBDXZ
char const * Gfx::IndexedTriangleSet::GetTypeName() const
{
    return s_TypeInfo.typeName;
}

// 0x100e96c0 ??0IndexedTriangleSet@Gfx@@QAE@XZ
Gfx::IndexedTriangleSet::IndexedTriangleSet()
    : m_CullCW(0), m_nTris(0), m_EnvironmentMapping(false), DrawFunction(0)
{
}

// A geodesic sphere (class II icosahedron of frequency 2) around the
// sphere: unit normals, scaled positions, one triangle per face.
// TODO 0x100e98d0 ??0IndexedTriangleSet@Gfx@@QAE@ABVBoundingSphere@1@@Z: the original calls Vec<Vec3>::append(const T *, int) out of line for the normal (VC6 here inlines it, see NOTES) and allocates ebx/esi the other way round; same calls and stores otherwise
Gfx::IndexedTriangleSet::IndexedTriangleSet(const Gfx::BoundingSphere &sphere)
    : m_CullCW(0), m_nTris(0), m_EnvironmentMapping(false), DrawFunction(0)
{
    parameters setup = { 2, 2, 1, 0, "test", 1, 1, 0, 1, 0, 1.0 };
    Geodesic geodesic(&setup);
    long count = geodesic.output_type ? geodesic.face_quantity(geodesic.classtype, geodesic.polyhedron) : 0;
    long f;
    for (f = 0; f <= count; f++) {
        if (geodesic.polyhedron == 1)
            geodesic.icosa_sphere(f);
        else if (geodesic.polyhedron == 2)
            geodesic.octa_sphere(f);
        else if (geodesic.polyhedron == 3)
            geodesic.tetra_sphere(f);
        for (long v = 1; v <= geodesic.vertices; v++) {
            Vec3<float> normal((float)geodesic.clean_float(sin(DEG2RAD * geodesic.sph_face[v].phi) * cos(DEG2RAD * geodesic.sph_face[v].theta)),
                (float)geodesic.clean_float(sin(DEG2RAD * geodesic.sph_face[v].phi) * sin(DEG2RAD * geodesic.sph_face[v].theta)),
                (float)geodesic.clean_float(cos(DEG2RAD * geodesic.sph_face[v].phi)));
            m_Normals.append(&normal, 1);
            m_Coords.append(normal * (float)geodesic.sph_face[v].radius * sphere.GetRadius() + sphere.GetCenter());
            m_DiffuseColors.append(0xffffff01);
            m_SpecularColors.append(0xffffff01);
        }
    }
    int nTris = 0;
    for (f = 0; f <= count; f++) {
        for (long k = 1; k <= geodesic.faces; k++) {
            m_Indices.append(geodesic.face_list[k].v3 + geodesic.vertices * f - 1);
            m_Indices.append(geodesic.face_list[k].v2 + geodesic.vertices * f - 1);
            m_Indices.append(geodesic.face_list[k].v1 + geodesic.vertices * f - 1);
            m_Prim.append(nTris * 3);
            nTris++;
        }
    }
    m_Prim.append(nTris * 3);
    SetNormalBinding(BindVertex);
    InitOffsets();
}

// The box as twelve triangles over its eight corners (normals point from the
// centre to the corners).
// TODO 0x100e9fc0 ??0IndexedTriangleSet@Gfx@@QAE@ABVBoundingBox@1@@Z: the original appends the vertex data and the first 25 indices/primitives through out-of-line Vec::append(const T *, 1) calls with the value in a stack temporary, the rest with the inline append(const T &); written with append(const T &) throughout (same effect)
Gfx::IndexedTriangleSet::IndexedTriangleSet(const Gfx::BoundingBox &box)
    : m_CullCW(0), m_nTris(0), m_EnvironmentMapping(false), DrawFunction(0)
{
    for (int i = 0; i < 8; i++) {
        Vec3<float> corner = box.GetCorner(i);
        Vec3<float> normal = corner - box.GetCenter();
        m_Normals.append(&normal, 1);
        m_Coords.append(&corner, 1);
        unsigned long color = 0xffffff01;
        m_DiffuseColors.append(&color, 1);
        color = 0xffffff01;
        m_SpecularColors.append(&color, 1);
    }
    m_Indices.append(0);
    m_Indices.append(1);
    m_Indices.append(2);
    m_Prim.append(0);
    m_Indices.append(1);
    m_Indices.append(3);
    m_Indices.append(2);
    m_Prim.append(3);
    m_Indices.append(0);
    m_Indices.append(2);
    m_Indices.append(4);
    m_Prim.append(6);
    m_Indices.append(2);
    m_Indices.append(6);
    m_Indices.append(4);
    m_Prim.append(9);
    m_Indices.append(0);
    m_Indices.append(4);
    m_Indices.append(1);
    m_Prim.append(12);
    m_Indices.append(4);
    m_Indices.append(5);
    m_Indices.append(1);
    m_Prim.append(15);
    m_Indices.append(6);
    m_Indices.append(5);
    m_Indices.append(4);
    m_Prim.append(18);
    m_Indices.append(6);
    m_Indices.append(7);
    m_Indices.append(5);
    m_Prim.append(21);
    m_Indices.append(5);
    m_Indices.append(3);
    m_Indices.append(1);
    m_Prim.append(24);
    m_Indices.append(5);
    m_Indices.append(7);
    m_Indices.append(3);
    m_Prim.append(27);
    m_Indices.append(3);
    m_Indices.append(6);
    m_Indices.append(2);
    m_Prim.append(30);
    m_Indices.append(3);
    m_Indices.append(7);
    m_Indices.append(6);
    m_Prim.append(33);
    m_Prim.append(36);
    SetNormalBinding(BindVertex);
    InitOffsets();
}

#pragma optimize("a", off) // evidence: the PackedTriStripSet constructor keeps the plain store order
// 0x100e97b0 ??0IndexedTriangleSet@Gfx@@QAE@AAVPackedTriStripSet@1@@Z
Gfx::IndexedTriangleSet::IndexedTriangleSet(Gfx::PackedTriStripSet &set)
    : IndexedVertexShape(set), m_CullCW(set.GetCullCW()), m_nTris(0), m_EnvironmentMapping(false), DrawFunction(0)
{
    ConvertPackedTriStripSetToIndexedTriangleSet(&set);
}

#pragma optimize("a", on) // back to no-alias code

// 0x100ea860 ?Render@IndexedTriangleSet@Gfx@@UAE?AW4Response@NodeTraverser@Graph@@PAVRenderTraverser@2@@Z
Graph::NodeTraverser::Response Gfx::IndexedTriangleSet::Render(Gfx::RenderTraverser *traverser)
{
    VertexClusterAttribute::HandleClusterStack(this, traverser->GetFrameTime());
    return Graph::NodeTraverser::Continue;
}

// 0x100ea880 ?Write@IndexedTriangleSet@Gfx@@UAEXPAVWriteTraverser@2@@Z
void Gfx::IndexedTriangleSet::Write(Gfx::WriteTraverser *traverser)
{
    if (WriteHeaderInfo(traverser, (unsigned char *)"GfxIndexedTriangleSet"))
        WriteInternals(traverser);
}

// 0x100ea8b0 ?Read@IndexedTriangleSet@Gfx@@UAEXPAVStream@Graph@@@Z
void Gfx::IndexedTriangleSet::Read(Graph::Stream *stream)
{
    int version;
    IndexedVertexShape::Read(stream);
    stream->Read(&version);
    ReadInternals(stream, version);
}

// 0x100ea8e0 ?GetAtom@IndexedTriangleSet@Gfx@@UAEHHPAPAV?$Vec3@M@@@Z
int Gfx::IndexedTriangleSet::GetAtom(int atom, Vec3<float> **coords)
{
    int *prim = m_Prim;
    int *indices = m_Indices;
    int n = prim[atom + 1] - prim[atom];
    int *index = &indices[prim[atom]];
    for (int i = 0; i < n; i++)
        coords[i] = &m_Coords[*index++];
    return n;
}

// 0x100ea950 ?GetNumberOfAtoms@IndexedTriangleSet@Gfx@@UBEHXZ
int Gfx::IndexedTriangleSet::GetNumberOfAtoms() const
{
    return m_Prim.length() - 1;
}

// 0x100ea960 ?GetNumberOfAtomBoundingBoxes@IndexedTriangleSet@Gfx@@UBEHXZ
int Gfx::IndexedTriangleSet::GetNumberOfAtomBoundingBoxes() const
{
    return m_AtomBBoxes.length();
}

// 0x100ea970 ?GetAtomBoundingBox@IndexedTriangleSet@Gfx@@UAE?AVBoundingBox@2@H@Z
Gfx::BoundingBox Gfx::IndexedTriangleSet::GetAtomBoundingBox(int atom)
{
    return m_AtomBBoxes[atom];
}

#pragma optimize("a", off) // evidence: the destructor stores the BasicObject vftable before loading m_TriIndexStart's pointer
// 0x100ea9d0 ??1IndexedTriangleSet@Gfx@@MAE@XZ
Gfx::IndexedTriangleSet::~IndexedTriangleSet()
{
}

#pragma optimize("a", on) // back to no-alias code

// 0x100eaa70 ?WriteInternals@IndexedTriangleSet@Gfx@@IAEXPAVWriteTraverser@2@@Z
void Gfx::IndexedTriangleSet::WriteInternals(Gfx::WriteTraverser *traverser)
{
    IndexedVertexShape::WriteInternals(traverser);
    Graph::FileStream *stream = traverser->GetStream();
    stream->Write(Version);
    stream->Write(m_CullCW);
    stream->Write((short)m_EnvironmentMapping);
}

// 0x100eaac0 ?ReadInternals@IndexedTriangleSet@Gfx@@IAEXPAVStream@Graph@@H@Z
void Gfx::IndexedTriangleSet::ReadInternals(Graph::Stream *stream, int version)
{
    stream->Read(&m_CullCW);
    if (version > 0) {
        short mapping;
        stream->Read(&mapping);
        m_EnvironmentMapping = mapping != 0;
    }
    SetDrawFunction();
}

// 0x100eab10 ?Refresh@IndexedTriangleSet@Gfx@@MAEXAAVBasicRefreshTraverser@Graph@@@Z
void Gfx::IndexedTriangleSet::Refresh(Graph::BasicRefreshTraverser &traverser)
{
    int *prim = m_Prim;
    int *indices = m_Indices;
    m_AtomBBoxes.setLength(m_Prim.length() - 1);
    Gfx::BoundingBox *box = m_AtomBBoxes;
    for (int i = 0; i < m_Prim.length() - 1; i++) {
        for (int j = prim[i]; j < prim[i + 1]; j++)
            *box += m_Coords[indices[j]];
        box++;
    }
    SetDrawFunction();
    Node::Refresh(traverser);
}

// 0x100eac20 ?SetDrawFunction@IndexedTriangleSet@Gfx@@MAEXXZ
void Gfx::IndexedTriangleSet::SetDrawFunction()
{
}

// Copies the strip set's vertex data, unrolls the strips into triangles
// (every second one with the first two indices swapped) and merges vertices
// whose coordinates, normals and texture coordinates are equal. As in the
// original: the strip length table is never freed, only the last texture
// stage decides, and the vertex moved into slot j after a merge is not
// compared again.
// TODO 0x100eac30 ?ConvertPackedTriStripSetToIndexedTriangleSet@IndexedTriangleSet@Gfx@@IAEXPAVPackedTriStripSet@2@@Z: the original calls Vec::setLength out of line for the vertex arrays (Vec3, unsigned long, AVec instances emitted by this unit) and inlines it only for m_Prim/m_Indices; VC6 here inlines every call, so the code diverges from the first loop on (29% similar); the operations were taken from the disassembly in order
void Gfx::IndexedTriangleSet::ConvertPackedTriStripSetToIndexedTriangleSet(Gfx::PackedTriStripSet *set)
{
    m_Binding = set->m_Binding;
    InitOffsets(Version);
    int i;
    for (i = 0; i < set->m_Coords.length(); i++) {
        m_Coords.setLength(m_Coords.length() + 1);
        m_Coords[m_Coords.length() - 1] = set->m_Coords[i];
    }
    if (GetNormalBinding() != BindNone) {
        for (i = 0; i < set->m_Normals.length(); i++) {
            m_Normals.setLength(m_Normals.length() + 1);
            m_Normals[m_Normals.length() - 1] = set->m_Normals[i];
        }
    }
    if (GetColorBinding() != BindNone) {
        for (i = 0; i < set->m_DiffuseColors.length(); i++) {
            unsigned long color = set->m_DiffuseColors[i];
            m_DiffuseColors.setLength(m_DiffuseColors.length() + 1);
            m_DiffuseColors[m_DiffuseColors.length() - 1] = color;
        }
        for (i = 0; i < set->m_SpecularColors.length(); i++) {
            unsigned long color = set->m_SpecularColors[i];
            m_SpecularColors.setLength(m_SpecularColors.length() + 1);
            m_SpecularColors[m_SpecularColors.length() - 1] = color;
        }
    }
    if (GetTextureBinding() != BindNone) {
        m_TCoords.setLength(m_TCoords.length() + set->m_TCoords.length());
        for (int stage = 0; stage < set->m_TCoords.length(); stage++) {
            for (int j = 0; j < set->m_TCoords[stage].length(); j++) {
                m_TCoords[stage].setLength(m_TCoords[stage].length() + 1);
                m_TCoords[stage][m_TCoords[stage].length() - 1] = set->m_TCoords[stage][j];
            }
        }
    }

    int nStrips = set->m_Prim.length() - 1;
    IVec<int> prim(set->m_Prim);
    int *stripLength = new int[nStrips];
    int nVerts = m_Coords.length();
    for (i = 0; i < nStrips; i++) {
        stripLength[i] = prim[i + 1] - prim[i];
        m_nTris += stripLength[i] - 2;
    }
    m_Prim.setLength(m_nTris + 1);
    for (i = 0; i < m_nTris + 1; i++)
        m_Prim[i] = i * 3;
    m_Indices.setLength(m_nTris * 3);
    int nIndices = 0;
    for (int strip = 0; strip < nStrips; strip++) {
        int v = prim[strip];
        int end = prim[strip + 1];
        m_Indices[nIndices++] = v;
        m_Indices[nIndices++] = v + 1;
        m_Indices[nIndices++] = v + 2;
        for (v += 3; v < end; v++) {
            m_Indices[nIndices++] = v - 1;
            m_Indices[nIndices++] = v - 2;
            m_Indices[nIndices++] = v;
            v++;
            if (v >= end)
                break;
            m_Indices[nIndices++] = v - 2;
            m_Indices[nIndices++] = v - 1;
            m_Indices[nIndices++] = v;
        }
    }

    for (i = 0; i < nVerts; i++) {
        for (int j = i + 1; j < nVerts; j++) {
            int c;
            int sameCoord = 1;
            for (c = 0; c < 3; c++) {
                if (sameCoord && m_Coords[i][c] != m_Coords[j][c])
                    sameCoord = 0;
            }
            if (!sameCoord)
                continue;
            int sameNormal = 1;
            for (c = 0; c < 3; c++) {
                if (sameNormal && m_Normals[i][c] != m_Normals[j][c])
                    sameNormal = 0;
            }
            if (!sameNormal)
                continue;
            bool sameTexture = true;
            if (GetTextureBinding() != BindNone) {
                for (int stage = 0; stage < m_TCoords.length(); stage++) {
                    int same = 1;
                    for (c = 0; c < 3; c++) {
                        if (same && m_TCoords[stage][i][c] != m_TCoords[stage][j][c])
                            same = 0;
                    }
                    sameTexture = same ? true : false;
                }
            }
            if (sameTexture != true)
                continue;
            int n;
            for (n = 0; n < nIndices; n++) {
                if (m_Indices[n] == j)
                    m_Indices[n] = i;
            }
            m_Coords.remove(j, 1);
            if (GetNormalBinding() != BindNone)
                m_Normals.remove(j, 1);
            if (GetColorBinding() != BindNone) {
                m_DiffuseColors.remove(j, 1);
                m_SpecularColors.remove(j, 1);
            }
            if (GetTextureBinding() != BindNone) {
                for (int stage = 0; stage < m_TCoords.length(); stage++)
                    m_TCoords[stage].remove(j, 1);
            }
            for (n = 0; n < nIndices; n++) {
                if (m_Indices[n] > j)
                    m_Indices[n]--;
            }
            nVerts = m_Coords.length();
        }
    }
    m_nVerts = m_Coords.length();
}

// defined after the code: the original loads it from memory
// the run-time type of Gfx::IndexedTriangleSet (values from the original's data section)
StdBase::TypeInfo const Gfx::IndexedTriangleSet::s_TypeInfo = { "Gfx::IndexedTriangleSet", &Gfx::IndexedVertexShape::s_TypeInfo };
int const Gfx::IndexedTriangleSet::Version = 1;

// 0x100e9770 ??_GIndexedTriangleSet@Gfx@@MAEPAXI@Z
// 0x100e9790 ??1?$Array@H@@UAE@XZ
// 0x100e9880 ??1?$Array@VBoundingBox@Gfx@@@@UAE@XZ
// 0x100eb5c0 ??_G?$Array@H@@UAEPAXI@Z
// 0x100eb5e0 ??_G?$Array@VBoundingBox@Gfx@@@@UAEPAXI@Z
// 0x100ebaa0 ??1?$Vec@VBoundingBox@Gfx@@@@UAE@XZ
// 0x100eba80 ??_G?$Vec@VBoundingBox@Gfx@@@@UAEPAXI@Z
// TODO 0x100ebfa0 ?reallocMem@?$Vec@VBoundingBox@Gfx@@@@IAEXHHH@Z: the original builds the initElements temporary without the BoundingVolume vftable store and EH state
// 0x100ec220 ?initElements@?$Vec@VBoundingBox@Gfx@@@@IAEXHH@Z
// 0x100ec2f0 ??_EBoundingBox@Gfx@@UAEPAXI@Z
// 0x100ebaf0 ?reallocMem@?$Vec@K@@IAEXHHH@Z
// 0x100ec2e0 ??_F?$AVec@V?$Vec3@M@@@@QAEXXZ
// TODO 0x100eb980 ??0?$Vec@VBoundingBox@Gfx@@@@QAE@HM@Z: Vec.h constructor instance; the inlined reallocMem/initElements of BoundingBox differ (see reallocMem below)
// TODO 0x100ebba0 ?reallocMem@?$Vec@V?$AVec@V?$Vec3@M@@@@@@IAEXHHH@Z: Vec.h instance; the element copy (AVec operator=) and the EH states of the array new differ
// TODO 0x100ebea0 ?initElements@?$Vec@V?$AVec@V?$Vec3@M@@@@@@IAEXHH@Z: Vec.h instance; the zero AVec temporary is built differently
// TODO 0x100eb600 ??0?$AVec@H@@QAE@ABV0@@Z: AVec.h copy constructor instance, called by the Convert function in the original; not emitted here (inlined)
// TODO 0x100eb6c0 ??_G?$AVec@H@@UAEPAXI@Z: not emitted here (IVec's destructor is inlined)
// TODO 0x100eb6e0 ??1?$AVec@H@@UAE@XZ: not emitted here
// TODO 0x100eb700 ?setLength@?$Vec@V?$Vec3@M@@@@QAEXH@Z: Vec.h instance called out of line by the Convert function; VC6 here inlines it
// TODO 0x100eb800 ?setLength@?$Vec@K@@QAEXH@Z: as above
// TODO 0x100eb940 ?setLength@?$Vec@V?$AVec@V?$Vec3@M@@@@@@QAEXH@Z: as above
// TODO 0x100eb780 ?append@?$Vec@V?$Vec3@M@@@@QAEXPBV?$Vec3@M@@H@Z: Vec.h instance (append(const T *, int)); VC6 here inlines every call (see NOTES)
// TODO 0x100eb8a0 ?append@?$Vec@K@@QAEXPBKH@Z: as above
// 0x100e95b0 _$E47
// 0x100e95c0 _$E46
// 0x100e95e0 _$E50
// 0x100e95f0 _$E49
// 0x100e9610 _$E53
// 0x100e9620 _$E52
// 0x100e9640 _$E56
// 0x100e9650 _$E55
// 0x100e9670 _$E59
// 0x100e9680 _$E58
