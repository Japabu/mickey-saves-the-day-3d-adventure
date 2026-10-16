// PiccoloRT.dll 0x1000a3e0-0x1000b81f: the function registry
// (MfScFunctionRegistry) and its modules and entries.
#include "MfScFunctionRegistry.h"
#include "MfScFunctionRegistryModule.h"
#include "MfScFunctionRegistryEntry.h"
#include "MfDblList.h"
#include "MfScHandleList.h"
#include "MfScDataHandle.h"
#include "MfScBytecodeRoutine.h"
#include "MfScDebugInfo.h"
#include "MfScPiccolo.h"
#include "MfAiSourceResolver.h"
#include <string.h>

// 0x1000a3e0 ??0MfScFunctionRegistry@@QAE@ABV0@@Z (compiler-generated)
// 0x1000a410 ??_EMfScFunctionRegistry@@UAEPAXI@Z (compiler-generated)
// 0x1000a460 ??0MfScFunctionRegistryModule@@QAE@ABV0@@Z (compiler-generated)
// 0x1000a4d0 ??4MfScFunctionRegistryModule@@QAEAAV0@ABV0@@Z (compiler-generated)
// 0x1000a500 ??_EMfScFunctionRegistryModule@@UAEPAXI@Z (compiler-generated)
// 0x1000a550 ??0MfScFunctionRegistryEntry@@QAE@ABV0@@Z (compiler-generated)
// 0x1000a570 ??_EMfScFunctionRegistryEntry@@UAEPAXI@Z (compiler-generated)
// 0x1000a660 ??_EMfScHandleList@@UAEPAXI@Z (compiler-generated)

// 0x1000a6b0 ??0MfScFunctionRegistry@@QAE@XZ
MfScFunctionRegistry::MfScFunctionRegistry()
    : mRefCount(1)
{
    mModules = new MfDblList;
    mLock = new MfCriticalSection;
}

// 0x1000a740 ??1MfScFunctionRegistry@@UAE@XZ
MfScFunctionRegistry::~MfScFunctionRegistry()
{
    Lock();
    if (mModules) {
        LiiExemplar lii;
        MfScFunctionRegistryModule exemplar(lii);
        mModules->Clear(exemplar);
    }
    delete mModules;
    mModules = 0;
    Unlock();
    delete mLock;
}

// 0x1000a800 ?GetInterfaceName@MfScFunctionRegistry@@UAEXPADHAAH1@Z
void MfScFunctionRegistry::GetInterfaceName(char *name, int size, int &major, int &minor)
{
    strncpy(name, "FunctionRegistry", size);
    major = 1;
    minor = 0;
}

// 0x1000a830 ?AddRef@MfScFunctionRegistry@@UAEHXZ
int MfScFunctionRegistry::AddRef()
{
    return ++mRefCount;
}

// 0x1000a840 ?Release@MfScFunctionRegistry@@UAEHXZ
int MfScFunctionRegistry::Release()
{
    if (--mRefCount == 0) {
        delete this;
        return 0;
    }
    return mRefCount;
}

// 0x1000a860 ?Add@MfScFunctionRegistry@@UAEHPAVMfScDataHandle@@@Z
int MfScFunctionRegistry::Add(MfScDataHandle *function)
{
    MfScBytecodeRoutine *routine = function ? dynamic_cast<MfScBytecodeRoutine *>(function->GetData()) : 0;
    Lock();
    MfScFunctionRegistryModule *module = FindModule(routine->ModuleName(), 0);
    if (!module) {
        module = new MfScFunctionRegistryModule;
        mModules->Add(module);
    }
    module->AddFunction(function);
    Unlock();
    return 1;
}

// 0x1000a930 ?Remove@MfScFunctionRegistry@@UAEHPAVMfScDataHandle@@@Z
int MfScFunctionRegistry::Remove(MfScDataHandle *function)
{
    int removed = 0;
    MfScBytecodeRoutine *routine = function ? dynamic_cast<MfScBytecodeRoutine *>(function->GetData()) : 0;
    Lock();
    MfDblListItem *pos;
    MfScFunctionRegistryModule *module = FindModule(routine->ModuleName(), &pos);
    if (module) {
        module->RemoveFunction(function);
        if (module->NumberOfFunctions() == 0) {
            mModules->Delete(pos);
            delete module;
        }
        removed = 1;
    }
    Unlock();
    return removed;
}

