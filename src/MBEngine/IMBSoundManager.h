// IMBSoundManager.h - plays sounds on channels.
// Declared first by the includer (still one copy per unit in wave 1): IMBUnknown, MBGUID, MBIID_IMBSoundManager.
#ifndef MBENGINE_IMBSOUNDMANAGER_H
#define MBENGINE_IMBSOUNDMANAGER_H

class IMBSoundDescription;
class MBSoundData;

class IMBSoundManager : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBSoundManager; }
    virtual void StartSound(IMBSoundDescription *) = 0;
    virtual void QueueSound(IMBSoundDescription *) = 0;
    virtual void StopSound(long) = 0;
    virtual void StopChannel(long) = 0;
    virtual void StopAllSound(void) = 0;
    virtual bool IsPaused(void) = 0;
    virtual bool IsChannelPaused(long) = 0;
    virtual bool IsChannelPlaying(long) = 0;
    // VC6 lays out overloaded virtuals in reverse declaration order
    virtual bool IsSoundPlaying(long) = 0;       // vftable+0x30
    virtual bool IsSoundPlaying(long, long) = 0; // vftable+0x2c
    virtual bool IsSoundPlayingOrQueued(long) = 0;
    virtual bool IsAnySoundPlaying(void) = 0;
    virtual void Pause(void) = 0;
    virtual void PauseChannel(long) = 0;
    virtual void PauseSound(long) = 0;
    virtual void Resume(void) = 0;
    virtual void ResumeChannel(long) = 0;
    virtual void ResumeSound(long) = 0;
    virtual short GetMasterVolume(void) = 0;
    virtual void SetMasterVolume(short) = 0;
    virtual short GetChannelVolume(long) = 0;
    virtual void SetChannelVolume(long, short) = 0;
    virtual float GetChannelRateMultiplier(long) = 0;
    virtual void SetChannelRateMultiplier(long, float) = 0;
    virtual MBSoundData *GetLockedSound(long) = 0;
    virtual void LockSound(long) = 0;
    virtual void UnlockSound(long) = 0;
};

#endif
