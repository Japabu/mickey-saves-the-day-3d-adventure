#include <math.h>
#include <string.h>
#include <stdio.h>
#include <float.h>
#include "MfTriangleSet.h"
#include "MfVector3.h"
#include "MfStreamConstructorEntry.h"

long const MfTriangleSet::STREAM_TYPE = 'TrS2';

// The coordinate axes, for the box and distance tests.
// 0x1001bf30 _$E5
// 0x1001bf40 _$E2
// 0x1001bf60 _$E4
// 0x1001bf70 _$E3
static MfVector3 sXAxis(1.0f, 0.0f, 0.0f);
// 0x1001bf80 _$E10
// 0x1001bf90 _$E7
// 0x1001bfb0 _$E9
// 0x1001bfc0 _$E8
static MfVector3 sYAxis(0.0f, 1.0f, 0.0f);
// 0x1001bfd0 _$E15
// 0x1001bfe0 _$E12
// 0x1001c000 _$E14
// 0x1001c010 _$E13
static MfVector3 sZAxis(0.0f, 0.0f, 1.0f);

// (included after the statics: their $E numbers say only the two headers above
// came before them)
#include "MfColModel.h"
#include "MfTextureVertex.h"
#include "MfTriangle.h"
#include "MfColor.h"
#include "MfMaterial.h"
#include "MfStream.h"
#include "MfMath.h"
#include "MfMatrix3.h"
#include "MfXformMatrix3.h"
#include "MfTriSquare.h"
#include "MfTriCube.h"
#include "MfTriSphere.h"
#include "MfTriCone.h"
#include "MfTriCylinder.h"
#include "MfTriLine.h"
#include "MfTriBezier.h"
#include "MfTriCircle.h"
#include "MfTriRing.h"

#define EPSILON 1e-5f

// The next set ID.
static int sNextID;

// 0x1001c020 ??0MfTriangleSet@@QAE@XZ
MfTriangleSet::MfTriangleSet()
    : mType(MF_TRIANGLE_EMPTY), mFlags(0), mSource(0), mNumVertices(0), mVertices(0), mNormals(0),
      mColors(0), mTextureVertices(0), mNumTexturesPerVertex(0), mNumTriangles(0), mTriangles(0),
      mMaterialIndices(0), mNumMaterials(0), mMaterials(0)
{
    mID = sNextID++;
}

// 0x1001c070 ??0MfTriangleSet@@QAE@ABV0@@Z
MfTriangleSet::MfTriangleSet(const MfTriangleSet &set)
    : mSource(0), mVertices(0), mNormals(0), mColors(0), mTextureVertices(0), mTriangles(0),
      mMaterialIndices(0), mMaterials(0)
{
    mType = set.mType;
    mFlags = set.mFlags;
    mID = sNextID++;
    mSource = set.mSource;
    mNumVertices = set.mNumVertices;
    mNumTexturesPerVertex = set.mNumTexturesPerVertex;
    mNumTriangles = set.mNumTriangles;
    mNumMaterials = set.mNumMaterials;
    if (mSource)
        return;
    int i;
    if (set.mVertices) {
        mVertices = new MfVector3[mNumVertices];
        for (i = 0; i < mNumVertices; i++)
            mVertices[i] = set.mVertices[i];
    }
    if (set.mNormals) {
        mNormals = new MfVector3[mNumVertices];
        for (i = 0; i < mNumVertices; i++)
            mNormals[i] = set.mNormals[i];
    }
    if (set.mColors) {
        mColors = new MfColor[mNumVertices];
        for (i = 0; i < mNumVertices; i++)
            mColors[i] = set.mColors[i];
    }
    if (set.mTextureVertices) {
        mTextureVertices = new MfTextureVertex[mNumTexturesPerVertex * mNumVertices];
        for (i = 0; i < mNumTexturesPerVertex * mNumVertices; i++)
            mTextureVertices[i] = set.mTextureVertices[i];
    }
    if (set.mTriangles) {
        mTriangles = new MfTriangle[mNumTriangles];
        for (i = 0; i < mNumTriangles; i++)
            mTriangles[i] = set.mTriangles[i];
    }
    if (set.mMaterialIndices) {
        mMaterialIndices = new short[mNumTriangles];
        for (i = 0; i < mNumTriangles; i++)
            mMaterialIndices[i] = set.mMaterialIndices[i];
    }
    if (set.mMaterials) {
        mMaterials = new MfMaterial[mNumMaterials];
        for (i = 0; i < mNumMaterials; i++)
            mMaterials[i] = set.mMaterials[i];
    }
}

// 0x1001c400 ??1MfTriangleSet@@QAE@XZ
MfTriangleSet::~MfTriangleSet()
{
    if (!mSource) {
        delete[] mVertices;
        delete mTriangles;
    }
    delete[] mNormals;
    delete[] mColors;
    delete mTextureVertices;
    delete mMaterialIndices;
    delete[] mMaterials;
}

// 0x1001c4a0 ?Create@MfTriangleSet@@QAEXPAV1@HH@Z
// A set that shares source's vertices and triangles, with its own attributes.
void MfTriangleSet::Create(MfTriangleSet *source, int flags, int numTextures)
{
    mSource = source;
    mType = source->mType;
    mFlags = flags;
    mNumVertices = source->mNumVertices;
    mVertices = source->mVertices;
    mNumTriangles = source->mNumTriangles;
    mTriangles = source->mTriangles;
    if (flags & MF_TRISET_NORMALS)
        mNormals = new MfVector3[mNumVertices];
    if (flags & MF_TRISET_COLORS)
        mColors = new MfColor[mNumVertices];
    if (flags & MF_TRISET_TEXTURES) {
        mNumTexturesPerVertex = numTextures;
        mTextureVertices = new MfTextureVertex[numTextures * mNumVertices];
    }
    if (flags & MF_TRISET_MATERIALS)
        mMaterialIndices = new short[mNumTriangles];
}

// 0x1001c5e0 ?Create@MfTriangleSet@@QAEXW4MfTriangleSetType@@HHHH@Z
void MfTriangleSet::Create(MfTriangleSetType type, int flags, int numTriangles, int numVertices,
                           int numTextures)
{
    mType = type;
    mFlags = flags;
    mNumTriangles = numTriangles;
    if (type == MF_TRIANGLE_LIST) {
        mNumVertices = numVertices;
        if (numVertices >= 3)
            mTriangles = new MfTriangle[numTriangles];
    } else if (type == MF_LINE_STRIP) {
        mNumVertices = numVertices;
        mNumTriangles = 0;
        mTriangles = 0;
    } else
        mNumVertices = numTriangles + 2;
    if (numVertices > 0) {
        mVertices = new MfVector3[numVertices];
        if (flags & MF_TRISET_NORMALS)
            mNormals = new MfVector3[numVertices];
        if (flags & MF_TRISET_COLORS)
            mColors = new MfColor[numVertices];
        if (flags & MF_TRISET_TEXTURES) {
            mNumTexturesPerVertex = numTextures;
            mTextureVertices = new MfTextureVertex[numTextures * numVertices];
        }
    }
    if ((flags & MF_TRISET_MATERIALS) && mNumTriangles > 0)
        mMaterialIndices = new short[numTriangles];
}

// 0x1000f4b0 ?GetID@MfTriangleSet@@QAEHXZ
int MfTriangleSet::GetID()
{
    return mID;
}

// 0x1000f6b0 ?GetFlags@MfTriangleSet@@QAEHXZ
int MfTriangleSet::GetFlags()
{
    return mFlags;
}

// 0x1001c7a0 ?GetType@MfTriangleSet@@QAE?AW4MfTriangleSetType@@XZ
MfTriangleSetType MfTriangleSet::GetType()
{
    return mType;
}

// 0x1001c7b0 ?ChangeID@MfTriangleSet@@QAEHXZ
int MfTriangleSet::ChangeID()
{
    mID = sNextID++;
    return mID;
}

// 0x1001c7d0 ?Vertex@MfTriangleSet@@QAEAAVMfVector3@@H@Z
MfVector3 &MfTriangleSet::Vertex(int i)
{
    return mVertices[i];
}

// 0x1001c7e0 ?Normal@MfTriangleSet@@QAEAAVMfVector3@@H@Z
MfVector3 &MfTriangleSet::Normal(int i)
{
    return mNormals[i];
}

// 0x1001c7f0 ?Color@MfTriangleSet@@QAEAAVMfColor@@H@Z
MfColor &MfTriangleSet::Color(int i)
{
    return mColors[i];
}

// 0x1001c810 ?GetNumTexturesPerVertex@MfTriangleSet@@QAEHXZ
int MfTriangleSet::GetNumTexturesPerVertex()
{
    return mNumTexturesPerVertex;
}

// 0x1001c820 ?TextureVertex@MfTriangleSet@@QAEAAUMfTextureVertex@@HH@Z
MfTextureVertex &MfTriangleSet::TextureVertex(int vertex, int texture)
{
    return mTextureVertices[mNumTexturesPerVertex * vertex + texture];
}

// 0x1001c840 ?GetNumTriangles@MfTriangleSet@@QAEHXZ
int MfTriangleSet::GetNumTriangles()
{
    return mNumTriangles;
}

// 0x1001c850 ?Triangle@MfTriangleSet@@QAEAAUMfTriangle@@H@Z
MfTriangle &MfTriangleSet::Triangle(int i)
{
    return mTriangles[i];
}

// 0x1001c860 ?MaterialIndex@MfTriangleSet@@QAEAAFH@Z
short &MfTriangleSet::MaterialIndex(int i)
{
    return mMaterialIndices[i];
}

// 0x1001c870 ?FindTriangleNormal@MfTriangleSet@@QAE?AVMfVector3@@H@Z
MfVector3 MfTriangleSet::FindTriangleNormal(int i)
{
    if (i >= 0 && i < mNumTriangles) {
        MfTriangle &t = mTriangles[i];
        MfVector3 a = mVertices[t.m_v[0]];
        MfVector3 b = mVertices[t.m_v[1]];
        MfVector3 c = mVertices[t.m_v[2]];
        return (b - a).cross(c - a).unit();
    }
    return MfVector3(0, 0, 0);
}

// 0x1001c9a0 ?GetNumMaterials@MfTriangleSet@@QAEHXZ
int MfTriangleSet::GetNumMaterials()
{
    return mNumMaterials;
}

