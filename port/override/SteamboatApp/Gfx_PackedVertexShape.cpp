// PORT override: the packed colours live in single float slots of m_CList; the original reinterprets them as unsigned long (4 bytes on Win32, 8 here), so the casts use unsigned int.
#pragma optimize("a", on) // evidence: ReadInternals keeps members in registers across stores and turns the clear loops into rep stosd (no-alias code), as in IndexedVertexShape
#include "Gfx_PackedVertexShape.h"
#include "MathConstants.h"
#include "StdBase.h"
#include "Graph_Stream.h"
#include "Graph_FileStream.h"
#include "Gfx_WriteTraverser.h"
#include "VisMisc.h"
#include "Gfx_Camera.h"
#include "Gfx_CurrentTransformationMatrix.h"
#include <math.h>

// 0x100f4190 ?GetTypeInfo@PackedVertexShape@Gfx@@UBEPBUTypeInfo@StdBase@@XZ
StdBase::TypeInfo const * Gfx::PackedVertexShape::GetTypeInfo() const
{
    return &s_TypeInfo;
}

// 0x100f41a0 ?GetTypeName@PackedVertexShape@Gfx@@UBEPBDXZ
char const * Gfx::PackedVertexShape::GetTypeName() const
{
    return s_TypeInfo.typeName;
}

// 0x100f41b0 ?Write@PackedVertexShape@Gfx@@UAEXPAVWriteTraverser@2@@Z
void Gfx::PackedVertexShape::Write(Gfx::WriteTraverser *traverser)
{
    unsigned char header[21] = "GfxPackedVertexShape";
    if (WriteHeaderInfo(traverser, header) <= -1)
        WriteInternals(traverser);
}

// 0x100f41f0 ?ReadVis@PackedVertexShape@Gfx@@UAEXPAVStream@Graph@@@Z
void Gfx::PackedVertexShape::ReadVis(Graph::Stream *stream)
{
    Shape::ReadVis(stream);
    ReadInternals(stream, 0);
}

// 0x100f4210 ?Read@PackedVertexShape@Gfx@@UAEXPAVStream@Graph@@@Z
void Gfx::PackedVertexShape::Read(Graph::Stream *stream)
{
    int version;
    Shape::Read(stream);
    stream->Read(&version);
    ReadInternals(stream, version);
}

#pragma optimize("a", off) // evidence: the constructor restores the exception list after the last matrix stores
// 0x100f4240 ??0PackedVertexShape@Gfx@@IAE@XZ
Gfx::PackedVertexShape::PackedVertexShape()
    : m_ChangedVertices(0), m_nVerts(0), m_Binding(0), m_Stride(0), m_CoordStride(3), m_NormalStride(3),
      m_ColorStride(2), m_TextureStride(3), m_NormalOffset(0), m_ColorOffset(0), m_TextureOffset(0)
{
    S.identity();
    S[0][0] = -1.0f;
    S[1][1] = -1.0f;
    S[3][0] = 0.0f;
    S[3][1] = 0.0f;
    S[3][2] = -1.0f;
    SS.identity();
    SS[0][0] = -1.0f;
    SS[1][1] = -1.0f;
    SS(3, 0) = 0.0f;
    SS[3][1] = 0.0f;
    SS[3][2] = 1.0f;
    P.identity();
    P[2][3] = 1.0f;
    P[3][3] = 0.0f;
    ST.identity();
    ST[0][0] = 0.5f;
    ST[1][1] = 0.5f;
    ST[3][0] = 0.5f;
    ST[3][1] = 0.5f;
    SST.identity();
    SST[0][0] = 0.5f;
    SST[1][1] = 0.5f;
    SST[3][0] = 0.5f;
    SST[3][1] = 0.5f;
}

#pragma optimize("a", off) // evidence: the destructor stores each BasicObject vftable before loading the next member's pointer
// 0x100f4740 ??1PackedVertexShape@Gfx@@MAE@XZ
Gfx::PackedVertexShape::~PackedVertexShape()
{
}
#pragma optimize("a", on) // back to no-alias code

