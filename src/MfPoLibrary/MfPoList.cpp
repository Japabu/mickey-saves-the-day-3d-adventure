// MfPoList (MfPoLibrary.dll 0x10031650-0x10032890; the inline members are
// emitted first by MfPoBase.cpp)
#include "MfPoList.h"
#include "MfPoBase.h"
#include "MfScDataHandle.h"
#include "MfScPropTokenTable.h"
#include "MfScThread.h"
#include "MfScVmEnv.h"

// 0x10031650 _$E24
// 0x10031660 _$E21
// 0x10031680 _$E23
// 0x10031690 _$E22
static MfString strClassName("PoList");
MfPoList *MfPoList::poExemplar;
MfScPropTokenTable *MfPoList::pttPropTokens;

// 0x100316a0 ?GetClassNameA@MfPoList@@EAE?AVMfString@@XZ
MfString MfPoList::GetClassName()
{
    return strClassName;
}

// 0x100316d0 ??0MfPoList@@QAE@PAVMfListValue@@@Z
MfPoList::MfPoList(MfListValue *list) : MfScObject(1)
{
    mList = list;
}

// 0x10031700 ??0MfPoList@@AAE@VExemplar@MfScData@@@Z
MfPoList::MfPoList(MfScData::Exemplar) : MfScObject(0), mList(0)
{
    uiFlags = (uiFlags & ~0x400) | 0x200;
    Name() = strClassName + MfString("<class>");
    poExemplar = this;

    SetProperty(0, "Clear", MfScDataHandle::CreateData((MfScMethod)&MfPoList::PMClear, "PoList::Clear", 0), 1);
    SetProperty(0, "First", MfScDataHandle::CreateData((MfScMethod)&MfPoList::PMFirst, "PoList::First", 0), 1);
    SetProperty(0, "Next", MfScDataHandle::CreateData((MfScMethod)&MfPoList::PMNext, "PoList::Next", 0), 1);
    SetProperty(0, "Current", MfScDataHandle::CreateData((MfScMethod)&MfPoList::PMCurrent, "PoList::Current", 0), 1);
    SetProperty(0, "Last", MfScDataHandle::CreateData((MfScMethod)&MfPoList::PMLast, "PoList::Last", 0), 1);
    SetProperty(0, "Prev", MfScDataHandle::CreateData((MfScMethod)&MfPoList::PMPrev, "PoList::Prev", 0), 1);
    SetProperty(0, "Append", MfScDataHandle::CreateData((MfScMethod)&MfPoList::PMAppend, "PoList::Append", 1), 1);
    SetProperty(0, "Prepend", MfScDataHandle::CreateData((MfScMethod)&MfPoList::PMPrepend, "PoList::Prepend", 1), 1);
    SetProperty(0, "Find", MfScDataHandle::CreateData((MfScMethod)&MfPoList::PMFind, "PoList::Find", -1), 1);
    SetProperty(0, "Delete", MfScDataHandle::CreateData((MfScMethod)&MfPoList::PMDelete, "PoList::Delete", -1), 1);
    SetProperty(0, "GetItem", MfScDataHandle::CreateData((MfScMethod)&MfPoList::PMGetItem, "PoList::GetItem", 1), 1);
    SetProperty(0, "SetItem", MfScDataHandle::CreateData((MfScMethod)&MfPoList::PMSetItem, "PoList::SetItem", 2), 1);

    pttPropTokens = new MfScPropTokenTable;
    pttPropTokens->Add("count", 0);
}

// 0x10031c90 ??1MfPoList@@UAE@XZ
MfPoList::~MfPoList()
{
    if (this == poExemplar) {
        delete pttPropTokens;
        pttPropTokens = 0;
        if (this == poExemplar)
            poExemplar = 0;
    }
}

