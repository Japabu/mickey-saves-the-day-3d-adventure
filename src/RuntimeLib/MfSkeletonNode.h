// MfSkeletonNode: a node of an actor's skeleton (a tree of nodes, each with a
// local and a global transform, a collision node and optional geometry), and
// the MfAiActorNode the application sees. Layout from the constructor
// ??0MfSkeletonNode@@QAE@XZ; vtable order from ??_7MfSkeletonNode@@6BMfTreeNode@@@
// and ??_7MfSkeletonNode@@6BMfAiActorNode@@@.
#ifndef RUNTIMELIB_MFSKELETONNODE_H
#define RUNTIMELIB_MFSKELETONNODE_H
#include "MfAiActorNode.h"
#include "MfColNode.h"
#include "MfLock.h"
#include "MfPropertyList.h"
#include "MfProtoNode.h"
#include "MfString.h"
#include "MfTransform3.h"
#include "MfTreeNode.h"
#include "RuntimeLib_api.h"
class MfActor;
class MfRenderNode;
class MfSpace;

class RUNTIME_API MfSkeletonNode : public MfTreeNode, public MfAiActorNode {
    // MfActor::GotoHomeConfig syncs the nodes of other actors attached here
    friend class MfActor;
public:
    enum { TYPE = 'Skel' };
    MfSkeletonNode();
    MfSkeletonNode(const MfSkeletonNode &);
    virtual ~MfSkeletonNode();
    virtual int Type() { return TYPE; }
    virtual int IsType(int type)
    {
        if (type == TYPE)
            return 1;
        return MfTreeNode::IsType(type);
    }
    virtual MfSkeletonNode *createCopy() { return new MfSkeletonNode(*this); }
    virtual int SetSpace(MfSpace *space, int recurse);
    virtual int RemoveSpace(MfSpace *space, int recurse);
    virtual void TransformPivot(const MfTransform3 &pivot);
    virtual void ScaleSubtree(MfVector3 scale, int, int);
    virtual void SetApparentScale(const MfVector3 &scale);
    virtual MfRenderNode *GetRenderNode();
protected:
    virtual void _syncToTransform();

public:
    // MfAi
    virtual void GetInterfaceName(char *, int, int &, int &);
    virtual int AddRef() { return 1; }
    virtual int Release() { return 1; }
    // MfAiActorNode
    virtual const MfString &GetNodeName();
    virtual MfActorNodeType GetNodeType();
    virtual MfAiActorNode *Parent();
    virtual MfAiActorNode *FirstChild();
    virtual MfAiActorNode *NextSibling();
    virtual int &TransformDirty() { return mTransformDirty; }
    virtual int GetTransform(MfTransform3 &local, MfTransform3 &home, MfTransform3 &global);
    virtual const MfTransform3 &GetLocalTransform() { return mTransform; }
    virtual const MfTransform3 &GetGlobalTransform() { return mGlobalTransform; }
    virtual void SetTransform(const MfTransform3 &);
    virtual MfAiActor *GetActor();
    virtual int GetRenderStyle();
    virtual void SetRenderStyle(int);
    virtual MfTriangleSet *GetTriangleSet();
    virtual MfAiColNode &ColNode() { return mColNode; }
    virtual void FlushColModel();
    virtual void ShareColModel(MfAiActorNode *);
    virtual void FlushGeometry();
    virtual int GetPrevTransform(MfTransform3 &);
    virtual const MfMaterial *GetMaterial(int);
    virtual int SetMaterial(int, MfMaterial *);
    virtual MfScDataHandle *GetPiccoloHandle();
    virtual void SetProperty(MfProperty &);
    virtual void SetProperty(MfProperty *);
    virtual int GetProperty(MfString, MfProperty &);
    virtual int GetProperty(const MfString &, MfProperty **);
    virtual void GetPropertyNames(MfStringList *);
    virtual int GetContainer(MfVector3 &, MfVector3 &);
    virtual MfJLimit *GetJointLimit();
    virtual MfProtoNode *GetPrototype() { return mProto; }
    virtual MfDblList *SphereList() { return mProto ? &mProto->mColSpheres : 0; }
    virtual int GetSplineStart(MfTransform3 &);
    virtual int GetSplineEnd(MfTransform3 &);
    virtual MfHandleNodeStyle GetHandleStyle();
    virtual MfCamera *GetCamera() { return 0; }
    virtual void SyncAllViews(int) {}
    virtual MfLight *GetLight() { return 0; }
    virtual void SetupLight(int) {}