// 0x1001c9b0 ?SetNumMaterials@MfTriangleSet@@QAEXH@Z
void MfTriangleSet::SetNumMaterials(int n)
{
    if (n == mNumMaterials)
        return;
    mNumMaterials = n;
    delete[] mMaterials;
    mMaterials = 0;
    if (mNumMaterials > 0) {
        mMaterials = new MfMaterial[mNumMaterials];
        if (!mMaterialIndices)
            mMaterialIndices = new short[mNumTriangles];
    } else {
        delete mMaterialIndices;
        mMaterialIndices = 0;
    }
}

// 0x1001cab0 ?Material@MfTriangleSet@@QAEAAVMfMaterial@@H@Z
MfMaterial &MfTriangleSet::Material(int i)
{
    return mMaterials[i];
}

// 0x1001cad0 ?RemoveDegenerateTriangles@MfTriangleSet@@QAEXXZ
// Drops triangles with a (nearly) zero-length edge or (nearly) parallel edges;
// the last triangle takes the place of a dropped one.
void MfTriangleSet::RemoveDegenerateTriangles()
{
    if (mType != MF_TRIANGLE_LIST)
        return;
    MfVector3 e1, e2;
    for (int i = 0; i < mNumTriangles; i++) {
        e1 = mVertices[mTriangles[i].m_v[1]] - mVertices[mTriangles[i].m_v[0]];
        e2 = mVertices[mTriangles[i].m_v[2]] - mVertices[mTriangles[i].m_v[1]];
        float l1 = e1.lengthSquared();
        if (l1 > EPSILON * EPSILON) {
            float l2 = e2.lengthSquared();
            if (l2 > EPSILON * EPSILON) {
                l1 = (float)sqrt(l1);
                l2 = (float)sqrt(l2);
                e1.x() /= l1;
                e1.y() /= l1;
                e1.z() /= l1;
                e2.x() /= l2;
                e2.y() /= l2;
                e2.z() /= l2;
                float d = e1.dot(e2);
                if (d < 1.0f - EPSILON && d > -(1.0f - EPSILON))
                    continue;
            }
        }
        mNumTriangles--;
        mTriangles[i] = mTriangles[mNumTriangles];
        if (mFlags & MF_TRISET_MATERIALS)
            mMaterialIndices[i] = mMaterialIndices[mNumTriangles];
        i--;
        if (mNumTriangles == 0)
            return;
    }
}

// 0x1001cd10 ?RemoveUnusedVertices@MfTriangleSet@@QAEXXZ
void MfTriangleSet::RemoveUnusedVertices()
{
    if (mType != MF_TRIANGLE_LIST)
        return;
    int *map = new int[mNumVertices];
    int i;
    for (i = 0; i < mNumVertices; i++)
        map[i] = -1;
    for (i = 0; i < mNumTriangles; i++) {
        map[mTriangles[i].m_v[0]] = 0;
        map[mTriangles[i].m_v[1]] = 0;
        map[mTriangles[i].m_v[2]] = 0;
    }
    int n = 0;
    for (i = 0; i < mNumVertices; i++) {
        if (map[i] != -1)
            map[i] = n++;
    }
    for (i = 0; i < mNumVertices; i++) {
        if (map[i] == -1)
            continue;
        mVertices[map[i]] = mVertices[i];
        if (mFlags & MF_TRISET_NORMALS)
            mNormals[map[i]] = mNormals[i];
        if (mFlags & MF_TRISET_COLORS)
            mColors[map[i]] = mColors[i];
        if (mFlags & MF_TRISET_TEXTURES)
            memcpy(&mTextureVertices[map[i] * mNumTexturesPerVertex],
                   &mTextureVertices[i * mNumTexturesPerVertex],
                   mNumTexturesPerVertex * sizeof(MfTextureVertex));
    }
    mNumVertices = n;
    for (i = 0; i < mNumTriangles; i++) {
        mTriangles[i].m_v[0] = map[mTriangles[i].m_v[0]];
        mTriangles[i].m_v[1] = map[mTriangles[i].m_v[1]];
        mTriangles[i].m_v[2] = map[mTriangles[i].m_v[2]];
    }
    delete map;
}

// 0x1001cf00 ?AssignAverageNormals@MfTriangleSet@@QAEXXZ
// Each vertex normal becomes the average of the normals of its triangles.
void MfTriangleSet::AssignAverageNormals()
{
    if (!mNormals) {
        mFlags |= MF_TRISET_NORMALS;
        mNormals = new MfVector3[mNumVertices];
    }
    int *count = new int[mNumVertices];
    int i;
    for (i = 0; i < mNumVertices; i++) {
        count[i] = 0;
        mNormals[i].setValue(0, 0, 0);
    }
    MfVector3 n;
    for (i = 0; i < mNumTriangles; i++) {
        n.NormalizeCross(mVertices[mTriangles[i].m_v[1]] - mVertices[mTriangles[i].m_v[0]],
                         mVertices[mTriangles[i].m_v[2]] - mVertices[mTriangles[i].m_v[0]]);
        count[mTriangles[i].m_v[0]]++;
        mNormals[mTriangles[i].m_v[0]] += n;
        count[mTriangles[i].m_v[1]]++;
        mNormals[mTriangles[i].m_v[1]] += n;
        count[mTriangles[i].m_v[2]]++;
        mNormals[mTriangles[i].m_v[2]] += n;
    }
    for (i = 0; i < mNumVertices; i++) {
        mNormals[i] /= (float)count[i];
        mNormals[i].normalize();
    }
    delete count;
}

// 0x1001d1b0 ?ApplyTransform@MfTriangleSet@@QAEXABVMfTransform3@@@Z
void MfTriangleSet::ApplyTransform(const MfTransform3 &t)
{
    int i;
    for (i = 0; i < mNumVertices; i++)
        mVertices[i] = t * mVertices[i];
    if (mFlags & MF_TRISET_NORMALS) {
        for (i = 0; i < mNumVertices; i++)
            mNormals[i] = t.rotation() * mNormals[i];
    }
}

// 0x1001d250 ?ApplyScale@MfTriangleSet@@QAEXABVMfVector3@@@Z
void MfTriangleSet::ApplyScale(const MfVector3 &scale)
{
    for (int i = 0; i < mNumVertices; i++)
        mVertices[i] *= scale;
}

// 0x1001d290 ?Merge@MfTriangleSet@@QAEXABV1@AAVMfTransform3@@@Z
// Appends set, its vertices and normals transformed by t. (The colors are
// copied with the size of a vector, the normals get the translation too.)
void MfTriangleSet::Merge(const MfTriangleSet &set, MfTransform3 &t)
{
    if (mType != MF_TRIANGLE_LIST || set.mType != MF_TRIANGLE_LIST || mSource)
        return;
    int numVertices = mNumVertices + set.mNumVertices;
    int numTriangles = mNumTriangles + set.mNumTriangles;
    int numMaterials = mNumMaterials + set.mNumMaterials;
    int i;
    MfVector3 *vectors = new MfVector3[numVertices];
    memcpy(vectors, mVertices, mNumVertices * sizeof(MfVector3));
    for (i = 0; i < set.mNumVertices; i++)
        t.Multiply(vectors[mNumVertices + i], set.mVertices[i]);
    delete[] mVertices;
    mVertices = vectors;
    if (mFlags & MF_TRISET_NORMALS) {
        vectors = new MfVector3[numVertices];
        memcpy(vectors, mNormals, mNumVertices * sizeof(MfVector3));
        for (i = 0; i < set.mNumVertices; i++)
            t.Multiply(vectors[mNumVertices + i], set.mNormals[i]);
        delete[] mNormals;
        mNormals = vectors;
    }
    if (mFlags & MF_TRISET_COLORS) {
        MfColor *colors = new MfColor[numVertices];
        memcpy(colors, mColors, mNumVertices * sizeof(MfVector3));
        memcpy(&colors[mNumVertices], set.mColors, set.mNumVertices * sizeof(MfVector3));
        delete[] mColors;
        mColors = colors;
    }
    if ((mFlags & MF_TRISET_TEXTURES) && (set.mFlags & MF_TRISET_TEXTURES)
        && mNumTexturesPerVertex == set.mNumTexturesPerVertex) {
        MfTextureVertex *textures = new MfTextureVertex[numVertices * mNumTexturesPerVertex];
        memcpy(textures, mTextureVertices, mNumTexturesPerVertex * mNumVertices * sizeof(MfTextureVertex));
        memcpy(&textures[mNumVertices * mNumTexturesPerVertex], set.mTextureVertices,
               set.mNumVertices * mNumTexturesPerVertex * sizeof(MfTextureVertex));
        delete mTextureVertices;
        mTextureVertices = textures;
    }
    if (numMaterials > 0) {
        MfMaterial *materials = new MfMaterial[numMaterials];
        for (i = 0; i < mNumMaterials; i++)
            materials[i] = mMaterials[i];
        for (i = 0; i < set.mNumMaterials; i++)
            materials[mNumMaterials + i] = set.mMaterials[i];
        delete[] mMaterials;
        mMaterials = materials;
    }
    MfTriangle *triangles = new MfTriangle[numTriangles];
    memcpy(triangles, mTriangles, mNumTriangles * sizeof(MfTriangle));
    memcpy(&triangles[mNumTriangles], set.mTriangles, set.mNumTriangles * sizeof(MfTriangle));
    delete mTriangles;
    mTriangles = triangles;
    for (i = mNumTriangles; i < numTriangles; i++) {
        mTriangles[i].m_v[0] += mNumVertices;
        mTriangles[i].m_v[1] += mNumVertices;
        mTriangles[i].m_v[2] += mNumVertices;
    }
    if (mFlags & MF_TRISET_MATERIALS) {
        short *indices = new short[numTriangles];
        memcpy(indices, mMaterialIndices, mNumTriangles * sizeof(short));
        memcpy(&indices[mNumTriangles], set.mMaterialIndices, set.mNumTriangles * sizeof(short));
        delete mMaterialIndices;
        mMaterialIndices = indices;
        for (i = mNumTriangles; i < numTriangles; i++)
            mMaterialIndices[i] += mNumMaterials;
    }
    mNumTriangles = numTriangles;
    mNumVertices = numVertices;
    mNumMaterials = numMaterials;
}

