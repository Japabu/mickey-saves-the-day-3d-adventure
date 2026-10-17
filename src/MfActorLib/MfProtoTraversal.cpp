// MfActorLib.dll 0x10017bb0-0x10018150: MfProtoTraversal, MfPermutation.
#include "MfProtoTraversal.h"
#include "MfProtoNode.h"

// 0x1000cbe0 ?Node@MfProtoTraversal@@QAEPAVMfProtoNode@@H@Z (inline; the first copy
// is the one of the old-format reader's object)

// 0x10017bb0 ??0MfProtoTraversal@@QAE@PAVMfProtoNode@@H@Z
MfProtoTraversal::MfProtoTraversal(MfProtoNode *root, int style)
{
    mNodes = (MfProtoNode **)operator new(root->treeSize(0) << 2);
    mNumNodes = 0;
    AddSubtree(root, mNumNodes, style);
}

// 0x10017bf0 ??1MfProtoTraversal@@QAE@XZ
MfProtoTraversal::~MfProtoTraversal()
{
    delete mNodes;
}

// 0x10017c00 ?AddSubtree@MfProtoTraversal@@IAEXPAVMfProtoNode@@AAHH@Z
void MfProtoTraversal::AddSubtree(MfProtoNode *node, int &index, int style)
{
    if (!style || node->Style() == style)
        mNodes[index++] = node;
    for (MfProtoNode *child = (MfProtoNode *)node->firstChild(); child;
         child = (MfProtoNode *)child->nextSibling())
        AddSubtree(child, index, style);
}

// 0x10017c50 ?NodeIndex@MfProtoTraversal@@QAEHPAVMfProtoNode@@@Z
int MfProtoTraversal::NodeIndex(MfProtoNode *node)
{
    for (int i = 0; i < mNumNodes; i++)
        if (mNodes[i] == node)
            return i;
    return -1;
}

// 0x10017c80 ?IndexWithName@MfProtoTraversal@@QAEHVMfString@@@Z
// Node names compare without case.
int MfProtoTraversal::IndexWithName(MfString name)
{
    for (int i = 0; i < mNumNodes; i++) {
        MfString nodeName(mNodes[i]->Name());
        nodeName.ConvertToUpper();
        name.ConvertToUpper();
        if (nodeName == name)
            return i;
    }
    return -1;
}

// 0x10017f00 ?NodeWithName@MfProtoTraversal@@QAEPAVMfProtoNode@@VMfString@@@Z
MfProtoNode *MfProtoTraversal::NodeWithName(MfString name)
{
    int i = IndexWithName(name);
    return i == -1 ? 0 : mNodes[i];
}

// 0x10017f70 ?UniqueNameWithPrefix@MfProtoTraversal@@QAE?AVMfString@@V2@@Z
// The prefix itself, or the prefix with the first number that makes it unique.
MfString MfProtoTraversal::UniqueNameWithPrefix(MfString prefix)
{
    if (IndexWithName(prefix) == -1)
        return prefix;
    MfString name;
    int n = 1;
    for (;;) {
        name = prefix + MfString(n);
        if (IndexWithName(name) == -1)
            break;
        n++;
    }
    return name;
}

// 0x100180a0 ??0MfPermutation@@QAE@PAVMfProtoTraversal@@0@Z
MfPermutation::MfPermutation(MfProtoTraversal *from, MfProtoTraversal *to)
{
    int i;
    mNumFrom = from->mNumNodes;
    mFromToTo = (int *)operator new(mNumFrom << 2);
    for (i = 0; i < mNumFrom; i++)
        mFromToTo[i] = to->NodeIndex(from->mNodes[i]);
    mNumTo = to->mNumNodes;
    mToToFrom = (int *)operator new(mNumTo * sizeof(int));
    for (i = 0; i < mNumTo; i++)
        mToToFrom[i] = from->NodeIndex(to->mNodes[i]);
}

// 0x10018130 ??1MfPermutation@@QAE@XZ
MfPermutation::~MfPermutation()
{
    delete mFromToTo;
    delete mToToFrom;
}
