#pragma optimize("a", on) // evidence: WriteInternals keeps m_clusters' and m_CMat's pointers across the stream calls (no-alias code)
#include "Gfx_VertexClusterAttribute.h"
#include "MathConstants.h"
#include "Gfx_Attribute.h"
#include "Gfx_AttributeStack.h"
#include "Gfx_IndexedVertexShape.h"
#include "Gfx_PackedVertexShape.h"
#include "Gfx_WriteTraverser.h"
#include "Graph_FileStream.h"
#include "Graph_PassiveTraverser.h"
#include "Graph_Stream.h"
#include "StdBase.h"
#include "VisMisc.h"

// 0x100e6690 ?GetTypeInfo@VertexClusterAttribute@Gfx@@UBEPBUTypeInfo@StdBase@@XZ
StdBase::TypeInfo const * Gfx::VertexClusterAttribute::GetTypeInfo() const
{
    return &s_TypeInfo;
}

// 0x100e66a0 ?GetTypeName@VertexClusterAttribute@Gfx@@UBEPBDXZ
char const * Gfx::VertexClusterAttribute::GetTypeName() const
{
    return s_TypeInfo.typeName;
}

#pragma optimize("a", off) // evidence: the constructor loads s_InstanceCount after the vftable store
// 0x100e66b0 ??0VertexClusterAttribute@Gfx@@QAE@XZ
Gfx::VertexClusterAttribute::VertexClusterAttribute()
    : m_nframes(0), m_nclusters(0), m_FPS(10), m_CurrentFrame(0.0f), m_StartFrame(0.0f), m_LastFrame(0.0f),
      m_Locked(false), m_LastTime(0), m_TimeStep(0.001f), m_FirstTime(true), m_Play(true), m_Looped(false),
      m_ForceChild(0), m_Interpolate(0), m_InterpolationPercentage(0.0f), m_Override(0), m_OverridePercentage(0.0f)
{
    if (++s_InstanceCount == 1)
        s_Stack = new Gfx::AttributeStack(0);
    m_clusters.setLength(0);
}

#pragma optimize("a", on) // back to no-alias code

// TODO 0x100e68a0 ?AddCluster@VertexClusterAttribute@Gfx@@QAEXH@Z: inline budget (the original calls the AVec/IVec constructors and the TMatrix3 operator= out of line)
void Gfx::VertexClusterAttribute::AddCluster(int post)
{
    Gfx::ClusterData *cluster = new Gfx::ClusterData;
    cluster->m_CMat.setLength(0);
    cluster->m_CPos.setLength(0);
    cluster->m_VertPos.setLength(0);
    cluster->m_VertIndex.setLength(0);
    cluster->m_post = post;
    m_clusters.setValueVerify(m_nclusters, cluster);
    m_nframes = 0;
    m_nclusters++;
}

// 0x100e6ae0 ?AddFrame@VertexClusterAttribute@Gfx@@QAEXAAV?$TMatrix3@M@@AAV?$Vec3@M@@@Z
void Gfx::VertexClusterAttribute::AddFrame(TMatrix3<float> &mat, Vec3<float> &pos)
{
    if (m_clusters.length())
        AddFrame(mat, pos, m_nclusters - 1);
}

// TODO 0x100e6b10 ?AddFrame@VertexClusterAttribute@Gfx@@QAEXAAV?$TMatrix3@M@@AAV?$Vec3@M@@H@Z: the inlined appends load len before alloclen in the original
void Gfx::VertexClusterAttribute::AddFrame(TMatrix3<float> &mat, Vec3<float> &pos, int i)
{
    if (m_clusters.length()) {
        Gfx::ClusterData *cluster = m_clusters[i];
        if (cluster) {
            cluster->m_CMat.append(mat);
            cluster->m_CPos.append(pos);
            m_nframes++;
            Compute3pointCenter(cluster);
        }
    }
}

// 0x100e6c00 ?AddVertex@VertexClusterAttribute@Gfx@@QAEXABV?$Vec3@M@@H@Z
void Gfx::VertexClusterAttribute::AddVertex(const Vec3<float> &pos, int index)
{
    if (m_clusters.length())
        AddVertex(pos, index, m_nclusters - 1);
}

