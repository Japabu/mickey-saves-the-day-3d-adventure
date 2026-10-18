#pragma optimize("a", on) // evidence: AppendIndices/AppendPrims and ReadInternals keep members in registers across stores and turn the clear loops into rep stosd (no-alias code)
#include "Gfx_IndexedVertexShape.h"
#include "MathConstants.h"
#include "StdBase.h"
#include "Graph_Stream.h"
#include "Graph_FileStream.h"
#include "Gfx_WriteTraverser.h"
#include "VisMisc.h"
#include "Gfx_Camera.h"
#include "Gfx_CurrentTransformationMatrix.h"
#include <math.h>

// 0x100f9120 ?GetTypeInfo@IndexedVertexShape@Gfx@@UBEPBUTypeInfo@StdBase@@XZ
StdBase::TypeInfo const * Gfx::IndexedVertexShape::GetTypeInfo() const
{
    return &s_TypeInfo;
}

// 0x100f9130 ?GetTypeName@IndexedVertexShape@Gfx@@UBEPBDXZ
char const * Gfx::IndexedVertexShape::GetTypeName() const
{
    return s_TypeInfo.typeName;
}

// 0x100f9140 ?Write@IndexedVertexShape@Gfx@@UAEXPAVWriteTraverser@2@@Z
void Gfx::IndexedVertexShape::Write(Gfx::WriteTraverser *traverser)
{
    unsigned char header[22] = "GfxIndexedVertexShape";
    if (WriteHeaderInfo(traverser, header) <= -1)
        WriteInternals(traverser);
}

// 0x100f9190 ?Read@IndexedVertexShape@Gfx@@UAEXPAVStream@Graph@@@Z
void Gfx::IndexedVertexShape::Read(Graph::Stream *stream)
{
    int version;
    Shape::Read(stream);
    stream->Read(&version);
    ReadInternals(stream, version);
}

// 0x100f91c0 ?SetDrawFunction@IndexedVertexShape@Gfx@@UAEXXZ
void Gfx::IndexedVertexShape::SetDrawFunction()
{
}

// 0x100f91d0 ?AppendIndices@IndexedVertexShape@Gfx@@QAEXPBHH_N@Z
void Gfx::IndexedVertexShape::AppendIndices(int const *indices, int n, bool refresh)
{
    m_Indices.setLength(m_Indices.length() + n);
    int start = m_Indices.length() - n;
    for (int i = 0; i < n; i++)
        m_Indices[start + i] = indices[i];
    if (refresh)
        ScheduleRefresh(false);
}

// 0x100f92a0 ?AppendPrims@IndexedVertexShape@Gfx@@QAEXPBHH_N@Z
void Gfx::IndexedVertexShape::AppendPrims(int const *prims, int n, bool refresh)
{
    m_Prim.setLength(m_Prim.length() + n);
    int start = m_Prim.length() - n;
    for (int i = 0; i < n; i++)
        m_Prim[start + i] = prims[i];
    if (refresh)
        ScheduleRefresh(false);
}

// 0x100f9370 ??0IndexedVertexShape@Gfx@@IAE@XZ
Gfx::IndexedVertexShape::IndexedVertexShape()
    : m_Stride(0), m_nVerts(0), m_NormalOffset(0), m_ColorOffset(0), m_TextureOffset(0),
      m_Binding(0x421), m_CoordStride(3), m_NormalStride(3), m_ColorStride(2), m_TextureStride(3)
{
    S.identity();
    S[0][0] = -1.0f;
    S[1][1] = -1.0f;
    S[3][0] = 0.0f;
    S[3][1] = 0.0f;
    S[3][2] = -1.0f;
    P.identity();
    P[2][3] = 1.0f;
    P[3][3] = 0.0f;
    ST.identity();
    ST[0][0] = 0.5f;
    ST[1][1] = 0.5f;
    ST[3][0] = 0.5f;
    ST[3][1] = 0.5f;
}

// TODO 0x100f9760 ??0IndexedVertexShape@Gfx@@IAE@ABVShape@1@@Z: (80% similar) the original records EH states (and keeps the member addresses) for m_SpecularColors and m_TCoords; VC6 here constructs them from esi without states
Gfx::IndexedVertexShape::IndexedVertexShape(const Gfx::Shape &shape)
    : Shape(shape), m_Stride(0), m_nVerts(0), m_NormalOffset(0), m_ColorOffset(0), m_TextureOffset(0),
      m_Binding(0x421), m_CoordStride(3), m_NormalStride(3), m_ColorStride(2), m_TextureStride(3)
{
}

