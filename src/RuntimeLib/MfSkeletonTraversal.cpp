// MfSkeletonTraversal (RuntimeLib.dll 1004f950-1004fef0).
#include "MfSkeletonTraversal.h"

// 0x1004f950 ??0MfSkeletonTraversal@@QAE@PAVMfSkeletonNode@@HH@Z
MfSkeletonTraversal::MfSkeletonTraversal(MfSkeletonNode *root, int type, int sameOwner)
    : MfTreeTraversal(root, type, 0), mSameOwner(sameOwner)
{
    int count = 0;
    MfSkeletonTraversal::AddTree(root, count, type);
    mNumNodes = count;
}

// 0x1004f9d0 ??_GMfSkeletonTraversal@@UAEPAXI@Z
// 0x1004f9f0 ??1MfSkeletonTraversal@@UAE@XZ

// 0x1004fa00 ?AddTree@MfSkeletonTraversal@@UAEXPAVMfTreeNode@@AAHH@Z
void MfSkeletonTraversal::AddTree(MfTreeNode *node, int &count, int type)
{
    if (!type || node->Type() == type)
        mNodes[count++] = node;
    for (MfTreeNode *child = node->firstChild(); child; child = child->nextSibling()) {
        if (!mSameOwner || ((MfSkeletonNode *)child)->mOwner == ((MfSkeletonNode *)node)->mOwner)
            AddTree(child, count, type);
    }
}

// 0x1004fa60 ?NodeIndex@MfSkeletonTraversal@@QAEHVMfString@@@Z
int MfSkeletonTraversal::NodeIndex(MfString name)
{
    name.ConvertToUpper();
    MfString nodeName;
    for (int i = 0; i < mNumNodes; i++) {
        nodeName = node(i)->name();
        nodeName.ConvertToUpper();
        if (nodeName == name)
            return i;
    }
    return -1;
}

// 0x1004fcc0 ?Node@MfSkeletonTraversal@@QAEPAVMfSkeletonNode@@VMfString@@@Z
MfSkeletonNode *MfSkeletonTraversal::Node(MfString name)
{
    int i = NodeIndex(name);
    return i == -1 ? 0 : node(i);
}

// 0x1004fd30 ?CopyTransforms@MfSkeletonTraversal@@QAEPAVMfTransform3@@XZ
MfTransform3 *MfSkeletonTraversal::CopyTransforms()
{
    MfTransform3 *transforms = new MfTransform3[mNumNodes];
    for (int i = 0; i < mNumNodes; i++)
        transforms[i] = node(i)->mTransform;
    return transforms;
}

// 0x1004fe00 ?GetTransforms@MfSkeletonTraversal@@QAEXPAVMfTransform3@@@Z
void MfSkeletonTraversal::GetTransforms(MfTransform3 *transforms)
{
    for (int i = 0; i < mNumNodes; i++)
        transforms[i] = node(i)->mTransform;
}

// 0x1004fe60 ?SetTransforms@MfSkeletonTraversal@@QAEXPBVMfTransform3@@@Z
void MfSkeletonTraversal::SetTransforms(const MfTransform3 *transforms)
{
    for (int i = 0; i < mNumNodes; i++)
        node(i)->setTransform(transforms[i]);
}

// 0x1004fe90 ?GetGlobalTransforms@MfSkeletonTraversal@@QAEXPAVMfTransform3@@@Z
void MfSkeletonTraversal::GetGlobalTransforms(MfTransform3 *transforms)
{
    for (int i = 0; i < mNumNodes; i++)
        transforms[i] = node(i)->mGlobalTransform;
}
