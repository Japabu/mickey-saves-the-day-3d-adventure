// MBSoundChannel.cpp - MBEngine.dll MBSoundChannel.obj
#include "MBSoundChannel_unit.h"

// 0x100471a0 ??0MBSoundChannel@@QAE@J@Z
MBSoundChannel::MBSoundChannel(long channelID)
    : mSoundEngine(0), mChannelID(channelID), mRateMultiplier(1.0f), mVolume(127),
      mIsPaused(false), mIsFinished(false)
{
}

// 0x100471f0 ??_GMBSoundChannel@@UAEPAXI@Z (compiler-generated)

// 0x10047220 ??1MBSoundChannel@@UAE@XZ
MBSoundChannel::~MBSoundChannel(void)
{
    delete mSoundEngine;
}

// 0x100472a0 ?StopSound@MBSoundChannel@@QAEXXZ
void MBSoundChannel::StopSound(void)
{
    if (mSoundEngine)
        mSoundEngine->Stop();
}

// 0x100472b0 ?ClearQueue@MBSoundChannel@@QAEXXZ
void MBSoundChannel::ClearQueue(void)
{
    MBList<IMBSoundDescription *>::iterator it = mSoundQueue.begin();
    while (it != mSoundQueue.end()) {
        (*it)->Release();
        it = mSoundQueue.erase(it);
    }
}

// 0x10047300 ?AddSound@MBSoundChannel@@QAEXPAVIMBSoundDescription@@@Z
void MBSoundChannel::AddSound(IMBSoundDescription *soundDescription)
{
    soundDescription->AddRef();
    mSoundQueue.push_back(soundDescription);
}

// 0x10047360 ?IsPaused@MBSoundChannel@@QBE_NXZ
bool MBSoundChannel::IsPaused(void) const
{
    return mIsPaused;
}

// 0x10047370 ?Pause@MBSoundChannel@@QAEXXZ
void MBSoundChannel::Pause(void)
{
    mIsPaused = true;
    if (mSoundEngine)
        mSoundEngine->Pause();
}

// 0x10047390 ?Resume@MBSoundChannel@@QAEXXZ
void MBSoundChannel::Resume(void)
{
    mIsPaused = false;
    if (mSoundEngine)
        mSoundEngine->Resume();
}

// 0x100473b0 ?PauseSound@MBSoundChannel@@QAEXJ@Z
void MBSoundChannel::PauseSound(long soundID)
{
}

// 0x100473c0 ?ResumeSound@MBSoundChannel@@QAEXJ@Z
void MBSoundChannel::ResumeSound(long soundID)
{
}

// 0x100473d0 ?Idle@MBSoundChannel@@UAEXXZ
void MBSoundChannel::Idle(void)
{
    if (mIsPaused)
        return;
    if (mSoundEngine) {
        mSoundEngine->Idle();
        if (!mSoundEngine->IsFinished())
            return;
    }
    delete mSoundEngine;
    mSoundEngine = 0;
    if (mSoundQueue.size()) {
        IMBSoundDescription *soundDescription = mSoundQueue.front();
        mSoundQueue.pop_front();
        mSoundEngine = CreateSoundEngine(soundDescription);
        mSoundEngine->LoadSound(soundDescription);
        soundDescription->Release();
        MBComPtr<IMBSoundManager> soundManager(MBIID_IMBSoundManager);
        if (soundManager)
            mSoundEngine->SetVolume(ScaleVolume(soundManager->GetMasterVolume()));
        mSoundEngine->Play();
    } else {
        mIsFinished = true;
    }
}

// 0x10047500 ?GetChannelID@MBSoundChannel@@QBEJXZ
long MBSoundChannel::GetChannelID(void) const
{
    return mChannelID;
}

// 0x10047510 ?IsFinished@MBSoundChannel@@QBE_NXZ
bool MBSoundChannel::IsFinished(void) const
{
    return mIsFinished;
}