#pragma optimize("a", off) // evidence: the destructor stores each BasicObject vftable before loading the next member's pointer
// 0x100f99c0 ??1IndexedVertexShape@Gfx@@MAE@XZ
Gfx::IndexedVertexShape::~IndexedVertexShape()
{
}

#pragma optimize("a", on) // back to no-alias code

// 0x100f9b80 ?WriteInternals@IndexedVertexShape@Gfx@@IAEXPAVWriteTraverser@2@@Z
void Gfx::IndexedVertexShape::WriteInternals(Gfx::WriteTraverser *traverser)
{
    Shape::WriteInternals(traverser);
    Graph::FileStream *stream = traverser->GetStream();
    stream->Write(Version);
    stream->Write(m_Binding);
    writeVeci(m_Prim, stream);
    writeVeci(m_Indices, stream);
    writeVVec3f(m_Coords, stream);
    if (GetNormalBinding() != BindNone)
        writeVVec3f(m_Normals, stream);
    if (GetColorBinding() != BindNone)
        writeVecdw(m_DiffuseColors, stream);
    if (GetColorBinding() != BindNone)
        writeVecdw(m_SpecularColors, stream);
    if (GetTextureBinding() != BindNone)
        writeVVVec3f(m_TCoords, stream);
}

// 0x100f9ca0 ?ReadInternals@IndexedVertexShape@Gfx@@IAEXPAVStream@Graph@@H@Z
void Gfx::IndexedVertexShape::ReadInternals(Graph::Stream *stream, int version)
{
    stream->Read(&m_Binding);
    readVeci(m_Prim, stream);
    readVeci(m_Indices, stream);
    if (version < 3) {
        readVecf(m_CList, stream);
    } else if (version < 4) {
        readVecf(m_Coordsv3, stream);
        m_nVerts = m_Coordsv3.length() / m_CoordStride;
        if (GetNormalBinding() != BindNone)
            readVecf(m_Normalsv3, stream);
        if (GetColorBinding() != BindNone)
            readVecdw(m_Colorsv3, stream);
        if (GetTextureBinding() != BindNone) {
            m_TCoordsv3.setLength(1);
            readVecf(m_TCoordsv3[0], stream);
        }
    } else {
        readVVec3f(m_Coords, stream);
        m_nVerts = m_Coords.length();
        if (GetNormalBinding() != BindNone)
            readVVec3f(m_Normals, stream);
        if (GetColorBinding() != BindNone) {
            readVecdw(m_DiffuseColors, stream);
            readVecdw(m_SpecularColors, stream);
        }
        if (GetTextureBinding() != BindNone)
            readVVVec3f(m_TCoords, stream);
    }
    InitOffsets(version);
    if (version < 1 && GetColorBinding() != BindNone) {
        // version 0 stored colours without alpha: spread the vertices out to the new stride
        int n = m_CList.length() / (m_Stride - 1);
        m_CList.verify(m_Stride * n - 1);
        for (int i = n - 1; i >= 0; i--) {
            int from = (m_Stride - 1) * i;
            int to = m_Stride * i;
            if (GetTextureBinding() != BindNone) {
                float *src = &m_CList[from + m_TextureOffset - 1];
                float *dst = &m_CList[to + m_TextureOffset];
                dst[2] = src[2];
                dst[1] = src[1];
                dst[0] = src[0];
            }
            if (GetNormalBinding() != BindNone) {
                float *src = &m_CList[from + m_NormalOffset - 1];
                float *dst = &m_CList[to + m_NormalOffset];
                dst[2] = src[2];
                dst[1] = src[1];
                dst[0] = src[0];
            }
            float *src = &m_CList[from + m_ColorOffset];
            float *dst = &m_CList[to + m_ColorOffset];
            dst[2] = src[2];
            dst[1] = src[1];
            dst[0] = src[0];
            src = &m_CList[from];
            dst = &m_CList[to];
            dst[2] = src[2];
            dst[1] = src[1];
            dst[0] = src[0];
            m_CList[to + m_ColorOffset + 3] = 1.0f;
        }
    }
    if (version < 2)
        PackCListColorRef();
    if (version < 3)
        SeparateVertexInfo();
    if (version == 3)
        SeparateVertexInfoV3();
    if (GetNormalBinding() != BindNone)
        NormalizeNormals();
    ScheduleRefresh(false);
}

