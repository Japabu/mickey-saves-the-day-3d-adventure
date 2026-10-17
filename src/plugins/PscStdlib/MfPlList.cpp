// PscStdlib.dll 0x10001240-0x10003fa0: the script class "List" and the list
// interface PiccoloRT makes script lists with.
#include "PscStdlib.h"

// The value a variable pool stores for a declared but undefined variable.
static MfScDataHandle *pdhUndefined = (MfScDataHandle *)-1;

// 0x10001240 _$E39
// 0x10001250 _$E36
// 0x10001270 _$E38
// 0x10001280 _$E37
// 0x1000f268
static MfString fsName("List");
// 0x1000f26c: the class object
static MfPlList *pplClass;
// 0x1000f270
static MfScStdLibListInterface *pslliInterface;
// 0x1000f274
static MfScPropTokenTable *pptTokens;
// 0x1000f278
static int bShutdown;

// 0x10001290 ?GetClassNameA@MfPlList@@UAE?AVMfString@@XZ
MfString MfPlList::GetClassNameA()
{
    return fsName;
}

// 0x100012c0 ?CreateList@MfPlListInterface@@UAEHPAPAVMfDblList@@PAPAVMfScDataHandle@@@Z
int MfPlListInterface::CreateList(MfDblList **list, MfScDataHandle **handle)
{
    MfPlList *pl = new MfPlList;
    *list = pl->GetList();
    *handle = MfScDataHandle::CreateData(pl);
    return 1;
}

// 0x10001340 ?GetList@MfPlListInterface@@UAEHPAPAVMfDblList@@PAVMfScDataHandle@@@Z
int MfPlListInterface::GetList(MfDblList **list, MfScDataHandle *handle)
{
    int ok = 0;
    *list = 0;
    if (handle) {
        MfPlList *pl = dynamic_cast<MfPlList *>(handle->GetData());
        if (pl) {
            *list = pl->GetList();
            if (*list)
                ok = 1;
        }
    }
    return ok;
}

// 0x100013a0 ??0MfPlList@@QAE@XZ
MfPlList::MfPlList()
    : MfScDataObject(kNoInstanceProperties | kDataObject)
{
    mList = new MfScHandleList;
    *m_name = "List<instance>";
}

// 0x10001420 ?GetPropTokenTable@MfPlList@@UAEPAVMfScPropTokenTable@@XZ
MfScPropTokenTable *MfPlList::GetPropTokenTable()
{
    return pptTokens;
}

// 0x10001430 ??_GMfScHandleList@@UAEPAXI@Z
// 0x10001450 ??_GMfPlList@@UAEPAXI@Z

