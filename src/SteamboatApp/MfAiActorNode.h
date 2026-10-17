#pragma once
// MfAiActorNode.h: a node of a Motivate actor's skeleton as the game sees it (MfActorLib.dll).
// Declaration from SteamboatApp.pdb; all members are pure virtual.
#include "MfAi.h"

class MfAiColNode;
class MfCamera;
class MfJLimit;
class MfLight;
class MfMaterial;
class MfProtoNode;
class MfTriangleSet;

enum MfActorNodeType {
    MF_NODE_GEOMETRY = 0,
    MF_NODE_HANDLE = 1,
    MF_NODE_CAMERA = 2,
    MF_NODE_FLOOR = 3,
    MF_NODE_LIGHT = 4,
    MF_NODE_SPLINE = 5
};

enum MfHandleNodeStyle {
    MF_HANDLE_NODE_CYLINDER = 0,
    MF_HANDLE_NODE_ORIENTED_CYLINDER = 1,
    MF_HANDLE_NODE_DISC = 2,
    MF_HANDLE_NODE_ORIENTED_DISC = 3,
    MF_HANDLE_NODE_POINT = 4,
    MF_HANDLE_NODE_ERROR = 5
};

// own copy: MfPoLibrary's MfAiActorNode.h opens MfPoLibrary/MfAi.h, a second MfAi next to mfutil's.
class MfAiActorNode : public MfAi {
public:
    virtual const MfString &GetNodeName() = 0;
    virtual MfActorNodeType GetNodeType() = 0;
    virtual MfAiActorNode *Parent() = 0;
    virtual MfAiActorNode *FirstChild() = 0;
    virtual MfAiActorNode *NextSibling() = 0;
    virtual int &TransformDirty() = 0;
    virtual int GetTransform(MfTransform3 &, MfTransform3 &, MfTransform3 &) = 0;
    virtual const MfTransform3 &GetLocalTransform() = 0;
    virtual const MfTransform3 &GetGlobalTransform() = 0;
    virtual void SetTransform(const MfTransform3 &) = 0;
    virtual MfAiActor *GetActor() = 0;
    virtual int GetRenderStyle() = 0;
    virtual void SetRenderStyle(int) = 0;
    virtual MfTriangleSet *GetTriangleSet() = 0;
    virtual MfAiColNode &ColNode() = 0;
    virtual void FlushColModel() = 0;
    virtual void ShareColModel(MfAiActorNode *) = 0;
    virtual void FlushGeometry() = 0;
    virtual int GetPrevTransform(MfTransform3 &) = 0;
    virtual const MfMaterial *GetMaterial(int) = 0;
    virtual int SetMaterial(int, MfMaterial *) = 0;
    virtual MfScDataHandle *GetPiccoloHandle() = 0;
    virtual void SetProperty(MfProperty &) = 0;
    virtual void SetProperty(MfProperty *) = 0;
    virtual int GetProperty(MfString, MfProperty &) = 0;
    virtual int GetProperty(const MfString &, MfProperty **) = 0;
    virtual void GetPropertyNames(MfStringList *) = 0;
    virtual int GetContainer(MfVector3 &, MfVector3 &) = 0;
    virtual MfJLimit *GetJointLimit() = 0;
    virtual MfProtoNode *GetPrototype() = 0;
    virtual MfDblList *SphereList() = 0;
    virtual int GetSplineStart(MfTransform3 &) = 0;
    virtual int GetSplineEnd(MfTransform3 &) = 0;
    virtual MfHandleNodeStyle GetHandleStyle() = 0;
    virtual MfCamera *GetCamera() = 0;
    virtual void SyncAllViews(int) = 0;
    virtual MfLight *GetLight() = 0;
    virtual void SetupLight(int) = 0;
};
