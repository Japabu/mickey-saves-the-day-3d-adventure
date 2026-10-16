// MfSkeletonTraversal: the nodes of a skeleton in traversal order (an
// MfTreeTraversal over MfSkeletonNodes), optionally only the nodes of one
// type and only the nodes of the root's owner. Unit 1004f950-1004fef0
// (RTTI name; the methods are not exported, the names are descriptive).
#ifndef RUNTIMELIB_MFSKELETONTRAVERSAL_H
#define RUNTIMELIB_MFSKELETONTRAVERSAL_H
#include "MfSkeletonNode.h"
#include "MfString.h"
#include "MfTreeTraversal.h"

class MfSkeletonTraversal : public MfTreeTraversal {
public:
    MfSkeletonTraversal(MfSkeletonNode *root, int type, int sameOwner);
    virtual ~MfSkeletonTraversal() {}
    // adds node and its subtree (nodes of the type, or all for type 0)
    virtual void AddTree(MfTreeNode *node, int &count, int type);

    MfSkeletonNode *node(int i) { return (MfSkeletonNode *)MfTreeTraversal::node(i); }
    int NodeIndex(MfString name);                 // -1 if no node has the name
    MfSkeletonNode *Node(MfString name);
    MfTransform3 *CopyTransforms();               // a new array with every node's local transform
    void GetTransforms(MfTransform3 *transforms);
    void SetTransforms(const MfTransform3 *transforms);
    void GetGlobalTransforms(MfTransform3 *transforms);

    int mSameOwner;                                         // 0x0c only the root owner's nodes
};

#endif
