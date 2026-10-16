// MfTriangleSet (MfActorLib.dll): indexed triangle geometry with normals,
// materials and texture coordinates.
#ifndef RUNTIMELIB_MFTRIANGLESET_H
#define RUNTIMELIB_MFTRIANGLESET_H
#include "MfStreamObject.h"
#include "enums.h"
class MfColor;
class MfMaterial;
class MfOrientedBox;
class MfTransform3;
class MfVector3;
struct MfTextureVertex;
struct MfTriangle;

class __declspec(dllimport) MfTriangleSet : public MfStreamObject {
public:
    MfTriangleSet();
    MfTriangleSet(const MfTriangleSet &);
    ~MfTriangleSet();
    MfTriangleSet &operator=(const MfTriangleSet &);
    virtual long StreamType() const;
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;

    void Create(MfTriangleSetType, int, int, int, int);
    void Create(MfTriangleSet *, int, int);
    void ApplyScale(const MfVector3 &);
    void ApplyTransform(const MfTransform3 &);
    void FindBoundingBox(MfVector3 &min, MfVector3 &max);
    void FindBoundingSphere(MfVector3 &center, float &radius);
    void FindOrientedBox(MfOrientedBox &);
    int GetNumMaterials();
    void SetNumMaterials(int);
    MfMaterial &Material(int);
    int GetNumTriangles();
    int GetNumVertices();
    MfVector3 &Vertex(int);
    MfVector3 &Normal(int);
    MfTriangle &Triangle(int);
    short &MaterialIndex(int);
    int GetID();
    int ChangeID();
    MfVector3 FindTriangleNormal(int tri);
    bool CheckRayIntersection(int tri, MfVector3 &origin, MfVector3 &direction, MfTransform3 &transform, float &t,
                              int backFaces);
    bool CheckSphereIntersection(int tri, MfVector3 &center, float radius, MfTransform3 &transform, MfVector3 &pos,
                                 float &depth);
    bool CheckIntersection(int tri, MfTransform3 &transform, MfTriangleSet *other, int otherTri,
                           MfTransform3 &otherTransform);
    float FindDistance(int tri, MfTransform3 &transform, MfTriangleSet *other, int otherTri,
                       MfTransform3 &otherTransform, MfVector3 &p1, MfVector3 &p2);

    MfTriangleSetType mType;                                // 0x04
    int mFlags;                                             // 0x08
    int mID;                                                // 0x0c
    int m10, m14, m18, m1c, m20, m24;                       // 0x10
    int mNumTexturesPerVertex;                              // 0x28
    int mNumTriangles;                                      // 0x2c
    int m30, m34;                                           // 0x30
    int mNumMaterials;                                      // 0x38
    int m3c;                                                // 0x3c
};

// a square of two triangles with a material (the scene background)
class __declspec(dllimport) MfTriSquare : public MfTriangleSet {
public:
    MfTriSquare(const MfMaterial &material, float width, float height);
};

// a closed cylinder of sides faces (MfManipSkill's old handle models)
class __declspec(dllimport) MfTriCylinder : public MfTriangleSet {
public:
    MfTriCylinder(const MfMaterial &material, float radius, float length, int sides);
};

#endif
