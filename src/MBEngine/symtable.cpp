// symtable.cpp - MBEngine.dll symtable.obj
#include "symtable.h"

long TSymtab::symbolTableCnt;
TSymtab *TSymtab::symbolTableList;

// 0x10035d60 ??0TSymtabNode@@QAE@XZ
TSymtabNode::TSymtabNode(void)
{
    nodeIndex = 0;
    name[0] = 0;
    value.num = 0;
    type = kNumType;
    left = right = 0;
}

// 0x10035d80 ??0TSymtabNode@@QAE@PBDJ@Z
TSymtabNode::TSymtabNode(const char *theName, long index)
{
    nodeIndex = index;
    if (MBstrlen(theName) < 31) {
        MBstrcpy(name, theName);
    } else {
        MBstrncpy(name, theName, 31);
        name[31] = 0;
    }
    value.num = 0;
    type = kNumType;
    left = right = 0;
}

// 0x10035df0 ??1TSymtabNode@@QAE@XZ
TSymtabNode::~TSymtabNode(void)
{
    if (left)
        delete left;
    delete right;
}

// 0x10035e30 ?SetValue@TSymtabNode@@QAEXPBD@Z
void TSymtabNode::SetValue(const char *str)
{
    if (MBstrlen(str) < 31) {
        MBstrcpy(value.str, str);
    } else {
        MBstrncpy(value.str, str, 31);
        value.str[31] = 0;
    }
    type = kStringType;
}

// 0x10035e90 ?SetValue@TSymtabNode@@QAEXJ@Z
void TSymtabNode::SetValue(long num)
{
    value.num = (float)num;
    type = kNumType;
}

// 0x10035eb0 ?SetValue@TSymtabNode@@QAEXM@Z
void TSymtabNode::SetValue(float num)
{
    value.num = num;
    type = kNumType;
}

// 0x10035ed0 ?Print@TSymtabNode@@QBEXXZ
void TSymtabNode::Print(void) const
{
    if (left)
        left->Print();
    printf("node: %-5ld\tname: %-16s\t", nodeIndex, name);
    switch (type) {
    case kNumType:
        printf("value: %g", value.num);
        break;
    case kNameType:
        printf("value: %-32s", name);
        break;
    case kStringType:
        printf("value: %-32s", value.str);
        break;
    }
    printf("\n");
    if (right)
        right->Print();
}

// EQUIV 0x10035f60 ?StreamOut@TSymtabNode@@QAEKPAVIMBStream@@@Z: reviewed: the three lengths of the final sum are added in another order
unsigned long TSymtabNode::StreamOut(IMBStream *outFile)
{
    unsigned long leftLen = left ? left->StreamOut(outFile) : 0;
    if (!outFile->Write(&nodeIndex, 4))
        return (unsigned long)-1;
    if (!outFile->Write(&type, 4))
        return (unsigned long)-1;
    unsigned long length = MBstrlen(name);
    if (!outFile->Write(&length, 4))
        return (unsigned long)-1;
    if (!outFile->Write(name, length))
        return (unsigned long)-1;
    unsigned long thisLen = length + 12;
    unsigned long rightLen = right ? right->StreamOut(outFile) : 0;
    if (rightLen == (unsigned long)-1 || leftLen == (unsigned long)-1)
        return (unsigned long)-1;
    return rightLen + leftLen + thisLen;
}

// 0x10036010 ?StreamIn@TSymtabNode@@QAEKPBD@Z
unsigned long TSymtabNode::StreamIn(const char *buffer)
{
    const char *start = buffer;
    nodeIndex = *(const unsigned long *)buffer;
    buffer += 4;
    type = *(const unsigned long *)buffer;
    buffer += 4;
    unsigned long length = *(const unsigned long *)buffer;
    buffer += 4;
    MBmemcpy(name, buffer, length);
    name[length] = 0;
    value.str[0] = 0;
    return length + (buffer - start);
}

// 0x10036060 ??0TSymtab@@QAE@XZ
TSymtab::TSymtab(void)
{
    root = 0;
    nodeCnt = 0;
    symbolTableIndex = symbolTableCnt;
    ++symbolTableCnt;
    next = symbolTableList;
    symbolTableList = this;
}

// 0x100360a0 ??1TSymtab@@QAE@XZ
TSymtab::~TSymtab(void)
{
    delete root;
}

// 0x100360c0 ?Search@TSymtab@@QBEPAVTSymtabNode@@PBD@Z
TSymtabNode *TSymtab::Search(const char *pString) const
{
    TSymtabNode *p = root;
    while (p) {
        int comp = MBstrcmp(pString, p->name);
        if (comp == 0)
            break;
        p = comp < 0 ? p->left : p->right;
    }
    return p;
}

// 0x10036100 ?Enter@TSymtab@@QAEPAVTSymtabNode@@PBD@Z
TSymtabNode *TSymtab::Enter(const char *pString)
{
    TSymtabNode *p = root;
    TSymtabNode **pp = &root;
    while (p) {
        int comp = MBstrcmp(pString, p->name);
        if (comp == 0)
            return p;
        pp = comp < 0 ? &p->left : &p->right;
        p = *pp;
    }
    p = new TSymtabNode(pString, nodeCnt++);
    *pp = p;
    return p;
}

// 0x10036170 ?StreamOut@TSymtab@@QAEKPAVIMBStream@@@Z
unsigned long TSymtab::StreamOut(IMBStream *outFile)
{
    if (!outFile->Write(&nodeCnt, 4))
        return (unsigned long)-1;
    return root ? root->StreamOut(outFile) + 4 : 4;
}


// Matched by this code:
// 0x100361b0 _$E22
// 0x100361d0 _$E21
