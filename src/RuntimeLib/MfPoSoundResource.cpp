// MfPoSoundResource (RuntimeLib.dll 0x10071b20-0x100738c0): the Piccolo
// object of a sound. SoundResource(actor) creates one with a new sound of the
// driver; LoadFromFile loads a wave file (found through the sound path of the
// actor), Play/Resume play it at the actor (looped/once), Stop stops it.
#include "MfPoSoundResource.h"
#include "MfLiveActor.h"
#include "MfPoActor.h"
#include "MfScClassInfo.h"
#include "MfScDataHandle.h"
#include "MfScObjString.h"
#include "MfScPropTokenTable.h"
#include "MfScPropertyExtList.h"
#include "MfScVmEnv.h"
#include <typeinfo.h>

// the parameters of a sound (MfISoundResource::GetParameter)
enum {
    SOUND_VOLUME, SOUND_PAN, SOUND_CUTOFF_DISTANCE, SOUND_ATTENUATION, SOUND_AMBIENT,
    SOUND_STREAM, SOUND_LOOP, SOUND_DURATION, SOUND_SAMPLING_RATE, SOUND_SAMPLE_SIZE
};

// 0x10071b20 _$E42
// 0x10071b30 _$E39
// 0x10071b50 _$E41
// 0x10071b60 _$E40
static MfString strClassName("SoundResource");
MfPoSoundResource *MfPoSoundResource::poExemplar;
MfScPropertyExtList *MfPoSoundResource::pexlExtensions;
MfScPropTokenTable *MfPoSoundResource::pttPropTokens;

// 0x10071b70 ?GetClassNameA@MfPoSoundResource@@EAE?AVMfString@@XZ (folded)
MfString MfPoSoundResource::GetClassName()
{
    return strClassName;
}

// 0x10071ba0 ??0MfPoSoundResource@@QAE@XZ
MfPoSoundResource::MfPoSoundResource() : MfPoBase(1), mResource(0)
{
    MfISoundDriver *driver = theSoundManager->GetDriver();
    if (driver)
        mResource = driver->CreateResource();
}

// 0x10071c10 ?GetPropTokenTable@MfPoSoundResource@@EAEPAVMfScPropTokenTable@@XZ (folded)
// 0x10071c20 ?GetPropertyExtensions@MfPoSoundResource@@EAEPAVMfScPropertyExtList@@XZ (folded)
// 0x10071c30 ??_GMfPoSoundResource@@UAEPAXI@Z (folded)

// 0x10071c50 ??0MfPoSoundResource@@AAE@VExemplar@MfScData@@@Z
MfPoSoundResource::MfPoSoundResource(MfScData::Exemplar) : MfPoBase(0), mResource(0)
{
    mFlags = (mFlags & ~0x400) | 0x200;
    Name() = strClassName + MfString("<class>");
    poExemplar = this;

    MfScClassInfo *ci = new MfScClassInfo("SoundResource");
    MfScCiMethod *m;
    SetProperty(0, "SoundResource", MfScDataHandle::CreateData((MfScMethod)&MfPoSoundResource::PMSoundResource, "SoundResource::SoundResource", 1), 1);
    m = new MfScCiMethod("SoundResource");
    *ci + m;
    *m + new MfScMethodParameterInfo("Actor actor", 0);
    SetProperty(0, "LoadFromFile", MfScDataHandle::CreateData((MfScMethod)&MfPoSoundResource::PMLoadFromFile, "SoundResource::LoadFromFile", 1), 1);
    m = new MfScCiMethod("LoadFromFile");
    *ci + m;
    *m + new MfScMethodParameterInfo("String filename", 0);
    SetProperty(0, "Play", MfScDataHandle::CreateData((MfScMethod)&MfPoSoundResource::PMPlay, "SoundResource::Play", 0), 1);
    m = new MfScCiMethod("Play");
    *ci + m;
    SetProperty(0, "Stop", MfScDataHandle::CreateData((MfScMethod)&MfPoSoundResource::PMStop, "SoundResource::Stop", 0), 1);
    m = new MfScCiMethod("Stop");
    *ci + m;
    SetProperty(0, "Resume", MfScDataHandle::CreateData((MfScMethod)&MfPoSoundResource::PMResume, "SoundResource::Resume", 0), 1);
    m = new MfScCiMethod("Resume");
    *ci + m;
    *ci + new MfScCiProperty("volume - float");
    *ci + new MfScCiProperty("pan - float");
    *ci + new MfScCiProperty("cutoffDistance - float");
    *ci + new MfScCiProperty("attenuation - float");
    *ci + new MfScCiProperty("ambient - boolean");
    *ci + new MfScCiProperty("stream - boolean");
    *ci + new MfScCiProperty("loop - boolean");
    *ci + new MfScCiProperty("duration - int (read only)");
    *ci + new MfScCiProperty("samplingRate - int (read only)");
    *ci + new MfScCiProperty("sampleSize - int (read only)");
    MfScDataObject::AddClassInfo(ci);

    pttPropTokens = new MfScPropTokenTable;
    pttPropTokens->Add("volume", 0);
    pttPropTokens->Add("pan", 1);
    pttPropTokens->Add("cutoffDistance", 2);
    pttPropTokens->Add("attenuation", 4);
    pttPropTokens->Add("ambient", 3);
    pttPropTokens->Add("stream", 5);
    pttPropTokens->Add("loop", 6);
    pttPropTokens->Add("duration", 7);
    pttPropTokens->Add("samplingRate", 8);
    pttPropTokens->Add("sampleSize", 9);
    pexlExtensions = new MfScPropertyExtList;
}

