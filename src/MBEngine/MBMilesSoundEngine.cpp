// MBMilesSoundEngine.cpp - MBEngine.dll MBMilesSoundEngine.obj
#include "MBMilesSoundEngine_unit.h"

template <class T>
inline T MBAbs(T value)
{
    if (value < 0)
        value = -value;
    return value;
}

// 0x100435b0 ??0MBMilesSoundEngine@@QAE@XZ
MBMilesSoundEngine::MBMilesSoundEngine(void)
{
    mSoundData = 0;
    mMilesAudio = 0;
    mResumeLoopCount = 0;
    mResumeTimePosition = 0;
}

// 0x100435d0 ??_GMBMilesSoundEngine@@UAEPAXI@Z (compiler-generated)

// 0x10043600 ??1MBMilesSoundEngine@@UAE@XZ
MBMilesSoundEngine::~MBMilesSoundEngine(void)
{
    MBMilesSoundEngine::DisposeSound();
}

// TODO 0x10043620 ?LoadSound@MBMilesSoundEngine@@UAE_NPAVIMBSoundDescription@@@Z: LoadSound: orig keeps this in ebx (spilled), soundLoaded in memory, no tail-merge of soundLoaded=true; Release of MBComPtr out-of-line only when declared-only; 6 variants tried (56% similar)
bool MBMilesSoundEngine::LoadSound(IMBSoundDescription *soundDescription)
{
    bool soundLoaded = false;
    DisposeSound();
    MBComPtr<IMBSoundManager> soundManager(MBIID_IMBSoundManager);
    MBSoundData *soundData = soundManager->GetLockedSound(soundDescription->GetSoundID());
    if (soundData) {
        soundLoaded = true;
    } else {
        IMBStreamLocator *streamLocator;
        soundDescription->GetStreamLocator(&streamLocator);
        if (streamLocator) {
            IMBStream *tempStream;
            streamLocator->GetStream(&tempStream);
            streamLocator->Release();
            if (tempStream) {
                unsigned long fileSize = tempStream->GetStreamSize();
                unsigned char *data = (unsigned char *)MBmalloc_release(fileSize);
                tempStream->Read(data, fileSize);
                soundData = new MBSoundData(data, fileSize);
                soundLoaded = true;
                tempStream->Release();
            }
        }
    }
    if (soundData)
        mMilesAudio = AIL_quick_load_mem(soundData->GetSoundData(), soundData->GetDataSize());
    mSoundDescription = soundDescription;
    soundDescription->AddRef();
    mSoundData = soundData;
    return soundLoaded;
}

// EQUIV 0x10043780 ?Play@MBMilesSoundEngine@@UAEXXZ: reviewed: register choice and the load order of independent member reads only
void MBMilesSoundEngine::Play(void)
{
    long loopCount = mSoundDescription->GetLoopCount();
    if (AIL_quick_play(mMilesAudio, loopCount == -1 ? 0 : loopCount)) {
        float mult = mSoundDescription->GetRateMultiplier() * mRateMultiplier;
        if (MBAbs(mult - 1.0f) > 0.1f)
            AIL_quick_set_speed(mMilesAudio,
                (long)(AIL_sample_playback_rate((HSAMPLE)mMilesAudio->handle) * mult));
        SendSoundStartedMessage(true);
    }
    mIsFinished = false;
}

// 0x10043820 ?Stop@MBMilesSoundEngine@@UAEXXZ
void MBMilesSoundEngine::Stop(void)
{
    AIL_quick_halt(mMilesAudio);
}

// 0x10043830 ?Pause@MBMilesSoundEngine@@UAEXXZ
void MBMilesSoundEngine::Pause(void)
{
    if (!mIsPaused && mMilesAudio) {
        mResumeLoopCount = AIL_sample_loop_count((HSAMPLE)mMilesAudio->handle);
        mResumeTimePosition = AIL_quick_ms_position(mMilesAudio);
        AIL_quick_halt(mMilesAudio);
        mIsPaused = true;
    }
}

// 0x10043870 ?Resume@MBMilesSoundEngine@@UAEXXZ
void MBMilesSoundEngine::Resume(void)
{
    if (mIsPaused == true && mMilesAudio) {
        AIL_quick_set_ms_position(mMilesAudio, mResumeTimePosition);
        AIL_quick_play(mMilesAudio, mResumeLoopCount);
        mIsPaused = false;
    }
}

// 0x100438a0 ?IsSoundPlaying@MBMilesSoundEngine@@UBE_NJ@Z
bool MBMilesSoundEngine::IsSoundPlaying(long soundID) const
{
    bool isPlaying = false;
    if (mSoundDescription->GetSoundID() == soundID)
        isPlaying = true;
    return isPlaying;
}

// 0x100438c0 ?IsSoundFinished@MBMilesSoundEngine@@MBE_NXZ
bool MBMilesSoundEngine::IsSoundFinished(void) const
{
    return AIL_quick_status(mMilesAudio) == 1 && !mIsPaused;
}

// 0x100438f0 ?IdleSound@MBMilesSoundEngine@@MAEXXZ
void MBMilesSoundEngine::IdleSound(void)
{
}

// 0x10043900 ?VolumeChanged@MBMilesSoundEngine@@MAEXXZ
void MBMilesSoundEngine::VolumeChanged(void)
{
    AIL_quick_set_volume(mMilesAudio, mVolume, 63);
}

// 0x10043920 ?RateMultiplierChanged@MBMilesSoundEngine@@MAEXXZ
void MBMilesSoundEngine::RateMultiplierChanged(void)
{
}

// 0x10043930 ?DisposeSound@MBMilesSoundEngine@@MAEXXZ
void MBMilesSoundEngine::DisposeSound(void)
{
    if (mMilesAudio) {
        AIL_quick_unload(mMilesAudio);
        mMilesAudio = 0;
    }
    if (mSoundData) {
        delete mSoundData;
        mSoundData = 0;
    }
    MBSoundEngine::DisposeSound();
}


// Matched by this code:
// 0x10043980 _$E20
// 0x100439a0 _$E19
