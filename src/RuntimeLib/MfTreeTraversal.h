// MfTreeTraversal (MfActorLib.dll): the nodes of a tree in traversal order.
// numNodes and node are inline in the original header.
#ifndef RUNTIMELIB_MFTREETRAVERSAL_H
#define RUNTIMELIB_MFTREETRAVERSAL_H
class MfTreeNode;

class __declspec(dllimport) MfTreeTraversal {
public:
    MfTreeTraversal(int size);
    MfTreeTraversal(MfTreeNode *root, int, int);
    MfTreeTraversal(const MfTreeTraversal &);
    virtual ~MfTreeTraversal();
    MfTreeTraversal &operator=(const MfTreeTraversal &);
    int numNodes() { return mNumNodes; }
    MfTreeNode *node(int i) { return mNodes[i]; }
    void setNode(int i, MfTreeNode *node) { mNodes[i] = node; }
    int nodeIndex(MfTreeNode *const node) const;
protected:
    void AddTree(MfTreeNode *, int &, int);

public:
    int mNumNodes;                                          // 0x04
    MfTreeNode **mNodes;                                    // 0x08
};

#endif
