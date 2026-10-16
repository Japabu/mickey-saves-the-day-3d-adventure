// MfPoSoundDriver (RuntimeLib.dll 0x10070e00-0x10071b20): the Piccolo class
// object "SoundDriver". Its properties are the driver's volume and speed of
// sound and the listener (a camera node); the methods pause, resume and stop
// all sounds.
#include "MfPoSoundDriver.h"
#include "MfPoCamera.h"
#include "MfAiActorNode.h"
#include "MfScClassInfo.h"
#include "MfScDataHandle.h"
#include "MfScPropTokenTable.h"
#include "MfScPropertyExtList.h"
#include "MfScVmEnv.h"

// 0x10070e00 _$E23
// 0x10070e10 _$E20
// 0x10070e30 _$E22
// 0x10070e40 _$E21
static MfString strClassName("SoundDriver");
int MfPoSoundDriver::bConstructing;
MfPoSoundDriver *MfPoSoundDriver::poExemplar;
MfScPropertyExtList *MfPoSoundDriver::pexlExtensions;
MfScPropTokenTable *MfPoSoundDriver::pttPropTokens;

// 0x10070e50 ?GetClassNameA@MfPoSoundDriver@@EAE?AVMfString@@XZ (folded)
MfString MfPoSoundDriver::GetClassName()
{
    return strClassName;
}

// 0x10070e80 ?GetPropTokenTable@MfPoSoundDriver@@EAEPAVMfScPropTokenTable@@XZ (folded)
// 0x10070e90 ?GetPropertyExtensions@MfPoSoundDriver@@EAEPAVMfScPropertyExtList@@XZ (folded)
// 0x10070ea0 ??_GMfPoSoundDriver@@UAEPAXI@Z (folded)

// 0x10070ec0 ??0MfPoSoundDriver@@AAE@VExemplar@MfScData@@@Z
MfPoSoundDriver::MfPoSoundDriver(MfScData::Exemplar) : MfPoBase(0)
{
    bConstructing = 1;
    mFlags = (mFlags & ~0x400) | 0x200;
    Name() = strClassName + MfString("<class>");
    poExemplar = this;

    MfScClassInfo *ci = new MfScClassInfo("SoundDriver");
    MfScCiMethod *m;
    SetProperty(0, "Stop", MfScDataHandle::CreateData((MfScMethod)&MfPoSoundDriver::PMStop, "SoundDriver::Stop", 0), 1);
    m = new MfScCiMethod("Stop");
    *ci + m;
    SetProperty(0, "Resume", MfScDataHandle::CreateData((MfScMethod)&MfPoSoundDriver::PMResume, "SoundDriver::Resume", 0), 1);
    m = new MfScCiMethod("Resume");
    *ci + m;
    SetProperty(0, "Start", MfScDataHandle::CreateData((MfScMethod)&MfPoSoundDriver::PMStart, "SoundDriver::Start", 0), 1);
    m = new MfScCiMethod("Start");
    *ci + m;
    *ci + new MfScCiProperty("volume - float");
    *ci + new MfScCiProperty("speedOfSound - float");
    *ci + new MfScCiProperty("listener - ActorNode");
    MfScDataObject::AddClassInfo(ci);

    pttPropTokens = new MfScPropTokenTable;
    pttPropTokens->Add("volume", 0);
    pttPropTokens->Add("speedOfSound", 1);
    pttPropTokens->Add("listener", 2);
    pexlExtensions = new MfScPropertyExtList;
    bConstructing = 0;
}

// 0x10071620 ??_GMfScPropertyExtList@@UAEPAXI@Z (local copy for the dllimport class)

// 0x10071640 ??1MfPoSoundDriver@@UAE@XZ
MfPoSoundDriver::~MfPoSoundDriver()
{
    if (this == poExemplar) {
        delete pttPropTokens;
        pttPropTokens = 0;
    }
    if (this == poExemplar) {
        delete pexlExtensions;
        pexlExtensions = 0;
    }
    if (this == poExemplar)
        poExemplar = 0;
}

// 0x100716e0 ?Initialize@MfPoSoundDriver@@SAXXZ
void MfPoSoundDriver::Initialize()
{
    if (poExemplar == 0) {
        MfScData::Exemplar exemplar;
        poExemplar = new MfPoSoundDriver(exemplar);
        MfScDataHandle *handle = MfScDataHandle::CreateData(poExemplar);
        handle->SetReadOnly();
        MfScVmEnv::SetGlobalVariable(strClassName, handle, 1, 0);
    }
}

static int bShutdown;

