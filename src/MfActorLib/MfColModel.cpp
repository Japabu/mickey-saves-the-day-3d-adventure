// MfActorLib.dll 0x10005140-0x10006c60: MfColModel. The object starts with the
// inline members of the collision model classes (exported classes).
//
// 0x10005140 ?Set@MfTextureVertex@@QAEXMM@Z
// 0x10005150 ??4MfTextureVertex@@QAEAAU0@ABU0@@Z (compiler-generated)
// 0x10005170 ?Set@MfTriangle@@QAEXHHH@Z
// 0x10005190 ??4MfTriangle@@QAEAAU0@ABU0@@Z (compiler-generated)
// 0x100051b0 ?GetBoundingBox@MfColModel@@QAEXAAVMfVector3@@0@Z
// 0x100051e0 ?SetUserData@MfColModel@@QAEHPAVMfAi@@@Z
// 0x100051f0 ?GetUserData@MfColModel@@QAEPAVMfAi@@XZ
// 0x10005200 ?SetBoundingBox@MfColModel@@QAEXAAVMfVector3@@0@Z
// 0x10005230 ?SetRoot@MfColModel@@QAEXPAVMfSphereNode@@@Z
// 0x10005240 ??4MfColModel@@QAEAAV0@ABV0@@Z (compiler-generated)
// 0x100052d0 ??0MfNode@@QAE@J@Z
// 0x10005300 ??0MfNode@@QAE@ABV0@@Z (compiler-generated)
// 0x10005340 ??0MfLeafNode@@QAE@XZ
// 0x10005360 ??0MfSphereNode@@QAE@XZ
// 0x10005390 ??0MfSphereNode@@QAE@ABV0@@Z (compiler-generated)
#include <string.h>
#include "MfColModel.h"
#include "MfTextureVertex.h"
#include "MfTriangle.h"
#include "MfStream.h"
#include "MfTriangleSet.h"
#include "MfAiHook.h"
#include "MfHookData.h"
#include "MfAiHookManager.h"
#include "MfStreamConstructorEntry.h"

// MF_HOOK_OVERRIDE_UPDATE/DELETE_SPHERE_TREE: a hook returning > 0 replaces
// the default behaviour.
struct MfColModelHookData : public MfHookData {
    MfColModel *model;
};

// 0x100053e0 ??0MfColModel@@QAE@XZ
MfColModel::MfColModel()
    : m_reserved(0), m_triangleSet(0), m_leaves(0), m_spheres(0), m_depth(0),
      m_userData(0)
{
}

// 0x10005440 ??_EMfColModel@@UAEPAXI@Z (compiler-generated)

// 0x10005490 ??0MfColModel@@QAE@ABV0@@Z
MfColModel::MfColModel(const MfColModel &model)
    : m_reserved(0), m_triangleSet(0), m_leaves(0), m_spheres(0), m_depth(0),
      m_userData(0)
{
    if (model.m_triangleSet) {
        SetTriangleSet(new MfTriangleSet(*model.m_triangleSet));
        UpdateSphereTree();
    }
}

// 0x10005560 ??1MfColModel@@UAE@XZ
MfColModel::~MfColModel()
{
    if (m_userData)
        m_userData->Release();
    DeleteSphereTree();
    delete m_triangleSet;
}

// 0x100055d0 ?SetTriangleSet@MfColModel@@QAEXPAVMfTriangleSet@@@Z
void MfColModel::SetTriangleSet(MfTriangleSet *set)
{
    if (m_triangleSet && set != m_triangleSet) {
        DeleteSphereTree();
        delete m_triangleSet;
    }
    m_triangleSet = set;
    if (set)
        set->FindBoundingBox(m_min, m_max);
}