// 0x100f48e0 ?WriteInternals@PackedVertexShape@Gfx@@IAEXPAVWriteTraverser@2@@Z
void Gfx::PackedVertexShape::WriteInternals(Gfx::WriteTraverser *traverser)
{
    Shape::WriteInternals(traverser);
    Graph::FileStream *stream = traverser->GetStream();
    stream->Write(Version);
    stream->Write(m_Binding);
    writeVeci(m_Prim, stream);
    writeVVec3f(m_Coords, stream);
    if (GetNormalBinding() != BindNone)
        writeVVec3f(m_Normals, stream);
    if (GetColorBinding() != BindNone) {
        writeVecdw(m_DiffuseColors, stream);
        writeVecdw(m_SpecularColors, stream);
    }
    if (GetTextureBinding() != BindNone)
        writeVVVec3f(m_TCoords, stream);
}

// 0x100f49d0 ?ReadInternals@PackedVertexShape@Gfx@@IAEXPAVStream@Graph@@H@Z
void Gfx::PackedVertexShape::ReadInternals(Graph::Stream *stream, int version)
{
    stream->Read(&m_Binding);
    readVeci(m_Prim, stream);
    if (version < 3) {
        readVecf(m_CList, stream);
    } else if (version < 4) {
        readVecf(m_Coordsv3, stream);
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
    ScheduleRefresh(false);
}

// 0x100f4d80 ?PackCListColorRef@PackedVertexShape@Gfx@@IAEXXZ
void Gfx::PackedVertexShape::PackCListColorRef()
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
            *(unsigned int *)&m_CList[k] = color;
            *(unsigned int *)&m_CList[k + 1] = 0xff000000;
        }
        for (int j = m_ColorOffset; j < m_CList.length() - 1; j += m_Stride - 2)
            m_CList.remove(j + 2, 2);
        m_Stride -= 2;
        if (m_NormalOffset > m_ColorOffset)
            m_NormalOffset -= 2;
        if (m_TextureOffset > m_ColorOffset)
            m_TextureOffset -= 2;
    }
}

// 0x100f4f80 ?UnPackCListColorRef@PackedVertexShape@Gfx@@IAEXXZ
void Gfx::PackedVertexShape::UnPackCListColorRef()
{
}

// 0x100f4f90 ?SeparateVertexInfoV3@PackedVertexShape@Gfx@@IAEXXZ
void Gfx::PackedVertexShape::SeparateVertexInfoV3()
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

// TODO 0x100f5380 ?SeparateVertexInfo@PackedVertexShape@Gfx@@IAEXXZ: 61% - register allocation: our copy keeps the constant 1 of m_TCoords.setLength(1) in ebx for every binding test, the original keeps 0 there (and 1 in ebp only around the texture block), which shifts the loop's stack slots; same structure as SeparateVertexInfoV3 (matching)
void Gfx::PackedVertexShape::SeparateVertexInfo()
{
    // older files interleave the vertex components in m_CList: split them into the lists
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
    int coord = 0, normal = 0, color = 0, texture = 0;
    for (int i = 0; i < m_CList.length(); i += m_Stride) {
        m_Coords[coord].e[0] = m_CList[i];
        m_Coords[coord].e[1] = m_CList[i + 1];
        m_Coords[coord].e[2] = m_CList[i + 2];
        coord++;
        if (GetColorBinding() != BindNone) {
            int k = m_ColorOffset + i;
            m_DiffuseColors[color] = *(unsigned int *)&m_CList[k];
            m_SpecularColors[color] = *(unsigned int *)&m_CList[k + 1];
            color++;
        }
        if (GetNormalBinding() != BindNone) {
            int k = m_NormalOffset + i;
            m_Normals[normal].e[0] = m_CList[k];
            m_Normals[normal].e[1] = m_CList[k + 1];
            m_Normals[normal].e[2] = m_CList[k + 2];
            normal++;
        }
        if (GetTextureBinding() != BindNone) {
            int k = m_TextureOffset + i;
            m_TCoords[0][texture].e[0] = m_CList[k];
            m_TCoords[0][texture].e[1] = m_CList[k + 1];
            m_TCoords[0][texture].e[2] = m_CList[k + 2];
            texture++;
        }
    }
    m_CList.setLength(0);
}