// 0x10001470 ??0MfPlList@@QAE@VMfPlClassArg@@@Z
MfPlList::MfPlList(MfPlClassArg)
    : MfScDataObject(0), mList(0)
{
    mFlags = (mFlags & ~kDataObject) | kObject;
    *m_name = fsName + MfString("<class>");
    pplClass = this;
    {
        MfString full("List::List");
        MfString name("List");
        SetProperty(0, name, MfScDataHandle::CreateData((MfScMethod)&MfPlList::List, full, -1), 1);
    }
    {
        MfString full("List::Clear");
        MfString name("Clear");
        SetProperty(0, name, MfScDataHandle::CreateData((MfScMethod)&MfPlList::Clear, full, 0), 1);
    }
    {
        MfString full("List::Copy");
        MfString name("Copy");
        SetProperty(0, name, MfScDataHandle::CreateData((MfScMethod)&MfPlList::Copy, full, 0), 1);
    }
    {
        MfString full("List::First");
        MfString name("First");
        SetProperty(0, name, MfScDataHandle::CreateData((MfScMethod)&MfPlList::First, full, 0), 1);
    }
    {
        MfString full("List::Next");
        MfString name("Next");
        SetProperty(0, name, MfScDataHandle::CreateData((MfScMethod)&MfPlList::Next, full, 0), 1);
    }
    {
        MfString full("List::Current");
        MfString name("Current");
        SetProperty(0, name, MfScDataHandle::CreateData((MfScMethod)&MfPlList::Current, full, 0), 1);
    }
    {
        MfString full("List::Last");
        MfString name("Last");
        SetProperty(0, name, MfScDataHandle::CreateData((MfScMethod)&MfPlList::Last, full, 0), 1);
    }
    {
        MfString full("List::Prev");
        MfString name("Prev");
        SetProperty(0, name, MfScDataHandle::CreateData((MfScMethod)&MfPlList::Prev, full, 0), 1);
    }
    {
        MfString full("List::Append");
        MfString name("Append");
        SetProperty(0, name, MfScDataHandle::CreateData((MfScMethod)&MfPlList::Append, full, 1), 1);
    }
    {
        MfString full("List::Prepend");
        MfString name("Prepend");
        SetProperty(0, name, MfScDataHandle::CreateData((MfScMethod)&MfPlList::Prepend, full, 1), 1);
    }
    {
        MfString full("List::Find");
        MfString name("Find");
        SetProperty(0, name, MfScDataHandle::CreateData((MfScMethod)&MfPlList::Find, full, -1), 1);
    }
    {
        MfString full("List::Delete");
        MfString name("Delete");
        SetProperty(0, name, MfScDataHandle::CreateData((MfScMethod)&MfPlList::Delete, full, -1), 1);
    }
    {
        MfString full("List::GetItem");
        MfString name("GetItem");
        SetProperty(0, name, MfScDataHandle::CreateData((MfScMethod)&MfPlList::GetItem, full, 1), 1);
    }
    {
        MfString full("List::SetItem");
        MfString name("SetItem");
        SetProperty(0, name, MfScDataHandle::CreateData((MfScMethod)&MfPlList::SetItem, full, 2), 1);
    }

    MfScClassInfo *ci = new MfScClassInfo("List");
    *ci + new MfScCiMethod("Clear");
    *ci + new MfScCiMethod("Copy");
    *ci + new MfScCiMethod("First");
    *ci + new MfScCiMethod("Next");
    *ci + new MfScCiMethod("Current");
    *ci + new MfScCiMethod("Last");
    *ci + new MfScCiMethod("Prev");
    MfScCiMethod *method = new MfScCiMethod("Append");
    *ci + method;
    *method + new MfScMethodParameterInfo("var item", 0);
    method = new MfScCiMethod("Prepend");
    *ci + method;
    *method + new MfScMethodParameterInfo("var item", 0);
    method = new MfScCiMethod("Find");
    *ci + method;
    *method + new MfScMethodParameterInfo("var item", 0);
    method = new MfScCiMethod("Delete");
    *ci + method;
    *method + new MfScMethodParameterInfo("var item", 0);
    method = new MfScCiMethod("GetItem");
    *ci + method;
    *method + new MfScMethodParameterInfo("var index", 0);
    method = new MfScCiMethod("SetItem");
    *ci + method;
    *method + new MfScMethodParameterInfo("var index", 0);
    *method + new MfScMethodParameterInfo("var item", 0);
    *ci + new MfScCiProperty("count");
    *ci + new MfScCiProperty("index");
    MfScDataObject::AddClassInfo(ci);

    pptTokens = new MfScPropTokenTable;
    pptTokens->Add("count", kTokenCount);
    pptTokens->Add("index", kTokenIndex);
}

// 0x100027a0 ??1MfPlClassArg@@QAE@XZ
// 0x100027b0 ??_GMfScPropTokenTable@@UAEPAXI@Z
// 0x100027d0 ??1MfSortedListItem@@UAE@XZ
// 0x100027e0 ??_GMfSortedListItem@@UAEPAXI@Z
// 0x10002800 ??_GMfScCiProperty@@UAEPAXI@Z
// 0x10002820 ??_GMfScClassInfo@@UAEPAXI@Z
// 0x10002840 ??_GMfScCiMethod@@UAEPAXI@Z
// 0x10002860 ??_GMfScMethodParameterInfo@@UAEPAXI@Z

// 0x10002880 ??1MfPlList@@UAE@XZ
MfPlList::~MfPlList()
{
    if (this == pplClass) {
        delete pptTokens;
        pptTokens = 0;
    }
    if (this == pplClass)
        pplClass = 0;
    delete mList;
}