#pragma optimize("a", off) // evidence: NormalizeNormals reloads m_Normals' length after each normalize
// 0x100fa0a0 ?NormalizeNormals@IndexedVertexShape@Gfx@@QAEXXZ
void Gfx::IndexedVertexShape::NormalizeNormals()
{
    if (GetNormalBinding() != BindNone) {
        for (int i = 0; i < m_Normals.length(); i++)
            m_Normals[i].normalize();
    }
}

#pragma optimize("a", on) // back to no-alias code

// 0x100fa130 ?PackCListColorRef@IndexedVertexShape@Gfx@@IAEXXZ
void Gfx::IndexedVertexShape::PackCListColorRef()
{
    // version 1 stored the colour as four floats: pack it into a diffuse and a specular DWORD
    if (GetColorBinding() != BindNone) {
        int n = m_CList.length() / m_Stride;
        for (int i = 0; i < n; i++) {
            int k = i * m_Stride + m_ColorOffset;
            int color = (int)(m_CList[k + 3] * 255.0f);
            color = (color << 8) | (int)(m_CList[k] * 255.0f);
            color = (color << 8) | (int)(m_CList[k + 1] * 255.0f);
            color = (color << 8) | (int)(m_CList[k + 2] * 255.0f);
            *(unsigned long *)&m_CList[k] = color;
            *(unsigned long *)&m_CList[k + 1] = 0xff000000;
        }
        for (int j = m_ColorOffset; j < m_CList.length() - 1; j += m_Stride - 2)
            m_CList.remove(j + 2, 2);
        m_Stride -= 2;
        if (m_NormalOffset > m_ColorOffset)
            m_NormalOffset -= 2;
        if (m_TextureOffset > m_ColorOffset)
            m_TextureOffset -= 2;
    }
    m_nVerts = m_CList.length() / m_Stride;
}

// 0x100fa320 ?UnPackCListColorRef@IndexedVertexShape@Gfx@@IAEXXZ
void Gfx::IndexedVertexShape::UnPackCListColorRef()
{
}

// 0x100fa330 ?SeparateVertexInfoV3@IndexedVertexShape@Gfx@@IAEXXZ
void Gfx::IndexedVertexShape::SeparateVertexInfoV3()
{
    // version 3 files keep each vertex component in a list of its own
    m_nVerts = m_Coordsv3.length() / m_CoordStride;
    m_Coords.setLength(m_Coordsv3.length() / m_CoordStride);
    if (GetNormalBinding() != BindNone)
        m_Normals.setLength(m_Normalsv3.length() / m_NormalStride);
    if (GetColorBinding() != BindNone) {
        m_DiffuseColors.setLength(m_Colorsv3.length() / m_ColorStride);
        m_SpecularColors.setLength(m_Colorsv3.length() / m_ColorStride);
    }
    if (GetTextureBinding() != BindNone) {
        m_TCoords.setLength(m_TCoordsv3.length());
        m_TCoords[0].setLength(m_TCoordsv3[0].length() / m_TextureStride);
    }
    int coord = 0, normal = 0, color = 0, texture = 0;
    int c = 0;
    for (int i = 0; i < m_Coordsv3.length(); i += m_CoordStride, c += m_ColorStride) {
        m_Coords[coord].e[0] = m_Coordsv3[i];
        m_Coords[coord].e[1] = m_Coordsv3[i + 1];
        m_Coords[coord].e[2] = m_Coordsv3[i + 2];
        coord++;
        if (GetColorBinding() != BindNone) {
            m_DiffuseColors[color] = m_Colorsv3[c];
            m_SpecularColors[color] = m_Colorsv3[c + 1];
            color++;
        }
        if (GetNormalBinding() != BindNone) {
            m_Normals[normal].e[0] = m_Normalsv3[i];
            m_Normals[normal].e[1] = m_Normalsv3[i + 1];
            m_Normals[normal].e[2] = m_Normalsv3[i + 2];
            normal++;
        }
        if (GetTextureBinding() != BindNone) {
            m_TCoords[0][texture].e[0] = m_TCoordsv3[0][i];
            m_TCoords[0][texture].e[1] = m_TCoordsv3[0][i + 1];
            m_TCoords[0][texture].e[2] = m_TCoordsv3[0][i + 2];
            texture++;
        }
    }
}