// 0x10071770 ?Shutdown@MfPoSoundDriver@@SAXXZ (folded)
void MfPoSoundDriver::Shutdown()
{
    if (bShutdown == 0) {
        MfScDataHandle *handle;
        bShutdown = 1;
        MfScVmEnv::GetGlobalVariable(strClassName, &handle);
        if (handle) {
            handle->mFlags &= ~8;
            MfScVmEnv::ClearGlobalVariable(strClassName, 0);
            handle->Release();
        }
    }
}

// 0x100717d0 ?GetProperty@MfPoSoundDriver@@UAE?AW4MfPropertyErrorCode@MfScDataObject@@ABVMfString@@PAPAVMfScDataHandle@@@Z
MfScDataObject::MfPropertyErrorCode MfPoSoundDriver::GetProperty(const MfString &name, MfScDataHandle **value)
{
    MfPropertyErrorCode err = (MfPropertyErrorCode)0;
    *value = 0;
    MfISoundDriver *driver = theSoundManager->GetDriver();
    if (GetPropertyExt(name, value))
        return (MfPropertyErrorCode)0;
    switch (LookupPropertyToken(name, pttPropTokens)) {
    case 0:     // volume
        if (driver) {
            float volume;
            driver->GetParameter(0, &volume);
            *value = MfScDataHandle::CreateData(volume);
        }
        break;
    case 1:     // speedOfSound
        if (driver) {
            float speed;
            driver->GetParameter(1, &speed);
            *value = MfScDataHandle::CreateData(speed);
        }
        break;
    case 2:     // listener
        if (theSoundManager && driver)
            *value = theSoundManager->GetListener()->GetPiccoloHandle();
        break;
    default:
        if (this == poExemplar)
            err = MfScDataObject::GetProperty(name, value);
        else
            err = poExemplar->GetProperty(name, value);
    }
    return err;
}

// 0x100718d0 ?SetProperty@MfPoSoundDriver@@UAE?AW4MfPropertyErrorCode@MfScDataObject@@PAVMfScDataHandle@@ABVMfString@@0H@Z
// Properties of the exemplar's own (the methods) can only be set while it is
// constructed; an existing property is not replaced.
MfScDataObject::MfPropertyErrorCode MfPoSoundDriver::SetProperty(MfScDataHandle *self, const MfString &name,
                                                                 MfScDataHandle *value, int release)
{
    if (SetPropertyExt(self, name, value, release))
        return (MfPropertyErrorCode)0;
    if (bConstructing && this == poExemplar)
        return MfScDataObject::SetProperty(self, name, value, release);
    MfScDataHandle *existing;
    if (MfScDataObject::GetProperty(name, &existing) == 0) {
        if (existing)
            existing->Release();
        if (release && value)
            value->Release();
        return (MfPropertyErrorCode)1;
    }
    MfPropertyErrorCode err = (MfPropertyErrorCode)0;
    MfISoundDriver *driver = theSoundManager->GetDriver();
    if (value) {
        switch (LookupPropertyToken(name, pttPropTokens)) {
        case 0:     // volume
            if (value->IsType(0x30)) {
                float volume = value->GetNumber();
                if (driver)
                    driver->SetParameter(0, volume);
            } else
                err = (MfPropertyErrorCode)4;
            break;
        case 1:     // speedOfSound
            if (value->IsType(0x30))
                driver->SetParameter(1, value->GetNumber());
            else
                err = (MfPropertyErrorCode)4;
            break;
        case 2: {   // listener
            MfPoCamera *camera;
            if (value->IsType(0x200) && (camera = dynamic_cast<MfPoCamera *>(value->GetData())) != 0
                && theSoundManager && driver)
                theSoundManager->SetListener(camera->mActorNode);
            else
                err = (MfPropertyErrorCode)4;
            break;
        }
        }
    } else
        err = (MfPropertyErrorCode)4;
    if (release && value)
        value->Release();
    return err;
}

// 0x10071aa0 ?FilloutPropertyNameList@MfPoSoundDriver@@EAEXPAVMfStringList@@@Z (folded)
void MfPoSoundDriver::FilloutPropertyNameList(MfStringList *list)
{
    MfScDataObject::FilloutPropertyNameList(list, pttPropTokens);
}

// 0x10071ac0 ?PMStop@MfPoSoundDriver@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
// "Stop" pauses all sounds
MfScDataHandle *MfPoSoundDriver::PMStop(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    theSoundManager->PauseAllSounds();
    return MfScDataHandle::CreateData(1);
}

// 0x10071ae0 ?PMResume@MfPoSoundDriver@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoSoundDriver::PMResume(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    theSoundManager->ResumeAllSounds();
    return MfScDataHandle::CreateData(1);
}

// 0x10071b00 ?PMStart@MfPoSoundDriver@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
// "Start" stops all sounds
MfScDataHandle *MfPoSoundDriver::PMStart(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    theSoundManager->StopAllSounds();
    return MfScDataHandle::CreateData(1);
}