// 0x10005620 ?UpdateSphereTree@MfColModel@@QAEXXZ
void MfColModel::UpdateSphereTree()
{
    MfAi *ai;
    if (MfInterfaceRegistry::GetInterface("MfAiHookManager", &ai)) {
        MfColModelHookData data;
        data.model = this;
        if (((MfAiHookManager *)ai)->CallHooks(MF_HOOK_OVERRIDE_UPDATE_SPHERE_TREE, &data) > 0)
            return;
    }
    DeleteSphereTree();
    if (!m_triangleSet || !m_triangleSet->GetNumTriangles() || m_triangleSet->GetType() != 0)
        return;
    m_triangleSet->RemoveDegenerateTriangles();
    m_triangleSet->RemoveUnusedVertices();
    m_triangleSet->FindBoundingBox(m_min, m_max);
    if ((m_min - m_max).length() < 0.0001f)
        return;
    m_triangleSet->FindOrientedBox(m_box);
    int n = m_triangleSet->GetNumTriangles();
    m_leaves = new MfLeafNode[n];
    MfVector3 *vertices = new MfVector3[3 * n];
    for (int i = 0; i < n; i++)
        FindLeafSphere(i, vertices);
    if (n == 1) {
        m_spheres = new MfSphereNode[1];
        m_spheres->m_left = m_spheres->m_right = m_leaves;
        m_spheres->m_center = m_leaves->m_center;
        m_spheres->m_radius = m_leaves->m_radius;
        m_depth = 1;
    } else {
        m_spheres = new MfSphereNode[n - 1];
        int next = 0;
        DivideAndConquer(0, n, vertices, next, 0);
    }
    delete[] vertices;
}

// 0x10005920 ?DeleteSphereTree@MfColModel@@UAEXXZ
void MfColModel::DeleteSphereTree()
{
    MfAi *ai;
    if (MfInterfaceRegistry::GetInterface("MfAiHookManager", &ai)) {
        MfColModelHookData data;
        data.model = this;
        if (((MfAiHookManager *)ai)->CallHooks(MF_HOOK_OVERRIDE_DELETE_SPHERE_TREE, &data) > 0)
            return;
    }
    delete[] m_leaves;
    m_leaves = 0;
    delete[] m_spheres;
    m_spheres = 0;
    m_depth = 0;
}

// 0x10005a00 ?FindLeafSphere@MfColModel@@IAEXHPAVMfVector3@@@Z
// Bounding sphere of triangle i; its corners go to vertices[3 * i] on.
void MfColModel::FindLeafSphere(int i, MfVector3 *vertices)
{
    int first = 3 * i;
    MfTriangle t = m_triangleSet->Triangle(i);
    vertices[first] = m_triangleSet->Vertex(t.m_v[0]);
    vertices[first + 1] = m_triangleSet->Vertex(t.m_v[1]);
    vertices[first + 2] = m_triangleSet->Vertex(t.m_v[2]);
    BoundTriangle(&vertices[first], m_leaves[i].m_center, m_leaves[i].m_radius);
    m_leaves[i].m_index = i;
}

// EQUIV 0x10005ae0 ?BoundTriangle@MfColModel@@KAXPAVMfVector3@@AAV2@AAM@Z: reviewed: stack slots and frame size (c1/c2 slots swapped, consistently), EH state numbering; same calls and float sums
// Smallest sphere around a triangle: half the longest edge for an obtuse
// triangle, else the circumscribed sphere.
void MfColModel::BoundTriangle(MfVector3 *v, MfVector3 &center, float &radius)
{
    MfVector3 e[3];
    e[0] = v[1] - v[0];
    e[1] = v[2] - v[1];
    e[2] = v[0] - v[2];
    float d[3];
    d[0] = -e[1].dot(e[2]);
    d[1] = -e[2].dot(e[0]);
    d[2] = -e[0].dot(e[1]);
    for (int i = 0; i < 3; i++) {
        if (d[i] < 0.0f) {
            center = v[i] + e[i] * 0.5f;
            radius = e[i].length() * 0.5f;
            return;
        }
    }
    MfVector3 sum = v[0] + v[1] + v[2];
    float c0 = d[2] * d[0];
    float c1 = d[0] * d[1];
    float c2 = d[2] * d[1];
    center = (sum - (v[0] * c0 + v[1] * c1 + v[2] * c2) / (c2 + c1 + c0)) * 0.5f;
    radius = (v[0] - center).length();
}

