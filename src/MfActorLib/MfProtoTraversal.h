#pragma once
// MfProtoTraversal: the nodes of a prototype skeleton (optionally only the
// ones of one style) in preorder.
#include "MfActorLib_api.h"
#include "MfString.h"
class MfProtoNode;

class MFACTOR_API MfProtoTraversal {
public:
    MfProtoTraversal(MfProtoNode *root, int style);
    ~MfProtoTraversal();
    MfProtoNode *Node(int i) { return mNodes[i]; }
    int NumNodes() { return mNumNodes; }
    int NodeIndex(MfProtoNode *);
    int IndexWithName(MfString);
    MfProtoNode *NodeWithName(MfString);
    MfString UniqueNameWithPrefix(MfString);
protected:
    void AddSubtree(MfProtoNode *, int &, int);
public:
    int mNumNodes;
    MfProtoNode **mNodes;
};

#include "MfPermutation.h"