// 0x100e6c30 ?AddVertex@VertexClusterAttribute@Gfx@@QAEXABV?$Vec3@M@@HH@Z
void Gfx::VertexClusterAttribute::AddVertex(const Vec3<float> &pos, int index, int i)
{
    if (m_clusters.length()) {
        Gfx::ClusterData *cluster = m_clusters[i];
        if (cluster) {
            cluster->m_VertPos.append(pos);
            cluster->m_VertIndex.append(index);
        }
    }
}

// 0x100e6d30 ?SetClusterName@VertexClusterAttribute@Gfx@@QAEXPBD@Z
void Gfx::VertexClusterAttribute::SetClusterName(const char *name)
{
    if (m_clusters.length())
        SetClusterName(name, m_nclusters - 1);
}

// EQUIV 0x100e6d50 ?SetClusterName@VertexClusterAttribute@Gfx@@QAEXPBDH@Z: reviewed: the original calls Text::clear out of line, VC6 inlines it; the rest is the same. Was: the original calls Text::clear out of line, VC6 here inlines it
void Gfx::VertexClusterAttribute::SetClusterName(const char *name, int i)
{
    if (m_clusters.length()) {
        Gfx::ClusterData *cluster = m_clusters[i];
        if (cluster) {
            cluster->m_name.clear();
            cluster->m_name += name;
        }
    }
}

// 0x100e6e40 ?HandleTime@VertexClusterAttribute@Gfx@@QAEXK@Z
void Gfx::VertexClusterAttribute::HandleTime(unsigned long time)
{
    if (m_FirstTime) {
        m_FirstTime = false;
        m_CurrentFrame = m_StartFrame;
        m_LastTime = time;
    }
    float elapsed = (float)(time - m_LastTime);
    m_LastTime = time;
    if (m_Play) {
        m_CurrentFrame += m_FPS * m_TimeStep * elapsed;
        if (m_Looped) {
            float lastFrame = m_nframes - 1;
            while (m_CurrentFrame > lastFrame)
                m_CurrentFrame -= m_nframes;
        } else if (m_CurrentFrame > m_nframes - 1) {
            m_CurrentFrame = m_nframes - 1.0f;
        }
    }
}

// 0x100e6f30 ?ApplyClusters@VertexClusterAttribute@Gfx@@QAEXPAVShape@2@K@Z
void Gfx::VertexClusterAttribute::ApplyClusters(Gfx::Shape *shape, unsigned long time)
{
    if (m_ForceChild == 0)
        HandleTime(time);
    else if (m_nframes > m_StartFrame)
        m_CurrentFrame = m_StartFrame;
    int frame = (int)m_CurrentFrame;
    if (frame >= 0 && frame < m_nframes) {
        if (shape->IsOfType(Gfx::IndexedVertexShape::GetStaticTypeInfo()))
            ApplyClustersToIndexedShape((Gfx::IndexedVertexShape *)shape, time);
        else if (shape->IsOfType(Gfx::PackedVertexShape::GetStaticTypeInfo()))
            ApplyClustersToPackedShape((Gfx::PackedVertexShape *)shape, time);
    }
}

