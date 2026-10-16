// MBMilesStreamingSoundEngine.cpp - MBEngine.dll MBMilesStreamingSoundEngine.obj
#include "MBMilesStreamingSoundEngine.h"

// 0x100439b0 ??0MBMilesStreamingSoundEngine@@QAE@XZ
MBMilesStreamingSoundEngine::MBMilesStreamingSoundEngine(void)
{
    mMilesStream = 0;
    mFileStream = 0;
    mResumeLoopCount = 0;
    mResumeTimePosition = 0;
}

// 0x10043a00 ??1MBMilesStreamingSoundEngine@@UAE@XZ
MBMilesStreamingSoundEngine::~MBMilesStreamingSoundEngine(void)
{
    MBMilesStreamingSoundEngine::DisposeSound();
}

// 0x10043a20 ?LoadSound@MBMilesStreamingSoundEngine@@UAE_NPAVIMBSoundDescription@@@Z
bool MBMilesStreamingSoundEngine::LoadSound(IMBSoundDescription *soundDescription)
{
    DisposeSound();
    mSoundDescription = soundDescription;
    soundDescription->AddRef();
    return true;
}

// EQUIV 0x10043a40 ?Play@MBMilesStreamingSoundEngine@@UAEXXZ: reviewed: a member argument is loaded before instead of after computing the other one
void MBMilesStreamingSoundEngine::Play(void)
{
    long loopCount = mSoundDescription->GetLoopCount();
    AIL_set_stream_loop_count(mMilesStream, loopCount == -1 ? 0 : loopCount);
    AIL_start_stream(mMilesStream);
    mIsFinished = false;
}

// 0x10043a70 ?Stop@MBMilesStreamingSoundEngine@@UAEXXZ
void MBMilesStreamingSoundEngine::Stop(void)
{
    AIL_pause_stream(mMilesStream, 1);
}

// 0x10043a80 ?Pause@MBMilesStreamingSoundEngine@@UAEXXZ
void MBMilesStreamingSoundEngine::Pause(void)
{
    if (!mIsPaused)
        mIsPaused = true;
}

// 0x10043a90 ?Resume@MBMilesStreamingSoundEngine@@UAEXXZ
void MBMilesStreamingSoundEngine::Resume(void)
{
}

// 0x10043aa0 ?IsSoundFinished@MBMilesStreamingSoundEngine@@MBE_NXZ
bool MBMilesStreamingSoundEngine::IsSoundFinished(void) const
{
    return AIL_stream_status(mMilesStream) == 2;
}

// 0x10043ac0 ?IsSoundPlaying@MBMilesStreamingSoundEngine@@UBE_NJ@Z
bool MBMilesStreamingSoundEngine::IsSoundPlaying(long soundID) const
{
    bool isPlaying = false;
    if (mSoundDescription->GetSoundID() == soundID)
        isPlaying = true;
    return isPlaying;
}

// 0x10043ae0 ?IdleSound@MBMilesStreamingSoundEngine@@MAEXXZ
void MBMilesStreamingSoundEngine::IdleSound(void)
{
}

// 0x10043af0 ?VolumeChanged@MBMilesStreamingSoundEngine@@MAEXXZ
void MBMilesStreamingSoundEngine::VolumeChanged(void)
{
    AIL_set_stream_volume(mMilesStream, mVolume);
}

// 0x10043b00 ?RateMultiplierChanged@MBMilesStreamingSoundEngine@@MAEXXZ
void MBMilesStreamingSoundEngine::RateMultiplierChanged(void)
{
}

// 0x10043b10 ?DisposeSound@MBMilesStreamingSoundEngine@@MAEXXZ
void MBMilesStreamingSoundEngine::DisposeSound(void)
{
    if (mMilesStream) {
        AIL_close_stream(mMilesStream);
        mMilesStream = 0;
    }
    MBSoundEngine::DisposeSound();
}


// Matched by this code:
// 0x100439d0 ??_GMBMilesStreamingSoundEngine@@UAEPAXI@Z