// 0x10002910 ?Initialize@MfPlList@@SAXXZ
void MfPlList::Initialize()
{
    if (!pplClass) {
        MfPlClassArg arg;
        pplClass = new MfPlList(arg);
        MfScDataHandle *h = MfScDataHandle::CreateData(pplClass);
        h->SetReadOnly();
        MfScVmEnv::SetGlobalVariable(fsName, h, 1, 0);
    }
    if (!pslliInterface) {
        pslliInterface = new MfPlListInterface;
        MfScStdLib::RegisterListInterface(pslliInterface);
    }
}

// 0x100029f0 ?GetConstants@MfPlList@@SA?AVMfString@@XZ
MfString MfPlList::GetConstants()
{
    return MfString("global LPS_FIRST = ") + MfString(0) + MfString(";")
        + MfString("global LPS_CURRENT = ") + MfString(1) + MfString(";")
        + MfString("global LPS_AFTER_CURRENT = ") + MfString(2) + MfString(";");
}

// 0x10002c30 ?Shutdown@MfPlList@@SAXXZ
void MfPlList::Shutdown()
{
    if (!bShutdown) {
        bShutdown = 1;
        MfScDataHandle *h;
        MfScVmEnv::GetGlobalVariable(fsName, &h);
        if (h) {
            h->SetReadWrite();
            MfScVmEnv::ClearGlobalVariable(fsName, 0);
            h->Release();
        }
    }
    if (pslliInterface) {
        MfScStdLib::RegisterListInterface(0);
        delete pslliInterface;
        pslliInterface = 0;
    }
}

// 0x10002cc0 ?GetProperty@MfPlList@@UAE?AW4MfPropertyErrorCode@MfScDataObject@@ABVMfString@@PAPAVMfScDataHandle@@@Z
MfScDataObject::MfPropertyErrorCode MfPlList::GetProperty(const MfString &name, MfScDataHandle **value)
{
    if (this == pplClass)
        return MfScDataObject::GetProperty(name, value);
    int token = LookupPropertyToken(name, pptTokens);
    if (token == kTokenCount) {
        *value = MfScDataHandle::CreateData(mList->GetCount());
        return kPropOk;
    }
    if (token == kTokenIndex) {
        int index = -1;
        MfScDataHandle *current = mList->Current();
        if (current) {
            MfDblListItem *pli;
            MfScDataHandle *h;
            index = 0;
            for (h = (MfScDataHandle *)mList->GetFirst(pli); h; h = (MfScDataHandle *)mList->GetNext(pli)) {
                if (h == current)
                    break;
                index++;
            }
            if (!h)
                index = -1;
        }
        *value = MfScDataHandle::CreateData(index);
        return kPropOk;
    }
    return pplClass->GetProperty(name, value);
}

// 0x10002da0 ?SetProperty@MfPlList@@UAE?AW4MfPropertyErrorCode@MfScDataObject@@PAVMfScDataHandle@@ABVMfString@@0H@Z
MfScDataObject::MfPropertyErrorCode MfPlList::SetProperty(MfScDataHandle *self, const MfString &name, MfScDataHandle *value, int passRef)
{
    if (this == pplClass)
        return MfScDataObject::SetProperty(self, name, value, passRef);
    MfPropertyErrorCode err;
    int token = LookupPropertyToken(name, pptTokens);
    if (token == kTokenCount || token == kTokenIndex)
        err = kPropReadOnly;
    else
        err = kPropNotFound;
    if (passRef && value)
        value->Release();
    return err;
}

// 0x10002e10 ?List@MfPlList@@QAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPlList::List(MfScDataHandle *, int argc, MfScDataHandle **argv)
{
    MfScDataHandle *h = MfScDataHandle::CreateData(new MfPlList);
    MfPlList *pl = h ? dynamic_cast<MfPlList *>(h->GetData()) : 0;
    for (int i = 0; i < argc; i++)
        if (argv[i])
            pl->mList->Add(argv[i]);
    return h;
}

// 0x10002ed0 ?Clear@MfPlList@@QAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPlList::Clear(MfScDataHandle *, int, MfScDataHandle **)
{
    if (mList) {
        mList->Clear();
        return 0;
    }
    ThrowScriptException(kExInvalidObject, "Clear() called on the List class object.");
    return 0;
}

// 0x10002f60 ?First@MfPlList@@QAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPlList::First(MfScDataHandle *, int, MfScDataHandle **)
{
    if (mList) {
        MfScDataHandle *h = mList->First();
        if (h)
            h->AddRef();
        return h;
    }
    ThrowScriptException(kExInvalidObject, "First() called on the List class object.");
    return 0;
}

