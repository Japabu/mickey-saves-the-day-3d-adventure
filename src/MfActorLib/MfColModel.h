#pragma once
// Collision model: a triangle set with a bounding-sphere tree (one leaf per
// triangle, n - 1 inner spheres) and an oriented bounding box.
#include "MfActorLib_api.h"
#include "MfStreamObject.h"
#include "MfTransform3.h"

class MfAi;
class MfStream;
class MfTriangleSet;

#include "MfOrientedBox.h"
#include "MfTextureVertex.h"
#include "MfTriangle.h"
#include "MfNode.h"
#include "MfLeafNode.h"
#include "MfSphereNode.h"

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
