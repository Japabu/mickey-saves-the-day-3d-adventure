// MfTreeNode (MfActorLib.dll): a node of an n-ary tree (first child / next
// sibling / parent links). Accessors and Type/IsType are inline in the
// original header (MfActorLib exports their bodies; callers inline them).
#ifndef RUNTIMELIB_MFTREENODE_H
#define RUNTIMELIB_MFTREENODE_H

class __declspec(dllimport) MfTreeNode {
public:
    enum { TYPE = 'Tree' };
    MfTreeNode();
    MfTreeNode(const MfTreeNode &);
    virtual ~MfTreeNode();
    virtual int Type() { return TYPE; }
    virtual int IsType(int type) { return type == TYPE; }

    MfTreeNode *firstChild() const { return mFirstChild; }
    MfTreeNode *nextSibling() const { return mNextSibling; }
    MfTreeNode *parent() const { return mParent; }
    int DestroysChildrenOnDestruct() { return mDestroysChildrenOnDestruct; }
    void DestroyChildrenOnDestruct(int on) { mDestroysChildrenOnDestruct = on; }
    int hasChild(MfTreeNode *) const;
    int hasDescendant(MfTreeNode *) const;
    int numChildren(int) const;
    int treeSize(int) const;
    void addChild(MfTreeNode *);
    void insertChild(MfTreeNode *, int);
    void removeChild(MfTreeNode *);

protected:
    MfTreeNode *mFirstChild;                                // 0x04
    MfTreeNode *mNextSibling;                               // 0x08
    MfTreeNode *mParent;                                    // 0x0c
    int mDestroysChildrenOnDestruct;                        // 0x10
};

#endif