    static MfSkeletonNode *CreateFromPrototype(MfProtoNode *proto);

    const MfString &name() { return mProto ? mProto->mName : mLocalName; }
    void SetLocalName(MfString name) { mLocalName = name; }
    MfTransform3 homeTransform()
    {
        if (mProto)
            return mProto->mHomeTransform;
        return mLocalHomeTransform;
    }
    void SetLocalHomeTransform(const MfTransform3 &home) { mLocalHomeTransform = home; }
    MfJLimit *jointLimit() { return mProto ? mProto->mJointLimit : 0; }
    MfDblList *colSpheres() { return mProto ? &mProto->mColSpheres : 0; }
    void ShowWireFrame(int on) { SetRenderStyle(on != 0); }
    int &AllowTransform() { return mAllowTransform; }
    const MfTransform3 &transform() const { return mTransform; }
    const MfTransform3 &globalTransform() const { return mGlobalTransform; }
    MfColModel *colModel() { return mProto ? mProto->mColModel : mLocalColModel; }
    MfColNode &colNode() { return mColNode; }
    MfPropertySet *GetInstPropertyTable() { return &mInstProperties; }
    int IsSprite() { return mIsSprite; }
    MfActor *GetOwner() { return mOwner; }
    MfActor *GetOwner() const { return mOwner; }
    MfProtoNode *ProtoNode() { return mProto; }
    int &LastPickedTriIndex() { return mLastPickedTriIndex; }

    void setTransform(const MfTransform3 &);
    void setTranslation(const MfVector3 &);
    void setRotation(const MfRotation3 &);
    void syncChildTransforms(int);
    void gotoHomeTransform();
    void setGeometry(MfTriangleSet *, MfString);
    MfColModel *SetLocalColModel(MfTriangleSet *);
    MfColModel *SetLocalColModel(MfColModel *, bool own);
    MfRenderNode *GetRenderNode(int create);
    void SetOwner(MfActor *owner, int recurse);
    void CreateRenderNode();
    void ReplaceRenderNode();
    void DeleteRenderNode();
    void changeParent(MfSkeletonNode *, MfTransform3 *);
    void OverrideMaterials(const MfMaterial &);
    void RestoreMaterials();
    MfProperty &Property(MfString);

    static MfLock parentLock;

    int m18;                                                // 0x18
    int m1c;                                                // 0x1c
    MfProtoNode *mProto;                                    // 0x20
    MfActor *mOwner;                                        // 0x24
    MfTransform3 mTransform;                                // 0x28 relative to the parent
    MfTransform3 mGlobalTransform;                          // 0x44
    int mTransformDirty;                                    // 0x60
    int mAllowTransform;                                    // 0x64
    int mIsSprite;                                          // 0x68
    MfColNode mColNode;                                     // 0x6c
    int mLastPickedTriIndex;                                // 0xbc
    MfSpace *mSpace;                                        // 0xc0
    MfRenderNode *mRenderNode;                              // 0xc4
    MfTransform3 *mRenderTransform;                         // 0xc8
    MfColModel *mLocalColModel;                             // 0xcc
    bool mOwnsLocalColModel;                                // 0xd0
    MfString mLocalName;                                    // 0xd4
    MfTransform3 mLocalHomeTransform;                       // 0xd8
    MfScDataHandle *mPiccoloHandle;                         // 0xf4
    MfPropertyList mInstProperties;                         // 0xf8
};

#endif
