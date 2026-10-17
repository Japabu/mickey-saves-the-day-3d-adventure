#pragma once
#ifndef MFACTOR_API
#define MFACTOR_API __declspec(dllimport)
#endif
// MfTreeTraversal: the nodes of a tree (optionally of one type) in preorder.
class MfTreeNode;

class MFACTOR_API MfTreeTraversal {
public:
    MfTreeTraversal(int numNodes);
    MfTreeTraversal(MfTreeNode *root, int type, int fill);
    virtual ~MfTreeTraversal();
    MfTreeNode * node(int);
    void setNode(int, MfTreeNode *);
    int nodeIndex(MfTreeNode * const) const;
    int numNodes();
protected:
    void AddTree(MfTreeNode *, int &, int);
    int mNumNodes;
    MfTreeNode **mNodes;
};