// 0x10005e20 ?DivideAndConquer@MfColModel@@IAEPAVMfNode@@HHPAVMfVector3@@AAHH@Z
// Builds the sphere tree over triangles first .. first + count - 1: splits them
// at the plane across the longest axis of their oriented box and bounds the two
// halves. A node gets the smaller of the sphere around its children and the
// sphere around its vertices.
MfNode *MfColModel::DivideAndConquer(int first, int count, MfVector3 *vertices, int &next, int depth)
{
    depth++;
    if (depth > m_depth)
        m_depth = depth;
    if (count == 1)
        return &m_leaves[first];

    MfVector3 center;
    float radius;
    MfTriangleSet::FindBoundingSphere(&vertices[3 * first], 3 * count, center, radius);
    MfOrientedBox box;
    MfTriangleSet::FindOrientedBox(&vertices[3 * first], 3 * count, box);
    MfVector3 point, normal;
    FindSplittingPlane(box, point, normal);
    int firstLeft, countLeft, firstRight, countRight;
    ReorderTriangles(first, count, point, normal, vertices,
                     firstLeft, countLeft, firstRight, countRight);

    int index = next++;
    MfNode *left = DivideAndConquer(firstLeft, countLeft, vertices, next, depth);
    MfNode *right = DivideAndConquer(firstRight, countRight, vertices, next, depth);
    m_spheres[index].m_left = left;
    m_spheres[index].m_right = right;

    MfVector3 d = right->m_center - left->m_center;
    MfVector3 c, p1, p2;
    float r;
    if (d.length() >= fabs(left->m_radius - right->m_radius) && d.length() >= 1e-6) {
        d.normalize();
        p1 = left->m_center - d * left->m_radius;
        p2 = right->m_center + d * right->m_radius;
        c = (p1 + p2) / 2.0f;
        d = p2 - p1;
        r = d.length() * 0.5f;
    } else if (left->m_radius > right->m_radius) {
        c = left->m_center;
        r = left->m_radius;
    } else {
        c = right->m_center;
        r = right->m_radius;
    }
    if (r < radius) {
        m_spheres[index].m_center = c;
        m_spheres[index].m_radius = r;
    } else {
        m_spheres[index].m_center = center;
        m_spheres[index].m_radius = radius;
    }
    return &m_spheres[index];
}

// 0x100062a0 ?FindSplittingPlane@MfColModel@@KAXABVMfOrientedBox@@AAVMfVector3@@1@Z
// The plane through the box centre across its longest axis.
void MfColModel::FindSplittingPlane(const MfOrientedBox &box, MfVector3 &point, MfVector3 &normal)
{
    MfVector3 xAxis = box.m_transform.rotation() * MfVector3(1.0f, 0.0f, 0.0f);
    MfVector3 yAxis = box.m_transform.rotation() * MfVector3(0.0f, 1.0f, 0.0f);
    MfVector3 zAxis = box.m_transform.rotation() * MfVector3(0.0f, 0.0f, 1.0f);
    float x = box.m_extent.x();
    float y = box.m_extent.y();
    float z = box.m_extent.z();
    if (x > y && x > z)
        normal = xAxis;
    else if (y > z)
        normal = yAxis;
    else
        normal = zAxis;
    point = box.m_transform.translation();
}

// 0x100063f0 ?ReorderTriangles@MfColModel@@IAEXHHABVMfVector3@@0PAV2@AAH222@Z
// Partitions triangles first .. first + count - 1 (with their vertices,
// materials and leaves) by the side of the plane their leaf centres lie on.
// If all of them end up on one side the range is split in the middle.
void MfColModel::ReorderTriangles(int first, int count, const MfVector3 &point, const MfVector3 &normal,
                                  MfVector3 *vertices, int &firstLeft, int &countLeft,
                                  int &firstRight, int &countRight)
{
    MfVector3 d;
    MfVector3 corners[3];
    MfLeafNode leaf;
    int i = first;
    int j = first + count - 1;
    while (i < j) {
        while (i < j) {
            d = m_leaves[i].m_center - point;
            if (d.dot(normal) > 0.0f)
                break;
            i++;
        }
        while (i < j) {
            d = m_leaves[j].m_center - point;
            if (d.dot(normal) < 0.0f)
                break;
            j--;
        }
        if (i < j) {
            memcpy(corners, &vertices[3 * i], sizeof(corners));
            memcpy(&vertices[3 * i], &vertices[3 * j], sizeof(corners));
            memcpy(&vertices[3 * j], corners, sizeof(corners));

            MfTriangle t = m_triangleSet->Triangle(i);
            m_triangleSet->Triangle(i) = m_triangleSet->Triangle(j);
            m_triangleSet->Triangle(j) = t;

            if (m_triangleSet->GetNumMaterials()) {
                short material = m_triangleSet->MaterialIndex(i);
                m_triangleSet->MaterialIndex(i) = m_triangleSet->MaterialIndex(j);
                m_triangleSet->MaterialIndex(j) = material;
            }

            leaf = m_leaves[i];
            m_leaves[i] = m_leaves[j];
            m_leaves[j] = leaf;
            m_leaves[i].m_index = i;
            m_leaves[j].m_index = j;
        }
    }
    firstLeft = first;
    countLeft = i - first;
    firstRight = i;
    countRight = count - countLeft;
    if (!countLeft || !countRight) {
        firstLeft = first;
        countLeft = count / 2;
        firstRight = firstLeft + countLeft;
        countRight = count - countLeft;
    }
}