// 0x1000a9d0 ?Find@MfScFunctionRegistry@@QAEPAVMfScFunctionRegistryEntry@@ABVMfString@@0@Z
MfScFunctionRegistryEntry *MfScFunctionRegistry::Find(const MfString &module, const MfString &name)
{
    Lock();
    MfScFunctionRegistryEntry *entry = 0;
    MfScFunctionRegistryModule *m = FindModule(module, 0);
    if (m)
        entry = m->FindFunction(name, 0);
    Unlock();
    return entry;
}

// 0x1000aa20 ?Find@MfScFunctionRegistry@@UAEHABVMfString@@0PAPAVMfScDataHandle@@@Z
int MfScFunctionRegistry::Find(const MfString &module, const MfString &name, MfScDataHandle **function)
{
    Lock();
    int found = 0;
    *function = 0;
    MfScFunctionRegistryEntry *entry = Find(module, name);
    if (entry) {
        entry->GetFunction(function);
        if (*function)
            found = 1;
    }
    Unlock();
    return found;
}

// 0x1000aa70 ?Find@MfScFunctionRegistry@@QAEPAVMfScFunctionRegistryEntry@@ABVMfString@@H@Z
MfScFunctionRegistryEntry *MfScFunctionRegistry::Find(const MfString &module, int line)
{
    Lock();
    MfScFunctionRegistryEntry *entry = 0;
    MfScFunctionRegistryModule *m = FindModule(module, 0);
    if (m)
        entry = m->FindFunction(line, 0);
    Unlock();
    return entry;
}

// 0x1000aac0 ?Find@MfScFunctionRegistry@@UAEHABVMfString@@HPAPAVMfScDataHandle@@@Z
int MfScFunctionRegistry::Find(const MfString &module, int line, MfScDataHandle **function)
{
    Lock();
    int found = 0;
    *function = 0;
    MfScFunctionRegistryEntry *entry = Find(module, line);
    if (entry) {
        entry->GetFunction(function);
        if (*function)
            found = 1;
    }
    Unlock();
    return found;
}

// 0x1000ab10 ?Find@MfScFunctionRegistry@@UAEHABVMfString@@PAVMfScHandleList@@@Z
int MfScFunctionRegistry::Find(const MfString &module, MfScHandleList *functions)
{
    Lock();
    int found = 0;
    functions->Clear();
    MfScFunctionRegistryModule *m = FindModule(module, 0);
    if (m) {
        m->GetFunctions(functions);
        found = 1;
    }
    Unlock();
    return found;
}

// 0x1000ab60 ?EnumFunctions@MfScFunctionRegistry@@UAEXPAVMfCbRegistryFunctionEnumerator@@PAX@Z
void MfScFunctionRegistry::EnumFunctions(MfCbRegistryFunctionEnumerator *enumerator, void *data)
{
    Lock();
    MfDblListItem *pos;
    MfScFunctionRegistryModule *m = (MfScFunctionRegistryModule *)mModules->GetFirst(pos);
    while (m && m->EnumFunctions(enumerator, data))
        m = (MfScFunctionRegistryModule *)mModules->GetNext(pos);
    Unlock();
}

// 0x1000abc0 ?EnumModules@MfScFunctionRegistry@@UAEXPAVMfCbRegistryModuleEnumerator@@PAX@Z
void MfScFunctionRegistry::EnumModules(MfCbRegistryModuleEnumerator *enumerator, void *data)
{
    Lock();
    MfDblListItem *pos;
    MfScFunctionRegistryModule *m = (MfScFunctionRegistryModule *)mModules->GetFirst(pos);
    while (m && m->EnumModule(enumerator, data))
        m = (MfScFunctionRegistryModule *)mModules->GetNext(pos);
    Unlock();
}

// 0x1000ac20 ?Clear@MfScFunctionRegistry@@UAEHXZ
int MfScFunctionRegistry::Clear()
{
    Lock();
    if (mModules) {
        LiiExemplar lii;
        MfScFunctionRegistryModule exemplar(lii);
        mModules->Clear(exemplar);
    }
    Unlock();
    return 1;
}

