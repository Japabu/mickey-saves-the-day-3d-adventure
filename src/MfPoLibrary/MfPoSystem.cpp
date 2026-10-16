// MfPoSystem (MfPoLibrary.dll 0x1003a400-0x1003ada0; its inline members are
// emitted first by MfPoLibrary.cpp)
#include "MfPoSystem.h"
#include "MfScClassInfo.h"
#include "MfScDataHandle.h"
#include "MfScPropTokenTable.h"
#include "MfScPropertyExtList.h"
#include "MfScVmEnv.h"

// 0x10030010 ?GetPropTokenTable@MfPoSystem@@EAEPAVMfScPropTokenTable@@XZ
// 0x10030020 ?GetPropertyExtensions@MfPoSystem@@EAEPAVMfScPropertyExtList@@XZ
// 0x10030030 ??0MfPoSystem@@QAE@ABV0@@Z (implicit)
// 0x10030060 ??_EMfPoSystem@@UAEPAXI@Z

// 0x1003a400 _$E24
// 0x1003a410 _$E21
// 0x1003a430 _$E23
// 0x1003a440 _$E22
static MfString strClassName("System");
MfPoSystem *MfPoSystem::poExemplar;
MfScPropTokenTable *MfPoSystem::pttPropTokens;
MfScPropertyExtList *MfPoSystem::pelPropExtensions;

// 0x1003a450 ?GetClassNameA@MfPoSystem@@EAE?AVMfString@@XZ
MfString MfPoSystem::GetClassName()
{
    return strClassName;
}

// 0x1003a480 ??0MfPoSystem@@QAE@XZ
MfPoSystem::MfPoSystem() : MfPoBase(1)
{
}

// 0x1003a4a0 ??0MfPoSystem@@AAE@VExemplar@MfScData@@@Z
MfPoSystem::MfPoSystem(MfScData::Exemplar) : MfPoBase(0)
{
    uiFlags = (uiFlags & ~0x400) | 0x200;
    Name() = strClassName + MfString("<class>");
    poExemplar = this;

    MfScClassInfo *ci = new MfScClassInfo("System");
    *ci + new MfScCiProperty("osName - String (read only)");
    *ci + new MfScCiProperty("ramSize - int (read only)");
    *ci + new MfScCiProperty("availableRamSize - int (read only)");

    pttPropTokens = new MfScPropTokenTable;
    pttPropTokens->Add("osName", 0);
    pttPropTokens->Add("ramSize", 1);
    pttPropTokens->Add("availableRamSize", 2);
    MfScDataObject::AddClassInfo(ci);
}

// 0x1003a940 ??1MfPoSystem@@UAE@XZ
MfPoSystem::~MfPoSystem()
{
    if (this == poExemplar) {
        delete pttPropTokens;
        pttPropTokens = 0;
        if (this == poExemplar) {
            delete pelPropExtensions;
            pelPropExtensions = 0;
            if (this == poExemplar)
                poExemplar = 0;
        }
    }
}

// 0x1003a9e0 ?Initialize@MfPoSystem@@SAXXZ
void MfPoSystem::Initialize()
{
    if (poExemplar == 0) {
        MfScData::Exemplar exemplar;
        poExemplar = new MfPoSystem(exemplar);
        MfScDataHandle *handle = MfScDataHandle::CreateData(poExemplar);
        handle->SetReadOnly();
        MfScVmEnv::SetGlobalVariable(strClassName, handle, 1, 0);
    }
}

static int bShutdown;

// 0x1003aa70 ?Shutdown@MfPoSystem@@SAXXZ
void MfPoSystem::Shutdown()
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

// 0x1003aad0 ?GetProperty@MfPoSystem@@UAE?AW4MfPropertyErrorCode@MfScDataObject@@ABVMfString@@PAPAVMfScDataHandle@@@Z
MfScDataObject::MfPropertyErrorCode MfPoSystem::GetProperty(const MfString &name, MfScDataHandle **value)
{
    MfPropertyErrorCode err = PEC_NO_ERROR;
    *value = 0;
    if (GetPropertyExt(name, value))
        return PEC_NO_ERROR;
    int token = LookupPropertyToken(name, pttPropTokens);
    if (token == 1) {
        MEMORYSTATUS memory;
        memory.dwLength = sizeof(memory);
        GlobalMemoryStatus(&memory);
        *value = MfScDataHandle::CreateData((int)memory.dwTotalPhys);
    } else if (token == 2) {
        MEMORYSTATUS memory;
        memory.dwLength = sizeof(memory);
        GlobalMemoryStatus(&memory);
        *value = MfScDataHandle::CreateData((int)memory.dwAvailPhys);
    } else if (token == 0) {
        OSVERSIONINFO version;
        version.dwOSVersionInfoSize = sizeof(version);
        GetVersionEx(&version);
        if (version.dwPlatformId == VER_PLATFORM_WIN32s)
            *value = MfScDataHandle::CreateData("Win32s on Windows 3.1", 0);
        else if (version.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
            *value = MfScDataHandle::CreateData("Win32 on Windows 95", 0);
        else if (version.dwPlatformId == VER_PLATFORM_WIN32_NT)
            *value = MfScDataHandle::CreateData("Win32 on Windows NT", 0);
        else
            *value = MfScDataHandle::CreateData("Unknown OS", 0);
    } else {
        if (this == poExemplar)
            err = MfScDataObject::GetProperty(name, value);
        else
            err = poExemplar->GetProperty(name, value);
    }
    return err;
}

// 0x1003ad00 ?SetProperty@MfPoSystem@@UAE?AW4MfPropertyErrorCode@MfScDataObject@@PAVMfScDataHandle@@ABVMfString@@0H@Z
MfScDataObject::MfPropertyErrorCode MfPoSystem::SetProperty(MfScDataHandle *self, const MfString &name,
                                                            MfScDataHandle *value, int release)
{
    return PEC_NO_ERROR;
}

// 0x1003ad10 ?InitConstants@MfPoSystem@@SA?AVMfString@@XZ
MfString MfPoSystem::InitConstants()
{
    MfString s;
    return s;
}

// 0x1003ad80 ?FilloutPropertyNameList@MfPoSystem@@EAEXPAVMfStringList@@@Z
void MfPoSystem::FilloutPropertyNameList(MfStringList *list)
{
    MfScDataObject::FilloutPropertyNameList(list, pttPropTokens);
}