// Moves the shape's coordinates: post clusters back to their rest positions
// first, then every other cluster's vertices by its frame matrix and
// position, then the post clusters on top of the moved coordinates.
// TODO 0x100e6fc0 ?ApplyClustersToIndexedShape@VertexClusterAttribute@Gfx@@IAEXPAVIndexedVertexShape@2@K@Z: the original treats the coordinate copies as not aliasing the cluster's lengths and pointers (it hoists them and counts the first loop down); VC6 here reloads them after every Vec3 copy even with /Oa; same loops, calls, indices and arithmetic (66% similar)
void Gfx::VertexClusterAttribute::ApplyClustersToIndexedShape(Gfx::IndexedVertexShape *shape, unsigned long)
{
    if (shape->m_Coords.length() < 3)
        return;
    int i;
    for (i = 0; i < m_nclusters; i++) {
        Gfx::ClusterData *cluster = m_clusters[i];
        if (cluster && cluster->m_post) {
            for (int j = 0; j < cluster->m_VertPos.length(); j++)
                shape->m_Coords[cluster->m_VertIndex[j]] = cluster->m_VertPos[j];
        }
    }
    for (i = 0; i < m_nclusters; i++) {
        Gfx::ClusterData *cluster = m_clusters[i];
        if (cluster && !cluster->m_post) {
            TMatrix3<float> *mat;
            Vec3<float> *pos;
            if (!m_Interpolate) {
                int frame = (int)m_CurrentFrame;
                mat = &cluster->m_CMat[frame];
                pos = &cluster->m_CPos[frame];
            } else {
                InterpolateClusterFrames(cluster, m_InterpolationPercentage);
                mat = &m_Interp_CMat;
                pos = &m_Interp_CPos;
            }
            if (m_Override)
                OverrideClusterFrames(m_OverrideCluster, m_OverridePercentage);
            for (int j = 0; j < cluster->m_VertPos.length(); j++) {
                Vec3<float> moved = cluster->m_VertPos[j] * *mat;
                moved += *pos;
                shape->m_Coords[cluster->m_VertIndex[j]] = moved;
            }
        }
    }
    for (i = 0; i < m_nclusters; i++) {
        Gfx::ClusterData *cluster = m_clusters[i];
        if (cluster && cluster->m_post) {
            TMatrix3<float> *mat;
            Vec3<float> *pos;
            if (!m_Interpolate) {
                int frame = (int)m_CurrentFrame;
                mat = &cluster->m_CMat[frame];
                pos = &cluster->m_CPos[frame];
            } else {
                InterpolateClusterFrames(cluster, m_InterpolationPercentage);
                mat = &m_Interp_CMat;
                pos = &m_Interp_CPos;
            }
            if (m_Override)
                OverrideClusterFrames(m_OverrideCluster, m_OverridePercentage);
            for (int j = 0; j < cluster->m_VertPos.length(); j++) {
                Vec3<float> &coord = shape->m_Coords[cluster->m_VertIndex[j]];
                Vec3<float> moved = coord * *mat;
                moved += *pos;
                coord = moved;
            }
        }
    }
    m_LastFrame = m_CurrentFrame;
}

// The same for a packed shape.
// TODO 0x100e7390 ?ApplyClustersToPackedShape@VertexClusterAttribute@Gfx@@IAEXPAVPackedVertexShape@2@K@Z: the original treats the coordinate copies as not aliasing the cluster's lengths and pointers (it hoists them and counts the first loop down); VC6 here reloads them after every Vec3 copy even with /Oa; same loops, calls, indices and arithmetic (66% similar)
void Gfx::VertexClusterAttribute::ApplyClustersToPackedShape(Gfx::PackedVertexShape *shape, unsigned long)
{
    if (shape->m_Coords.length() < 3)
        return;
    int i;
    for (i = 0; i < m_nclusters; i++) {
        Gfx::ClusterData *cluster = m_clusters[i];
        if (cluster && cluster->m_post) {
            for (int j = 0; j < cluster->m_VertPos.length(); j++)
                shape->m_Coords[cluster->m_VertIndex[j]] = cluster->m_VertPos[j];
        }
    }
    for (i = 0; i < m_nclusters; i++) {
        Gfx::ClusterData *cluster = m_clusters[i];
        if (cluster && !cluster->m_post) {
            TMatrix3<float> *mat;
            Vec3<float> *pos;
            if (!m_Interpolate) {
                int frame = (int)m_CurrentFrame;
                mat = &cluster->m_CMat[frame];
                pos = &cluster->m_CPos[frame];
            } else {
                InterpolateClusterFrames(cluster, m_InterpolationPercentage);
                mat = &m_Interp_CMat;
                pos = &m_Interp_CPos;
            }
            if (m_Override)
                OverrideClusterFrames(m_OverrideCluster, m_OverridePercentage);
            for (int j = 0; j < cluster->m_VertPos.length(); j++) {
                Vec3<float> moved = cluster->m_VertPos[j] * *mat;
                moved += *pos;
                shape->m_Coords[cluster->m_VertIndex[j]] = moved;
            }
        }
    }
    for (i = 0; i < m_nclusters; i++) {
        Gfx::ClusterData *cluster = m_clusters[i];
        if (cluster && cluster->m_post) {
            TMatrix3<float> *mat;
            Vec3<float> *pos;
            if (!m_Interpolate) {
                int frame = (int)m_CurrentFrame;
                mat = &cluster->m_CMat[frame];
                pos = &cluster->m_CPos[frame];
            } else {
                InterpolateClusterFrames(cluster, m_InterpolationPercentage);
                mat = &m_Interp_CMat;
                pos = &m_Interp_CPos;
            }
            if (m_Override)
                OverrideClusterFrames(m_OverrideCluster, m_OverridePercentage);
            for (int j = 0; j < cluster->m_VertPos.length(); j++) {
                Vec3<float> &coord = shape->m_Coords[cluster->m_VertIndex[j]];
                Vec3<float> moved = coord * *mat;
                moved += *pos;
                coord = moved;
            }
        }
    }
    m_LastFrame = m_CurrentFrame;
}