// 0x1001d820 ?FindBoundingBox@MfTriangleSet@@QAEXAAVMfVector3@@0@Z
void MfTriangleSet::FindBoundingBox(MfVector3 &min, MfVector3 &max)
{
    FindBoundingBox(mVertices, mNumVertices, min, max);
}

// 0x1001d840 ?FindBoundingBox@MfTriangleSet@@SAXPAVMfVector3@@HAAV2@1@Z
void MfTriangleSet::FindBoundingBox(MfVector3 *vertices, int n, MfVector3 &min, MfVector3 &max)
{
    if (!vertices)
        return;
    max = vertices[0];
    min = max;
    for (int i = 1; i < n; i++) {
        MfVector3 v = vertices[i];
        if (v.x() < min.x())
            min.x() = v.x();
        if (v.y() < min.y())
            min.y() = v.y();
        if (v.z() < min.z())
            min.z() = v.z();
        if (v.x() > max.x())
            max.x() = v.x();
        if (v.y() > max.y())
            max.y() = v.y();
        if (v.z() > max.z())
            max.z() = v.z();
    }
}

// 0x1001d920 ?FindBoundingSphere@MfTriangleSet@@QAEXAAVMfVector3@@AAM@Z
void MfTriangleSet::FindBoundingSphere(MfVector3 &center, float &radius)
{
    FindBoundingSphere(mVertices, mNumVertices, center, radius);
}

#define ABS(x) ((x) < 0 ? -(x) : (x))
#define RAY_EPSILON 1e-6f

// 0x1001e3b0 ?CheckRayIntersectionCommon@MfTriangleSet@@IAE_NHAAVMfVector3@@0AAMH000@Z
// Badouel's ray-triangle test (Graphics Gems I): the ray hits the plane of
// (v0, v1, v2) nearer than distance, and the hit point lies in the triangle.
// With cullBack set, back faces are skipped unless the material is two-sided.
bool MfTriangleSet::CheckRayIntersectionCommon(int i, MfVector3 &origin, MfVector3 &direction,
                                               float &distance, int cullBack, MfVector3 &v0,
                                               MfVector3 &v1, MfVector3 &v2)
{
    MfVector3 e1, e2, n;
    e1 = v1 - v0;
    e2 = v2 - v0;
    n = e1.cross(e2);
    if (cullBack && mMaterialIndices && !(mMaterials[mMaterialIndices[i]].m_shading & 1)) {
        if (direction.dot(n) > 0)
            return false;
    }
    float d = -v0.dot(n);
    float denom = n.dot(direction);
    if (denom > -RAY_EPSILON && denom < RAY_EPSILON)
        return false;
    float t = -(n.dot(origin) + d) / denom;
    if (t >= distance)
        return false;
    distance = t;
    if (t <= 0)
        return false;
    MfVector3 p = origin + direction * t;
    int i1, i2;
    if (ABS(n.x()) >= ABS(n.y()) && ABS(n.x()) >= ABS(n.z())) {
        i1 = 1;
        i2 = 2;
    } else if (ABS(n.y()) >= ABS(n.x()) && ABS(n.y()) >= ABS(n.z())) {
        i1 = 0;
        i2 = 2;
    } else {
        i1 = 0;
        i2 = 1;
    }
    float u0, u1, u2, w0, w1, w2;
    if (i1 == 0) {
        u0 = p.x() - v0.x();
        u1 = v1.x() - v0.x();
        u2 = v2.x() - v0.x();
    } else {
        u0 = p.y() - v0.y();
        u1 = v1.y() - v0.y();
        u2 = v2.y() - v0.y();
    }
    if (i2 == 1) {
        w0 = p.y() - v0.y();
        w1 = v1.y() - v0.y();
        w2 = v2.y() - v0.y();
    } else {
        w0 = p.z() - v0.z();
        w1 = v1.z() - v0.z();
        w2 = v2.z() - v0.z();
    }
    float alpha, beta;
    if (u1 <= RAY_EPSILON && u1 >= -RAY_EPSILON) {
        beta = u0 / u2;
        if (beta >= 0 && beta <= 1)
            alpha = (w0 - beta * w2) / w1;
    } else {
        beta = (w0 * u1 - u0 * w1) / (w2 * u1 - u2 * w1);
        if (beta >= 0 && beta <= 1) {
            if (ABS(w1) > ABS(u1))
                alpha = (w0 - beta * w2) / w1;
            else
                alpha = (u0 - beta * u2) / u1;
        }
    }
    if (alpha >= 0 && beta >= 0 && alpha + beta <= 1)
        return true;
    return false;
}

// 0x1001e8b0 ?CheckRayIntersection@MfTriangleSet@@QAE_NHAAVMfVector3@@0AAMH@Z
bool MfTriangleSet::CheckRayIntersection(int i, MfVector3 &origin, MfVector3 &direction, float &distance,
                                         int flags)
{
    return CheckRayIntersectionCommon(i, origin, direction, distance, flags,
                                      mVertices[mTriangles[i].m_v[0]], mVertices[mTriangles[i].m_v[1]],
                                      mVertices[mTriangles[i].m_v[2]]);
}

// 0x1001e900 ?CheckRayIntersection@MfTriangleSet@@QAE_NHAAVMfVector3@@0AAVMfTransform3@@AAMH@Z
// The same for the set placed by t.
bool MfTriangleSet::CheckRayIntersection(int i, MfVector3 &origin, MfVector3 &direction, MfTransform3 &t,
                                         float &distance, int flags)
{
    MfVector3 a, b, c;
    t.Multiply(a, mVertices[mTriangles[i].m_v[0]]);
    t.Multiply(b, mVertices[mTriangles[i].m_v[1]]);
    t.Multiply(c, mVertices[mTriangles[i].m_v[2]]);
    return CheckRayIntersectionCommon(i, origin, direction, distance, flags, a, b, c);
}

// EQUIV 0x1001d940 ?FindBoundingSphere@MfTriangleSet@@SAXPAVMfVector3@@HAAV2@AAM@Z: reviewed: stack slots of the extreme points and temporaries only, consistently renamed; same x87 sequence
// Ritter's bounding sphere (Graphics Gems I): a sphere through the pair of
// extreme points that lie farthest apart, grown to take in every point.
void MfTriangleSet::FindBoundingSphere(MfVector3 *vertices, int n, MfVector3 &center, float &radius)
{
    if (!vertices)
        return;
    MfVector3 p;
    MfVector3 xmin, xmax, ymin, ymax, zmin, zmax;
    xmin = xmax = ymin = ymax = zmin = zmax = vertices[0];
    int i;
    for (i = 1; i < n; i++) {
        p = vertices[i];
        if (p.x() < xmin.x())
            xmin = p;
        if (p.x() > xmax.x())
            xmax = p;
        if (p.y() < ymin.y())
            ymin = p;
        if (p.y() > ymax.y())
            ymax = p;
        if (p.z() < zmin.z())
            zmin = p;
        if (p.z() > zmax.z())
            zmax = p;
    }
    float xspan = (xmax - xmin).lengthSquared();
    float yspan = (ymax - ymin).lengthSquared();
    float zspan = (zmax - zmin).lengthSquared();
    MfVector3 dia1, dia2;
    if (xspan > yspan && xspan > zspan) {
        dia1 = xmin;
        dia2 = xmax;
    } else if (yspan > zspan) {
        dia1 = ymin;
        dia2 = ymax;
    } else {
        dia1 = zmin;
        dia2 = zmax;
    }
    center = (dia1 + dia2) * 0.5f;
    float radius2 = (dia2 - center).lengthSquared();
    radius = (float)sqrt(radius2);
    for (i = 0; i < n; i++) {
        p = vertices[i];
        float distance2 = (p - center).lengthSquared();
        if (distance2 > radius2) {
            float distance = (float)sqrt(distance2);
            radius = (radius + distance) * 0.5f;
            radius2 = radius * radius;
            center = (center * radius + p * (distance - radius)) / distance;
        }
    }
}

// 0x1001dec0 ?FindOrientedBox@MfTriangleSet@@QAEXAAVMfOrientedBox@@@Z
void MfTriangleSet::FindOrientedBox(MfOrientedBox &box)
{
    FindOrientedBox(mVertices, mNumVertices, box);
}

// EQUIV 0x1001dee0 ?FindOrientedBox@MfTriangleSet@@SAXPAVMfVector3@@HAAVMfOrientedBox@@@Z: reviewed: register choice, stack slots, commutative fmul operand order and square() called through a register vs memory
// A box along the principal axes of the points' covariance.
void MfTriangleSet::FindOrientedBox(MfVector3 *vertices, int n, MfOrientedBox &box)
{
    MfVector3 mean;
    MfVector3 squares;
    float xy = 0, xz = 0, yz = 0;
    MfXformMatrix3 rotation;
    int i;
    for (i = 0; i < n; i++)
        mean += vertices[i];
    float count = (float)n;
    mean /= count;
    for (i = 0; i < n; i++) {
        squares += vertices[i].square();
        xy += vertices[i].x() * vertices[i].y();
        xz += vertices[i].x() * vertices[i].z();
        yz += vertices[i].y() * vertices[i].z();
    }
    squares /= count;
    squares -= mean.square();
    xy = xy / count - mean.x() * mean.y();
    xz = xz / count - mean.x() * mean.z();
    yz = yz / count - mean.y() * mean.z();
    MfMatrix3 covariance;
    covariance[0][0] = squares.x();
    covariance[0][1] = xy;
    covariance[0][2] = xz;
    covariance[1][0] = xy;
    covariance[1][1] = squares.y();
    covariance[1][2] = yz;
    covariance[2][0] = xz;
    covariance[2][1] = yz;
    covariance[2][2] = squares.z();
    MfXformMatrix3 axes;
    covariance.principalAxis(axes);
    rotation = ~axes;
    MfVector3 p, lo, hi;
    rotation.multiply(vertices[0], lo);
    hi = lo;
    for (i = 1; i < n; i++) {
        rotation.multiply(vertices[i], p);
        if (p.x() < lo.x())
            lo.x() = p.x();
        if (p.y() < lo.y())
            lo.y() = p.y();
        if (p.z() < lo.z())
            lo.z() = p.z();
        if (p.x() > hi.x())
            hi.x() = p.x();
        if (p.y() > hi.y())
            hi.y() = p.y();
        if (p.z() > hi.z())
            hi.z() = p.z();
    }
    box.m_extent = hi - lo;
    rotation.setTranslation(rotation * ((hi + lo) * 0.5f));
    box.m_transform = MfTransform3(rotation);
}