// 0x10031d20 ?Initialize@MfPoList@@SAXXZ
void MfPoList::Initialize()
{
    if (poExemplar == 0) {
        MfScData::Exemplar exemplar;
        poExemplar = new MfPoList(exemplar);
        MfScDataHandle *handle = MfScDataHandle::CreateData(poExemplar);
        handle->SetReadOnly();
        MfScVmEnv::SetGlobalVariable(strClassName, handle, 1, 0);
    }
}

static int bShutdown;

// 0x10031db0 ?Shutdown@MfPoList@@SAXXZ
void MfPoList::Shutdown()
{
    if (bShutdown == 0) {
        MfScDataHandle *handle;
        bShutdown = 1;
        MfScVmEnv::GetGlobalVariable(strClassName, &handle);
        if (handle) {
            handle->SetReadWrite();
            MfScVmEnv::ClearGlobalVariable(strClassName, 0);
            handle->Release();
        }
    }
}

// 0x10031e10 ?GetProperty@MfPoList@@UAE?AW4MfPropertyErrorCode@MfScDataObject@@ABVMfString@@PAPAVMfScDataHandle@@@Z
MfScDataObject::MfPropertyErrorCode MfPoList::GetProperty(const MfString &name, MfScDataHandle **value)
{
    if (this == poExemplar)
        return MfScDataObject::GetProperty(name, value);
    if (LookupPropertyToken(name, pttPropTokens) == 0) {
        *value = MfScDataHandle::CreateData(mList->GetCount());
        return PEC_NO_ERROR;
    }
    return poExemplar->GetProperty(name, value);
}

// 0x10031e80 ?SetProperty@MfPoList@@UAE?AW4MfPropertyErrorCode@MfScDataObject@@PAVMfScDataHandle@@ABVMfString@@0H@Z
MfScDataObject::MfPropertyErrorCode MfPoList::SetProperty(MfScDataHandle *self, const MfString &name,
                                                          MfScDataHandle *value, int release)
{
    if (this == poExemplar)
        return MfScDataObject::SetProperty(self, name, value, release);
    // a known property is read only
    int token = LookupPropertyToken(name, pttPropTokens);
    MfPropertyErrorCode err = token != 0 ? PEC_NO_SUCH_PROPERTY : PEC_PERMISSION_DENIED;
    if (release && value)
        value->Release();
    return err;
}

// 0x10031ee0 ?PMClear@MfPoList@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoList::PMClear(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    mList->Clear();
    return 0;
}

// 0x10031ef0 ?PMFirst@MfPoList@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoList::PMFirst(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    MfScDataHandle *result = 0;
    MfTypedValue *value = (MfTypedValue *)mList->GetFirst(mList->pliCursor);
    if (value)
        result = MfPoBase::PropertyValueToPiccolo(value);
    return result;
}

// 0x10031f20 ?PMNext@MfPoList@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoList::PMNext(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    MfScDataHandle *result = 0;
    MfTypedValue *value = (MfTypedValue *)mList->GetNext(mList->pliCursor);
    if (value)
        result = MfPoBase::PropertyValueToPiccolo(value);
    return result;
}

// 0x10031f50 ?PMCurrent@MfPoList@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoList::PMCurrent(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    MfScDataHandle *result = 0;
    MfTypedValue *value = (MfTypedValue *)mList->GetCurrent(mList->pliCursor);
    if (value)
        result = MfPoBase::PropertyValueToPiccolo(value);
    return result;
}

// 0x10031f80 ?PMLast@MfPoList@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoList::PMLast(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    MfScDataHandle *result = 0;
    MfTypedValue *value = (MfTypedValue *)mList->GetLast(mList->pliCursor);
    if (value)
        result = MfPoBase::PropertyValueToPiccolo(value);
    return result;
}

// 0x10031fb0 ?PMPrev@MfPoList@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoList::PMPrev(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    MfScDataHandle *result = 0;
    MfTypedValue *value = (MfTypedValue *)mList->GetPrev(mList->pliCursor);
    if (value)
        result = MfPoBase::PropertyValueToPiccolo(value);
    return result;
}

