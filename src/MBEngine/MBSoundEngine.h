// MBSoundEngine.h - plays one sound on a channel (base of the Miles engines).
#ifndef MBENGINE_MBSOUNDENGINE_H
#define MBENGINE_MBSOUNDENGINE_H

class IMBSoundDescription;

class MBSoundEngine {
public:
    MBSoundEngine(void);
    virtual ~MBSoundEngine(void);
    virtual bool LoadSound(IMBSoundDescription *) = 0;
    bool HasSound(void) const;
    void Idle(void);
    virtual void Play(void) = 0;
    virtual void Stop(void) = 0;
    bool IsPaused(void) const;
    virtual void Pause(void) = 0;
    virtual void Resume(void) = 0;
    short GetVolume(void) const;
    void SetVolume(short volume);
    float GetRateMultiplier(void) const;
    void SetRateMultiplier(float rateMultiplier);
    bool IsFinished(void) const;
    long GetSoundID(void) const;
    virtual bool IsSoundPlaying(long) const = 0;
protected:
    virtual bool IsSoundFinished(void) const = 0;
    virtual void IdleSound(void) = 0;
    virtual void VolumeChanged(void) = 0;
    virtual void RateMultiplierChanged(void) = 0;
    virtual void DisposeSound(void);
    void SendSoundStartedMessage(bool completed);
    void SendSoundEndedMessage(bool completed);
    IMBSoundDescription *mSoundDescription;
    float mRateMultiplier;
    short mVolume;
    bool mIsPaused;
    bool mIsFinished;
};

#endif