// 0x10072d80 ??1MfPoSoundResource@@UAE@XZ
MfPoSoundResource::~MfPoSoundResource()
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
    if (mResource)
        mResource->Release();
}

// 0x10072e30 ?Initialize@MfPoSoundResource@@SAXXZ
void MfPoSoundResource::Initialize()
{
    if (poExemplar == 0) {
        MfScData::Exemplar exemplar;
        poExemplar = new MfPoSoundResource(exemplar);
        MfScDataHandle *handle = MfScDataHandle::CreateData(poExemplar);
        handle->SetReadOnly();
        MfScVmEnv::SetGlobalVariable(strClassName, handle, 1, 0);
    }
}

static int bShutdown;

// 0x10072ec0 ?Shutdown@MfPoSoundResource@@SAXXZ (folded)
void MfPoSoundResource::Shutdown()
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

// 0x10072f20 ?GetProperty@MfPoSoundResource@@UAE?AW4MfPropertyErrorCode@MfScDataObject@@ABVMfString@@PAPAVMfScDataHandle@@@Z
MfScDataObject::MfPropertyErrorCode MfPoSoundResource::GetProperty(const MfString &name, MfScDataHandle **value)
{
    MfPropertyErrorCode err = (MfPropertyErrorCode)0;
    *value = 0;
    if (GetPropertyExt(name, value))
        return (MfPropertyErrorCode)0;
    if (this == poExemplar)
        err = MfScDataObject::GetProperty(name, value);
    else if (mResource) {
        switch (LookupPropertyToken(name, pttPropTokens)) {
        case 0: {     // volume
            float f;
            mResource->GetParameter(SOUND_VOLUME, &f);
            *value = MfScDataHandle::CreateData(f);
            break;
        }
        case 1: {     // pan
            float f;
            mResource->GetParameter(SOUND_PAN, &f);
            *value = MfScDataHandle::CreateData(f);
            break;
        }
        case 2: {     // cutoffDistance
            float f;
            mResource->GetParameter(SOUND_CUTOFF_DISTANCE, &f);
            *value = MfScDataHandle::CreateData(f);
            break;
        }
        case 4: {     // attenuation
            float f;
            mResource->GetParameter(SOUND_ATTENUATION, &f);
            *value = MfScDataHandle::CreateData(f);
            break;
        }
        case 3: {     // ambient
            float f;
            mResource->GetParameter(SOUND_AMBIENT, &f);
            *value = MfScDataHandle::CreateData(f > 0.0f ? 1 : 0);
            break;
        }
        case 5: {     // stream
            float f;
            mResource->GetParameter(SOUND_STREAM, &f);
            *value = MfScDataHandle::CreateData(f > 0.0f ? 1 : 0);
            break;
        }
        case 6: {     // loop
            float f;
            mResource->GetParameter(SOUND_LOOP, &f);
            *value = MfScDataHandle::CreateData(f > 0.0f ? 1 : 0);
            break;
        }
        case 7: {     // duration
            float f;
            mResource->GetParameter(SOUND_DURATION, &f);
            *value = MfScDataHandle::CreateData((int)f);
            break;
        }
        case 8: {     // samplingRate
            float f;
            mResource->GetParameter(SOUND_SAMPLING_RATE, &f);
            *value = MfScDataHandle::CreateData((int)f);
            break;
        }
        case 9: {     // sampleSize
            float f;
            mResource->GetParameter(SOUND_SAMPLE_SIZE, &f);
            *value = MfScDataHandle::CreateData((int)f);
            break;
        }
        default:
            err = poExemplar->GetProperty(name, value);
        }
    }
    return err;
}