// EQUIV 0x100e7760 ?PrintInfo@VertexClusterAttribute@Gfx@@QAEXXZ: reviewed: register choices only (no instruction differs once registers, stack offsets and relocations are normalized). Was: register allocation only: ebx/edi swapped between the cluster and the index pointer (89% similar)
void Gfx::VertexClusterAttribute::PrintInfo()
{
    printf("Nclusters %d  Nframes %d \n", m_nclusters, m_nframes);
    for (int i = 0; i < m_nclusters; i++) {
        Gfx::ClusterData *cluster = m_clusters[i];
        int j;
        for (j = 0; j < cluster->m_VertPos.length(); j++)
            printf(" Vt %d %f %f %f\n", cluster->m_VertIndex[j], cluster->m_VertPos[j][0], cluster->m_VertPos[j][1],
                cluster->m_VertPos[j][2]);
        for (j = 0; j < cluster->m_CMat.length(); j++)
            printf(" Mat Pos %f %f %f   Mat %f %f %f  %f %f %f  %f %f %f\n", cluster->m_CPos[j][0], cluster->m_CPos[j][1],
                cluster->m_CPos[j][2], cluster->m_CMat[j][0][0], cluster->m_CMat[j][0][1], cluster->m_CMat[j][0][2],
                cluster->m_CMat[j][1][0], cluster->m_CMat[j][1][1], cluster->m_CMat[j][1][2], cluster->m_CMat[j][2][0],
                cluster->m_CMat[j][2][1], cluster->m_CMat[j][2][2]);
    }
}

// 0x100e78a0 ?HandleClusterStack@VertexClusterAttribute@Gfx@@SAXPAVShape@2@K@Z
void Gfx::VertexClusterAttribute::HandleClusterStack(Gfx::Shape *shape, unsigned long time)
{
    if (s_Stack && s_Stack->GetTop()) {
        Gfx::VertexClusterAttribute *attribute = (Gfx::VertexClusterAttribute *)s_Stack->GetTop();
        attribute->ApplyClusters(shape, time);
    }
}

// 0x100e78d0 ?Push@VertexClusterAttribute@Gfx@@UBEXPAVPassiveTraverser@Graph@@@Z
void Gfx::VertexClusterAttribute::Push(Graph::PassiveTraverser *) const
{
    s_Stack->Push(this);
}

// 0x100e78e0 ?Pop@VertexClusterAttribute@Gfx@@UBEXPAVPassiveTraverser@Graph@@@Z
void Gfx::VertexClusterAttribute::Pop(Graph::PassiveTraverser *) const
{
    s_Stack->Pop();
}

// 0x100e78f0 ?Write@VertexClusterAttribute@Gfx@@UAEXPAVWriteTraverser@2@@Z
void Gfx::VertexClusterAttribute::Write(Gfx::WriteTraverser *traverser)
{
    Text name("GfxVertexClusterAttribute");
    writeText(name, traverser->GetStream());
    WriteInternals(traverser);
}

// 0x100e7970 ?Read@VertexClusterAttribute@Gfx@@UAEXPAVStream@Graph@@@Z
void Gfx::VertexClusterAttribute::Read(Graph::Stream *stream)
{
    int version;
    Attribute::Read(stream);
    stream->Read(&version);
    ReadInternals(stream, version);
}

#pragma optimize("a", off) // evidence: the destructor stores each member vftable before loading the next pointer
// 0x100e79a0 ??1VertexClusterAttribute@Gfx@@UAE@XZ
Gfx::VertexClusterAttribute::~VertexClusterAttribute()
{
    if (--s_InstanceCount == 0) {
        delete s_Stack;
        s_Stack = 0;
    }
    int n = m_clusters.length();
    if (n) {
        for (int i = 0; i < n; i++) {
            Gfx::ClusterData *cluster = m_clusters[i];
            if (cluster)
                delete cluster;
            m_clusters[i] = 0;
        }
    }
}