// 0x1000acb0 ?CheckOrphans@MfScFunctionRegistry@@UAEHXZ
int MfScFunctionRegistry::CheckOrphans()
{
    if (mModules) {
        Lock();
        MfDblListItem *pos;
        MfScFunctionRegistryModule *m = (MfScFunctionRegistryModule *)mModules->GetFirst(pos);
        while (m) {
            m->CheckOrphans();
            if (m->NumberOfFunctions() == 0) {
                mModules->Delete(pos);
                delete m;
                m = (MfScFunctionRegistryModule *)mModules->GetCurrent(pos);
            } else {
                m = (MfScFunctionRegistryModule *)mModules->GetNext(pos);
            }
        }
        Unlock();
    }
    return 1;
}

// 0x1000ad40 ?FindModule@MfScFunctionRegistry@@MAEPAVMfScFunctionRegistryModule@@ABVMfString@@PAPAVMfDblListItem@@@Z
MfScFunctionRegistryModule *MfScFunctionRegistry::FindModule(const MfString &module, MfDblListItem **pos)
{
    Lock();
    MfDblListItem *item;
    MfScFunctionRegistryModule *m = (MfScFunctionRegistryModule *)mModules->GetFirst(item);
    while (m) {
        MfAiSourceType *type = 0;
        MfAiSourceResolver *resolver = MfScPiccolo::GetSourceResolver();
        if (resolver)
            resolver->Find(m->mModuleType, &type);
        if (type && type->IsSameModule(m->mModule, module)) {
            if (pos)
                *pos = item;
            Unlock();
            type->Release();
            return m;
        }
        if (type)
            type->Release();
        m = (MfScFunctionRegistryModule *)mModules->GetNext(item);
    }
    Unlock();
    return 0;
}

// 0x1000ae20 ??0MfScFunctionRegistryEntry@@QAE@PAVMfScDataHandle@@@Z
MfScFunctionRegistryEntry::MfScFunctionRegistryEntry(MfScDataHandle *function)
    : mData(function)
{
    function->AddRef();
}

// 0x1000ae40 ??0MfScFunctionRegistryEntry@@QAE@VLiiExemplar@@@Z
MfScFunctionRegistryEntry::MfScFunctionRegistryEntry(LiiExemplar)
    : mData(0)
{
}

// 0x1000ae60 ??1MfScFunctionRegistryEntry@@UAE@XZ
MfScFunctionRegistryEntry::~MfScFunctionRegistryEntry()
{
    if (mData)
        mData->Release();
}

// 0x1000ae80 ?Name@MfScFunctionRegistryEntry@@QBE?AVMfString@@XZ
MfString MfScFunctionRegistryEntry::Name() const
{
    MfString name;
    if (mData) {
        MfScBytecodeRoutine *routine = dynamic_cast<MfScBytecodeRoutine *>(mData->GetData());
        name = routine->Name();
    }
    return name;
}

// 0x1000af30 ?ModuleName@MfScFunctionRegistryEntry@@QBE?AVMfString@@XZ
MfString MfScFunctionRegistryEntry::ModuleName() const
{
    MfString name;
    if (mData) {
        MfScBytecodeRoutine *routine = dynamic_cast<MfScBytecodeRoutine *>(mData->GetData());
        name = routine->ModuleName();
    }
    return name;
}

// 0x1000afe0 ?ModuleType@MfScFunctionRegistryEntry@@QBE?AVMfString@@XZ
MfString MfScFunctionRegistryEntry::ModuleType() const
{
    MfString type;
    if (mData) {
        MfScBytecodeRoutine *routine = dynamic_cast<MfScBytecodeRoutine *>(mData->GetData());
        type = routine->ModuleType();
    }
    return type;
}

// 0x1000b090 ?GetRefCount@MfScFunctionRegistryEntry@@QBEHXZ
int MfScFunctionRegistryEntry::GetRefCount() const
{
    return (int)mData->GetRefCount();
}

