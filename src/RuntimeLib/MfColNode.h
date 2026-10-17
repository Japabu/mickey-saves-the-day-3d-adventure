// MfColNode: a collision node, the MfAiColNode the runtime hands out for an
// actor node or a space. It is also a node of the bounding-sphere tree
// (MfNode type 2). Layout from the constructor ??0MfColNode@@QAE@PAVMfColModel@@@Z.
#ifndef RUNTIMELIB_MFCOLNODE_H
#define RUNTIMELIB_MFCOLNODE_H
#include "MfAiColNode.h"
#include "MfLock.h"
#include "MfNode.h"
#include "MfTransform3.h"
#include "RuntimeLib_api.h"
class MfColModel;
struct MfSearchData;

class RUNTIME_API MfColNode : public MfNode, public MfAiColNode {
public:
    MfColNode(MfColModel *colModel = 0);
    MfColNode(const MfColNode &);
    ~MfColNode();

    virtual void GetInterfaceName(char *, int, int &, int &);
    virtual int AddRef();
    virtual int Release();
    virtual MfTransform3 &Transform() { return mTransform; }
    virtual MfColModel *&ColModel() { return mColModel; }
    virtual int GetCheckStyle();
    virtual void SetCheckStyle(int);
    virtual MfAiActorNode *&Owner() { return mOwner; }
    virtual bool CheckCollision(MfAiColNode *, MfColResultNode &);
    virtual bool CheckCollision(MfVector3 &, MfVector3 &, MfColResultRay &, bool, float);
    virtual bool CheckCollision(MfVector3 &, float, MfColResultSphere &);
    virtual bool CheckContainment(MfVector3 &, float);
    virtual bool GetMinDistance(MfAiColNode *, MfDistResult &, float);
    virtual bool BeginConstruction();
    virtual bool AddNode(MfAiColNode *);
    virtual bool EndConstruction();
    virtual MfAiColReport *CreateColReport() const;
    virtual MfAiColInterface *CreateColInterface(MfColInterfaceType) const;
    virtual bool SetUserData(MfAi *userData) { mUserData = userData; return true; }
    virtual MfAi *GetUserData() { return mUserData; }

    void OwnColModel(MfColModel *colModel) { mColModel = colModel; mOwnsColModel = true; }
    int SetColFlag(const int &flag) { mColFlags |= flag; return 1; }
    int ClearColFlag(const int &flag) { mColFlags &= ~flag; return 1; }
    int TestColFlag(const int &flag) const { return (mColFlags & flag) != 0; }

protected:
    bool BallIntersect(MfVector3 &, float);
    bool MinDistance(MfColNode *, MfDistResult &, float);
    bool SphereIntersect(MfVector3 &, float, MfColResultSphere &);
    float CheckTriangleIntersection(MfColNode *, MfColResultNode &, bool);
    float RayIntersect(MfVector3 &, MfVector3 &, bool, float, MfColNode *&, int &);
    static MfLock checkLock;
    static MfSearchData *stack;
    static int stackSize;

public:
    MfTransform3 mTransform;                                // 0x18
    MfColModel *mColModel;                                  // 0x34
    int mColFlags;                                          // 0x38 (the check style)
    MfAiActorNode *mOwner;                                  // 0x3c
    long mRefCount;                                         // 0x40
    bool mOwnsColModel;                                     // 0x44
    MfLock mLock;                                           // 0x48
    MfAi *mUserData;                                        // 0x4c
};

#endif
