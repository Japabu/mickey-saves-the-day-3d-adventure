// MfPoSound (RuntimeLib.dll 0x1006f770-0x10070e00): the Piccolo class
// object "Sound". PlaySound plays a wave file through the sound manager,
// StopSound and Beep work; the other methods are registered but do nothing.
#include "MfPoSound.h"
#include "MfPoActor.h"
#include "MfPoActorNode.h"
#include "MfScDataHandle.h"
#include "MfScObjString.h"
#include "MfScVmEnv.h"
#include <typeinfo.h>

// 0x1006f770 _$E19
// 0x1006f780 _$E16
// 0x1006f7a0 _$E18
// 0x1006f7b0 _$E17
static MfString strClassName("Sound");
MfPoSound *MfPoSound::poExemplar;

// 0x1006f7c0 ?GetClassNameA@MfPoSound@@EAE?AVMfString@@XZ (folded)
MfString MfPoSound::GetClassName()
{
    return strClassName;
}

// 0x1006f7f0 ??_GMfPoSound@@UAEPAXI@Z (folded)

// 0x1006f810 ??0MfPoSound@@AAE@VExemplar@MfScData@@@Z
MfPoSound::MfPoSound(MfScData::Exemplar) : MfPoBase(0)
{
    mResource = 0;
    mFlags = (mFlags & ~0x400) | 0x200;
    Name() = strClassName + MfString("<class>");
    poExemplar = this;

    SetProperty(0, "PlaySound", MfScDataHandle::CreateData((MfScMethod)&MfPoSound::PMPlaySound, "Sound::PlaySound", -1), 1);
    SetProperty(0, "StopSound", MfScDataHandle::CreateData((MfScMethod)&MfPoSound::PMStopSound, "Sound::StopSound", 1), 1);
    SetProperty(0, "LoadSound", MfScDataHandle::CreateData((MfScMethod)&MfPoSound::PMNotLoaded, "Sound::LoadSound", 2), 1);
    SetProperty(0, "UnloadSound", MfScDataHandle::CreateData((MfScMethod)&MfPoSound::PMNotLoaded, "Sound::UnloadSound", 1), 1);
    SetProperty(0, "SetVolume", MfScDataHandle::CreateData((MfScMethod)&MfPoSound::PMNotImplemented, "Sound::SetVolume", 2), 1);
    SetProperty(0, "SetFrequency", MfScDataHandle::CreateData((MfScMethod)&MfPoSound::PMNotImplemented, "Sound::SetFrequency", 2), 1);
    SetProperty(0, "SetPan", MfScDataHandle::CreateData((MfScMethod)&MfPoSound::PMNotImplemented, "Sound::SetPan", 2), 1);
    SetProperty(0, "SetRoomSize", MfScDataHandle::CreateData((MfScMethod)&MfPoSound::PMNotImplemented, "Sound::SetRoomSize", 1), 1);
    SetProperty(0, "SetAttenuation", MfScDataHandle::CreateData((MfScMethod)&MfPoSound::PMNotImplemented, "Sound::SetAttenuation", 4), 1);
    SetProperty(0, "SetConeSize", MfScDataHandle::CreateData((MfScMethod)&MfPoSound::PMNotImplemented, "Sound::SetConeSize", 2), 1);
    SetProperty(0, "SetConeAmbient", MfScDataHandle::CreateData((MfScMethod)&MfPoSound::PMNotImplemented, "Sound::SetConeAmbient", 2), 1);
    SetProperty(0, "SetSourceVelocity", MfScDataHandle::CreateData((MfScMethod)&MfPoSound::PMNotImplemented, "Sound::SetSourceVelocity", 2), 1);
    SetProperty(0, "GetSourceVelocity", MfScDataHandle::CreateData((MfScMethod)&MfPoSound::PMNotImplemented, "Sound::GetSourceVelocity", 1), 1);
    SetProperty(0, "SetListenerVelocity", MfScDataHandle::CreateData((MfScMethod)&MfPoSound::PMNotImplemented, "Sound::SetListenerVelocity", 1), 1);
    SetProperty(0, "SetListenerNode", MfScDataHandle::CreateData((MfScMethod)&MfPoSound::PMNotImplemented, "Sound::SetListenerNode", 1), 1);
    SetProperty(0, "GetListenerVelocity", MfScDataHandle::CreateData((MfScMethod)&MfPoSound::PMNotImplemented, "Sound::GetListenerVelocity", 0), 1);
    SetProperty(0, "SetSpeedOfSound", MfScDataHandle::CreateData((MfScMethod)&MfPoSound::PMNotImplemented, "Sound::SetSpeedOfSound", 1), 1);
    SetProperty(0, "GetSpeedOfSound", MfScDataHandle::CreateData((MfScMethod)&MfPoSound::PMGetSpeedOfSound, "Sound::GetSpeedOfSound", 0), 1);
    SetProperty(0, "GetVolume", MfScDataHandle::CreateData((MfScMethod)&MfPoSound::PMNotImplemented, "Sound::GetVolume", 1), 1);
    SetProperty(0, "GetFrequency", MfScDataHandle::CreateData((MfScMethod)&MfPoSound::PMNotImplemented, "Sound::GetFrequency", 1), 1);
    SetProperty(0, "GetPan", MfScDataHandle::CreateData((MfScMethod)&MfPoSound::PMNotImplemented, "Sound::GetPan", 1), 1);
    SetProperty(0, "GetRoomSize", MfScDataHandle::CreateData((MfScMethod)&MfPoSound::PMNotImplemented, "Sound::GetRoomSize", 0), 1);
    SetProperty(0, "GetSoundDuration", MfScDataHandle::CreateData((MfScMethod)&MfPoSound::PMNotImplemented, "Sound::GetSoundDuration", 1), 1);
    SetProperty(0, "GetConeSize", MfScDataHandle::CreateData((MfScMethod)&MfPoSound::PMNotImplemented, "Sound::GetConeSize", 1), 1);
    SetProperty(0, "GetConeAmbient", MfScDataHandle::CreateData((MfScMethod)&MfPoSound::PMNotImplemented, "Sound::GetConeAmbient", 1), 1);
    SetProperty(0, "ClearSound", MfScDataHandle::CreateData((MfScMethod)&MfPoSound::PMNotImplemented, "Sound::ClearSound", 1), 1);
    SetProperty(0, "Beep", MfScDataHandle::CreateData((MfScMethod)&MfPoSound::PMBeep, "Sound::Beep", 1), 1);
    SetProperty(0, "ClearAllSounds", MfScDataHandle::CreateData((MfScMethod)&MfPoSound::PMNotImplemented, "Sound::ClearAllSounds", -1), 1);
}