// 0x10031fe0 ?PMAppend@MfPoList@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoList::PMAppend(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    MfScDataHandle *result = 0;
    MfTypedValue *value = MfPoBase::PiccoloToPropertyValue(args[0], 1);
    if (value) {
        mList->Append(value);
        result = MfScDataHandle::CreateData(mList->GetCount());
    } else {
        MfScVmEnv *env = MfScThread::GetThread()->GetVmEnv();
        env->ThrowException(12, "Invalid parameter type passed to the property list.");
    }
    return result;
}

// 0x10032090 ?PMPrepend@MfPoList@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoList::PMPrepend(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    MfScDataHandle *result = 0;
    MfTypedValue *value = MfPoBase::PiccoloToPropertyValue(args[0], 1);
    if (value) {
        mList->Prepend(value);
        result = MfScDataHandle::CreateData(mList->GetCount());
    } else {
        MfScVmEnv *env = MfScThread::GetThread()->GetVmEnv();
        env->ThrowException(12, "Invalid parameter type passed to the property list.");
    }
    return result;
}

// EQUIV 0x10032140 ?PMDelete@MfPoList@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z: reviewed: result in ebp instead of a stack slot; the nArgs == 0 path shares the value delete with the 1-argument path
MfScDataHandle *MfPoList::PMDelete(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    MfScDataHandle *result = 0;
    if (nArgs == 0) {
        if (mList->pliCursor) {
            MfTypedValue *value = (MfTypedValue *)mList->GetCurrent(mList->pliCursor);
            result = MfPoBase::PropertyValueToPiccolo(value);
            mList->Delete(mList->pliCursor);
            if (value)
                delete value;
        }
    } else if (nArgs == 1) {
        if (IsValidSearchItem(args[0])) {
            MfTypedValue *key = MfPoBase::PiccoloToPropertyValue(args[0], 1);
            if (SearchListForItem(key, 0)) {
                MfTypedValue *value = (MfTypedValue *)mList->GetCurrent(mList->pliCursor);
                result = args[0];
                mList->Delete(mList->pliCursor);
                if (value)
                    delete value;
            }
            if (key)
                delete key;
        } else {
            MfScVmEnv *env = MfScThread::GetThread()->GetVmEnv();
            env->ThrowException(12, "The parameter passed to MfPoList::Delete is an invalid type.");
        }
    } else {
        MfScVmEnv *env = MfScThread::GetThread()->GetVmEnv();
        env->ThrowException(18, "MfPoList::Delete requires 0 or 1 parameters.");
    }
    return result;
}

// 0x100322d0 ?PMGetItem@MfPoList@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoList::PMGetItem(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    MfScDataHandle *result = 0;
    if (args[0]) {
        if (args[0]->IsType(MfScDataHandle::HT_INTEGER)) {
            int index = args[0]->GetInt();
            if (index >= 0 && index < mList->GetCount()) {
                MfDblListItem *pli;
                MfTypedValue *value = (MfTypedValue *)mList->GetFirst(pli);
                for (; index > 0; index--)
                    value = (MfTypedValue *)mList->GetNext(pli);
                result = MfPoBase::PropertyValueToPiccolo(value);
            } else {
                MfScVmEnv *env = MfScThread::GetThread()->GetVmEnv();
                env->ThrowException(12, "Index for array is out of bounds.");
            }
        } else {
            MfScVmEnv *env = MfScThread::GetThread()->GetVmEnv();
            env->ThrowException(10, "GetItem Expecting an integer operand.");
        }
    } else {
        MfScVmEnv *env = MfScThread::GetThread()->GetVmEnv();
        env->ThrowException(6, "List::GetItem requires a non-nil parameter.");
    }
    return result;
}