// 0x1000b0a0 ?GetFunction@MfScFunctionRegistryEntry@@QAEXPAPAVMfScDataHandle@@@Z
void MfScFunctionRegistryEntry::GetFunction(MfScDataHandle **function)
{
    *function = 0;
    if (mData) {
        mData->AddRef();
        *function = mData;
    }
}

// 0x1000b0c0 ?EnumFunction@MfScFunctionRegistryEntry@@QAEHPAVMfCbRegistryFunctionEnumerator@@PAX@Z
int MfScFunctionRegistryEntry::EnumFunction(MfCbRegistryFunctionEnumerator *enumerator, void *data)
{
    return enumerator->Enumerate(mData, data);
}

// 0x1000b0e0 ??0MfScFunctionRegistryModule@@QAE@XZ
MfScFunctionRegistryModule::MfScFunctionRegistryModule()
{
    mFunctions = new MfDblList;
}

// 0x1000b160 ??0MfScFunctionRegistryModule@@QAE@PAVMfScDataHandle@@@Z
MfScFunctionRegistryModule::MfScFunctionRegistryModule(MfScDataHandle *function)
{
    mFunctions = new MfDblList;
    AddFunction(function);
}

// 0x1000b1f0 ??0MfScFunctionRegistryModule@@QAE@VLiiExemplar@@@Z
MfScFunctionRegistryModule::MfScFunctionRegistryModule(LiiExemplar)
    : mFunctions(0)
{
}

// 0x1000b260 ??1MfScFunctionRegistryModule@@UAE@XZ
MfScFunctionRegistryModule::~MfScFunctionRegistryModule()
{
    if (mFunctions) {
        LiiExemplar lii;
        MfScFunctionRegistryEntry exemplar(lii);
        mFunctions->Clear(exemplar);
    }
    delete mFunctions;
}

// 0x1000b310 ?Destroy@MfScFunctionRegistryEntry@@UAEXPAX@Z (= ?Destroy@MfScFunctionRegistryModule@@UAEXPAX@Z)
void MfScFunctionRegistryEntry::Destroy(void *item)
{
    delete (MfScFunctionRegistryEntry *)item;
}

void MfScFunctionRegistryModule::Destroy(void *item)
{
    delete (MfScFunctionRegistryModule *)item;
}

// 0x1000b330 ?FindFunction@MfScFunctionRegistryModule@@QAEPAVMfScFunctionRegistryEntry@@ABVMfString@@PAPAVMfDblListItem@@@Z
MfScFunctionRegistryEntry *MfScFunctionRegistryModule::FindFunction(const MfString &name, MfDblListItem **pos)
{
    MfDblListItem *item;
    MfScFunctionRegistryEntry *entry = (MfScFunctionRegistryEntry *)mFunctions->GetFirst(item);
    while (entry) {
        if (entry->Name() == name) {
            if (pos)
                *pos = item;
            return entry;
        }
        entry = (MfScFunctionRegistryEntry *)mFunctions->GetNext(item);
    }
    return 0;
}

// 0x1000b3e0 ?FindFunction@MfScFunctionRegistryModule@@QAEPAVMfScFunctionRegistryEntry@@HPAPAVMfDblListItem@@@Z
// The function whose code covers the line, else the next one after it.
MfScFunctionRegistryEntry *MfScFunctionRegistryModule::FindFunction(int line, MfDblListItem **pos)
{
    MfDblListItem *item;
    MfScFunctionRegistryEntry *nearest = 0;
    int distance = 0x7fffffff;
    MfScFunctionRegistryEntry *entry = (MfScFunctionRegistryEntry *)mFunctions->GetFirst(item);
    while (entry) {
        MfScDataHandle *function;
        entry->GetFunction(&function);
        MfScBytecodeRoutine *routine = function ? dynamic_cast<MfScBytecodeRoutine *>(function->GetData()) : 0;
        MfScDebugInfo *info = routine->DebugInfo();
        if (info) {
            int first, last;
            info->GetRange(first, last);
            if (first > 0) {
                if (line >= first && line <= last) {
                    function->Release();
                    return entry;
                }
                if (line < first && first - line < distance) {
                    nearest = entry;
                    distance = first - distance;
                }
            }
        }
        function->Release();
        entry = (MfScFunctionRegistryEntry *)mFunctions->GetNext(item);
    }
    return nearest;
}