#pragma optimize("a", on) // back to no-alias code

// 0x100e7af0 ?WriteInternals@VertexClusterAttribute@Gfx@@AAEXPAVWriteTraverser@2@@Z
void Gfx::VertexClusterAttribute::WriteInternals(Gfx::WriteTraverser *traverser)
{
    Attribute::WriteInternals(traverser);
    Graph::FileStream *stream = traverser->GetStream();
    stream->Write(Version);
    stream->Write(m_StartFrame);
    stream->Write(m_FPS);
    stream->Write((short)m_Looped);
    if (Version > 1)
        stream->Write((short)m_Interpolate);
    int n = m_nclusters;
    stream->Write(n);
    stream->Write(m_nframes);
    for (int i = 0; i < n; i++) {
        Gfx::ClusterData *cluster = m_clusters[i];
        if (cluster == 0) {
            if (Version > 2)
                stream->Write(0);
            stream->Write(0);
            stream->Write(0);
            stream->Write(0);
            stream->Write(0);
        } else {
            if (Version > 2)
                stream->Write(cluster->m_post);
            int nframes = cluster->m_CMat.length();
            stream->Write(nframes);
            for (int j = 0; j < nframes; j++)
                writeMatrix3f(cluster->m_CMat[j], stream);
            writeVVec3f(cluster->m_CPos, stream);
            writeVVec3f(cluster->m_VertPos, stream);
            writeVeci(cluster->m_VertIndex, stream);
            if (Version > 3)
                writeText(cluster->m_name, stream);
        }
    }
}

// The name is read only when this build's Version (not the stream's)
// is above 3, as in the original.
#pragma optimize("a", off) // evidence: the inlined Vec::reallocMem reloads len and alloclen after the calls
// EQUIV 0x100e7c50 ?ReadInternals@VertexClusterAttribute@Gfx@@AAEXPAVStream@Graph@@H@Z: reviewed: register choices only: the original keeps this in ebx and the frame counter in a register where VC6 uses ebp and a slot; same loads, stores, calls and conditions. Was: register allocation only (ebx/ebp swapped; the frame counter is stored to its slot before the loop test in the original) (84% similar)
void Gfx::VertexClusterAttribute::ReadInternals(Graph::Stream *stream, int version)
{
    stream->Read(&m_StartFrame);
    stream->Read(&m_FPS);
    short looped;
    stream->Read(&looped);
    m_Looped = looped != 0;
    if (version > 1) {
        short interpolate;
        stream->Read(&interpolate);
        m_Interpolate = interpolate != 0;
    }
    int n = 0;
    stream->Read(&n);
    stream->Read(&m_nframes);
    for (int i = 0; i < n; i++) {
        AddCluster(0);
        Gfx::ClusterData *cluster = m_clusters[m_nclusters - 1];
        if (version > 2)
            stream->Read(&cluster->m_post);
        int nframes;
        stream->Read(&nframes);
        for (int j = 0; j < nframes; j++) {
            cluster->m_CMat.verify(j);
            readMatrix3f(cluster->m_CMat[j], stream);
        }
        readVVec3f(cluster->m_CPos, stream);
        readVVec3f(cluster->m_VertPos, stream);
        readVeci(cluster->m_VertIndex, stream);
        m_nframes = nframes;
        Compute3pointCenter(cluster);
        if (Version > 3)
            cluster->m_name = readText(stream);
    }
}

#pragma optimize("a", on) // back to no-alias code

// A cluster with three frames: the centre and radius of the circle through
// its three positions (the perpendicular through the middle of the second
// edge, scaled to meet the first edge's perpendicular plane).
// TODO 0x100e7f20 ?Compute3pointCenter@VertexClusterAttribute@Gfx@@AAEXPAVClusterData@2@@Z: inline budget: the original inlines the two edge subtractions (with -=) and the cross products and calls the Vec3(x, y, z) constructor for the halved-edge sums, mag2, multScal, dot, operator-, operator+ and operator*= out of line; VC6 here already calls the first operator-= out of line, so frame and slots differ; the sequence of vector operations was read from the original (26% similar)
void Gfx::VertexClusterAttribute::Compute3pointCenter(Gfx::ClusterData *cluster)
{
    if (m_nframes == 3) {
        Vec3<float> edge1 = cluster->m_CPos[1] - cluster->m_CPos[0];
        Vec3<float> edge2 = cluster->m_CPos[2] - cluster->m_CPos[1];
        Vec3<float> middle1 = edge1 * 0.5f + cluster->m_CPos[0];
        Vec3<float> middle2 = edge2 * 0.5f + cluster->m_CPos[1];
        Vec3<float> normal = edge1.cross(edge2);
        normal.normalize();
        Vec3<float> toCenter = edge2.cross(normal);
        edge1.normalize();
        float t = (middle2 - middle1).dot(edge1);
        toCenter.normalize();
        t /= toCenter.dot(edge1);
        toCenter *= t;
        cluster->m_center = middle2 + toCenter;
        cluster->m_radius = (cluster->m_center - cluster->m_CPos[0]).mag();
    }
}