// TODO 0x100fa720 ?SeparateVertexInfo@IndexedVertexShape@Gfx@@IAEXXZ: (75% similar) same operations; the original walks the coordinates with pointers and keeps i in ebx (register allocation of the copy loop)
void Gfx::IndexedVertexShape::SeparateVertexInfo()
{
    m_nVerts = m_CList.length() / m_Stride;
    m_Coords.setLength(m_CList.length() / m_Stride);
    if (GetNormalBinding() != BindNone)
        m_Normals.setLength(m_CList.length() / m_Stride);
    if (GetColorBinding() != BindNone) {
        m_DiffuseColors.setLength(m_CList.length() / m_Stride);
        m_SpecularColors.setLength(m_CList.length() / m_Stride);
    }
    if (GetTextureBinding() != BindNone) {
        m_TCoords.setLength(1);
        m_TCoords[0].setLength(m_CList.length() / m_Stride);
    }
    int n = 0;
    for (int i = 0; i < m_CList.length(); i += m_Stride) {
        m_Coords[n][0] = m_CList[i];
        m_Coords[n][1] = m_CList[i + 1];
        m_Coords[n][2] = m_CList[i + 2];
        if (GetColorBinding() != BindNone) {
            m_DiffuseColors[n] = *(unsigned long *)&m_CList[i + m_ColorOffset];
            m_SpecularColors[n] = *(unsigned long *)&m_CList[i + m_ColorOffset + 1];
        }
        if (GetNormalBinding() != BindNone) {
            m_Normals[n][0] = m_CList[i + m_NormalOffset];
            m_Normals[n][1] = m_CList[i + m_NormalOffset + 1];
            m_Normals[n][2] = m_CList[i + m_NormalOffset + 2];
        }
        if (GetTextureBinding() != BindNone) {
            m_TCoords[0][n][0] = m_CList[i + m_TextureOffset];
            m_TCoords[0][n][1] = m_CList[i + m_TextureOffset + 1];
            m_TCoords[0][n][2] = m_CList[i + m_TextureOffset + 2];
        }
        n++;
    }
    m_CList.setLength(0);
}

// TODO 0x100fab40 ?InitOffsets@IndexedVertexShape@Gfx@@IAEXH@Z: (84% similar) the original loads the constant 3 into esi once before the version test, VC6 here in each branch
void Gfx::IndexedVertexShape::InitOffsets(int version)
{
    if (version < 2) {
        m_NormalOffset = m_ColorOffset = m_TextureOffset = 0;
        m_Stride = 3;
        if (GetColorBinding() != BindNone) {
            m_ColorOffset = m_Stride;
            m_Stride += 4;
        }
        if (GetNormalBinding() != BindNone) {
            m_NormalOffset = m_Stride;
            m_Stride += 3;
        }
        if (GetTextureBinding() != BindNone) {
            m_TextureOffset = m_Stride;
            m_Stride += 3;
        }
    } else {
        m_NormalOffset = m_ColorOffset = m_TextureOffset = 0;
        m_Stride = m_CoordStride = 3;
        if (GetColorBinding() != BindNone) {
            m_ColorStride = 2;
            m_ColorOffset = m_Stride;
            m_Stride += m_ColorStride;
        }
        if (GetNormalBinding() != BindNone) {
            m_NormalStride = 3;
            m_NormalOffset = m_Stride;
            m_Stride += m_NormalStride;
        }
        if (GetTextureBinding() != BindNone) {
            m_TextureStride = 3;
            m_TextureOffset = m_Stride;
            m_Stride += m_TextureStride;
        }
    }
}

// 0x100facc0 ?SetBindings@IndexedVertexShape@Gfx@@QAEXHW4Bind@12@00@Z
void Gfx::IndexedVertexShape::SetBindings(int nverts, Bind normal, Bind color, Bind texture)
{
    m_Binding = (((((m_Binding & ~NormalMask) | (normal << NormalShift)) & ~ColorMask) | (color << ColorShift))
                 & ~TextureMask) | (texture << TextureShift);
    InitOffsets(Version);
    int length = m_CList.length();
    m_CList.verify(m_Stride * nverts - 1);
    m_CList.setLength(length);
    m_nVerts = nverts;
}