// EQUIV 0x10003000 ?Next@MfPlList@@QAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z: reviewed: an independent address computation and member load swap places
MfScDataHandle *MfPlList::Next(MfScDataHandle *, int, MfScDataHandle **)
{
    if (mList) {
        MfScDataHandle *h = mList->Next();
        if (h)
            h->AddRef();
        return h;
    }
    ThrowScriptException(kExInvalidObject, "Next() called on the List class object.");
    return 0;
}

// 0x100030b0 ?Current@MfPlList@@QAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPlList::Current(MfScDataHandle *, int, MfScDataHandle **)
{
    if (mList) {
        MfScDataHandle *h = mList->Current();
        if (h)
            h->AddRef();
        return h;
    }
    ThrowScriptException(kExInvalidObject, "Current() called on the List class object.");
    return 0;
}

// 0x10003150 ?Last@MfPlList@@QAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPlList::Last(MfScDataHandle *, int, MfScDataHandle **)
{
    if (mList) {
        MfScDataHandle *h = mList->Last();
        if (h)
            h->AddRef();
        return h;
    }
    ThrowScriptException(kExInvalidObject, "Last() called on the List class object.");
    return 0;
}

// EQUIV 0x100031f0 ?Prev@MfPlList@@QAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z: reviewed: an independent address computation and member load swap places
MfScDataHandle *MfPlList::Prev(MfScDataHandle *, int, MfScDataHandle **)
{
    if (mList) {
        MfScDataHandle *h = mList->Prev();
        if (h)
            h->AddRef();
        return h;
    }
    ThrowScriptException(kExInvalidObject, "Prev() called on the List class object.");
    return 0;
}

// 0x100032a0 ?Append@MfPlList@@QAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPlList::Append(MfScDataHandle *, int, MfScDataHandle **argv)
{
    if (argv[0]) {
        if (mList) {
            mList->Append(argv[0]);
            return MfScDataHandle::CreateData(mList->GetCount());
        }
        ThrowScriptException(kExInvalidObject, "Append() called on the List class object.");
    } else
        ThrowScriptException(kExNilParameter, "List::Append requires a non-nil parameter.");
    return 0;
}

// 0x10003380 ?Prepend@MfPlList@@QAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPlList::Prepend(MfScDataHandle *, int, MfScDataHandle **argv)
{
    if (argv[0]) {
        if (mList) {
            mList->Prepend(argv[0]);
            return MfScDataHandle::CreateData(mList->GetCount());
        }
        ThrowScriptException(kExInvalidObject, "Prepend() called on the List class object.");
    } else
        ThrowScriptException(kExNilParameter, "List::Prepend requires a non-nil parameter.");
    return 0;
}

// EQUIV 0x10003460 ?Delete@MfPlList@@QAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z: reviewed: register allocation only: for argc == 0 the original computes Current() in eax and copies it to esi after the test (AddRef through eax), ours computes it in esi; the rest is identical, two bytes later
MfScDataHandle *MfPlList::Delete(MfScDataHandle *, int argc, MfScDataHandle **argv)
{
    MfScDataHandle *item = 0;
    if (!mList) {
        ThrowScriptException(kExInvalidObject, "Delete() called on the List class object.");
        return 0;
    }
    if (argc == 0) {
        item = mList->Current();
        if (item) {
            item->AddRef();
            mList->Delete(mList->CurrentPos());
        }
        return item;
    }
    if (argc == 1) {
        if (argv[0]) {
            MfDblListItem *pli = FindItem(argv[0], 0);
            if (pli) {
                item = argv[0];
                mList->Delete(pli);
                item->AddRef();
            }
            return item;
        }
        ThrowScriptException(kExNilParameter, "List::Delete requires a non-nil parameter.");
        return 0;
    }
    ThrowScriptException(kExParameterCount, "List::Delete requires 0 or 1 parameters.");
    return 0;
}

