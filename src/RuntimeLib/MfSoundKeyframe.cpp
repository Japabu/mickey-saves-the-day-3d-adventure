// MfSoundKeyframe (RuntimeLib.dll 10053220-100538e0): a sound a skill starts at
// its time ('SKey' keyframes of the sound track).
#include "MfSkillKeyframes.h"
#include "MfSoundManager.h"
#include "MfStream.h"

// 0x10053220 ??0MfSoundKeyframe@@QAE@XZ
MfSoundKeyframe::MfSoundKeyframe()
    : mVolume(1.0f), mName(NULL_STRING), mSound(0), mMute(0), mAmbient(0), mStream(0)
{
}

// 0x100532b0 ??0MfSoundKeyframe@@QAE@ABV0@@Z
MfSoundKeyframe::MfSoundKeyframe(const MfSoundKeyframe &key)
    : MfKeyframe(key), mVolume(key.mVolume), mName(key.mName), mSound(0), mMute(key.mMute),
      mAmbient(key.mAmbient), mStream(key.mStream)
{
    Load();
}

// 0x10053340 ?CreateCopy@MfSoundKeyframe@@UAEPAVMfKeyframe@@XZ
MfKeyframe *MfSoundKeyframe::CreateCopy()
{
    MfSoundKeyframe *copy = new MfSoundKeyframe(*this);
    copy->Load();
    return copy;
}

// 0x100533b0 ??1MfSoundKeyframe@@UAE@XZ
MfSoundKeyframe::~MfSoundKeyframe()
{
    if (mSound)
        mSound->Release();
}

// 0x10053420 ?SetVolume@MfSoundKeyframe@@QAEXM@Z
void MfSoundKeyframe::SetVolume(float volume)
{
    mVolume = volume;
    if (mVolume > 0)
        mVolume = 0;
    if (mVolume < -100.0f)
        mVolume = -100.0f;
    if (mSound)
        mSound->SetChannelVolume(0, mMute ? -100.0f : mVolume);
}

// 0x100534a0 ?SetName@MfSoundKeyframe@@QAEXVMfString@@@Z
void MfSoundKeyframe::SetName(MfString name)
{
    MfString file;
    MfString ext;
    name.SplitPath(0, 0, &file, &ext);
    name = file + ext;
    if (mName != name) {
        mName = name;
        if (mSound) {
            mSound->Release();
            mSound = 0;
        }
        Load();
    }
}

// 0x100535b0 ?Play@MfSoundKeyframe@@QAEHPAVMfAiActorNode@@@Z
int MfSoundKeyframe::Play(MfAiActorNode *node)
{
    if (mSound)
        theSoundManager->PlaySound(mSound, 0, 0, 1);
    return mSound != 0;
}

// 0x100535e0 ?Load@MfSoundKeyframe@@QAEHXZ
int MfSoundKeyframe::Load()
{
    if (theSoundManager) {
        MfISoundDriver *driver = theSoundManager->GetDriver();
        if (driver && !(mName == NULL_STRING)) {
            if (mSound)
                return 1;
            mSound = driver->CreateResource();
            int error = mSound->Load(theSoundManager->FindSoundPath(mName));
            if (!error)
                mSound->SetChannelVolume(0, mMute ? -100.0f : mVolume);
            else {
                mSound->Release();
                mSound = 0;
            }
            return error == 0;
        }
    }
    return 0;
}

// 0x10053740 ?CreateSoundKeyframe@@YAPAVMfStreamObject@@XZ (original (unnamed): creates the objects of the stream type)
static MfStreamObject *CreateSoundKeyframe()
{
    return new MfSoundKeyframe;
}

// 0x100537a0 _$E8 (original $E: the static initializer of the constructor entry, with 100537b0)
// 0x100537b0 _$E7
static MfStreamConstructorEntry constructorEntry(MfSoundKeyframe::STREAM_TYPE, CreateSoundKeyframe);

// 0x100537d0 ?StreamType@MfSoundKeyframe@@UBEJXZ
long MfSoundKeyframe::StreamType() const
{
    return STREAM_TYPE;
}

// 0x100537e0 ?ReadFromStream@MfSoundKeyframe@@UAE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfSoundKeyframe::ReadFromStream(MfStream &stream)
{
    MfKeyframe::ReadFromStream(stream);
    int unused1, unused2;
    stream >> mVolume >> mMute >> mAmbient >> unused1 >> unused2;
    int value;
    if (stream.userFormat > 26) {
        stream >> value;
        mStream = value == 0;
    }
    if (stream.userFormat > 27)
        stream >> value;
    stream >> mName;
    return stream.Status();
}

// 0x10053870 ?WriteToStream@MfSoundKeyframe@@UBE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfSoundKeyframe::WriteToStream(MfStream &stream) const
{
    MfKeyframe::WriteToStream(stream);
    stream << mVolume << mMute << mAmbient << -1 << 0;
    stream << (int)(mStream == 0) << 0 << mName;
    return stream.Status();
}