// TODO 0x100e8280 ?InterpolateClusterFrames@VertexClusterAttribute@Gfx@@IAEXPAVClusterData@2@M@Z: 21% similar: same branches and arithmetic; the original calls SquareMatrix<float, 3>::setValues and Vec3(x, y, z) out of line (VC6 inlines both: rep movsd and direct stores), forms the fraction with fisub from a spilled int and keeps it on the FPU stack
void Gfx::VertexClusterAttribute::InterpolateClusterFrames(Gfx::ClusterData *cluster, float t)
{
    int n = m_nframes;
    if (n < 2 || t == 0.0f) {
        m_Interp_CMat.setValues(cluster->m_CMat[0]);
        m_Interp_CPos = cluster->m_CPos[0];
        return;
    }
    if (t == 1.0f) {
        m_Interp_CMat.setValues(cluster->m_CMat[n - 1]);
        m_Interp_CPos = cluster->m_CPos[m_nframes - 1];
        return;
    }
    if (t < 0.0)
        t = 0.0f;
    else if (t > 1.0)
        t = 1.0f;
    int last = n - 1;
    float position = last * t;
    int i = (int)position;
    int j = i + 1;
    if (j >= n)
        j = last;
    float frac = position - i;
    float w = 1.0f - frac;
    TMatrix3<float> &mi = cluster->m_CMat[i];
    TMatrix3<float> &mj = cluster->m_CMat[j];
    m_Interp_CMat(0, 0) = frac * mj(0, 0) + w * mi(0, 0);
    m_Interp_CMat(0, 1) = frac * mj(0, 1) + w * mi(0, 1);
    m_Interp_CMat(0, 2) = frac * mj(0, 2) + w * mi(0, 2);
    m_Interp_CMat(1, 0) = frac * mj(1, 0) + w * mi(1, 0);
    m_Interp_CMat(1, 1) = frac * mj(1, 1) + w * mi(1, 1);
    m_Interp_CMat(1, 2) = frac * mj(1, 2) + w * mi(1, 2);
    m_Interp_CMat(2, 0) = frac * mj(2, 0) + w * mi(2, 0);
    m_Interp_CMat(2, 1) = frac * mj(2, 1) + w * mi(2, 1);
    m_Interp_CMat(2, 2) = frac * mj(2, 2) + w * mi(2, 2);
    if (m_nframes != 4) {
        m_Interp_CPos = cluster->m_CPos[i] * w + cluster->m_CPos[j] * frac;
    } else {
        // four frames: keep the position on the circle around the cluster centre
        m_Interp_CPos = cluster->m_CPos[i] * w + cluster->m_CPos[j] * frac;
        Vec3<float> d = m_Interp_CPos - cluster->m_center;
        d *= cluster->m_radius / d.mag();
        m_Interp_CPos = d + cluster->m_center;
    }
}

