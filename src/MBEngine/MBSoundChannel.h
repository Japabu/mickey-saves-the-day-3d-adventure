// MBSoundChannel.h - a sound channel: a queue of sounds played by a sound engine.
// Declared first by the includer (still one copy per unit in wave 1): MBList.
#ifndef MBENGINE_MBSOUNDCHANNEL_H
#define MBENGINE_MBSOUNDCHANNEL_H

class IMBSoundDescription;
class MBSoundEngine;

class MBSoundChannel {
public:
    MBSoundChannel(long channelID);
    virtual ~MBSoundChannel(void);
    void StopSound(long soundID);
    void StopSound(void);
    void ClearQueue(void);
    void AddSound(IMBSoundDescription *soundDescription);
    bool IsPaused(void) const;
    void Pause(void);
    void Resume(void);
    void PauseSound(long soundID);
    void ResumeSound(long soundID);
    virtual void Idle(void);
    long GetChannelID(void) const;
    bool IsFinished(void) const;
    bool IsSoundPlayingOrQueued(long soundID);
    bool IsSoundPlaying(long soundID);
    short GetVolume(void) const;
    void SetVolume(short volume);
    void MasterVolumeChanged(short masterVolume);
    float GetRateMultiplier(void) const;
    void SetRateMultiplier(float rateMultiplier);
protected:
    void VolumeChanged(void);
    void RateMultiplierChanged(void);
    short ScaleVolume(short masterVolume) const;
    virtual MBSoundEngine *CreateSoundEngine(IMBSoundDescription *) const = 0;
    MBList<IMBSoundDescription *> mSoundQueue;
    MBSoundEngine *mSoundEngine;
    long mChannelID;
    float mRateMultiplier;
    short mVolume;
    bool mIsPaused;
    bool mIsFinished;
};

#endif