// 0x1001ea20 ?PointSegmentDistance2@@YAMAAVMfVector3@@00AAM@Z
// The squared distance of p from the segment a-b; t gets the squared length of
// the projection of p - a onto the segment (clamped to the segment).
static float PointSegmentDistance2(MfVector3 &p, MfVector3 &a, MfVector3 &b, float &t)
{
    MfVector3 e = b - a;
    MfVector3 w = p - a;
    float ee = e.lengthSquared();
    float ww = w.lengthSquared();
    float c = w.dot(e);
    float projection = c * c / ee;
    float distance2 = ww - projection;
    t = projection;
    if (c < 0) {
        t = 0;
        distance2 = ww;
    } else if (projection > ee) {
        w = p - b;
        t = ee;
        distance2 = w.lengthSquared();
    }
    return distance2;
}

// 0x1001ec70 ?CheckSphereIntersection@MfTriangleSet@@QAE_NHAAVMfVector3@@M0AAM@Z
// Whether the sphere touches triangle i: point gets the contact point, depth
// the penetration (negative).
bool MfTriangleSet::CheckSphereIntersection(int i, MfVector3 &center, float radius, MfVector3 &point,
                                            float &depth)
{
    MfVector3 n;
    MfVector3 v0;
    MfVector3 d;
    n = FindTriangleNormal(i);
    v0 = mVertices[mTriangles[i].m_v[0]];
    d = v0 - center;
    if (d.dot(n) < 0)
        n = -n;
    depth = 3.402823466e+38f;  // FLT_MAX
    float distance;
    if (CheckRayIntersection(i, center, n, depth, 0)) {
        distance = depth;
        if (distance < radius) {
            point = center + n * distance;
            depth -= radius;
            return true;
        }
        return false;
    }
    if (depth > radius)
        return false;
    distance = depth;
    float r2 = radius * radius - distance * distance;
    MfVector3 q = center + n * distance;
    point = q;
    MfVector3 v1, v2;
    float t;
    float distance2;
    v1 = mVertices[mTriangles[i].m_v[1]];
    if ((distance2 = PointSegmentDistance2(q, v0, v1, t)) < r2) {
        depth = MfMath::sqrt(depth * depth + distance2) - radius;
        MfVector3 e = v1 - v0;
        e.normalize();
        point = v0 + e * MfMath::sqrt(t);
        return true;
    }
    v2 = mVertices[mTriangles[i].m_v[2]];
    if ((distance2 = PointSegmentDistance2(q, v0, v2, t)) < r2) {
        depth = MfMath::sqrt(depth * depth + distance2) - radius;
        MfVector3 e = v2 - v0;
        e.normalize();
        point = v0 + e * MfMath::sqrt(t);
        return true;
    }
    if ((distance2 = PointSegmentDistance2(q, v1, v2, t)) < r2) {
        depth = MfMath::sqrt(depth * depth + distance2) - radius;
        MfVector3 e = v2 - v1;
        e.normalize();
        point = v1 + e * MfMath::sqrt(t);
        return true;
    }
    return false;
}

// 0x1001eb90 ?CheckSphereIntersection@MfTriangleSet@@QAE_NHAAVMfVector3@@MAAVMfTransform3@@0AAM@Z
// The same for the set placed by t: the sphere is moved into the set's frame
// and the contact point back.
bool MfTriangleSet::CheckSphereIntersection(int i, MfVector3 &center, float radius, MfTransform3 &t,
                                            MfVector3 &point, float &depth)
{
    MfVector3 localCenter;
    MfVector3 localPoint;
    (~t).Multiply(localCenter, center);
    bool hit = CheckSphereIntersection(i, localCenter, radius, localPoint, depth);
    if (hit)
        t.Multiply(point, localPoint);
    return hit;
}

// The triangle-triangle overlap test of Gottschalk's RAPID library (tri.cpp):
// the triangles are projected onto 17 axes, the two normals, the nine edge
// cross products and the six in-plane edge normals; they overlap if no axis
// separates them.

// 0x100205d0 ?VcrossV@@YAXPAM00@Z
static void VcrossV(float *r, float *a, float *b)
{
    r[0] = a[1] * b[2] - a[2] * b[1];
    r[1] = a[2] * b[0] - a[0] * b[2];
    r[2] = a[0] * b[1] - a[1] * b[0];
}

