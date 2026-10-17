// MBMilesSoundEngine.h - plays a sound held in memory with Miles.
#ifndef MBENGINE_MBMILESSOUNDENGINE_H
#define MBENGINE_MBMILESSOUNDENGINE_H

#include "MBSoundEngine.h"

struct AUDIO_TYPE;
class IMBSoundDescription;
class MBSoundData;

class MBMilesSoundEngine : public MBSoundEngine {
public:
    MBMilesSoundEngine(void);
    virtual ~MBMilesSoundEngine(void);
    virtual bool LoadSound(IMBSoundDescription *soundDescription);
    virtual void Play(void);
    virtual void Stop(void);
    virtual void Pause(void);
    virtual void Resume(void);
    virtual bool IsSoundPlaying(long soundID) const;
protected:
    virtual bool IsSoundFinished(void) const;
    virtual void IdleSound(void);
    virtual void VolumeChanged(void);
    virtual void RateMultiplierChanged(void);
    virtual void DisposeSound(void);
    MBSoundData *mSoundData;
    AUDIO_TYPE *mMilesAudio;
    long mResumeLoopCount;
    long mResumeTimePosition;
};

#endif