// 0x10047520 ?StopSound@MBSoundChannel@@QAEXJ@Z
void MBSoundChannel::StopSound(long soundID)
{
    if (mSoundEngine && mSoundEngine->IsSoundPlaying(soundID))
        mSoundEngine->Stop();
    MBList<IMBSoundDescription *>::iterator it = mSoundQueue.begin();
    while (it != mSoundQueue.end()) {
        if ((*it)->GetSoundID() == soundID) {
            (*it)->Release();
            it = mSoundQueue.erase(it);
        } else {
            ++it;
        }
    }
}

// 0x100475a0 ?IsSoundPlayingOrQueued@MBSoundChannel@@QAE_NJ@Z
bool MBSoundChannel::IsSoundPlayingOrQueued(long soundID)
{
    MBList<IMBSoundDescription *>::iterator it;
    for (it = mSoundQueue.begin(); it != mSoundQueue.end(); ++it) {
        if ((*it)->GetSoundID() == soundID)
            return true;
    }
    return IsSoundPlaying(soundID);
}

// 0x100475e0 ?IsSoundPlaying@MBSoundChannel@@QAE_NJ@Z
bool MBSoundChannel::IsSoundPlaying(long soundID)
{
    if (mSoundEngine)
        return mSoundEngine->IsSoundPlaying(soundID);
    return false;
}

// 0x10047600 ?GetVolume@MBSoundChannel@@QBEFXZ
short MBSoundChannel::GetVolume(void) const
{
    return mVolume;
}

// 0x10047610 ?SetVolume@MBSoundChannel@@QAEXF@Z
void MBSoundChannel::SetVolume(short volume)
{
    mVolume = volume;
    VolumeChanged();
}

// 0x10047630 ?GetRateMultiplier@MBSoundChannel@@QBEMXZ
float MBSoundChannel::GetRateMultiplier(void) const
{
    return mRateMultiplier;
}

// 0x10047640 ?SetRateMultiplier@MBSoundChannel@@QAEXM@Z
void MBSoundChannel::SetRateMultiplier(float rateMultiplier)
{
    mRateMultiplier = rateMultiplier;
}

// 0x10047650 ?MasterVolumeChanged@MBSoundChannel@@QAEXF@Z
void MBSoundChannel::MasterVolumeChanged(short masterVolume)
{
    if (mSoundEngine)
        mSoundEngine->SetVolume(ScaleVolume(masterVolume));
}

// 0x10047680 ?VolumeChanged@MBSoundChannel@@IAEXXZ
void MBSoundChannel::VolumeChanged(void)
{
    if (mSoundEngine) {
        MBComPtr<IMBSoundManager> soundManager(MBIID_IMBSoundManager);
        if (soundManager)
            mSoundEngine->SetVolume(ScaleVolume(soundManager->GetMasterVolume()));
    }
}

// 0x10047700 ?RateMultiplierChanged@MBSoundChannel@@IAEXXZ
void MBSoundChannel::RateMultiplierChanged(void)
{
    if (mSoundEngine)
        mSoundEngine->SetRateMultiplier(mRateMultiplier);
}

// 0x10047720 ?ScaleVolume@MBSoundChannel@@IBEFF@Z
short MBSoundChannel::ScaleVolume(short masterVolume) const
{
    return (short)(mVolume * masterVolume / 127);
}


// Matched by this code:
// 0x10047750 ??Eiterator@?$list@PAVIMBSoundDescription@@V?$MBAllocator@PAVIMBSoundDescription@@@@@std@@QAE?AV012@H@Z
// 0x10047760 _$E19
// 0x10047780 _$E18
// 0x1005dcf0 $E19
// 0x1005dd10 $E18
// 0x10062730 $E19
// 0x10062750 $E18
// 0x10062760 $E19
// 0x10062780 $E18
// 0x10063ce0 $E19
// 0x10063d00 $E18
// 0x10064dd0 $E19
// 0x10064df0 $E18
// 0x10065ee0 $E19
// 0x10065f00 $E18