// 0x100035e0 ?GetItem@MfPlList@@QAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPlList::GetItem(MfScDataHandle *, int, MfScDataHandle **argv)
{
    if (argv[0]) {
        if (mList) {
            if (argv[0]->IsType(MfScDataHandle::kInteger)) {
                int index = argv[0]->GetInt();
                if (index >= 0 && index < mList->GetCount()) {
                    MfDblListItem *pli;
                    MfScDataHandle *h = (MfScDataHandle *)mList->GetFirst(pli);
                    while (index > 0) {
                        h = (MfScDataHandle *)mList->GetNext(pli);
                        index--;
                    }
                    h->AddRef();
                    return h;
                }
                ThrowScriptException(kExBadOperand, "Index for array is out of bounds.");
            } else
                ThrowScriptException(kExNotInteger, "GetItem Expecting an integer operand.");
        } else
            ThrowScriptException(kExInvalidObject, "GetItem() called on the List class object.");
    } else
        ThrowScriptException(kExNilParameter, "List::GetItem requires a non-nil parameter.");
    return 0;
}

// 0x10003760 ?SetItem@MfPlList@@QAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPlList::SetItem(MfScDataHandle *, int, MfScDataHandle **argv)
{
    MfScDataHandle *result = MfScDataHandle::CreateData(0);
    if (argv[0]) {
        if (mList) {
            if (argv[0]->IsType(MfScDataHandle::kInteger)) {
                int index = argv[0]->GetInt();
                if (index >= 0 && index < mList->GetCount()) {
                    MfDblListItem *pli;
                    mList->GetFirst(pli);
                    while (index > 0) {
                        mList->GetNext(pli);
                        index--;
                    }
                    mList->Insert(argv[1], MfDblList::LP_BEFORE, pli);
                    mList->Delete(pli);
                    result->SetInt(1);
                    return result;
                }
                ThrowScriptException(kExBadOperand, "Index for array is out of bounds.");
            } else
                ThrowScriptException(kExNotInteger, "GetItem Expecting an integer operand.");
        } else
            ThrowScriptException(kExInvalidObject, "SetItem() called on the List class object.");
    } else
        ThrowScriptException(kExNilParameter, "List::SetItem requires a non-nil parameter.");
    return 0;
}

// 0x10003910 ?Find@MfPlList@@QAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPlList::Find(MfScDataHandle *, int argc, MfScDataHandle **argv)
{
    MfScDataHandle *found = 0;
    if (argc == 1) {
        if (argv[0]) {
            MfDblListItem *pli = FindItem(argv[0], 0);
            if (pli) {
                found = argv[0];
                found->AddRef();
            }
            return found;
        }
        ThrowScriptException(kExNilParameter, "List::Find requires a non-nil parameter.");
    } else if (argc == 2) {
        if (argv[0] && argv[1]) {
            if (argv[1]->IsType(MfScDataHandle::kInteger)) {
                if (FindItem(argv[0], argv[1]->GetInt())) {
                    found = argv[0];
                    found->AddRef();
                }
                return found;
            }
            ThrowScriptException(kExNotInteger, "The second parameter to List::Find must be an integer.");
        } else
            ThrowScriptException(kExNilParameter, "One of the parameters to List::Find was nil.");
    } else
        ThrowScriptException(kExParameterCount, "List::Find requires 0 or 1 parameters.");
    return 0;
}

// 0x10003ac0 ?Copy@MfPlList@@QAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPlList::Copy(MfScDataHandle *, int, MfScDataHandle **)
{
    MfPlList *copy = new MfPlList;
    MfDblListItem *pli;
    MfScDataHandle *h = (MfScDataHandle *)mList->GetFirst(pli);
    while (pli) {
        if (h)
            copy->mList->Add(h);
        h = (MfScDataHandle *)mList->GetNext(pli);
    }
    return MfScDataHandle::CreateData(copy);
}

// 0x10003b60 ?Matches@HandleListSearchClass@@UAEHPAX0@Z
int HandleListSearchClass::Matches(void *item, void *key)
{
    MfScDataHandle *a = (MfScDataHandle *)item;
    MfScDataHandle *b = (MfScDataHandle *)key;
    if (a->IsType(MfScDataHandle::kInteger) && b->IsType(MfScDataHandle::kInteger)) {
        if (a->GetInt() == b->GetInt())
            return 1;
    } else if (a->IsType(MfScDataHandle::kFloat) && b->IsType(MfScDataHandle::kFloat)) {
        if (a->GetFloat() == b->GetFloat())
            return 1;
    } else if (a->IsType(MfScDataHandle::kString) && b->IsType(MfScDataHandle::kString)) {
        MfScObjString *sa = (MfScObjString *)a->GetData();
        MfScObjString *sb = (MfScObjString *)b->GetData();
        if (*sb == *(MfString *)sa)
            return 1;
    } else if (a->IsType(MfScDataHandle::kData) && b->IsType(MfScDataHandle::kData)) {
        if (a->GetData() == b->GetData())
            return 1;
    }
    return 0;
}

