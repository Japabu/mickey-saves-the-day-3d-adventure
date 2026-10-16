// MfActorLib.dll 0x1001be00-0x1001bf00: MfTreeTraversal.
#include "MfTreeTraversal.h"
#include "MfTreeNode.h"

// 0x1000c8d0 ?setNode@MfTreeTraversal@@QAEXHPAVMfTreeNode@@@Z
void MfTreeTraversal::setNode(int i, MfTreeNode *p)
{
    mNodes[i] = p;
}

// 0x1000c8f0 ?node@MfTreeTraversal@@QAEPAVMfTreeNode@@H@Z
MfTreeNode * MfTreeTraversal::node(int i)
{
    return mNodes[i];
}

// 0x1000c900 ??0MfTreeTraversal@@QAE@ABV0@@Z (compiler-generated)
// 0x1000c920 ??_EMfTreeTraversal@@UAEPAXI@Z (compiler-generated)

// 0x1001be00 ??0MfTreeTraversal@@QAE@H@Z
MfTreeTraversal::MfTreeTraversal(int numNodes)
{
    mNumNodes = numNodes;
    mNodes = (MfTreeNode **)operator new(numNodes << 2);
}

// 0x1001be30 ??0MfTreeTraversal@@QAE@PAVMfTreeNode@@HH@Z
MfTreeTraversal::MfTreeTraversal(MfTreeNode *root, int type, int fill)
{
    mNumNodes = root->treeSize(type);
    mNodes = (MfTreeNode **)operator new(mNumNodes << 2);
    int count = 0;
    if (fill)
        AddTree(root, count, type);
    mNumNodes = count;
}

// 0x1001be90 ?AddTree@MfTreeTraversal@@IAEXPAVMfTreeNode@@AAHH@Z
void MfTreeTraversal::AddTree(MfTreeNode *node, int &index, int type)
{
    if (!type || node->Type() == type)
        mNodes[index++] = node;
    for (MfTreeNode *child = node->mFirstChild; child; child = child->mNextSibling)
        AddTree(child, index, type);
}

// 0x1001bee0 ??1MfTreeTraversal@@UAE@XZ
MfTreeTraversal::~MfTreeTraversal()
{
    delete mNodes;
}

// 0x1001bf00 ?nodeIndex@MfTreeTraversal@@QBEHQAVMfTreeNode@@@Z
int MfTreeTraversal::nodeIndex(MfTreeNode * const p) const
{
    int i;
    for (i = 0; i < mNumNodes; i++) {
        if (mNodes[i] == p)
            break;
    }
    if (i < mNumNodes)
        return i;
    return -1;
}