// 0x10070280 ??1MfPoSound@@UAE@XZ
MfPoSound::~MfPoSound()
{
    if (this == poExemplar)
        poExemplar = 0;
    if (mResource)
        mResource->Release();
}

// 0x100702f0 ?Initialize@MfPoSound@@SAXXZ
void MfPoSound::Initialize()
{
    if (poExemplar == 0) {
        MfScData::Exemplar exemplar;
        poExemplar = new MfPoSound(exemplar);
        MfScDataHandle *handle = MfScDataHandle::CreateData(poExemplar);
        handle->SetReadOnly();
        MfScVmEnv::SetGlobalVariable(strClassName, handle, 1, 0);
    }
}

static int bShutdown;

// 0x10070380 ?Shutdown@MfPoSound@@SAXXZ (folded)
void MfPoSound::Shutdown()
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

// 0x100703e0 ?GetProperty@MfPoSound@@UAE?AW4MfPropertyErrorCode@MfScDataObject@@ABVMfString@@PAPAVMfScDataHandle@@@Z
MfScDataObject::MfPropertyErrorCode MfPoSound::GetProperty(const MfString &name, MfScDataHandle **value)
{
    MfPropertyErrorCode err = (MfPropertyErrorCode)0;
    *value = 0;
    if (name == "numChannels")
        *value = MfScDataHandle::CreateData(8);
    else if (this == poExemplar)
        err = MfScDataObject::GetProperty(name, value);
    else
        err = poExemplar->GetProperty(name, value);
    return err;
}

// 0x10070450 ?SetProperty@MfPoSound@@UAE?AW4MfPropertyErrorCode@MfScDataObject@@PAVMfScDataHandle@@ABVMfString@@0H@Z
MfScDataObject::MfPropertyErrorCode MfPoSound::SetProperty(MfScDataHandle *self, const MfString &name,
                                                           MfScDataHandle *value, int release)
{
    if (this == poExemplar)
        return MfScDataObject::SetProperty(self, name, value, release);
    if (release && value)
        value->Release();
    return (MfPropertyErrorCode)0;
}

// 0x100704a0 ?PMGetSpeedOfSound@MfPoSound@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoSound::PMGetSpeedOfSound(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    float speed = 0.0f;
    MfISoundDriver *driver = theSoundManager->GetDriver();
    if (driver)
        driver->GetParameter(1, &speed);
    return MfScDataHandle::CreateData(speed);
}

// 0x100704e0 ?PMNotImplemented@MfPoSound@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
// (in the original one function per method, folded by the linker)
MfScDataHandle *MfPoSound::PMNotImplemented(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    return MfScDataHandle::CreateData(1);
}

