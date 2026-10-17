#pragma once
#ifndef MFACTOR_API
#define MFACTOR_API __declspec(dllimport)
#endif
// MfProtoTraversal: the nodes of a prototype skeleton (optionally only the
// ones of one style) in preorder.
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

// MfPermutation: how the nodes of one skeleton map onto the nodes of another
// (-1 where a node has no counterpart).
class MFACTOR_API MfPermutation {
public:
    MfPermutation(MfProtoTraversal *from, MfProtoTraversal *to);
    ~MfPermutation();
    int mNumFrom;           // +0x00
    int mNumTo;             // +0x04
    int *mFromToTo;         // +0x08 index in to of each node of from
    int *mToToFrom;         // +0x0c index in from of each node of to
};