// 0x10003c30 ?FindItem@MfPlList@@AAEPAVMfDblListItem@@PAVMfScDataHandle@@H@Z
MfDblListItem *MfPlList::FindItem(MfScDataHandle *item, int start)
{
    MfDblListItem *pli = 0;
    if (mList) {
        HandleListSearchClass search;
        pli = mList->Find(item, mList->CurrentPos(), start, search);
    }
    return pli;
}

// 0x10003c90 ?Copy@MfListItemInterface@@UAEPAXPAX@Z
// 0x10003ca0 ?Iterate@MfListItemInterface@@UAEHPAX@Z
// 0x10003cb0 ?Destroy@MfListItemInterface@@UAEXPAX@Z
// 0x10003cc0 ??1HandleListSearchClass@@QAE@XZ

// 0x10003cd0 ?CountCircLinks@MfPlList@@MAEPAVMfScLinkData@@PAVMfScDataHandle@@@Z
MfScLinkData *MfPlList::CountCircLinks(MfScDataHandle *target)
{
    MfScListLinkData<MfDblListItem *> *links = new MfScListLinkData<MfDblListItem *>;
    int count = 0;
    MfDblListItem *pli;
    for (MfScDataHandle *h = (MfScDataHandle *)mList->GetFirst(pli); h; h = (MfScDataHandle *)mList->GetNext(pli)) {
        if (h == target) {
            count++;
            links->Add(pli);
        } else if (h && h != pdhUndefined && h->IsType(MfScDataHandle::kContainer)) {
            MfScData *data = h->GetData();
            h->SetVisiting();
            int n = data->GetThroughCount(target);
            if (n) {
                count += n;
                links->Add(pli);
            }
            h->ResetVisiting();
        }
    }
    links->mCount = count;
    return links;
}

// 0x10003df0 ?ReleaseCircLinks@MfPlList@@MAEXPAVMfScLinkData@@@Z
void MfPlList::ReleaseCircLinks(MfScLinkData *links)
{
    for (MfScListLinkItem<MfDblListItem *> *item = ((MfScListLinkData<MfDblListItem *> *)links)->m_head; item; item = item->m_next)
        mList->Delete(item->m_value);
}

// 0x10003e20 ?GetThroughCount@MfPlList@@MAEHPAVMfScDataHandle@@@Z
int MfPlList::GetThroughCount(MfScDataHandle *target)
{
    int count = 0;
    MfDblListItem *pli;
    for (MfScDataHandle *h = (MfScDataHandle *)mList->GetFirst(pli); h; h = (MfScDataHandle *)mList->GetNext(pli)) {
        if (h == target)
            count++;
        else if (h && h != pdhUndefined && h->IsType(MfScDataHandle::kContainer) && !h->IsVisiting()) {
            MfScData *data = h->GetData();
            h->SetVisiting();
            count += data->GetThroughCount(target);
            h->ResetVisiting();
        }
    }
    return count;
}

// 0x10003eb0 ?FilloutPropertyNameList@MfPlList@@UAEXPAVMfStringList@@@Z
void MfPlList::FilloutPropertyNameList(MfStringList *names)
{
    MfScDataObject::FilloutPropertyNameList(names, pptTokens);
}

// 0x10003ed0 ??_G?$MfScListLinkData@PAVMfDblListItem@@@@UAEPAXI@Z
// 0x10003ef0 ??1?$MfScListLinkData@PAVMfDblListItem@@@@UAE@XZ
// 0x10003f60 ??_EMfScClassInfo@@W7AEPAXI@Z
// 0x10003f70 ??_EMfScPropTokenTable@@WBE@AEPAXI@Z
// 0x10003f80 ??_EMfScCiMethod@@W3AEPAXI@Z
// 0x10003f90 ??_EMfScCiProperty@@W3AEPAXI@Z
