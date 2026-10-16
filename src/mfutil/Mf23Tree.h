#pragma once
// Nodes of the 2-3 tree behind MfOrderedList (not exported; class names from
// the RTTI descriptors). The data lives in the leaves; an inner node keeps the
// largest key of its first two subtrees.
class Mf23TreeNode;

class Mf23TreeBase {
public:
    enum { BASE = 0, NODE = 1, LEAF = 2 };
    Mf23TreeBase() : parent(0), index(-1) {}
    virtual ~Mf23TreeBase() {}
    virtual int Type() { return BASE; }

    Mf23TreeNode *parent;
    int index;              // position in parent->child[]
};

class Mf23TreeNode : public Mf23TreeBase {
public:
    Mf23TreeNode()
    {
        key[0] = 0;
        key[1] = 0;
        child[0] = 0;
        child[1] = 0;
        child[2] = 0;
    }
    ~Mf23TreeNode()
    {
        delete child[0];
        delete child[1];
        delete child[2];
    }
    int Type() { return NODE; }

    void *key[2];
    Mf23TreeBase *child[3];
};

class Mf23TreeLeaf : public Mf23TreeBase {
public:
    Mf23TreeLeaf() { data = 0; }
    int Type() { return LEAF; }

    void *data;
};

// The key a subtree contributes to its parent: a leaf's data or a node's first key.
#define MF23_KEY(t) (((Mf23TreeLeaf *)(t))->data)
