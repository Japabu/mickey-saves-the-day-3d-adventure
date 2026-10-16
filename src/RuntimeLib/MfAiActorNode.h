// MfAiActorNode: a node of an actor's skeleton as the application sees it.
// Declaration from the SteamboatApp PDB.
// Overloads are declared in reverse vtable order: the compiler groups them at
// the first declaration and lays them out last-declared first.
#ifndef RUNTIMELIB_MFAIACTORNODE_H
#define RUNTIMELIB_MFAIACTORNODE_H

#include "MfAi.h"
#include "enums.h"
class MfAiActor;
class MfAiColNode;
class MfCamera;
class MfDblList;
class MfJLimit;
class MfLight;
class MfMaterial;
class MfProperty;
class MfProtoNode;
class MfScDataHandle;
class MfString;
class MfStringList;
class MfTransform3;
class MfTriangleSet;
class MfVector3;

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

#endif
