// PiccoloRT.dll 0x10007900-0x1000832f: MfScDataObject and the variable-pool
// iterators it uses.
#include "MfScDataObject.h"
#include "MfScDataHandle.h"
#include "MfScHandleDict.h"
#include "MfScLinkData.h"
#include "MfScPropToken.h"
#include "MfScPropTokenTable.h"
#include "MfScPropertyExt.h"
#include "MfScClassInfo.h"
#include "MfSortedList.h"
#include "MfStringList.h"
#include "MfAiClassInfo.h"

int MfScDataObject::bAddClassInfo = 1;
MfAiClassInfo *MfScDataObject::paiciCiInterface;
MfSortedList *MfScDataObject::pslClassInfo;

// Counts the references to one handle in a variable pool and remembers the names.
class MfScCircLinkCounter : public MfCallBack {
public:
    MfScCircLinkCounter(MfScDataHandle *target, MfScListLinkData<MfString> *links)
        : m_target(target), m_count(0), m_links(links) {}
    virtual int CallBack(void *key, void *data, void *);
    MfScDataHandle *m_target;               // +4
    int m_count;                            // +8
    MfScListLinkData<MfString> *m_links;    // +0xc
};

// Counts the references to one handle through a variable pool.
class MfScThroughCounter : public MfCallBack {
public:
    MfScThroughCounter(MfScDataHandle *target) : m_target(target), m_count(0) {}
    virtual int CallBack(void *key, void *data, void *);
    MfScDataHandle *m_target;   // +4
    int m_count;                // +8
};

// Adds the property names of a token table to a string list.
class MfScPropNameLister : public MfCallBack {
public:
    MfScPropNameLister(MfStringList *list) : m_list(list) {}
    virtual int CallBack(void *key, void *data, void *);
    MfStringList *m_list;       // +4
};

// Adds the names of a variable pool to a string list.
class MfScHandleNameLister : public MfCallBack {
public:
    MfScHandleNameLister(MfStringList *list) : m_list(list) {}
    virtual int CallBack(void *key, void *data, void *);
    MfStringList *m_list;       // +4
};

// 0x10007900 ??0MfScDataObject@@QAE@I@Z
MfScDataObject::MfScDataObject(unsigned int flags)
    : mInstanceProperties(0), mFlags(0x400)
{
    if (~flags & kNoInstanceProperties)
        mInstanceProperties = new MfScHandleDict;
    if (flags & kObject) {
        mFlags &= ~0x400;
        mFlags |= 0x200;
    }
}

// 0x100079b0 ??1MfScDataObject@@UAE@XZ
MfScDataObject::~MfScDataObject()
{
    delete mInstanceProperties;
}

// 0x10007a10 ?GetProperty@MfScDataObject@@UAE?AW4MfPropertyErrorCode@1@ABVMfString@@PAPAVMfScDataHandle@@@Z
MfScDataObject::MfPropertyErrorCode MfScDataObject::GetProperty(MfString const &name, MfScDataHandle **value)
{
    *value = 0;
    if (!mInstanceProperties)
        return kPropNoInstanceProperties;
    MfScDataHandle *h = mInstanceProperties->Find(name);
    if (h == pdhUndefined) {
        *value = 0;
        return kPropOk;
    }
    if (!h)
        return kPropNotFound;
    h->AddRef();
    *value = h;
    return kPropOk;
}

// 0x10007a70 ?SetProperty@MfScDataObject@@UAE?AW4MfPropertyErrorCode@1@PAVMfScDataHandle@@ABVMfString@@0H@Z
MfScDataObject::MfPropertyErrorCode MfScDataObject::SetProperty(MfScDataHandle *self, MfString const &name,
                                                                MfScDataHandle *value, int passRef)
{
    if (!mInstanceProperties) {
        if (passRef == 1 && value)
            value->Release();
        return kPropNoInstanceProperties;
    }
    if (self && self->IsReadOnly()) {
        if (passRef == 1 && value)
            value->Release();
        return kPropReadOnly;
    }
    if (!value) {
        mInstanceProperties->Replace(name, pdhUndefined, MfScHandleDict::kAddRef);
        return kPropOk;
    }
    mInstanceProperties->Replace(name, value, (MfScHandleDict::HandleDictPassRef)(passRef != 0));
    return kPropOk;
}

// 0x10007b10 ?CountCircLinks@MfScDataObject@@MAEPAVMfScLinkData@@PAVMfScDataHandle@@@Z
MfScLinkData *MfScDataObject::CountCircLinks(MfScDataHandle *target)
{
    MfScListLinkData<MfString> *links = new MfScListLinkData<MfString>;
    if (mInstanceProperties) {
        MfScCircLinkCounter counter(target, links);
        mInstanceProperties->Iterate(counter);
        links->mCount = counter.m_count;
    }
    return links;
}

// 0x10007ba0 ??1MfScCircLinkCounter@@UAE@XZ (inline, header)