// 0x100f57a0 ?SetDrawFunction@PackedVertexShape@Gfx@@UAEXXZ
void Gfx::PackedVertexShape::SetDrawFunction()
{
}

// 0x100f57b0 ?InitOffsets@PackedVertexShape@Gfx@@IAEXH@Z
void Gfx::PackedVertexShape::InitOffsets(int version)
{
    m_Stride = 3;
    if (version < 2) {
        m_NormalOffset = m_ColorOffset = m_TextureOffset = 0;
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
        if (GetColorBinding() != BindNone) {
            m_ColorOffset = m_Stride;
            m_Stride += 2;
        }
        if (GetNormalBinding() != BindNone) {
            m_NormalOffset = m_Stride;
            m_Stride += 3;
        }
        if (GetTextureBinding() != BindNone) {
            m_TextureOffset = m_Stride;
            m_Stride += 3;
        }
    }
    SetDrawFunction();
}

// 0x100f5910 ?SetBindings@PackedVertexShape@Gfx@@QAEXHW4Bind@12@00@Z
void Gfx::PackedVertexShape::SetBindings(int nverts, Bind normal, Bind color, Bind texture)
{
    m_Binding = (((((m_Binding & ~NormalMask) | (normal << NormalShift)) & ~ColorMask) | (color << ColorShift))
                 & ~TextureMask) | (texture << TextureShift);
    InitOffsets(Version);
    int length = m_CList.length();
    m_CList.verify(m_Stride * nverts - 1);
    m_CList.setLength(length);
}

// 0x100f5b20 ?AppendPrims@PackedVertexShape@Gfx@@QAEXPBHH_N@Z
void Gfx::PackedVertexShape::AppendPrims(int const *prims, int n, bool refresh)
{
    m_Prim.setLength(m_Prim.length() + n);
    int start = m_Prim.length() - n;
    for (int i = 0; i < n; i++)
        m_Prim[start + i] = prims[i];
    if (refresh)
        ScheduleRefresh(false);
}

// 0x100f5bf0 ?SetPrim@PackedVertexShape@Gfx@@QAEXHH_N@Z
void Gfx::PackedVertexShape::SetPrim(int value, int i, bool refresh)
{
    if (i >= m_Prim.length())
        m_Prim.setLength(i + 1);
    m_Prim[i] = value;
    if (refresh)
        ScheduleRefresh(false);
}

// TODO 0x100f5cd0 ?Clear@PackedVertexShape@Gfx@@QAEXXZ: inline budget: the original inlines the int initElements only, VC6 here also the first Vec3 one
void Gfx::PackedVertexShape::Clear()
{
    m_Prim.setLength(0);
    m_Coords.setLength(0);
    m_Normals.setLength(0);
    m_DiffuseColors.setLength(0);
    m_SpecularColors.setLength(0);
    for (int i = 0; i < m_TCoords.length(); i++)
        m_TCoords[i].setLength(0);
    m_TCoords.setLength(0);
}

// defined after the code: the original loads them from memory
short const Gfx::PackedVertexShape::NormalMask = 0x001f;
short const Gfx::PackedVertexShape::ColorMask = 0x03e0;
short const Gfx::PackedVertexShape::TextureMask = 0x7c00;
short const Gfx::PackedVertexShape::NormalShift = 0;
short const Gfx::PackedVertexShape::ColorShift = 5;
short const Gfx::PackedVertexShape::TextureShift = 10;
int const Gfx::PackedVertexShape::Version = 4;

