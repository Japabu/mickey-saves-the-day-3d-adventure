#pragma once
// MfTriangleSet: vertices (with optional normals, colors and texture
// coordinates) and the triangles that index them. A set made with
// Create(source, ...) shares the source's vertices and triangles and owns only
// its own per-vertex attributes. Layout from the code (no PDB has it).
#include "MfStreamObject.h"
#include "enums.h"
class MfColor;
class MfMaterial;
class MfOrientedBox;
class MfStream;
struct MfTextureVertex;
class MfTransform3;
struct MfTriangle;
class MfVector3;

// mFlags: which per-vertex (per-triangle) arrays the set has
enum {
    MF_TRISET_NORMALS = 1,
    MF_TRISET_TEXTURES = 2,
    MF_TRISET_COLORS = 4,
    MF_TRISET_MATERIALS = 8     // a material index per triangle
};

class __declspec(dllexport) MfTriangleSet : public MfStreamObject {
public:
    virtual long StreamType() const;
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;
    MfColor & Color(int);
    MfMaterial & Material(int);
    MfTextureVertex & TextureVertex(int, int);
    MfTriangle & Triangle(int);
    MfTriangleSet();
    MfTriangleSet(MfTriangleSet const &);
    MfTriangleSetType GetType();
    MfVector3 & Normal(int);
    MfVector3 & Vertex(int);
    MfVector3 FindTriangleNormal(int);
    bool CheckIntersection(int, MfTransform3 &, MfTriangleSet *, int, MfTransform3 &);
    bool CheckRayIntersection(int, MfVector3 &, MfVector3 &, MfTransform3 &, float &, int);
    bool CheckRayIntersection(int, MfVector3 &, MfVector3 &, float &, int);
    bool CheckSphereIntersection(int, MfVector3 &, float, MfTransform3 &, MfVector3 &, float &);
    bool CheckSphereIntersection(int, MfVector3 &, float, MfVector3 &, float &);
    float FindDistance(int, MfTransform3 &, MfTriangleSet *, int, MfTransform3 &, MfVector3 &, MfVector3 &);
    int ChangeID();
    int GetFlags();
    int GetID();
    int GetNumMaterials();
    int GetNumTexturesPerVertex();
    int GetNumTriangles();
    int GetNumVertices();
    short & MaterialIndex(int);
    static void __cdecl FindBoundingBox(MfVector3 *, int, MfVector3 &, MfVector3 &);
    static void __cdecl FindBoundingSphere(MfVector3 *, int, MfVector3 &, float &);
    static void __cdecl FindOrientedBox(MfVector3 *, int, MfOrientedBox &);
    void ApplyScale(MfVector3 const &);
    void ApplyTransform(MfTransform3 const &);
    void AssignAverageNormals();
    void Create(MfTriangleSet *, int, int);
    void Create(MfTriangleSetType, int, int, int, int);
    void FindBoundingBox(MfVector3 &, MfVector3 &);
    void FindBoundingSphere(MfVector3 &, float &);
    void FindOrientedBox(MfOrientedBox &);
    void Merge(MfTriangleSet const &, MfTransform3 &);
    void RemoveDegenerateTriangles();
    void RemoveUnusedVertices();
    void SetNumMaterials(int);
    ~MfTriangleSet();
    static long const STREAM_TYPE;
protected:
    bool CheckRayIntersectionCommon(int, MfVector3 &, MfVector3 &, float &, int, MfVector3 &, MfVector3 &, MfVector3 &);
    bool PointTriDistance(int, MfVector3 &, MfTransform3 &, MfVector3 &, float &);
public:
    MfTriangleSetType mType;            // 0x04
    int mFlags;                         // 0x08 MF_TRISET_*
    int mID;                            // 0x0c
    MfTriangleSet *mSource;             // 0x10 owner of the shared vertices/triangles
    int mNumVertices;                   // 0x14
    MfVector3 *mVertices;               // 0x18
    MfVector3 *mNormals;                // 0x1c
    MfColor *mColors;                   // 0x20
    MfTextureVertex *mTextureVertices;  // 0x24 mNumTexturesPerVertex per vertex
    int mNumTexturesPerVertex;          // 0x28
    int mNumTriangles;                  // 0x2c
    MfTriangle *mTriangles;             // 0x30
    short *mMaterialIndices;            // 0x34
    int mNumMaterials;                  // 0x38
    MfMaterial *mMaterials;             // 0x3c
};