// 0x10007bb0 ?CallBack@MfScCircLinkCounter@@UAEHPAX00@Z
int MfScCircLinkCounter::CallBack(void *key, void *data, void *)
{
    MfScDataHandle *h = (MfScDataHandle *)data;
    if (h == m_target) {
        m_count++;
        m_links->Add(*(MfString *)key);
        return 1;
    }
    if (h && h != pdhUndefined && h->IsType(0x7c0)) {
        MfScData *d = h->GetData();
        h->SetVisiting();
        int n = d->GetThroughCount(m_target);
        if (n) {
            m_count += n;
            m_links->Add(*(MfString *)key);
        }
        h->ResetVisiting();
    }
    return 1;
}

// 0x10007c60 ?ReleaseCircLinks@MfScDataObject@@MAEXPAVMfScLinkData@@@Z
void MfScDataObject::ReleaseCircLinks(MfScLinkData *data)
{
    MfScListLinkData<MfString> *links = (MfScListLinkData<MfString> *)data;
    for (MfScListLinkItem<MfString> *item = links->m_head; item; item = item->m_next)
        mInstanceProperties->Delete(item->m_value, 0);
}

// 0x10007c90 ?GetThroughCount@MfScDataObject@@MAEHPAVMfScDataHandle@@@Z
int MfScDataObject::GetThroughCount(MfScDataHandle *target)
{
    MfScThroughCounter counter(target);
    if (mInstanceProperties)
        mInstanceProperties->Iterate(counter);
    return counter.m_count;
}

// 0x10007cf0 ?CallBack@MfScThroughCounter@@UAEHPAX00@Z
int MfScThroughCounter::CallBack(void *, void *data, void *)
{
    MfScDataHandle *h = (MfScDataHandle *)data;
    if (h == m_target) {
        m_count++;
        return 1;
    }
    if (h && h != pdhUndefined && h->IsType(0x7c0) && !h->IsVisiting()) {
        MfScData *d = h->GetData();
        h->SetVisiting();
        m_count += d->GetThroughCount(m_target);
        h->ResetVisiting();
    }
    return 1;
}

// 0x10007d60 ?AddClassInfo@MfScDataObject@@SAXPAVMfScClassInfo@@@Z
void MfScDataObject::AddClassInfo(MfScClassInfo *ci)
{
    if (bAddClassInfo) {
        if (!pslClassInfo)
            pslClassInfo = new MfSortedList;
        LiiExemplar lii;
        MfScClassInfo exemplar(lii);
        MfDblListItem *pos = pslClassInfo->Find(ci, exemplar);
        if (pos) {
            MfScClassInfo *existing = (MfScClassInfo *)pslClassInfo->GetCurrent(pos);
            existing->AddItems(ci);
            delete ci;
        } else {
            pslClassInfo->Add(ci);
        }
    } else {
        delete ci;
    }
}

// 0x10007e70 ?DisplayClassInfo@MfScDataObject@@SAXPAVCWnd@@@Z
void MfScDataObject::DisplayClassInfo(CWnd *parent)
{
    if (paiciCiInterface)
        paiciCiInterface->ShowClassInfo(parent);
}

// 0x10007e90 ?DestroyClassInfo@MfScDataObject@@SAXXZ
void MfScDataObject::DestroyClassInfo()
{
    if (pslClassInfo) {
        MfDblListItem *pos;
        for (MfScClassInfo *ci = (MfScClassInfo *)pslClassInfo->GetFirst(pos); ci;
             ci = (MfScClassInfo *)pslClassInfo->GetNext(pos))
            delete ci;
        delete pslClassInfo;
        pslClassInfo = 0;
    }
}

// 0x10007ef0 ?CheckClassInfoInterface@MfScDataObject@@SAXXZ
void MfScDataObject::CheckClassInfoInterface()
{
    if (!paiciCiInterface) {
        DestroyClassInfo();
        bAddClassInfo = 0;
    }
}

// 0x10007f10 ?CreateClassTree@MfScDataObject@@SAPAVCWnd@@PAV2@H@Z
CWnd *MfScDataObject::CreateClassTree(CWnd *parent, int style)
{
    CWnd *wnd = 0;
    MfAiClassTree *tree;
    if (paiciCiInterface && paiciCiInterface->CreateClassTree(parent, style, &tree)) {
        MfDblListItem *pos;
        for (MfScClassInfo *ci = (MfScClassInfo *)pslClassInfo->GetFirst(pos); ci;
             ci = (MfScClassInfo *)pslClassInfo->GetNext(pos))
            tree->InsertClass(ci);
        tree->Show();
        wnd = tree->GetWindow();
        tree->Release();
    }
    return wnd;
}

// 0x10007f90 ?LookupPropertyToken@MfScDataObject@@QAEHABVMfString@@PAVMfScPropTokenTable@@@Z
int MfScDataObject::LookupPropertyToken(MfString const &name, MfScPropTokenTable *table)
{
    return LookupPropertyToken(this, name, table);
}

// 0x10007fb0 ?LookupPropertyToken@MfScDataObject@@SAHPAV1@ABVMfString@@PAVMfScPropTokenTable@@@Z
int MfScDataObject::LookupPropertyToken(MfScDataObject *object, MfString const &name, MfScPropTokenTable *table)
{
    int token = -1;
    if (!table && object)
        table = object->GetPropTokenTable();
    if (table) {
        MfScPropToken *t = table->Find(name);
        if (t)
            token = t->Token();
    }
    return token;
}