// 0x100704f0 ?PMPlaySound@MfPoSound@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
// PlaySound(actor, node, file, int, int, number [, int]): plays the wave file
// (a name without the extension ".wav" is replaced by its file name, without
// directory, plus ".wav") at the node of the actor; the
// numbers are checked but not used. Answers the sound or -1.
MfScDataHandle *MfPoSound::PMPlaySound(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    MfPoActor *actor = 0;
    MfPoActorNode *node = 0;
    MfScObjString *file = 0;
    if (nArgs > 0) {
        int *optional = new int[nArgs];
        for (int i = 0; i < nArgs; i++)
            optional[i] = 1;
        optional[1] = 0;
        if (CheckNull(MfString("Sound.PlaySound()"), nArgs, args, optional)) {
            delete optional;
            return MfScDataHandle::CreateData(-1);
        }
        delete optional;
    }
    if (nArgs != 6 && nArgs != 7) {
        WrongNumber(MfString("Sound"), MfString("PlaySound"), nArgs, 2, 6, 7);
        return 0;
    }
    if (nArgs == 6) {
        if (args[0]->IsType(0x200))
            actor = args[0] ? dynamic_cast<MfPoActor *>(args[0]->GetData()) : 0;
        if (args[1] && args[1]->IsType(0x200))
            node = dynamic_cast<MfPoActorNode *>(args[1]->GetData());
        if (!(actor && ((args[1] && node) || !args[1]) && args[2]->IsType(0x100) && args[3]->IsType(0x10)
              && args[4]->IsType(0x10) && args[5]->IsType(0x30))) {
            InvalidType(MfString("Sound"), MfString("PlaySound"), 0, nArgs, args, &typeid(MfPoActor),
                        &typeid(MfPoActorNode), &typeid(MfScObjString), &typeid(int), &typeid(int),
                        &typeid(float));
            return 0;
        }
        file = (MfScObjString *)args[2]->GetData();
    }
    if (nArgs == 7) {
        actor = 0;
        if (args[0]->IsType(0x200))
            actor = args[0] ? dynamic_cast<MfPoActor *>(args[0]->GetData()) : 0;
        if (args[1] && args[1]->IsType(0x200))
            node = dynamic_cast<MfPoActorNode *>(args[1]->GetData());
        if (!(actor && ((args[1] && node) || !args[1]) && args[2]->IsType(0x100) && args[3]->IsType(0x10)
              && args[4]->IsType(0x10) && args[5]->IsType(0x30) && args[6]->IsType(0x10))) {
            InvalidType(MfString("Sound"), MfString("PlaySound"), 0, nArgs, args, &typeid(MfPoActor),
                        &typeid(MfPoActorNode), &typeid(MfScObjString), &typeid(int), &typeid(int),
                        &typeid(float), &typeid(int));
            return 0;
        }
        file = (MfScObjString *)args[2]->GetData();
    }
    MfAiActor *aiActor = actor->mActor;
    MfAiActorNode *aiNode = 0;
    if (node)
        aiNode = node->mActorNode;
    MfString path(*(MfString *)file);
    MfString name;
    MfString extension;
    path.SplitPath(0, 0, &name, &extension);
    extension.ConvertToUpper();
    if (extension != ".WAV")
        path = name + MfString(".wav");
    if (mResource)
        mResource->Release();
    mResource = theSoundManager->PlaySound(path, aiActor, aiNode);
    MfScDataHandle *result;
    if (!mResource)
        result = MfScDataHandle::CreateData(-1);
    else
        result = MfScDataHandle::CreateData((int)mResource);
    return result;
}

// 0x10070a90 ?PMBeep@MfPoSound@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
// Beep(milliseconds): a 500 Hz beep of at least 10 ms.
MfScDataHandle *MfPoSound::PMBeep(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (CheckNull(MfString("Sound.Beep()"), nArgs, args, 0))
        return 0;
    if (args[0]->IsType(0x10)) {
        int duration = args[0]->mLong;
        if (duration < 10)
            duration = 10;
        Beep(500, duration);
        return MfScDataHandle::CreateData(1);
    }
    InvalidType(MfString("Sound"), MfString("Beep"), 0, nArgs, args, &typeid(int));
    return MfScDataHandle::CreateData(0);
}

// 0x10070b60 ?PMStopSound@MfPoSound@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoSound::PMStopSound(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (!CheckNull(MfString("Sound.StopSound()"), nArgs, args, 0)) {
        if (args[0]->IsType(0x100)) {
            MfISoundDriver *driver = theSoundManager->GetDriver();
            if (driver) {
                driver->Halt();
                driver->Purge();
            }
            return MfScDataHandle::CreateData(1);
        }
        InvalidType(MfString("Sound"), MfString("StopSound"), 0, nArgs, args, &typeid(MfScObjString));
    }
    return MfScDataHandle::CreateData(0);
}

// 0x10070c40 ?PMNotLoaded@MfPoSound@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
// LoadSound and UnloadSound (folded)
MfScDataHandle *MfPoSound::PMNotLoaded(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    return MfScDataHandle::CreateData(-1);
}

// 0x10070c50 ?InitConstants@MfPoSound@@SA?AVMfString@@XZ
// The script constants of the sound flags (all 0).
MfString MfPoSound::InitConstants()
{
    MfString s;
    s << "global " << "MF_SOUND_QUEUEWAVE" << " = " << 0 << ";";
    s << "global " << "MF_SOUND_CLEARQUEUE" << " = " << 0 << ";";
    s << "global " << "MF_SOUND_USELRUCHANNEL" << " = " << 0 << ";";
    s << "global " << "MF_SOUND_HIGHPRIORITY" << " = " << 0 << ";";
    s << "global " << "MF_SOUND_FILE" << " = " << 0 << ";";
    s << "global " << "MF_SOUND_FILESTREAM" << " = " << 0 << ";";
    s << "global " << "MF_SOUND_LOADTO_SNDCARDMEM" << " = " << 0 << ";";
    return s;
}
