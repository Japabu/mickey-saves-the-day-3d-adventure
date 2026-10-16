// MfTransitionProperty, MfTransitionType, MfTransPropList
// (RuntimeLib.dll 1008a590-1008a9e0).
#include <iostream.h>
#include "MfTransitionProperty.h"
#include "MfState.h"

// 0x1008a590 ??0MfTransitionProperty@@QAE@XZ
MfTransitionProperty::MfTransitionProperty()
{
    mType = 0;
    mTransition = 0;
    m1c = 0;
    m04 = 1;
}

// 0x1008a5e0 ??1MfTransitionProperty@@UAE@XZ
MfTransitionProperty::~MfTransitionProperty()
{
    if (m1c)
        delete m1c;
    m1c = 0;
}

// 0x1008a640 ??0MfTransitionType@@QAE@ABVMfString@@@Z
MfTransitionType::MfTransitionType(const MfString &kind)
    : mKindName(kind)
{
    mType = TYPE;
    if (mKindName == "jump")
        mKind = JUMP;
    else if (mKindName == "self_internal")
        mKind = SELF_INTERNAL;
    else if (mKindName == "self_external")
        mKind = SELF_EXTERNAL;
}

// 0x1008a710 ??1MfTransitionType@@UAE@XZ
MfTransitionType::~MfTransitionType()
{
}

// 0x1008a760 ?Clone@MfTransitionType@@UAEPAVMfFSMMutable@@XZ
MfFSMMutable *MfTransitionType::Clone()
{
    MfTransitionType *copy = new MfTransitionType(mKindName);
    copy->SetFullPath(mPath);
    copy->mMachine = mMachine;
    return copy;
}

// 0x1008a7e0 ?Write@MfTransitionType@@UAEXAAVostream@@AAHH@Z
void MfTransitionType::Write(ostream &os, int &indent, int mode)
{
    MfString tabs;
    for (int i = 0; i < indent; i++)
        tabs << '\t';
    os << tabs;
    os << "trans_type " << mKindName << endl;
}

// 0x1008a890 ??0MfTransPropList@@QAE@H@Z
MfTransPropList::MfTransPropList(int ownsItems)
    : mOwnsItems(ownsItems)
{
}

// 0x1008a8d0 ??1MfTransPropList@@UAE@XZ
MfTransPropList::~MfTransPropList()
{
    Clear();
}

// 0x1008a940 ?Clear@MfTransPropList@@UAEXXZ
void MfTransPropList::Clear()
{
    if (mOwnsItems == 1) {
        MfDblListItem *pli;
        for (MfFSMMutable *p = (MfFSMMutable *)GetFirst(pli); p; p = (MfFSMMutable *)GetNext(pli))
            delete p;
    }
    MfDblList::Clear();
}

// EQUIV 0x1008a980 ?Remove@MfTransPropList@@QAEPAXPAXH@Z: reviewed: as MfFSMList::Remove, the not-found path returns the null pointer reloaded from its slot in the original and the constant 0 here; everything else is identical
void *MfTransPropList::Remove(void *item, int keep)
{
    MfDblListItem *pli = MfDblList::Find(item);
    void *removed = pli;
    if (pli) {
        removed = MfDblList::Delete(pli);
        if (mOwnsItems == 1 && removed == item && !keep) {
            if (removed)
                delete (MfFSMMutable *)removed;
            removed = 0;
        }
    }
    return removed;
}

// 0x1008a920 ?Contains@MfTransPropList@@QAEHPAX@Z (folded with MfLiveActorList::Contains)
int MfTransPropList::Contains(void *item)
{
    int found = 0;
    if (MfDblList::Find(item))
        found = 1;
    return found;
}