// 0x10007ff0 ?FilloutPropertyNameList@MfScDataObject@@SAXPAVMfStringList@@PAVMfScPropTokenTable@@@Z
void MfScDataObject::FilloutPropertyNameList(MfStringList *list, MfScPropTokenTable *table)
{
    MfScPropNameLister lister(list);
    table->Iterate(lister);
}

// 0x10008040 ?CallBack@MfScPropNameLister@@UAEHPAX00@Z
int MfScPropNameLister::CallBack(void *, void *data, void *)
{
    m_list->Add(((MfScPropToken *)data)->Property());
    return 1;
}

// 0x10008060 ?GetPropertyNames@MfScDataObject@@UAEXPAVMfStringList@@H@Z
void MfScDataObject::GetPropertyNames(MfStringList *list, int sort)
{
    FilloutPropertyNameList(list);
    if (list->GetCount() == 0) {
        MfScPropTokenTable *table = GetPropTokenTable();
        if (table)
            FilloutPropertyNameList(list, table);
    }
    if (sort && list->GetCount() > 0)
        list->Sort(1);
}

// 0x100080b0 ?CopyHandleDictNamesToList@MfScDataObject@@SAXPAVMfScHandleDict@@PAVMfStringList@@@Z
void MfScDataObject::CopyHandleDictNamesToList(MfScHandleDict *dict, MfStringList *list)
{
    MfScHandleNameLister lister(list);
    dict->Iterate(lister);
}

// 0x10008100 ??_GMfScCircLinkCounter@@UAEPAXI@Z (compiler-generated; folded)

// 0x10008120 ?GetInstanceProperties@MfScDataObject@@UAEXPAVMfStringList@@H@Z
void MfScDataObject::GetInstanceProperties(MfStringList *list, int sort)
{
    if (mInstanceProperties)
        CopyHandleDictNamesToList(mInstanceProperties, list);
    if (sort && list->GetCount())
        list->Sort(1);
}

// 0x10008160 ?CallBack@MfScHandleNameLister@@UAEHPAX00@Z
int MfScHandleNameLister::CallBack(void *key, void *data, void *)
{
    MfScDataHandle *h = (MfScDataHandle *)data;
    if (h && (h == pdhUndefined || !h->IsType(0x800)))
        m_list->Add(*(MfString *)key);
    return 1;
}

// EQUIV 0x10008190 ?GetPropertyExt@MfScDataObject@@QAEHABVMfString@@PAPAVMfScDataHandle@@@Z: reviewed: the Next() result goes through eax into ecx in the original, and the found/not-found exits are laid out in another order
int MfScDataObject::GetPropertyExt(MfString const &name, MfScDataHandle **value)
{
    int found = 0;
    MfScPropertyExtList *exts = GetPropertyExtensions();
    if (exts) {
        MfDblListItem *pos;
        for (MfScPropertyExt *ext = exts->First(pos); ext; ext = exts->Next(pos)) {
            if (ext->GetProperty(name, value)) {
                found = 1;
                break;
            }
        }
    }
    return found;
}

// 0x100081f0 ?SetPropertyExt@MfScDataObject@@QAEHPAVMfScDataHandle@@ABVMfString@@0H@Z
int MfScDataObject::SetPropertyExt(MfScDataHandle *self, MfString const &name, MfScDataHandle *value, int passRef)
{
    int found = 0;
    MfScPropertyExtList *exts = GetPropertyExtensions();
    if (exts) {
        MfDblListItem *pos;
        for (MfScPropertyExt *ext = exts->First(pos); ext; ext = exts->Next(pos)) {
            if (ext->SetProperty(self, name, value, passRef)) {
                found = 1;
                break;
            }
        }
    }
    return found;
}

// EQUIV 0x10008270 ?GetPropertyExtNames@MfScDataObject@@QAEXPAVMfStringList@@H@Z: reviewed: the Next() result goes through eax into ecx in the original (same as GetPropertyExt); same calls and flow
void MfScDataObject::GetPropertyExtNames(MfStringList *list, int sort)
{
    MfScPropertyExtList *exts = GetPropertyExtensions();
    if (exts) {
        MfDblListItem *pos;
        for (MfScPropertyExt *ext = exts->First(pos); ext; ext = exts->Next(pos)) {
            MfScPropTokenTable *table = ext->GetPropTokenTable();
            if (table) {
                MfScPropNameLister lister(list);
                table->Iterate(lister);
            }
        }
    }
    if (sort && list->GetCount())
        list->Sort(1);
}


// Matched by this code:
// 0x10008380 ?Add@?$MfScListLinkData@VMfString@@@@QAEXVMfString@@@Z
// 0x10008410 ??_G?$MfScListLinkData@VMfString@@@@UAEPAXI@Z
// 0x10008430 ??0?$MfScListLinkItem@VMfString@@@@QAE@VMfString@@@Z
// 0x10008490 ??1?$MfScListLinkData@VMfString@@@@UAE@XZ