// 0x10020620 ?VdotV@@YAMPAM0@Z
static float VdotV(float *a, float *b)
{
    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

// 0x10020650 ?max3@@YAMMMM@Z
static float max3(float a, float b, float c)
{
    float t = a;
    if (b > t)
        t = b;
    if (c > t)
        t = c;
    return t;
}

// 0x10020680 ?min3@@YAMMMM@Z
static float min3(float a, float b, float c)
{
    float t = a;
    if (b < t)
        t = b;
    if (c < t)
        t = c;
    return t;
}

// 0x100206b0 ?project6@@YAHPAM000000@Z (the copy VC5 calls when its inline budget is used up)
inline int project6(float *ax, float *p1, float *p2, float *p3, float *q1, float *q2, float *q3)
{
    float P1 = VdotV(ax, p1);
    float P2 = VdotV(ax, p2);
    float P3 = VdotV(ax, p3);
    float Q1 = VdotV(ax, q1);
    float Q2 = VdotV(ax, q2);
    float Q3 = VdotV(ax, q3);
    float mx1 = max3(P1, P2, P3);
    float mn1 = min3(P1, P2, P3);
    float mx2 = max3(Q1, Q2, Q3);
    float mn2 = min3(Q1, Q2, Q3);
    if (mn1 > mx2 || mn2 > mx1)
        return 0;
    return 1;
}

// 0x1001f200 ?tri_contact@@YA_NPAM00000@Z
bool tri_contact(float *P1, float *P2, float *P3, float *Q1, float *Q2, float *Q3)
{
    float p1[3], p2[3], p3[3];
    float q1[3], q2[3], q3[3];
    float e1[3], e2[3], e3[3];
    float f1[3], f2[3], f3[3];
    float g1[3], g2[3], g3[3];
    float h1[3], h2[3], h3[3];
    float n1[3], m1[3];
    float ef11[3], ef12[3], ef13[3];
    float ef21[3], ef22[3], ef23[3];
    float ef31[3], ef32[3], ef33[3];

    p1[0] = P1[0] - P1[0];  p1[1] = P1[1] - P1[1];  p1[2] = P1[2] - P1[2];
    p2[0] = P2[0] - P1[0];  p2[1] = P2[1] - P1[1];  p2[2] = P2[2] - P1[2];
    p3[0] = P3[0] - P1[0];  p3[1] = P3[1] - P1[1];  p3[2] = P3[2] - P1[2];

    q1[0] = Q1[0] - P1[0];  q1[1] = Q1[1] - P1[1];  q1[2] = Q1[2] - P1[2];
    q2[0] = Q2[0] - P1[0];  q2[1] = Q2[1] - P1[1];  q2[2] = Q2[2] - P1[2];
    q3[0] = Q3[0] - P1[0];  q3[1] = Q3[1] - P1[1];  q3[2] = Q3[2] - P1[2];

    e1[0] = p2[0] - p1[0];  e1[1] = p2[1] - p1[1];  e1[2] = p2[2] - p1[2];
    e2[0] = p3[0] - p2[0];  e2[1] = p3[1] - p2[1];  e2[2] = p3[2] - p2[2];
    e3[0] = p1[0] - p3[0];  e3[1] = p1[1] - p3[1];  e3[2] = p1[2] - p3[2];

    f1[0] = q2[0] - q1[0];  f1[1] = q2[1] - q1[1];  f1[2] = q2[2] - q1[2];
    f2[0] = q3[0] - q2[0];  f2[1] = q3[1] - q2[1];  f2[2] = q3[2] - q2[2];
    f3[0] = q1[0] - q3[0];  f3[1] = q1[1] - q3[1];  f3[2] = q1[2] - q3[2];

    VcrossV(n1, e1, e2);
    VcrossV(m1, f1, f2);

    VcrossV(g1, e1, n1);
    VcrossV(g2, e2, n1);
    VcrossV(g3, e3, n1);
    VcrossV(h1, f1, m1);
    VcrossV(h2, f2, m1);
    VcrossV(h3, f3, m1);

    VcrossV(ef11, e1, f1);
    VcrossV(ef12, e1, f2);
    VcrossV(ef13, e1, f3);
    VcrossV(ef21, e2, f1);
    VcrossV(ef22, e2, f2);
    VcrossV(ef23, e2, f3);
    VcrossV(ef31, e3, f1);
    VcrossV(ef32, e3, f2);
    VcrossV(ef33, e3, f3);

    if (!project6(n1, p1, p2, p3, q1, q2, q3)) return false;
    if (!project6(m1, p1, p2, p3, q1, q2, q3)) return false;

    if (!project6(ef11, p1, p2, p3, q1, q2, q3)) return false;
    if (!project6(ef12, p1, p2, p3, q1, q2, q3)) return false;
    if (!project6(ef13, p1, p2, p3, q1, q2, q3)) return false;
    if (!project6(ef21, p1, p2, p3, q1, q2, q3)) return false;
    if (!project6(ef22, p1, p2, p3, q1, q2, q3)) return false;
    if (!project6(ef23, p1, p2, p3, q1, q2, q3)) return false;
    if (!project6(ef31, p1, p2, p3, q1, q2, q3)) return false;
    if (!project6(ef32, p1, p2, p3, q1, q2, q3)) return false;
    if (!project6(ef33, p1, p2, p3, q1, q2, q3)) return false;

    if (!project6(g1, p1, p2, p3, q1, q2, q3)) return false;
    if (!project6(g2, p1, p2, p3, q1, q2, q3)) return false;
    if (!project6(g3, p1, p2, p3, q1, q2, q3)) return false;
    if (!project6(h1, p1, p2, p3, q1, q2, q3)) return false;
    if (!project6(h2, p1, p2, p3, q1, q2, q3)) return false;
    if (!project6(h3, p1, p2, p3, q1, q2, q3)) return false;

    return true;
}

// 0x100207b0 ?CheckIntersection@MfTriangleSet@@QAE_NHAAVMfTransform3@@PAV1@H0@Z
// Whether triangle i of this set (placed by t) and triangle j of set (placed
// by setTransform) intersect.
bool MfTriangleSet::CheckIntersection(int i, MfTransform3 &t, MfTriangleSet *set, int j,
                                      MfTransform3 &setTransform)
{
    MfVector3 v;
    float a0[3], a1[3], a2[3], b0[3], b1[3], b2[3];
    t.Multiply(v, mVertices[mTriangles[i].m_v[0]]);
    a0[0] = v.x(); a0[1] = v.y(); a0[2] = v.z();
    t.Multiply(v, mVertices[mTriangles[i].m_v[1]]);
    a1[0] = v.x(); a1[1] = v.y(); a1[2] = v.z();
    t.Multiply(v, mVertices[mTriangles[i].m_v[2]]);
    a2[0] = v.x(); a2[1] = v.y(); a2[2] = v.z();
    setTransform.Multiply(v, set->mVertices[set->mTriangles[j].m_v[0]]);
    b0[0] = v.x(); b0[1] = v.y(); b0[2] = v.z();
    setTransform.Multiply(v, set->mVertices[set->mTriangles[j].m_v[1]]);
    b1[0] = v.x(); b1[1] = v.y(); b1[2] = v.z();
    setTransform.Multiply(v, set->mVertices[set->mTriangles[j].m_v[2]]);
    b2[0] = v.x(); b2[1] = v.y(); b2[2] = v.z();
    return tri_contact(a0, a1, a2, b0, b1, b2);
}

// 0x10020980 ?fn_10020980@@YAPAVMfStreamObject@@XZ stream constructor for 'TrS2'
static MfStreamObject *fn_10020980()
{
    return new MfTriangleSet;
}

// 0x100209e0 _$E47 (jumps to _$E46)
// 0x100209f0 _$E46 (constructs the stream constructor entry below)
static MfStreamConstructorEntry s_entry(MfTriangleSet::STREAM_TYPE, fn_10020980);

// 0x10020a10 ?StreamType@MfTriangleSet@@UBEJXZ
long MfTriangleSet::StreamType() const
{
    return STREAM_TYPE;
}

// 0x10020a20 ?ReadFromStream@MfTriangleSet@@UAE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfTriangleSet::ReadFromStream(MfStream &s)
{
    int type, flags, numVertices, numTriangles, numTextures, numMaterials;
    s >> type >> flags >> numVertices >> numTriangles >> numTextures >> numMaterials;
    Create((MfTriangleSetType)type, flags, numTriangles, numVertices, numTextures);
    SetNumMaterials(numMaterials);
    s.ReadBytes(mNumVertices * sizeof(MfVector3), mVertices);
    if (mFlags & MF_TRISET_NORMALS)
        s.ReadBytes(mNumVertices * sizeof(MfVector3), mNormals);
    int i;
    if (mFlags & MF_TRISET_COLORS) {
        for (i = 0; i < mNumVertices; i++)
            s >> mColors[i];
    }
    if (mFlags & MF_TRISET_TEXTURES)
        s.ReadBytes(mNumTexturesPerVertex * mNumVertices * sizeof(MfTextureVertex), mTextureVertices);
    if (mType == MF_TRIANGLE_LIST)
        s.ReadBytes(mNumTriangles * sizeof(MfTriangle), mTriangles);
    if (mFlags & MF_TRISET_MATERIALS)
        s.ReadBytes(mNumTriangles * sizeof(short), mMaterialIndices);
    for (i = 0; i < mNumMaterials; i++)
        s >> mMaterials[i];
    return s.Status();
}

// 0x10020b80 ?WriteToStream@MfTriangleSet@@UBE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfTriangleSet::WriteToStream(MfStream &s) const
{
    s << mType << mFlags << mNumVertices << mNumTriangles;
    s << mNumTexturesPerVertex << mNumMaterials;
    s.WriteBytes(mNumVertices * sizeof(MfVector3), mVertices);
    if (mFlags & MF_TRISET_NORMALS)
        s.WriteBytes(mNumVertices * sizeof(MfVector3), mNormals);
    int i;
    if (mFlags & MF_TRISET_COLORS) {
        for (i = 0; i < mNumVertices; i++)
            s << mColors[i];
    }
    if (mFlags & MF_TRISET_TEXTURES)
        s.WriteBytes(mNumTexturesPerVertex * mNumVertices * sizeof(MfTextureVertex), mTextureVertices);
    if (mType == MF_TRIANGLE_LIST)
        s.WriteBytes(mNumTriangles * sizeof(MfTriangle), mTriangles);
    if (mFlags & MF_TRISET_MATERIALS)
        s.WriteBytes(mNumTriangles * sizeof(short), mMaterialIndices);
    for (i = 0; i < mNumMaterials; i++)
        s << mMaterials[i];
    return s.Status();
}

// 0x10020cb0 ?PointTriDistance@MfTriangleSet@@IAE_NHAAVMfVector3@@AAVMfTransform3@@0AAM@Z
// If point is nearer than sqrt(distance2) to triangle i (placed by t) along
// the triangle's normal, stores the nearest point and the squared distance.
bool MfTriangleSet::PointTriDistance(int i, MfVector3 &point, MfTransform3 &t, MfVector3 &nearest,
                                     float &distance2)
{
    MfVector3 normal;
    MfVector3 v;
    MfVector3 d;
    normal = t.rotation() * FindTriangleNormal(i);
    t.Multiply(v, mVertices[mTriangles[i].m_v[0]]);
    d = v - point;
    if (d.dot(normal) < 0)
        normal = -normal;
    float distance = 3.402823466e+38f;  // FLT_MAX
    if (CheckRayIntersection(i, point, normal, t, distance, 0)) {
        if (distance * distance < distance2) {
            distance2 = distance * distance;
            nearest = point + normal * distance;
            return true;
        }
    }
    return false;
}

// 0x10022ac0 ?PointSegmentDistance2@@YAMAAVMfVector3@@000@Z (out-of-line copy)
// The squared distance of p from the segment a-b; nearest gets the nearest
// point when it lies inside the segment.
inline float PointSegmentDistance2(MfVector3 &p, MfVector3 &a, MfVector3 &b, MfVector3 &nearest)
{
    MfVector3 pa = p - a;
    MfVector3 pb = p - b;
    MfVector3 dir = b - a;
    dir.normalize();
    if (pa.dot(dir) <= 0)
        return pa.lengthSquared();
    if (pb.dot(dir) >= 0)
        return pb.lengthSquared();
    float t = pa.dot(dir);
    nearest = a + dir * t;
    return pa.lengthSquared() - t * t;
}

// 0x10022c90 ?SegmentDistance2@@YAMAAVMfVector3@@00000@Z (out-of-line copy)
// The squared distance of the nearest points pa, pb of the segments a0-a1 and
// b0-b1, or -1 if they do not lie inside both segments.
inline float SegmentDistance2(MfVector3 &a0, MfVector3 &a1, MfVector3 &b0, MfVector3 &b1, MfVector3 &pa,
                              MfVector3 &pb)
{
    MfVector3 da = a1 - a0;
    da.normalize();
    MfVector3 db = b1 - b0;
    db.normalize();
    float ta = da.dot(b0) - da.dot(a0);
    float tb = db.dot(b0) - db.dot(a0);
    float c = da.dot(db);
    float sa, sb;
    if (c < RAY_EPSILON && c > -RAY_EPSILON) {
        sa = ta;
        sb = -tb;
    } else {
        sb = (tb - c * ta) / (c * c - 1);
        sa = ta + sb * c;
    }
    if (sa * sa > (a1 - a0).lengthSquared() || sa < 0)
        return -1.0f;
    if (sb * sb > (b1 - b0).lengthSquared() || sb < 0)
        return -1.0f;
    pa = a0 + da * sa;
    pb = b0 + db * sb;
    return (pa - pb).lengthSquared();
}


// EQUIV 0x10020e80 ?FindDistance@MfTriangleSet@@QAEMHAAVMfTransform3@@PAV1@H0AAVMfVector3@@2@Z: reviewed: MfVector3::operator= of one copy inlined vs called; order of independent stores; register choice
// The distance of triangle i of this set (placed by t) from triangle j of set
// (placed by setTransform), -1 if they intersect; p1 and p2 get the nearest
// points. Vertex pairs, vertices against edges, edge pairs and vertices against
// the other triangle are tried in turn.
float MfTriangleSet::FindDistance(int i, MfTransform3 &t, MfTriangleSet *set, int j,
                                  MfTransform3 &setTransform, MfVector3 &p1, MfVector3 &p2)
{
    if (CheckIntersection(i, t, set, j, setTransform))
        return -1.0f;
    float best = FLT_MAX;
    MfVector3 a0, a1, a2;
    t.Multiply(a0, mVertices[mTriangles[i].m_v[0]]);
    t.Multiply(a1, mVertices[mTriangles[i].m_v[1]]);
    t.Multiply(a2, mVertices[mTriangles[i].m_v[2]]);
    MfVector3 b0, b1, b2;
    setTransform.Multiply(b0, set->mVertices[set->mTriangles[j].m_v[0]]);
    setTransform.Multiply(b1, set->mVertices[set->mTriangles[j].m_v[1]]);
    setTransform.Multiply(b2, set->mVertices[set->mTriangles[j].m_v[2]]);
    MfVector3 x, y;
    float d;
    d = (a0 - b0).lengthSquared();
    if (d < best) {
        best = d;
        p1 = a0;
        p2 = b0;
    }
    d = (a0 - b1).lengthSquared();
    if (d < best) {
        best = d;
        p1 = a0;
        p2 = b1;
    }
    d = (a0 - b2).lengthSquared();
    if (d < best) {
        best = d;
        p1 = a0;
        p2 = b2;
    }
    d = (a1 - b0).lengthSquared();
    if (d < best) {
        best = d;
        p1 = a1;
        p2 = b0;
    }
    d = (a1 - b1).lengthSquared();
    if (d < best) {
        best = d;
        p1 = a1;
        p2 = b1;
    }
    d = (a1 - b2).lengthSquared();
    if (d < best) {
        best = d;
        p1 = a1;
        p2 = b2;
    }
    d = (a2 - b0).lengthSquared();
    if (d < best) {
        best = d;
        p1 = a2;
        p2 = b0;
    }
    d = (a2 - b1).lengthSquared();
    if (d < best) {
        best = d;
        p1 = a2;
        p2 = b1;
    }
    d = (a2 - b2).lengthSquared();
    if (d < best) {
        best = d;
        p1 = a2;
        p2 = b2;
    }
    if (best < 1e-4f)
        return (float)sqrt(best);
    d = PointSegmentDistance2(a0, b0, b1, x);
    if (d < best) {
        best = d;
        p1 = a0;
        p2 = x;
    }
    d = PointSegmentDistance2(a0, b1, b2, x);
    if (d < best) {
        best = d;
        p1 = a0;
        p2 = x;
    }
    d = PointSegmentDistance2(a0, b2, b0, x);
    if (d < best) {
        best = d;
        p1 = a0;
        p2 = x;
    }
    d = PointSegmentDistance2(a1, b0, b1, x);
    if (d < best) {
        best = d;
        p1 = a1;
        p2 = x;
    }
    d = PointSegmentDistance2(a1, b1, b2, x);
    if (d < best) {
        best = d;
        p1 = a1;
        p2 = x;
    }
    d = PointSegmentDistance2(a1, b2, b0, x);
    if (d < best) {
        best = d;
        p1 = a1;
        p2 = x;
    }
    d = PointSegmentDistance2(a2, b0, b1, x);
    if (d < best) {
        best = d;
        p1 = a2;
        p2 = x;
    }
    d = PointSegmentDistance2(a2, b1, b2, x);
    if (d < best) {
        best = d;
        p1 = a2;
        p2 = x;
    }
    d = PointSegmentDistance2(a2, b2, b0, x);
    if (d < best) {
        best = d;
        p1 = a2;
        p2 = x;
    }
    d = PointSegmentDistance2(b0, a0, a1, x);
    if (d < best) {
        best = d;
        p2 = b0;
        p1 = x;
    }
    d = PointSegmentDistance2(b0, a1, a2, x);
    if (d < best) {
        best = d;
        p2 = b0;
        p1 = x;
    }
    d = PointSegmentDistance2(b0, a2, a0, x);
    if (d < best) {
        best = d;
        p2 = b0;
        p1 = x;
    }
    d = PointSegmentDistance2(b1, a0, a1, x);
    if (d < best) {
        best = d;
        p2 = b1;
        p1 = x;
    }
    d = PointSegmentDistance2(b1, a1, a2, x);
    if (d < best) {
        best = d;
        p2 = b1;
        p1 = x;
    }
    d = PointSegmentDistance2(b1, a2, a0, x);
    if (d < best) {
        best = d;
        p2 = b1;
        p1 = x;
    }
    d = PointSegmentDistance2(b2, a0, a1, x);
    if (d < best) {
        best = d;
        p2 = b2;
        p1 = x;
    }
    d = PointSegmentDistance2(b2, a1, a2, x);
    if (d < best) {
        best = d;
        p2 = b2;
        p1 = x;
    }
    d = PointSegmentDistance2(b2, a2, a0, x);
    if (d < best) {
        best = d;
        p2 = b2;
        p1 = x;
    }
    if (best < 1e-4f)
        return (float)sqrt(best);
    d = SegmentDistance2(a0, a1, b0, b1, x, y);
    if (d < best && d >= 0) {
        best = d;
        p1 = x;
        p2 = y;
    }
    d = SegmentDistance2(a0, a1, b1, b2, x, y);
    if (d < best && d >= 0) {
        best = d;
        p1 = x;
        p2 = y;
    }
    d = SegmentDistance2(a0, a1, b2, b0, x, y);
    if (d < best && d >= 0) {
        best = d;
        p1 = x;
        p2 = y;
    }
    d = SegmentDistance2(a1, a2, b0, b1, x, y);
    if (d < best && d >= 0) {
        best = d;
        p1 = x;
        p2 = y;
    }
    d = SegmentDistance2(a1, a2, b1, b2, x, y);
    if (d < best && d >= 0) {
        best = d;
        p1 = x;
        p2 = y;
    }
    d = SegmentDistance2(a1, a2, b2, b0, x, y);
    if (d < best && d >= 0) {
        best = d;
        p1 = x;
        p2 = y;
    }
    d = SegmentDistance2(a2, a0, b0, b1, x, y);
    if (d < best && d >= 0) {
        best = d;
        p1 = x;
        p2 = y;
    }
    d = SegmentDistance2(a2, a0, b1, b2, x, y);
    if (d < best && d >= 0) {
        best = d;
        p1 = x;
        p2 = y;
    }
    d = SegmentDistance2(a2, a0, b2, b0, x, y);
    if (d < best && d >= 0) {
        best = d;
        p1 = x;
        p2 = y;
    }
    if (best < 1e-4f)
        return (float)sqrt(best);
    if (PointTriDistance(i, b0, t, p1, best))
        p2 = b0;
    if (PointTriDistance(i, b1, t, p1, best))
        p2 = b1;
    if (PointTriDistance(i, b2, t, p1, best))
        p2 = b2;
    if (set->PointTriDistance(j, a0, setTransform, p2, best))
        p1 = a0;
    if (set->PointTriDistance(j, a1, setTransform, p2, best))
        p1 = a1;
    if (set->PointTriDistance(j, a2, setTransform, p2, best))
        p1 = a2;
    return (float)sqrt(best);
}

// EQUIV 0x10022fe0 ?CheckIntersection@MfOrientedBox@@QAE_NPAV1@AAVMfTransform3@@1@Z: reviewed: (75% similar) the original expands the MfVector3 constructor inside the inline MfTransform3() of t1, t2 and rel, ours calls the import (VC5 stops expanding it once the function holds more than two of the Bf statements); the EH states follow from that. Every x87 expression tree and compare is the original's (checked symbolically; review 2026-10-16 fixed the B1 x B2 = B0 sum grouping), one independent store/compare pair is swapped, r is in edx instead of ecx
// Whether this box (placed by transform) overlaps box (placed by
// boxTransform): the separating axis test of RAPID's obb_disjoint (Gottschalk, UNC), with the 15
// candidate axes tried in its order. m_extent holds full sizes.
bool MfOrientedBox::CheckIntersection(MfOrientedBox *box, MfTransform3 &transform, MfTransform3 &boxTransform)
{
    MfTransform3 t1, t2;
    t1.Multiply(transform, m_transform);
    t2.Multiply(boxTransform, box->m_transform);
    MfTransform3 rel;
    rel.Multiply(~t1, t2);
    MfXformMatrix3 B(rel.rotation(), rel.translation());   // T is column 3
    float a[3], b[3];
    a[0] = m_extent[0] * 0.5f;
    a[1] = m_extent[1] * 0.5f;
    a[2] = m_extent[2] * 0.5f;
    b[0] = box->m_extent[0] * 0.5f;
    b[1] = box->m_extent[1] * 0.5f;
    b[2] = box->m_extent[2] * 0.5f;
    float Bf[3][3];
    float t, s;
    int r;
    const float reps = 1e-6f;
    Bf[0][0] = ABS(B[0][0]) + reps;
    Bf[0][1] = ABS(B[0][1]) + reps;
    Bf[0][2] = ABS(B[0][2]) + reps;
    Bf[1][0] = ABS(B[1][0]) + reps;
    Bf[1][1] = ABS(B[1][1]) + reps;
    Bf[1][2] = ABS(B[1][2]) + reps;
    Bf[2][0] = ABS(B[2][0]) + reps;
    Bf[2][1] = ABS(B[2][1]) + reps;
    Bf[2][2] = ABS(B[2][2]) + reps;

    r = 1;

    // A1 x A2 = A0
    t = ABS(B[0][3]);
    r &= (t <= (a[0] + b[0] * Bf[0][0] + b[1] * Bf[0][1] + b[2] * Bf[0][2]));
    if (!r)
        return false;

    // B1 x B2 = B0
    s = (B[0][3] * B[0][0] + B[1][3] * B[1][0]) + B[2][3] * B[2][0];
    t = ABS(s);
    r &= (t <= (b[0] + a[0] * Bf[0][0] + a[1] * Bf[1][0] + a[2] * Bf[2][0]));
    if (!r)
        return false;

    // A2 x A0 = A1
    t = ABS(B[1][3]);
    r &= (t <= (a[1] + b[0] * Bf[1][0] + b[1] * Bf[1][1] + b[2] * Bf[1][2]));
    if (!r)
        return false;

    // A0 x A1 = A2
    t = ABS(B[2][3]);
    r &= (t <= (a[2] + b[0] * Bf[2][0] + b[1] * Bf[2][1] + b[2] * Bf[2][2]));
    if (!r)
        return false;

    // B2 x B0 = B1
    s = B[0][3] * B[0][1] + B[1][3] * B[1][1] + B[2][3] * B[2][1];
    t = ABS(s);
    r &= (t <= (b[1] + a[0] * Bf[0][1] + a[1] * Bf[1][1] + a[2] * Bf[2][1]));
    if (!r)
        return false;

    // B0 x B1 = B2
    s = B[0][3] * B[0][2] + B[1][3] * B[1][2] + B[2][3] * B[2][2];
    t = ABS(s);
    r &= (t <= (b[2] + a[0] * Bf[0][2] + a[1] * Bf[1][2] + a[2] * Bf[2][2]));
    if (!r)
        return false;

    // A0 x B0
    s = B[2][3] * B[1][0] - B[1][3] * B[2][0];
    t = ABS(s);
    r &= (t <= (a[1] * Bf[2][0] + a[2] * Bf[1][0] + b[1] * Bf[0][2] + b[2] * Bf[0][1]));
    if (!r)
        return false;

    // A0 x B1
    s = B[2][3] * B[1][1] - B[1][3] * B[2][1];
    t = ABS(s);
    r &= (t <= (a[1] * Bf[2][1] + a[2] * Bf[1][1] + b[0] * Bf[0][2] + b[2] * Bf[0][0]));
    if (!r)
        return false;

    // A0 x B2
    s = B[2][3] * B[1][2] - B[1][3] * B[2][2];
    t = ABS(s);
    r &= (t <= (a[1] * Bf[2][2] + a[2] * Bf[1][2] + b[0] * Bf[0][1] + b[1] * Bf[0][0]));
    if (!r)
        return false;

    // A1 x B0
    s = B[0][3] * B[2][0] - B[2][3] * B[0][0];
    t = ABS(s);
    r &= (t <= (a[0] * Bf[2][0] + a[2] * Bf[0][0] + b[1] * Bf[1][2] + b[2] * Bf[1][1]));
    if (!r)
        return false;

    // A1 x B1
    s = B[0][3] * B[2][1] - B[2][3] * B[0][1];
    t = ABS(s);
    r &= (t <= (a[0] * Bf[2][1] + a[2] * Bf[0][1] + b[0] * Bf[1][2] + b[2] * Bf[1][0]));
    if (!r)
        return false;

    // A1 x B2
    s = B[0][3] * B[2][2] - B[2][3] * B[0][2];
    t = ABS(s);
    r &= (t <= (a[0] * Bf[2][2] + a[2] * Bf[0][2] + b[0] * Bf[1][1] + b[1] * Bf[1][0]));
    if (!r)
        return false;

    // A2 x B0
    s = B[1][3] * B[0][0] - B[0][3] * B[1][0];
    t = ABS(s);
    r &= (t <= (a[0] * Bf[1][0] + a[1] * Bf[0][0] + b[1] * Bf[2][2] + b[2] * Bf[2][1]));
    if (!r)
        return false;

    // A2 x B1
    s = B[1][3] * B[0][1] - B[0][3] * B[1][1];
    t = ABS(s);
    r &= (t <= (a[0] * Bf[1][1] + a[1] * Bf[0][1] + b[0] * Bf[2][2] + b[2] * Bf[2][0]));
    if (!r)
        return false;

    // A2 x B2
    s = B[1][3] * B[0][2] - B[0][3] * B[1][2];
    t = ABS(s);
    r &= (t <= (a[0] * Bf[1][2] + a[1] * Bf[0][2] + b[0] * Bf[2][1] + b[1] * Bf[2][0]));
    return r ? true : false;
}

// EQUIV 0x10023960 ?CheckRayIntersection@MfOrientedBox@@QAE_NAAVMfTransform3@@AAVMfVector3@@1M@Z: reviewed: zero components stored with an x87 load of +0.0 instead of an integer 0 (same bits); order of independent stores
// Whether the ray from origin along dir (at most length long if length > 0)
// meets this box (placed by t): Haines' ray-convex polyhedron test (Graphics
// Gems II) against the six face planes.
bool MfOrientedBox::CheckRayIntersection(MfTransform3 &t, MfVector3 &origin, MfVector3 &dir, float length)
{
    MfVector3 normal[6];
    MfTransform3 xf;
    MfVector3 p;
    float d[6];
    xf.Multiply(t, m_transform);
    normal[0] = xf.rotation() * sXAxis;
    normal[1] -= normal[0];
    normal[2] = xf.rotation() * sYAxis;
    normal[3] -= normal[2];
    normal[4] = xf.rotation() * sZAxis;
    normal[5] -= normal[4];
    p.setValue(m_extent.x() * 0.5f, 0, 0);
    p.Premultiply(xf);
    d[0] = -normal[0].dot(p);
    p.setValue(m_extent.x() * -0.5f, 0, 0);
    p.Premultiply(xf);
    d[1] = -normal[1].dot(p);
    p.setValue(0, m_extent.y() * 0.5f, 0);
    p.Premultiply(xf);
    d[2] = -normal[2].dot(p);
    p.setValue(0, m_extent.y() * -0.5f, 0);
    p.Premultiply(xf);
    d[3] = -normal[3].dot(p);
    p.setValue(0, 0, m_extent.z() * 0.5f);
    p.Premultiply(xf);
    d[4] = -normal[4].dot(p);
    p.setValue(0, 0, m_extent.z() * -0.5f);
    p.Premultiply(xf);
    d[5] = -normal[5].dot(p);

    float tnear = -FLT_MAX;
    float tfar;
    if (length > 0)
        tfar = length;
    else
        tfar = FLT_MAX;
    for (int i = 0; i < 6; i++) {
        float vn = normal[i].dot(origin) + d[i];
        float vd = normal[i].dot(dir);
        if (ABS(vd) < RAY_EPSILON) {
            if (vn > 0)
                return false;
        } else {
            float s = -vn / vd;
            if (vd > 0) {
                if (s < 0)
                    return false;
                if (s < tfar)
                    tfar = s;
            } else {
                if (s > tnear)
                    tnear = s;
            }
        }
        if (tnear > tfar)
            return false;
    }
    return true;
}

#define PI 3.1415927f

// 0x10023e00 ??0MfTriSquare@@QAE@ABVMfMaterial@@MM@Z
// A width x height square in the xy plane, facing +z, textured once.
MfTriSquare::MfTriSquare(const MfMaterial &material, float width, float height)
{
    Create(MF_TRIANGLE_LIST, MF_TRISET_NORMALS | MF_TRISET_TEXTURES | MF_TRISET_MATERIALS, 2, 4, 1);
    width *= 0.5f;
    height *= 0.5f;
    Vertex(0).setValue(-width, -height, 0);
    Vertex(1).setValue(width, -height, 0);
    Vertex(2).setValue(width, height, 0);
    Vertex(3).setValue(-width, height, 0);
    TextureVertex(0, 0).Set(0, 1);
    TextureVertex(1, 0).Set(1, 1);
    TextureVertex(2, 0).Set(1, 0);
    TextureVertex(3, 0).Set(0, 0);
    for (int i = 0; i < 4; i++)
        Normal(i).setValue(0, 0, 1);
    Triangle(0).Set(0, 1, 2);
    Triangle(1).Set(3, 0, 2);
    SetNumMaterials(1);
    Material(0) = material;
    MaterialIndex(1) = 0;
    MaterialIndex(0) = 0;
}

// 0x10024d10 ??0MfTriLine@@QAE@ABVMfMaterial@@ABVMfVector3@@1@Z
MfTriLine::MfTriLine(const MfMaterial &material, const MfVector3 &from, const MfVector3 &to)
{
    int flags = MF_TRISET_MATERIALS;
    if (material.m_numTextures)
        flags = MF_TRISET_TEXTURES | MF_TRISET_MATERIALS;
    Create(MF_LINE_STRIP, flags, 0, 2, material.m_numTextures);
    SetNumMaterials(1);
    Material(0) = material;
    Update(from, to);
}

// 0x10024db0 ?Update@MfTriLine@@QAEXABVMfVector3@@0@Z
void MfTriLine::Update(const MfVector3 &from, const MfVector3 &to)
{
    Vertex(0) = from;
    Vertex(1) = to;
}

// 0x10024df0 ??0MfTriBezier@@QAE@ABVMfMaterial@@QAVMfVector3@@H@Z
// A cubic Bezier curve through steps + 1 points (control points p[0..3]).
MfTriBezier::MfTriBezier(const MfMaterial &material, MfVector3 *const points, int steps)
{
    if (steps == -1)
        steps = 10;
    if (steps < 1)
        steps = 1;
    int flags = MF_TRISET_MATERIALS;
    if (material.m_numTextures)
        flags = MF_TRISET_TEXTURES | MF_TRISET_MATERIALS;
    Create(MF_LINE_STRIP, flags, 0, steps + 1, material.m_numTextures);
    SetNumMaterials(1);
    Material(0) = material;
    Update(points);
}

// 0x10024ea0 ?Update@MfTriBezier@@QAEXQAVMfVector3@@@Z
void MfTriBezier::Update(MfVector3 *const points)
{
    for (int i = 0; i < mNumVertices; i++)
        Vertex(i).interpolate(points[0], points[1], points[2], points[3], (float)i / (mNumVertices - 1));
}

// 0x10024f00 ??0MfTriCircle@@QAE@ABVMfMaterial@@MH@Z
// A circle of the given radius in the xz plane.
MfTriCircle::MfTriCircle(const MfMaterial &material, float radius, int steps)
{
    if (steps == -1)
        steps = 12;
    if (steps < 3)
        steps = 3;
    int flags = MF_TRISET_MATERIALS;
    if (material.m_numTextures)
        flags = MF_TRISET_TEXTURES | MF_TRISET_MATERIALS;
    Create(MF_LINE_STRIP, flags, 0, steps + 1, material.m_numTextures);
    SetNumMaterials(1);
    Material(0) = material;
    for (int i = 0; i < steps; i++) {
        float angle = i * (2 * PI) / steps;
        MfVector3 &v = Vertex(i);
        v.y() = 0;
        v.x() = (float)cos(angle) * radius;
        v.z() = (float)sin(angle) * radius;
    }
    Vertex(steps) = Vertex(0);
}

// 0x10024790 ??0MfTriCone@@QAE@ABVMfMaterial@@MMH@Z
// A cone along y, centred on the origin, tip up; the base is not closed.
MfTriCone::MfTriCone(const MfMaterial &material, float radius, float height, int steps)
{
    if (steps == -1)
        steps = 12;
    if (steps < 3)
        steps = 3;
    if (height < 0.001f)
        height = 0.001f;
    int numVertices = steps + 2;
    int numTriangles = 2 * steps;
    int flags = MF_TRISET_NORMALS | MF_TRISET_MATERIALS;
    if (material.m_numTextures)
        flags = MF_TRISET_NORMALS | MF_TRISET_TEXTURES | MF_TRISET_MATERIALS;
    Create(MF_TRIANGLE_LIST, flags, numTriangles, numVertices, material.m_numTextures);
    float slope = radius / height;
    int i;
    for (i = 0; i < steps; i++) {
        float angle = i * (2 * PI) / steps;
        float c = (float)cos(angle);
        float s = (float)sin(angle);
        Vertex(i).setValue(c * radius, height * -0.5f, s * radius);
        Normal(i).setValue(c, slope, s);
        Normal(i).normalize();
    }
    Vertex(steps).setValue(0, height * 0.5f, 0);
    Normal(steps).setValue(0, 1, 0);
    Vertex(steps + 1).setValue(0, height * -0.5f, 0);
    Normal(steps + 1).setValue(0, -1, 0);
    int t = 0;
    for (i = 0; i < steps; i++) {
        Triangle(t++).Set(i, steps, (i + 1) % steps);
        Triangle(t++).Set(i, (i + 1) % steps, steps + 1);
    }
    SetNumMaterials(1);
    Material(0) = material;
    i = 0;
    while (i < mNumTriangles)
        MaterialIndex(i++) = 0;
}

// 0x10024a30 ??0MfTriCylinder@@QAE@ABVMfMaterial@@MMH@Z
// A closed cylinder along y, centred on the origin.
MfTriCylinder::MfTriCylinder(const MfMaterial &material, float radius, float height, int steps)
{
    if (steps == -1)
        steps = 12;
    if (steps < 3)
        steps = 3;
    int numVertices = 2 * steps + 2;
    int numTriangles = 4 * steps;
    int flags = MF_TRISET_NORMALS | MF_TRISET_MATERIALS;
    if (material.m_numTextures)
        flags = MF_TRISET_NORMALS | MF_TRISET_TEXTURES | MF_TRISET_MATERIALS;
    Create(MF_TRIANGLE_LIST, flags, numTriangles, numVertices, material.m_numTextures);
    int i;
    for (i = 0; i < steps; i++) {
        float angle = i * (2 * PI) / steps;
        float c = (float)cos(angle);
        float s = (float)sin(angle);
        Vertex(i).setValue(c * radius, height * -0.5f, s * radius);
        Normal(i).setValue(c, 0, s);
        Vertex(i + steps).setValue(c * radius, height * 0.5f, s * radius);
        Normal(i + steps).setValue(c, 0, s);
    }
    Vertex(2 * steps).setValue(0, height * -0.5f, 0);
    Normal(2 * steps).setValue(0, -1, 0);
    Vertex(2 * steps + 1).setValue(0, height * 0.5f, 0);
    Normal(2 * steps + 1).setValue(0, 1, 0);
    int t = 0;
    for (i = 0; i < steps; i++) {
        int next = (i + 1) % steps;
        Triangle(t++).Set(i + steps, next + steps, i);
        Triangle(t++).Set(next + steps, next, i);
        Triangle(t++).Set(i, next, 2 * steps);
        Triangle(t++).Set(next + steps, i + steps, 2 * steps + 1);
    }
    SetNumMaterials(1);
    Material(0) = material;
    i = 0;
    while (i < mNumTriangles)
        MaterialIndex(i++) = 0;
}

// EQUIV 0x100243f0 ??0MfTriSphere@@QAE@ABVMfMaterial@@MH@Z: reviewed: register and slot allocation, ring y copied by integer move vs fld/fstp, index arithmetic strength-reduced to the same values
// A sphere: steps / 2 - 1 rings of steps vertices between the two poles
// (vertex 0 at the bottom, the last one at the top).
MfTriSphere::MfTriSphere(const MfMaterial &material, float radius, int steps)
{
    float cosTable[100], sinTable[100];
    if (steps == -1)
        steps = 12;
    if (steps < 4)
        steps = 4;
    if (steps > 100)
        steps = 100;
    int rings = steps / 2;
    int numVertices = (rings - 1) * steps + 2;
    int numTriangles = 2 * (rings - 1) * steps;
    int flags = MF_TRISET_NORMALS | MF_TRISET_MATERIALS;
    if (material.m_numTextures)
        flags = MF_TRISET_NORMALS | MF_TRISET_TEXTURES | MF_TRISET_MATERIALS;
    Create(MF_TRIANGLE_LIST, flags, numTriangles, numVertices, material.m_numTextures);
    Normal(0).setValue(0, -1, 0);
    Normal(mNumVertices - 1).setValue(0, 1, 0);
    int i, j;
    for (i = 0; i < steps; i++) {
        float angle = i * (2 * PI) / steps;
        sinTable[i] = (float)sin(angle);
        cosTable[i] = (float)cos(angle);
    }
    int v = 1;
    for (i = 1; i < rings; i++) {
        float angle = i * PI / rings;
        float y = (float)-cos(angle);
        float r = (float)-sin(angle);
        for (j = 0; j < steps; j++)
            Normal(v++).setValue(cosTable[j] * r, y, sinTable[j] * r);
    }
    for (i = 0; i < mNumVertices; i++)
        Vertex(i) = Normal(i) * radius;
    int t = 0;
    for (i = 0; i < steps; i++)
        Triangle(t++).Set(0, i + 1, (i + 1) % steps + 1);
    int first = 1;
    for (i = 0; i < rings - 2; i++) {
        for (j = 0; j < steps; j++) {
            int next = (j + 1) % steps;
            Triangle(t++).Set(first + j, first + j + steps, first + steps + next);
            Triangle(t++).Set(first + j, first + steps + next, first + next);
        }
        first += steps;
    }
    for (j = 0; j < steps; j++)
        Triangle(t++).Set(mNumVertices - 1, mNumVertices - 1 - steps + (j + 1) % steps,
                          mNumVertices - 1 - steps + j);
    SetNumMaterials(1);
    Material(0) = material;
    i = 0;
    while (i < mNumTriangles)
        MaterialIndex(i++) = 0;
}

// 0x10025030 ??0MfTriRing@@QAE@ABVMfMaterial@@MMMH@Z
// A flat ring (washer) of the given width along y: four vertices per step
// (inner top, inner bottom, outer bottom, outer top).
MfTriRing::MfTriRing(const MfMaterial &material, float inner, float outer, float width, int steps)
{
    if (steps == -1)
        steps = 12;
    if (steps < 3)
        steps = 3;
    int numVertices = 4 * steps;
    int numTriangles = 8 * steps;
    int flags = MF_TRISET_NORMALS | MF_TRISET_MATERIALS;
    if (material.m_numTextures)
        flags = MF_TRISET_NORMALS | MF_TRISET_TEXTURES | MF_TRISET_MATERIALS;
    Create(MF_TRIANGLE_LIST, flags, numTriangles, numVertices, material.m_numTextures);
    int i;
    int v = 0;
    for (i = 0; i < steps; i++) {
        float angle = i * (2 * PI) / steps;
        float c = (float)cos(angle);
        float s = (float)sin(angle);
        Vertex(v).setValue(c * inner, width * 0.5f, s * inner);
        Normal(v++).setValue(0, 1, 0);
        Vertex(v).setValue(c * inner, width * -0.5f, s * inner);
        Normal(v++).setValue(-c, 0, -s);
        Vertex(v).setValue(c * outer, width * -0.5f, s * outer);
        Normal(v++).setValue(0, -1, 0);
        Vertex(v).setValue(c * outer, width * 0.5f, s * outer);
        Normal(v++).setValue(c, 0, s);
    }
    int t = 0;
    v = 0;
    for (i = 0; i < steps; i++) {
        int next = (v + 4) % mNumVertices;
        Triangle(t++).Set(next, next + 3, v);
        Triangle(t++).Set(next + 3, v + 3, v);
        Triangle(t++).Set(next, v, v + 1);
        Triangle(t++).Set(next + 1, next, v + 1);
        Triangle(t++).Set(next + 1, v + 1, v + 2);
        Triangle(t++).Set(next + 2, next + 1, v + 2);
        Triangle(t++).Set(next + 3, next + 2, v + 3);
        Triangle(t++).Set(next + 2, v + 2, v + 3);
        v += 4;
    }
    SetNumMaterials(1);
    Material(0) = material;
    i = 0;
    while (i < mNumTriangles)
        MaterialIndex(i++) = 0;
}

// 0x10023fa0 ??0MfTriCube@@QAE@ABVMfMaterial@@ABVMfVector3@@@Z
// A box of the given size centred on the origin: eight shared corners, each
// carrying the normal of one face.
MfTriCube::MfTriCube(const MfMaterial &material, const MfVector3 &size)
{
    int flags = MF_TRISET_NORMALS | MF_TRISET_MATERIALS;
    if (material.m_numTextures)
        flags = MF_TRISET_NORMALS | MF_TRISET_TEXTURES | MF_TRISET_MATERIALS;
    Create(MF_TRIANGLE_LIST, flags, 12, 8, material.m_numTextures);
    float x = size.x() * 0.5f;
    float y = size.y() * 0.5f;
    float z = size.z() * 0.5f;
    Vertex(0).setValue(-x, -y, z);
    Vertex(1).setValue(x, -y, z);
    Vertex(2).setValue(x, y, z);
    Vertex(3).setValue(-x, y, z);
    Vertex(4).setValue(-x, -y, -z);
    Vertex(5).setValue(x, -y, -z);
    Vertex(6).setValue(x, y, -z);
    Vertex(7).setValue(-x, y, -z);
    int i;
    for (i = 0; i < material.m_numTextures; i++) {
        TextureVertex(0, i).Set(1, 0);
        TextureVertex(1, i).Set(0, 0);
        TextureVertex(2, i).Set(0, 1);
        TextureVertex(3, i).Set(0, 0);
        TextureVertex(4, i).Set(1, 1);
        TextureVertex(5, i).Set(0, 1);
        TextureVertex(6, i).Set(1, 1);
        TextureVertex(7, i).Set(1, 0);
    }
    for (i = 0; i < mNumVertices; i++)
        Normal(i).setValue(1, 0, 0);
    Normal(2).setValue(0, 0, 1);
    Triangle(0).Set(0, 1, 2);
    Triangle(1).Set(3, 0, 2);
    Normal(1).setValue(1, 0, 0);
    Triangle(2).Set(5, 6, 1);
    Triangle(3).Set(6, 2, 1);
    Normal(5).setValue(0, 0, -1);
    Triangle(4).Set(4, 7, 5);
    Triangle(5).Set(7, 6, 5);
    Normal(3).setValue(-1, 0, 0);
    Triangle(6).Set(4, 0, 3);
    Triangle(7).Set(7, 4, 3);
    Normal(6).setValue(0, 1, 0);
    Triangle(8).Set(3, 2, 6);
    Triangle(9).Set(7, 3, 6);
    Normal(4).setValue(0, -1, 0);
    Triangle(10).Set(5, 1, 4);
    Triangle(11).Set(1, 0, 4);
    SetNumMaterials(1);
    Material(0) = material;
    i = 0;
    while (i < mNumTriangles)
        MaterialIndex(i++) = 0;
}