// TODO 0x100faed0 ?Clear@IndexedVertexShape@Gfx@@QAEXXZ: (68% similar) inline budget: the original inlines the int initElements only, VC6 here also the first Vec3 one
void Gfx::IndexedVertexShape::Clear()
{
    m_Indices.setLength(0);
    m_Prim.setLength(0);
    m_Coords.setLength(0);
    m_Normals.setLength(0);
    m_DiffuseColors.setLength(0);
    m_SpecularColors.setLength(0);
    for (int i = 0; i < m_TCoords.length(); i++)
        m_TCoords[i].setLength(0);
    m_TCoords.setLength(0);
}

// the run-time type of Gfx::IndexedVertexShape (values from the original's data section)
StdBase::TypeInfo const Gfx::IndexedVertexShape::s_TypeInfo = { "Gfx::IndexedVertexShape", &Gfx::Shape::s_TypeInfo };
// defined after the code: the original loads them from memory
short const Gfx::IndexedVertexShape::NormalMask = 0x001f;
short const Gfx::IndexedVertexShape::ColorMask = 0x03e0;
short const Gfx::IndexedVertexShape::TextureMask = 0x7c00;
short const Gfx::IndexedVertexShape::NormalShift = 0;
short const Gfx::IndexedVertexShape::ColorShift = 5;
short const Gfx::IndexedVertexShape::TextureShift = 10;
int const Gfx::IndexedVertexShape::Version = 4;

// 0x100f9740 ??_GIndexedVertexShape@Gfx@@MAEPAXI@Z
// 0x100f9030 _$E43
// 0x100f9040 _$E42
// 0x100f9060 _$E46
// 0x100f9070 _$E45
// 0x100f9090 _$E49
// 0x100f90a0 _$E48
// 0x100f90c0 _$E52
// 0x100f90d0 _$E51
// 0x100f90f0 _$E55
// 0x100f9100 _$E54

// TODO 0x100fb080 ?ApplySphereMap@IndexedVertexShape@Gfx@@IAEXPAVCamera@2@H@Z: 47% - inline budget: the original calls the Vec3 operators (-, * float, +, dot, mag2, multScal), multVec3PointLeftAffine (GetWorldFrom) and multVector3Right out of line and sums the point transform from the translation down; same operations and order of the vertex loop
void Gfx::IndexedVertexShape::ApplySphereMap(Gfx::Camera *camera, int stage)
{
    // texture coordinates from the eye vector reflected about each world-space normal
    const TMatrix4<float> &model = Gfx::CurrentTransformationMatrix::GetInstance()->GetModelMatrix();
    const TMatrix4<float> &matrix = Gfx::CurrentTransformationMatrix::GetInstance()->GetMatrix();
    for (int i = 0; i < m_Normals.length(); i++) {
        Vec3<float> normal = model.multVector3Left(m_Normals[i]);
        Vec4<float> p = model.multVec3PointLeft(m_Coords[i]);
        Vec3<float> point;
        if (p.e[3] != 1.0f && p.e[3] != 0.0f) {
            point.e[0] = p.e[0] / p.e[3];
            point.e[1] = p.e[1] / p.e[3];
            point.e[2] = p.e[2] / p.e[3];
        } else {
            point.e[0] = p.e[0];
            point.e[1] = p.e[1];
            point.e[2] = p.e[2];
        }
        Vec3<float> eye = point - camera->GetWorldFrom();
        eye.multScal(1.0f / (float)sqrt(eye.mag2()));
        Vec3<float> reflected = eye + normal * (2.0f * normal.dot(eye * -1.0f));
        Vec3<float> t = matrix.multVector3Right(reflected);
        t.e[0] = (t.e[0] + 1.0f) * 0.5f;
        t.e[1] = (t.e[1] + 1.0f) * 0.5f;
        m_TCoords[stage][i] = t;
    }
}

// Template and inline instances this unit also emits:
// TODO 0x100fb3b0 ??0?$Vec@K@@QAE@HM@Z: 74% - inline instance (body in the header); the original reloads e after operator new for the inlined reallocMem's delete test (code without /Oa); a #pragma optimize("a", off) at the end of the unit makes the Vec/AVec/RVec(int, float) constructors match but breaks nine /Oa functions here (ReadInternals, SetBindings, AppendPrims, ...)
