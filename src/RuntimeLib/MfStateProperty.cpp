// MfStateProperty, MfStatePropertyList (RuntimeLib.dll 10087c00-10087f50).
#include "MfStateProperty.h"

// 0x10087c00 ??0MfStateProperty@@QAE@XZ
MfStateProperty::MfStateProperty()
{
    m04 = 0;
    mMachine = 0;
    mParent = 0;
    mDisplayPos = 0;
    mProperties = 0;
    mType = TYPE;
    m24 = 3;
}

// 0x10087c60 ??1MfStateProperty@@UAE@XZ
MfStateProperty::~MfStateProperty()
{
    if (mProperties)
        delete mProperties;
    mProperties = 0;
    if (mDisplayPos)
        delete mDisplayPos;
    mDisplayPos = 0;
}

// 0x10087ce0 ?SetProperties@MfStateProperty@@QAEXPAVMfStatePropertyList@@@Z
void MfStateProperty::SetProperties(MfStatePropertyList *list)
{
    MfDblListItem *pli;
    if (!mProperties)
        mProperties = new MfStatePropertyList(0);
    else
        mProperties->Clear();
    if (list) {
        for (MfStateProperty *p = (MfStateProperty *)list->GetFirst(pli); p; p = (MfStateProperty *)list->GetNext(pli))
            mProperties->AddProperty((MfStateProperty *)p->Clone());
    }
}

// 0x10087da0 ?CollectAll@MfStateProperty@@UAEXPAVMfDblList@@@Z
void MfStateProperty::CollectAll(MfDblList *list)
{
    list->Add(this);
    if (mProperties) {
        MfDblListItem *pli;
        for (MfStateProperty *p = (MfStateProperty *)mProperties->GetFirst(pli); p;
             p = (MfStateProperty *)mProperties->GetNext(pli))
            p->CollectAll(list);
    }
}

// 0x10087df0 ??0MfStatePropertyList@@QAE@H@Z
MfStatePropertyList::MfStatePropertyList(int ownsItems)
    : mOwnsItems(ownsItems)
{
}

// 0x10087e30 ??1MfStatePropertyList@@UAE@XZ
MfStatePropertyList::~MfStatePropertyList()
{
    Clear();
}

// 0x10087e80 ?AddProperty@MfStatePropertyList@@QAEXPAVMfStateProperty@@@Z
void MfStatePropertyList::AddProperty(MfStateProperty *property)
{
    Append(property);
}

// 0x1008a920 ?Contains@MfStatePropertyList@@QAEHPAX@Z (folded with MfLiveActorList::Contains)
int MfStatePropertyList::Contains(void *item)
{
    int found = 0;
    if (MfDblList::Find(item))
        found = 1;
    return found;
}

// 0x10087e90 ?Remove@MfStatePropertyList@@QAEPAXPAXH@Z
void *MfStatePropertyList::Remove(void *item, int keep)
{
    void *removed = 0;
    MfDblListItem *pli = MfDblList::Find(item);
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

// 0x10087ef0 ?Remove@MfStatePropertyList@@QAEPAXAAPAVMfDblListItem@@PAXH@Z
void *MfStatePropertyList::Remove(MfDblListItem *&pli, void *item, int keep)
{
    void *removed = 0;
    pli = MfDblList::Find(item);
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
