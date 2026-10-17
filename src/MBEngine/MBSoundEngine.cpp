// MBSoundEngine.cpp - MBEngine.dll MBSoundEngine.obj
#include "MBSoundEngine_unit.h"

// 0x10047820 ??0MBSoundEngine@@QAE@XZ
MBSoundEngine::MBSoundEngine(void)
    : mSoundDescription(0), mRateMultiplier(1.0f), mVolume(127), mIsPaused(false), mIsFinished(true)
{
}

// 0x10047850 ??_GMBSoundEngine@@UAEPAXI@Z (compiler-generated)

// 0x10047880 ??1MBSoundEngine@@UAE@XZ
MBSoundEngine::~MBSoundEngine(void)
{
    if (mSoundDescription) {
        SendSoundEndedMessage(false);
        DisposeSound();
    }
}

// 0x100478a0 ?HasSound@MBSoundEngine@@QBE_NXZ
bool MBSoundEngine::HasSound(void) const
{
    return mSoundDescription != 0;
}

// 0x100478b0 ?IsPaused@MBSoundEngine@@QBE_NXZ
bool MBSoundEngine::IsPaused(void) const
{
    return mIsPaused;
}

// 0x100478c0 ?Idle@MBSoundEngine@@QAEXXZ
void MBSoundEngine::Idle(void)
{
    if (HasSound()) {
        IdleSound();
        if (IsSoundFinished()) {
            SendSoundEndedMessage(true);
            DisposeSound();
            mIsFinished = true;
        }
    }
}

// 0x10047900 ?GetVolume@MBSoundEngine@@QBEFXZ
short MBSoundEngine::GetVolume(void) const
{
    return mVolume;
}

// 0x10047910 ?SetVolume@MBSoundEngine@@QAEXF@Z
void MBSoundEngine::SetVolume(short volume)
{
    mVolume = volume;
    VolumeChanged();
}

// 0x10047930 ?GetRateMultiplier@MBSoundEngine@@QBEMXZ
float MBSoundEngine::GetRateMultiplier(void) const
{
    return mRateMultiplier;
}

// 0x10047940 ?SetRateMultiplier@MBSoundEngine@@QAEXM@Z
void MBSoundEngine::SetRateMultiplier(float rateMultiplier)
{
    mRateMultiplier = rateMultiplier;
    RateMultiplierChanged();
}

// 0x10047950 ?IsFinished@MBSoundEngine@@QBE_NXZ
bool MBSoundEngine::IsFinished(void) const
{
    return mIsFinished;
}

// 0x10047960 ?GetSoundID@MBSoundEngine@@QBEJXZ
long MBSoundEngine::GetSoundID(void) const
{
    if (HasSound())
        return mSoundDescription->GetSoundID();
    return 0;
}

// 0x10047980 ?DisposeSound@MBSoundEngine@@MAEXXZ
void MBSoundEngine::DisposeSound(void)
{
    if (mSoundDescription) {
        mSoundDescription->Release();
        mSoundDescription = 0;
    }
}

// 0x100479a0 ?SendSoundStartedMessage@MBSoundEngine@@IAEX_N@Z
void MBSoundEngine::SendSoundStartedMessage(bool completed)
{
    SoundEndedEvent event;
    event.mSoundDescription = mSoundDescription;
    event.mCompleted = completed;
    MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
    broadcaster->BroadcastEvent(-4, 0, &event);
}

// 0x10047a40 ?SendSoundEndedMessage@MBSoundEngine@@IAEX_N@Z
void MBSoundEngine::SendSoundEndedMessage(bool completed)
{
    SoundEndedEvent event;
    event.mSoundDescription = mSoundDescription;
    event.mCompleted = completed;
    MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
    broadcaster->BroadcastEvent(-4, 1, &event);
}
