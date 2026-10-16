// MfActorLib.dll 0x1001baf0-0x1001bdc0: MfTreeNode.
#include "MfTreeNode.h"

// 0x1000c8a0 ?Type@MfTreeNode@@UAEHXZ
int MfTreeNode::Type()
{
    return TYPE;
}

// 0x1000c8b0 ?IsType@MfTreeNode@@UAEHH@Z (inline; the first copy is the one of the
// old-format reader's object)

// 0x1001baf0 ??0MfTreeNode@@QAE@XZ
MfTreeNode::MfTreeNode() : mDestroysChildrenOnDestruct(1)
{
    mNextSibling = 0;
    mFirstChild = 0;
    mParent = 0;
}

// 0x1001bb60 ??0MfTreeNode@@QAE@ABV0@@Z
// Copies the whole subtree below the node.
MfTreeNode::MfTreeNode(MfTreeNode const &src)
{
    mNextSibling = 0;
    mFirstChild = 0;
    mParent = 0;
    mDestroysChildrenOnDestruct = src.mDestroysChildrenOnDestruct;
    for (MfTreeNode *child = src.mFirstChild; child; child = child->mNextSibling)
        addChild(new MfTreeNode(*child));
}

// 0x1001bb10 ??_EMfTreeNode@@UAEPAXI@Z (compiler-generated)

// 0x1001bbf0 ??1MfTreeNode@@UAE@XZ
MfTreeNode::~MfTreeNode()
{
    if (mDestroysChildrenOnDestruct == 1) {
        MfTreeNode *child = mFirstChild;
        while (child) {
            MfTreeNode *next = child->mNextSibling;
            delete child;
            child = next;
        }
    }
}

// 0x1001bc20 ?hasChild@MfTreeNode@@QBEHPAV1@@Z
int MfTreeNode::hasChild(MfTreeNode *node) const
{
    for (MfTreeNode *child = mFirstChild; child; child = child->mNextSibling)
        if (child == node)
            return 1;
    return 0;
}

// 0x1001bc50 ?hasDescendant@MfTreeNode@@QBEHPAV1@@Z
int MfTreeNode::hasDescendant(MfTreeNode *node) const
{
    if (this == node)
        return 1;
    for (MfTreeNode *child = mFirstChild; child; child = child->mNextSibling)
        if (child->hasDescendant(node))
            return 1;
    return 0;
}

// 0x1001bca0 ?addChild@MfTreeNode@@QAEXPAV1@@Z
void MfTreeNode::addChild(MfTreeNode *node)
{
    node->mParent = this;
    node->mNextSibling = 0;
    if (mFirstChild) {
        MfTreeNode *last = mFirstChild;
        while (last->mNextSibling)
            last = last->mNextSibling;
        last->mNextSibling = node;
    } else {
        mFirstChild = node;
    }
}

// 0x1001bce0 ?insertChild@MfTreeNode@@QAEXPAV1@H@Z
void MfTreeNode::insertChild(MfTreeNode *node, int index)
{
    node->mParent = this;
    if (index <= 0) {
        node->mNextSibling = mFirstChild;
        mFirstChild = node;
        return;
    }
    MfTreeNode *prev = mFirstChild;
    for (int i = 1; i < index && prev->mNextSibling; i++)
        prev = prev->mNextSibling;
    node->mNextSibling = prev->mNextSibling;
    prev->mNextSibling = node;
}

// 0x1001bd30 ?removeChild@MfTreeNode@@QAEXPAV1@@Z
void MfTreeNode::removeChild(MfTreeNode *node)
{
    if (node == mFirstChild) {
        mFirstChild = node->mNextSibling;
    } else {
        for (MfTreeNode *prev = mFirstChild; prev->mNextSibling; prev = prev->mNextSibling) {
            if (prev->mNextSibling == node) {
                prev->mNextSibling = node->mNextSibling;
                break;
            }
        }
    }
    node->mNextSibling = 0;
    node->mParent = 0;
}

// 0x1001bd90 ?numChildren@MfTreeNode@@QBEHH@Z
int MfTreeNode::numChildren(int type) const
{
    int n = 0;
    for (MfTreeNode *child = mFirstChild; child; child = child->mNextSibling)
        if (!type || child->Type() == type)
            n++;
    return n;
}

// 0x1001bdc0 ?treeSize@MfTreeNode@@QBEHH@Z
int MfTreeNode::treeSize(int type) const
{
    int n = (!type || ((MfTreeNode *)this)->Type() == type) ? 1 : 0;
    for (MfTreeNode *child = mFirstChild; child; child = child->mNextSibling)
        n += child->treeSize(type);
    return n;
}