// 0x100e86e0 ?OverrideClusterFrames@VertexClusterAttribute@Gfx@@IAEXPAVClusterData@2@M@Z
void Gfx::VertexClusterAttribute::OverrideClusterFrames(Gfx::ClusterData *cluster, float t)
{
    if (t < 0.0)
        t = 0.0f;
    else if (t > 1.0)
        t = 1.0f;
    float w = 1.0f - t;
    TMatrix3<float> &m = cluster->m_CMat[0];
    m_Interp_CMat[0][0] = w * m_Interp_CMat[0][0] + t * m[0][0];
    m_Interp_CMat[0][1] = w * m_Interp_CMat[0][1] + t * m[0][1];
    m_Interp_CMat[0][2] = w * m_Interp_CMat[0][2] + t * m[0][2];
    m_Interp_CMat[1][0] = w * m_Interp_CMat[1][0] + t * m[1][0];
    m_Interp_CMat[1][1] = w * m_Interp_CMat[1][1] + t * m[1][1];
    m_Interp_CMat[1][2] = w * m_Interp_CMat[1][2] + t * m[1][2];
    m_Interp_CMat[2][0] = w * m_Interp_CMat[2][0] + t * m[2][0];
    m_Interp_CMat[2][1] = w * m_Interp_CMat[2][1] + t * m[2][1];
    m_Interp_CMat[2][2] = w * m_Interp_CMat[2][2] + t * m[2][2];
    m_Interp_CPos = m_Interp_CPos * w + cluster->m_CPos[0] * t;
}

// defined after the code: the original loads it from memory
int const Gfx::VertexClusterAttribute::Version = 4;

// 0x100e6880 ??_GVertexClusterAttribute@Gfx@@UAEPAXI@Z
// TODO 0x100e6da0 ?clear@Text@@QAEXXZ: emitted once SetClusterName calls it out of line
// 0x100e65a0 _$E20
// 0x100e65b0 _$E19
// 0x100e65d0 _$E23
// 0x100e65e0 _$E22
// 0x100e6600 _$E26
// 0x100e6610 _$E25
// 0x100e6630 _$E29
// 0x100e6640 _$E28
// 0x100e6660 _$E32
// 0x100e6670 _$E31

// Template and inline instances this unit also emits:
// 0x100e6ac0 ??1?$IVec@H@@UAE@XZ
// 0x100e86c0 ??4?$TMatrix3@M@@QAEAAV0@ABV0@@Z
// 0x100e8930 ??0?$TMatrix3@M@@QAE@XZ
// 0x100e8b20 ??_G?$IVec@H@@UAEPAXI@Z
// 0x100e8b40 ?dot@?$FixedVec@M$02@@QBEMABV1@@Z
// 0x100e8b70 ??H?$Vec3@M@@QBE?AV0@ABV0@@Z
// 0x100e8bc0 ??X?$Vec3@M@@QAEAAV0@M@Z
// 0x100e8bf0 ??0?$SquareMatrix@M$02@@QAE@XZ
// 0x100e8d10 ??1?$Vec@V?$TMatrix3@M@@@@UAE@XZ
// 0x100e8e10 ??1?$Vec@PAVClusterData@Gfx@@@@UAE@XZ
// 0x100e8f10 ??_G?$Vec@V?$TMatrix3@M@@@@UAEPAXI@Z
// 0x100e8f30 ??_G?$Vec@PAVClusterData@Gfx@@@@UAEPAXI@Z
// 0x100e9100 ?initElements@?$Vec@V?$TMatrix3@M@@@@IAEXHH@Z
// 0x100e9160 ?reallocMem@?$Vec@V?$Vec3@M@@@@IAEXHHH@Z
// 0x100e9290 ??0?$SquareMatrix@M$02@@QAE@ABV0@@Z
// EQUIV 0x100e8e30 ?verify@?$Vec@PAVClusterData@Gfx@@@@QAEHH@Z: reviewed: only the stack slots of the two spilled values (alloclen for the fild, the new length) are swapped
// TODO 0x100e8950 ??0?$AVec@V?$Vec3@M@@@@QAE@HM@Z: 51% similar: matches without /Oa (the original reloads e after new and keeps the delete [] test); "a" off at the end of the unit makes it match but breaks six other instances of the unit
// TODO 0x100e8a40 ??0?$IVec@H@@QAE@HM@Z: 49% similar: as AVec<Vec3<float> >(int, float) (matches without /Oa)
// TODO 0x100e8f50 ?reallocMem@?$Vec@V?$TMatrix3@M@@@@IAEXHHH@Z: 41% similar: the TMatrix3 copies in the inlined element loop (TMatrix3 copy puzzle); neither /Oa setting matches
// TODO 0x100e8c10 ??0?$Vec@V?$TMatrix3@M@@@@QAE@HM@Z: not emitted by our object (the IVec/AVec constructors inline Vec(int, float))
// TODO 0x100e8d30 ??0?$Vec@V?$Vec3@M@@@@QAE@HM@Z: not emitted by our object (as Vec<TMatrix3<float> >(int, float))
