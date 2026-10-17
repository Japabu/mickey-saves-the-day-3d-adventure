#pragma once
#ifndef MFACTOR_API
#define MFACTOR_API __declspec(dllimport)
#endif
// MfTreeNode: a node with a first child, a next sibling and a parent.
class MFACTOR_API MfTreeNode {
public:
    enum { TYPE = 'Tree' };
    MfTreeNode();
    MfTreeNode(MfTreeNode const &);
    virtual ~MfTreeNode();
    virtual int Type();
    virtual int IsType(int type) { return type == TYPE; }
    MfTreeNode & operator=(MfTreeNode const &);
    MfTreeNode * firstChild() const { return mFirstChild; }
    MfTreeNode * nextSibling() const { return mNextSibling; }
    MfTreeNode * parent() const { return mParent; }
    int DestroysChildrenOnDestruct();
    void DestroyChildrenOnDestruct(int);
    int hasChild(MfTreeNode *) const;
    int hasDescendant(MfTreeNode *) const;
    int numChildren(int type) const;
    int treeSize(int type) const;
    void addChild(MfTreeNode *);
    void insertChild(MfTreeNode *, int);
    void removeChild(MfTreeNode *);
protected:
    MfTreeNode *mFirstChild;
    MfTreeNode *mNextSibling;
    MfTreeNode *mParent;
    int mDestroysChildrenOnDestruct;
    friend class MfTreeTraversal;
};