// 0x1000b4f0 ?AddFunction@MfScFunctionRegistryModule@@UAEXPAVMfScDataHandle@@@Z
void MfScFunctionRegistryModule::AddFunction(MfScDataHandle *function)
{
    MfScBytecodeRoutine *routine = function ? dynamic_cast<MfScBytecodeRoutine *>(function->GetData()) : 0;
    MfDblListItem *pos;
    MfScFunctionRegistryEntry *old = FindFunction(routine->Name(), &pos);
    if (old) {
        mFunctions->Delete(pos);
        delete old;
    }
    if (mFunctions->GetCount() == 0) {
        mModule = routine->ModuleName();
        mModuleType = routine->ModuleType();
    }
    mFunctions->Add(new MfScFunctionRegistryEntry(function));
}

// 0x1000b5e0 ?RemoveFunction@MfScFunctionRegistryModule@@UAEXPAVMfScDataHandle@@@Z
void MfScFunctionRegistryModule::RemoveFunction(MfScDataHandle *function)
{
    MfScBytecodeRoutine *routine = function ? dynamic_cast<MfScBytecodeRoutine *>(function->GetData()) : 0;
    MfDblListItem *pos;
    MfScFunctionRegistryEntry *entry = FindFunction(routine->Name(), &pos);
    if (entry) {
        mFunctions->Delete(pos);
        delete entry;
    }
}

// 0x1000b650 ?NumberOfFunctions@MfScFunctionRegistryModule@@QAEHXZ
int MfScFunctionRegistryModule::NumberOfFunctions()
{
    return mFunctions->GetCount();
}

// 0x1000b660 ?CheckOrphans@MfScFunctionRegistryModule@@QAEXXZ
// Drops the functions only the registry still refers to.
void MfScFunctionRegistryModule::CheckOrphans()
{
    MfDblListItem *pos;
    MfScFunctionRegistryEntry *entry = (MfScFunctionRegistryEntry *)mFunctions->GetFirst(pos);
    while (entry) {
        if (entry->GetRefCount() == 1) {
            mFunctions->Delete(pos);
            delete entry;
            entry = (MfScFunctionRegistryEntry *)mFunctions->GetCurrent(pos);
        } else {
            entry = (MfScFunctionRegistryEntry *)mFunctions->GetNext(pos);
        }
    }
}

// 0x1000b6d0 ?GetFunctions@MfScFunctionRegistryModule@@QAEHPAVMfScHandleList@@@Z
int MfScFunctionRegistryModule::GetFunctions(MfScHandleList *functions)
{
    MfDblListItem *pos;
    MfScFunctionRegistryEntry *entry = (MfScFunctionRegistryEntry *)mFunctions->GetFirst(pos);
    while (entry) {
        MfScDataHandle *function;
        entry->GetFunction(&function);
        functions->Add(function);
        function->Release();
        entry = (MfScFunctionRegistryEntry *)mFunctions->GetNext(pos);
    }
    return 1;
}

// 0x1000b730 ?EnumFunctions@MfScFunctionRegistryModule@@QAEHPAVMfCbRegistryFunctionEnumerator@@PAX@Z
int MfScFunctionRegistryModule::EnumFunctions(MfCbRegistryFunctionEnumerator *enumerator, void *data)
{
    MfDblListItem *pos;
    MfScFunctionRegistryEntry *entry = (MfScFunctionRegistryEntry *)mFunctions->GetFirst(pos);
    while (entry) {
        if (!entry->EnumFunction(enumerator, data))
            return 0;
        entry = (MfScFunctionRegistryEntry *)mFunctions->GetNext(pos);
    }
    return 1;
}

// 0x1000b790 ?EnumModule@MfScFunctionRegistryModule@@QAEHPAVMfCbRegistryModuleEnumerator@@PAX@Z
int MfScFunctionRegistryModule::EnumModule(MfCbRegistryModuleEnumerator *enumerator, void *data)
{
    MfDblListItem *pos;
    MfScFunctionRegistryEntry *entry = (MfScFunctionRegistryEntry *)mFunctions->GetFirst(pos);
    if (entry)
        return enumerator->Enumerate(mModule, entry->ModuleType(), data);
    return 0;
}
