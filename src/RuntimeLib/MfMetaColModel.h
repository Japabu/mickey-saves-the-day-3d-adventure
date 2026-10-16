// MfMetaColModel: a collision model over the collision nodes of a space (the
// environment's meta model): a sphere tree over the nodes' own sphere trees.
// Exported from RuntimeLib.dll (names from the exports; vtable 100b3084).
#ifndef RUNTIMELIB_MFMETACOLMODEL_H
#define RUNTIMELIB_MFMETACOLMODEL_H
#include "MfColModel.h"
#include "MfLock.h"
class MfColNode;
class MfNode;
class MfVector3;

// size 0x60
class __declspec(dllexport) MfMetaColModel : public MfColModel {
public:
    MfMetaColModel();
    virtual ~MfMetaColModel();
    virtual void DeleteSphereTree();
    void AddColNode(MfColNode *node);
    // held while meta models are built
    static MfLock constructLock;
    void MakeMeta();
protected:
    MfNode *DivideAndConquer(int, int, int &, int);
    void FindBoundingSphere(MfNode *, MfNode *, MfVector3 &, float &);
    void FindSplittingPlane(int, int, MfVector3 &, MfVector3 &);
    void ReorderColNodes(int, int, MfVector3 &, MfVector3 &, int &, int &, int &, int &);
public:
    int mNumColNodes;                                       // 0x5c

    // the nodes being collected (AddColNode) and the room for them
    static MfColNode **colNodes;
    static int maxColNodes;
};

#endif