// 0x100f46d0 ??_GPackedVertexShape@Gfx@@MAEPAXI@Z
// 0x100f40a0 _$E43
// 0x100f40b0 _$E42
// 0x100f40d0 _$E46
// 0x100f40e0 _$E45
// 0x100f4100 _$E49
// 0x100f4110 _$E48
// 0x100f4130 _$E52
// 0x100f4140 _$E51
// 0x100f4160 _$E55
// 0x100f4170 _$E54
// 0x100f46f0 ??1?$AVec@V?$RVec@M@@@@UAE@XZ
// TODO 0x100f5e80 ?ApplySphereMap@PackedVertexShape@Gfx@@IAEXPAVCamera@2@H@Z: 18% - inline budget: the original calls the matrix copies, multRight, multVecLeft, multVector3Left/setTranslation (in inverseAffine) and the Vec3/Vec4 helpers out of line and spills each temporary to its own stack copy; the statements, the unused copies (model, view, inverse, projected, shadowProjection) and their order are recovered from the bytes
void Gfx::PackedVertexShape::ApplySphereMap(Gfx::Camera *camera, int stage)
{
    // texture coordinates from the eye vector reflected about each eye-space normal; the
    // reflection is turned back to world space and projected towards the view direction
    Vec4<float> down(0, 0, -1, 0);
    TMatrix4<float> model(Gfx::CurrentTransformationMatrix::GetInstance()->GetModelMatrix());
    TMatrix4<float> view(Gfx::CurrentTransformationMatrix::GetInstance()->GetViewMatrix());
    Vec3<float> eye = camera->GetWorldFrom();
    Vec3<float> eyeView = Gfx::CurrentTransformationMatrix::GetInstance()->GetViewMatrix().multVec3PointLeftAffine(eye);
    TMatrix4<float> modelView = Gfx::CurrentTransformationMatrix::GetInstance()->GetModelMatrix()
        * Gfx::CurrentTransformationMatrix::GetInstance()->GetViewMatrix();
    TMatrix4<float> projection = modelView.inverseAffine() * S * P;
    TMatrix4<float> shadowProjection = modelView.inverseAffine() * SS * P;
    for (int i = 0; i < m_Normals.length(); i++) {
        Vec4<float> n;
        n.e[0] = m_Normals[i].e[0];
        n.e[1] = m_Normals[i].e[1];
        n.e[2] = m_Normals[i].e[2];
        n.e[3] = 0;
        n.multScal(1.0f / (float)sqrt(n.mag2()));
        Vec4<float> nv = modelView.multVecLeft(n);
        Vec3<float> normalh;
        if (nv.e[3] != 1.0f && nv.e[3] != 0.0f) {
            normalh.e[0] = nv.e[0] / nv.e[3];
            normalh.e[1] = nv.e[1] / nv.e[3];
            normalh.e[2] = nv.e[2] / nv.e[3];
        } else {
            normalh.e[0] = nv.e[0];
            normalh.e[1] = nv.e[1];
            normalh.e[2] = nv.e[2];
        }
        Vec3<float> normal = normalh;
        normal.multScal(1.0f / (float)sqrt(normal.mag2()));
        Vec4<float> pv = modelView.multVec3PointLeft(m_Coords[i]);
        Vec3<float> point;
        if (pv.e[3] != 1.0f && pv.e[3] != 0.0f) {
            point.e[0] = pv.e[0] / pv.e[3];
            point.e[1] = pv.e[1] / pv.e[3];
            point.e[2] = pv.e[2] / pv.e[3];
        } else {
            point.e[0] = pv.e[0];
            point.e[1] = pv.e[1];
            point.e[2] = pv.e[2];
        }
        Vec3<float> dir = point - eyeView;
        dir.multScal(1.0f / (float)sqrt(dir.mag2()));
        Vec3<float> back = dir * -1.0f;
        float d = 2 * back.dot(normal);
        Vec4<float> r;
        r.e[0] = normal.e[0] * d + dir.e[0];
        r.e[1] = normal.e[1] * d + dir.e[1];
        r.e[2] = normal.e[2] * d + dir.e[2];
        r.e[3] = 0;
        TMatrix4<float> inverse = modelView.inverseAffine();
        TMatrix4<float> world = modelView.inverseAffine();
        Vec4<float> rw = world.multVecLeft(r);
        Vec3<float> t;
        if (rw.e[2] > 0) {
            Vec4<float> q = down - rw;
            q.e[0] /= q.e[2];
            q.e[1] /= q.e[2];
            q.e[2] /= q.e[2];
            t = Vec3<float>(q);
            t.e[0] = (q.e[0] + 1.0f) * 0.5f;
            t.e[1] = (q.e[1] + 1.0f) * 0.5f;
        } else {
            Vec4<float> projected = projection.multVecLeft(r);
            Vec4<float> up = down;
            up.multScal(-1.0f);
            Vec4<float> q = up;
            q.subVec(rw);
            q.e[0] /= q.e[2];
            q.e[1] /= q.e[2];
            q.e[2] /= q.e[2];
            q.multScal(-1.0f);
            q.e[0] = (q.e[0] + 1.0f) * 0.5f;
            q.e[1] = (q.e[1] + 1.0f) * 0.5f;
            t = Vec3<float>(q);
        }
        m_TCoords[stage][i] = t;
    }
}

