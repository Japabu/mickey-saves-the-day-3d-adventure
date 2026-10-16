// MfRenderNode: the render geometry of a skeleton node (exported). Layout
// from ??0MfRenderNode@@IAE@PAVMfIRenderNode@@@Z (0x40 bytes).
#ifndef RUNTIMELIB_MFRENDERNODE_H
#define RUNTIMELIB_MFRENDERNODE_H
#include "MfIRenderer.h"
#include "MfTransform3.h"
class MfLight;
class MfMaterial;
class MfRenderScene;
class MfTriangleSet;
class MfVector3;

// what a render node is created from
struct MfRenderNodeInfo {
    int mID;                                                // 0x00 the triangle set's ID
    MfTriangleSet *mTriangles;                              // 0x04
    int m08;                                                // 0x08
    bool m0c;                                               // 0x0c
    bool m0d;                                               // 0x0d
    int m10, m14;                                           // 0x10
};

class __declspec(dllexport) MfRenderNode {
    friend class MfRenderer;
public:
    // (the copy constructor and assignment are the implicit ones)
    ~MfRenderNode();
    void Enable(bool on);
    bool IsEnabled();
    void SetParent(MfRenderNode *parent);
    bool Create(MfRenderNodeInfo *info);
    bool Create(MfTriangleSet &triangles);
    void SetVertices(MfRenderNodeInfo *info);
    void SetTransform(MfTransform3 &transform);
    void ChangeMaterial(int index, MfMaterial *material);
    void SetupLight(MfLight *light);
    void EnableLight(bool on);
    void TransformVertices(MfTransform3 &transform, MfVector3 &scale);
    void SetApparentScale(MfVector3 &scale);
    void SetWireframe(bool on);
    bool IsWireframed();
protected:
    MfRenderNode(MfIRenderNode *node);

public:
    MfIRenderNode *mNode;                                   // 0x00
    MfIRendLight *mLight;                                   // 0x04
    MfIRendEdit *mEdit;                                     // 0x08
    bool mEnabled;                                          // 0x0c
    bool mWireframed;                                       // 0x0d
    MfTransform3 mTransform;                                // 0x10
    bool mTransformDirty;                                   // 0x2c given to mNode by MfRenderScene::Sync
    MfRenderScene *mScene;                                  // 0x30
    int m34;                                                // 0x34
    MfRenderNode *mFirstChild;                              // 0x38
    MfRenderNode *mNext;                                    // 0x3c the next node of the scene
};

#endif