// 0x10032420 ?PMSetItem@MfPoList@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoList::PMSetItem(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    MfScDataHandle *result = MfScDataHandle::CreateData(0);
    if (args[0]) {
        if (args[0]->IsType(MfScDataHandle::HT_INTEGER)) {
            int index = args[0]->GetInt();
            if (index >= 0 && index < mList->GetCount()) {
                MfDblListItem *pli;
                mList->GetFirst(pli);
                for (; index > 0; index--)
                    mList->GetNext(pli);
                MfTypedValue *value = MfPoBase::PiccoloToPropertyValue(args[1], 1);
                if (value) {
                    mList->Insert(value, MfDblList::LP_BEFORE, pli);
                    MfTypedValue *old = (MfTypedValue *)mList->Delete(pli);
                    if (old)
                        delete old;
                    result->SetInt(1);
                }
            } else {
                MfScVmEnv *env = MfScThread::GetThread()->GetVmEnv();
                env->ThrowException(12, "Index for array is out of bounds.");
            }
        } else {
            MfScVmEnv *env = MfScThread::GetThread()->GetVmEnv();
            env->ThrowException(10, "GetItem Expecting an integer operand.");
        }
    } else {
        MfScVmEnv *env = MfScThread::GetThread()->GetVmEnv();
        env->ThrowException(6, "List::SetItem requires a non-nil parameter.");
    }
    return result;
}

// EQUIV 0x100325c0 ?PMFind@MfPoList@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z: reviewed: imports called through memory instead of cached in registers, this in ebx; same calls, codes and order
MfScDataHandle *MfPoList::PMFind(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    MfScDataHandle *result = 0;
    MfScVmEnv *env;
    if (nArgs != 1 && nArgs != 2) {
        env = MfScThread::GetThread()->GetVmEnv();
        env->ThrowException(18, "MfPoList::Find requires 0 or 1 parameters.");
        return 0;
    }
    if (!args[0]) {
        env = MfScThread::GetThread()->GetVmEnv();
        env->ThrowException(6, "The first parameter cannot be nil.");
    } else if (!IsValidSearchItem(args[0])) {
        env = MfScThread::GetThread()->GetVmEnv();
        env->ThrowException(12, "The search parameter is not a valid type.");
    } else if (nArgs == 1) {
        MfTypedValue *key = MfPoBase::PiccoloToPropertyValue(args[0], 1);
        if (SearchListForItem(key, 0)) {
            result = args[0];
            result->AddRef();
        }
        if (key)
            delete key;
    } else if (nArgs == 2) {
        if (args[1]) {
            if (args[1]->IsType(MfScDataHandle::HT_INTEGER)) {
                MfTypedValue *key = MfPoBase::PiccoloToPropertyValue(args[0], 1);
                if (SearchListForItem(key, args[1]->GetInt())) {
                    result = args[0];
                    result->AddRef();
                }
                if (key)
                    delete key;
            } else {
                env = MfScThread::GetThread()->GetVmEnv();
                env->ThrowException(10, "The second parameter to must be an integer.");
            }
        } else {
            env = MfScThread::GetThread()->GetVmEnv();
            env->ThrowException(6, "The second parameter cannot be nil.");
        }
    }
    return result;
}

// 0x10032800 ?IsValidSearchItem@MfPoList@@AAEHPAVMfScDataHandle@@@Z
int MfPoList::IsValidSearchItem(MfScDataHandle *item)
{
    return item->IsType(MfScDataHandle::HT_NUMBER | MfScDataHandle::HT_STRING) != 0;
}

// 0x10032820 ?SearchListForItem@MfPoList@@AAEHPAVMfTypedValue@@H@Z
int MfPoList::SearchListForItem(MfTypedValue *value, int start)
{
    MfListValueSearchItr matcher;
    mList->pliCursor = mList->Find(value, mList->pliCursor, start, matcher);
    return mList->pliCursor != 0;
}

// 0x10032870 ?FilloutPropertyNameList@MfPoList@@EAEXPAVMfStringList@@@Z
void MfPoList::FilloutPropertyNameList(MfStringList *list)
{
    MfScDataObject::FilloutPropertyNameList(list, pttPropTokens);
}
