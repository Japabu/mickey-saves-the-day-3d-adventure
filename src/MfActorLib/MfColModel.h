#pragma once
#ifndef MFACTOR_API
#define MFACTOR_API __declspec(dllimport)
#endif
// Collision model: a triangle set with a bounding-sphere tree (one leaf per
// triangle, n - 1 inner spheres) and an oriented bounding box.
#include "MfStreamObject.h"
#include "MfTransform3.h"

class MfAi;
class MfStream;
class MfTriangleSet;

// size 0x28; the copy members are compiler-generated
class MFACTOR_API MfOrientedBox {
public:
    bool CheckIntersection(MfOrientedBox *, MfTransform3 &, MfTransform3 &);
    bool CheckRayIntersection(MfTransform3 &, MfVector3 &, MfVector3 &, float);
    MfTransform3 m_transform;   // 0x00
    MfVector3 m_extent;         // 0x1c half sizes along the box axes
};

// size 8
struct MFACTOR_API MfTextureVertex {
    void Set(float u, float v) { m_u = u; m_v = v; }
    float m_u, m_v;
};

// size 0xc
struct MFACTOR_API MfTriangle {
    void Set(int a, int b, int c) { m_v[0] = a; m_v[1] = b; m_v[2] = c; }
    int m_v[3];
};

enum { MF_LEAF_NODE = 0, MF_SPHERE_NODE = 1 };

// size 0x14; bounding-sphere tree node
class MFACTOR_API MfNode {
public:
    MfNode(long type) { m_type = type; m_index = 0; }
    ~MfNode() {}
    MfVector3 m_center;         // 0x00
    unsigned m_type : 2;        // 0x0c bits 0-1 (MF_LEAF_NODE, MF_SPHERE_NODE)
    unsigned m_index : 30;      // 0x0c bits 2-31 (leaf: triangle index)
    float m_radius;             // 0x10
};

class MFACTOR_API MfLeafNode : public MfNode {
public:
    MfLeafNode() : MfNode(MF_LEAF_NODE) {}
};

// size 0x1c
class MFACTOR_API MfSphereNode : public MfNode {
public:
    MfSphereNode() : MfNode(MF_SPHERE_NODE) { m_left = 0; m_right = 0; }
    MfNode *m_left;             // 0x14
    MfNode *m_right;            // 0x18
};

// size 0x5c
class MFACTOR_API MfColModel : public MfStreamObject {
public:
    MfColModel();
    MfColModel(const MfColModel &);
    virtual long StreamType() const;
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;
    virtual ~MfColModel();

    void GetBoundingBox(MfVector3 &min, MfVector3 &max)
    {
        min = m_min;
        max = m_max;
    }
    int SetUserData(MfAi *ai) { m_userData = ai; return 1; }
    MfAi *GetUserData() { return m_userData; }
    void SetBoundingBox(MfVector3 &min, MfVector3 &max)
    {
        m_min = min;
        m_max = max;
    }
    MfTriangleSet *GetTriangleSet() { return m_triangleSet; }
    MfSphereNode *GetRoot() { return m_spheres; }
    void SetRoot(MfSphereNode *root) { m_spheres = root; }
    void SetDepth(int depth) { m_depth = depth; }

    void SetTriangleSet(MfTriangleSet *);
    void UpdateSphereTree();
    virtual void DeleteSphereTree();

    static const long STREAM_TYPE;
protected:
    static void BoundTriangle(MfVector3 *, MfVector3 &, float &);
    static void FindSplittingPlane(const MfOrientedBox &, MfVector3 &, MfVector3 &);
    void FindLeafSphere(int, MfVector3 *);
    MfNode *DivideAndConquer(int, int, MfVector3 *, int &, int);
    void ReorderTriangles(int, int, const MfVector3 &, const MfVector3 &, MfVector3 *,
                          int &, int &, int &, int &);

    int m_reserved;                 // 0x04
    MfTriangleSet *m_triangleSet;   // 0x08
    MfLeafNode *m_leaves;           // 0x0c one per triangle
    MfSphereNode *m_spheres;        // 0x10 root first
    int m_depth;                    // 0x14 0: no tree
    MfOrientedBox m_box;            // 0x18
    MfVector3 m_min;                // 0x40 bounding box
    MfVector3 m_max;                // 0x4c
    MfAi *m_userData;               // 0x58
};