// 0x100067f0 ?fn_100067f0@@YAPAVMfStreamObject@@XZ (original fn_100067f0:) stream constructor for 'CoM2'
static MfStreamObject *fn_100067f0()
{
    return new MfColModel;
}

// 0x10006850 _$E11 (jumps to _$E10)
// 0x10006860 _$E10 (constructs the stream constructor entry below)
static MfStreamConstructorEntry s_colModelEntry('CoM2', fn_100067f0);

const long MfColModel::STREAM_TYPE = 'CoM2';

// 0x10006880 ?StreamType@MfColModel@@UBEJXZ
long MfColModel::StreamType() const
{
    return STREAM_TYPE;
}

// 0x10006890 ?ReadFromStream@MfColModel@@UAE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfColModel::ReadFromStream(MfStream &s)
{
    int hasTriangles, left, right;
    s >> hasTriangles;
    if (!hasTriangles)
        return s.Status();
    m_triangleSet = new MfTriangleSet;
    s >> *m_triangleSet;
    s >> m_depth;
    if (!m_depth)
        return s.Status();
    int n = m_triangleSet->GetNumTriangles();
    m_leaves = new MfLeafNode[n];
    int i;
    for (i = 0; i < n; i++) {
        if (s.userFormat == 100)
            s.ReadBytes(0x10, &m_leaves[i]);
        else
            s.ReadBytes(sizeof(MfNode), &m_leaves[i]);
    }
    int numSpheres = n == 1 ? 1 : n - 1;
    m_spheres = new MfSphereNode[numSpheres];
    for (i = 0; i < numSpheres; i++) {
        if (s.userFormat == 100)
            s.ReadBytes(0x10, &m_spheres[i]);
        else
            s.ReadBytes(sizeof(MfNode), &m_spheres[i]);
        s >> left >> right;
        if (left & 0x80000000) {
            left &= 0x7fffffff;
            m_spheres[i].m_left = &m_spheres[left];
        } else
            m_spheres[i].m_left = &m_leaves[left];
        if (right & 0x80000000) {
            right &= 0x7fffffff;
            m_spheres[i].m_right = &m_spheres[right];
        } else
            m_spheres[i].m_right = &m_leaves[right];
    }
    if (s.userFormat >= 0x6a)
        s.ReadBytes(sizeof(MfOrientedBox), &m_box);
    return s.Status();
}

// EQUIV 0x10006af0 ?WriteToStream@MfColModel@@UBE?AW4MfStreamError@@AAVMfStream@@@Z: reviewed: base + offset added in the other operand order (same address); rest identical
// Children are written as indices: leaves plain, spheres with the top bit set.
MfStreamError MfColModel::WriteToStream(MfStream &s) const
{
    int i;
    if (!m_triangleSet) {
        s << 0;
        return s.Status();
    }
    s << 1;
    s << *m_triangleSet;
    if (!m_leaves) {
        s << 0;
        return s.Status();
    }
    s << m_depth;
    int n = m_triangleSet->GetNumTriangles();
    for (i = 0; i < n; i++)
        s.WriteBytes(sizeof(MfNode), &m_leaves[i]);
    int numSpheres = n == 1 ? 1 : n - 1;
    for (i = 0; i < numSpheres; i++) {
        s.WriteBytes(sizeof(MfNode), &m_spheres[i]);
        MfNode *left = m_spheres[i].m_left;
        if (left->m_type == MF_LEAF_NODE)
            s << (int)(((unsigned long)left - (unsigned long)m_leaves) / sizeof(MfLeafNode));
        else
            s << (int)(((unsigned long)left - (unsigned long)m_spheres) / sizeof(MfSphereNode) | 0x80000000);
        MfNode *right = m_spheres[i].m_right;
        if (right->m_type == MF_LEAF_NODE)
            s << (int)(((unsigned long)right - (unsigned long)m_leaves) / sizeof(MfLeafNode));
        else
            s << (int)(((unsigned long)right - (unsigned long)m_spheres) / sizeof(MfSphereNode) | 0x80000000);
    }
    s.WriteBytes(sizeof(MfOrientedBox), &m_box);
    return s.Status();
}


// Also matched by this code:
// runtimelib.dll 0x1007fa10 ?SetRoot@MfColModel@@QAEXPAVMfSphereNode@@@Z