// 0x100f6c00 ??_G?$AVec@V?$RVec@M@@@@UAEPAXI@Z
// 0x100f6dc0 ??1?$Vec@K@@UAE@XZ
// 0x100f6de0 ??1?$Vec@V?$AVec@V?$Vec3@M@@@@@@UAE@XZ
// 0x100f6e30 ??0?$Vec@V?$RVec@M@@@@QAE@HM@Z
// 0x100f6f30 ??_G?$Vec@K@@UAEPAXI@Z
// 0x100f6f50 ??_G?$Vec@V?$AVec@V?$Vec3@M@@@@@@UAEPAXI@Z
// 0x100f6f70 ??_G?$Vec@V?$RVec@M@@@@UAEPAXI@Z
// 0x100f6f90 ??1?$Vec@V?$RVec@M@@@@UAE@XZ
// 0x100f6fe0 ?initElements@?$Vec@V?$Vec3@M@@@@IAEXHH@Z
// 0x100f7040 ?initElements@?$Vec@K@@IAEXHH@Z
// 0x100f7460 ??_E?$RVec@M@@UAEPAXI@Z
// 0x100f74c0 ??_F?$RVec@M@@QAEXXZ

// Template and inline instances this unit also emits:
// TODO 0x100f6c20 ??0?$AVec@H@@QAE@HM@Z: 49% - inline instance (body in the header); the original reloads e after operator new for the inlined reallocMem's delete test (code without /Oa); a #pragma optimize("a", off) at the end of the unit makes the Vec/AVec/RVec(int, float) constructors match but breaks nine /Oa functions here (ReadInternals, SetBindings, AppendPrims, ...)
// 0x100f6d00 ??0?$Vec3@M@@QAE@ABV?$FixedVec@M$03@@@Z
// TODO 0x100f6d50 ??G?$Vec4@M@@QBE?AV0@ABV0@@Z: not emitted: VC6 inlines Vec4::operator- into ApplySphereMap here, the original calls it out of line
// TODO 0x100f7060 ?reallocMem@?$Vec@V?$RVec@M@@@@IAEXHHH@Z: 42% - stack slots and register allocation of the inlined RVec element copy/initialization loop
// TODO 0x100f72b0 ??0?$Vec@M@@QAE@HM@Z: 50% - inline instance (body in the header); the original reloads e after operator new for the inlined reallocMem's delete test (code without /Oa); a #pragma optimize("a", off) at the end of the unit makes the Vec/AVec/RVec(int, float) constructors match but breaks nine /Oa functions here (ReadInternals, SetBindings, AppendPrims, ...)
// TODO 0x100f7390 ?initElements@?$Vec@V?$RVec@M@@@@IAEXHH@Z: 32% - stack slots and register allocation of the inlined RVec element copy/initialization loop
// TODO 0x100f74d0 ??0?$RVec@M@@QAE@HM@Z: 49% - inline instance (body in the header); the original reloads e after operator new for the inlined reallocMem's delete test (code without /Oa); a #pragma optimize("a", off) at the end of the unit makes the Vec/AVec/RVec(int, float) constructors match but breaks nine /Oa functions here (ReadInternals, SetBindings, AppendPrims, ...)