// 0x10073170 ?SetProperty@MfPoSoundResource@@UAE?AW4MfPropertyErrorCode@MfScDataObject@@PAVMfScDataHandle@@ABVMfString@@0H@Z
// A value of the wrong type is ignored; duration, sampling rate and sample
// size cannot be set.
MfScDataObject::MfPropertyErrorCode MfPoSoundResource::SetProperty(MfScDataHandle *self, const MfString &name,
                                                                   MfScDataHandle *value, int release)
{
    MfPropertyErrorCode err = (MfPropertyErrorCode)0;
    if (this == poExemplar)
        return MfScDataObject::SetProperty(self, name, value, release);
    if (value && mResource) {
        switch (LookupPropertyToken(name, pttPropTokens)) {
        case 0:     // volume
            if (value->IsType(0x30))
                mResource->SetChannelVolume(SOUND_VOLUME, value->GetNumber());
            break;
        case 6:     // loop
            if (value->IsType(0x10))
                mResource->SetChannelVolume(SOUND_LOOP, (float)value->mLong);
            break;
        case 1:     // pan
            if (value->IsType(0x30))
                mResource->SetChannelVolume(SOUND_PAN, value->GetNumber());
            break;
        case 2:     // cutoffDistance
            if (value->IsType(0x30))
                mResource->SetChannelVolume(SOUND_CUTOFF_DISTANCE, value->GetNumber());
            break;
        case 4:     // attenuation
            if (value->IsType(0x30))
                mResource->SetChannelVolume(SOUND_ATTENUATION, value->GetNumber());
            break;
        case 3:     // ambient
            if (value->IsType(0x30))
                mResource->SetChannelVolume(SOUND_AMBIENT, value->GetNumber());
            break;
        case 5:     // stream
            if (value->IsType(0x10))
                mResource->SetChannelVolume(SOUND_STREAM, (float)value->mLong);
            break;
        default:
            err = (MfPropertyErrorCode)4;
        }
    } else
        err = (MfPropertyErrorCode)4;
    if (release && value)
        value->Release();
    return err;
}

// 0x100733a0 ?PMSoundResource@MfPoSoundResource@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
// SoundResource(actor): a new sound object for the actor.
MfScDataHandle *MfPoSoundResource::PMSoundResource(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    MfScDataHandle *result = 0;
    if (!CheckNull(MfString("SoundResource.SoundResource()"), nArgs, args, 0)) {
        if (args[0]->IsType(0x200)) {
            MfPoActor *actor = args[0] ? dynamic_cast<MfPoActor *>(args[0]->GetData()) : 0;
            result = MfScDataHandle::CreateData(new MfPoSoundResource);
            MfPoSoundResource *sound = result ? dynamic_cast<MfPoSoundResource *>(result->GetData()) : 0;
            sound->mActor = actor->mActor;
        } else
            InvalidType(MfString("SoundResource"), MfString("SoundResource"), 0, nArgs, args, &typeid(MfPoActor));
    }
    return result;
}

// EQUIV 0x10073500 ?PMLoadFromFile@MfPoSoundResource@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z: reviewed: zero register only: the original keeps ebx = 0 through the expanded MfStringRef constructor (stores the terminator as an immediate there, from bl here), re-zeroes ebx after the extension test and so computes Load(found) == 0 as xor/cmp ebx/sete instead of neg/sbb/inc; the same calls, arguments and conditions in the same order
// LoadFromFile(file): loads the wave file (a name without the extension
// ".wav" is replaced by its file name, without directory, plus ".wav").
// Answers whether it could be loaded.
MfScDataHandle *MfPoSoundResource::PMLoadFromFile(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (!mResource)
        return MfScDataHandle::CreateData(0);
    if (CheckNull(MfString("SoundResource.LoadFromFile()"), nArgs, args, 0))
        return MfScDataHandle::CreateData(0);
    if (!args[0]->IsType(0x100)) {
        InvalidType(MfString("SoundResource"), MfString("LoadFromFile"), 0, nArgs, args, &typeid(MfScObjString));
        return MfScDataHandle::CreateData(0);
    }
    if (mActor)
        theSoundManager->SetActorPath(((MfLiveActor *)mActor)->SoundPath());
    MfString path(*(MfString *)(MfScObjString *)args[0]->GetData());
    MfString name;
    MfString extension;
    path.SplitPath(0, 0, &name, &extension);
    extension.ConvertToUpper();
    if (extension != ".WAV")
        path = name + MfString(".wav");
    MfString found = theSoundManager->FindSoundPath(path);
    return MfScDataHandle::CreateData(mResource->Load(found) == 0);
}

// 0x10073810 ?PMPlay@MfPoSoundResource@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoSoundResource::PMPlay(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (mResource)
        theSoundManager->PlaySound(mResource, mActor, 0, 1);
    return MfScDataHandle::CreateData(1);
}

// 0x10073840 ?PMStop@MfPoSoundResource@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoSoundResource::PMStop(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (mResource)
        theSoundManager->StopSound(mResource);
    return MfScDataHandle::CreateData(1);
}

// 0x10073870 ?PMResume@MfPoSoundResource@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoSoundResource::PMResume(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (mResource)
        theSoundManager->PlaySound(mResource, mActor, 0, 0);
    return MfScDataHandle::CreateData(1);
}

// 0x100738a0 ?FilloutPropertyNameList@MfPoSoundResource@@EAEXPAVMfStringList@@@Z (folded)
void MfPoSoundResource::FilloutPropertyNameList(MfStringList *list)
{
    MfScDataObject::FilloutPropertyNameList(list, pttPropTokens);
}
