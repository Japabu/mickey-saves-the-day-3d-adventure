// MBMilesStreamingSoundEngine.h - plays a streamed sound with Miles.
#ifndef MBENGINE_MBMILESSTREAMINGSOUNDENGINE_H
#define MBENGINE_MBMILESSTREAMINGSOUNDENGINE_H

#include "MBSoundEngine.h"

class IMBSoundDescription;
class MBFileStream;
struct _STREAM;

class MBMilesStreamingSoundEngine : public MBSoundEngine {
public:
    MBMilesStreamingSoundEngine(void);
    virtual ~MBMilesStreamingSoundEngine(void);
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
    _STREAM *mMilesStream;
    MBFileStream *mFileStream;
    long mResumeLoopCount;
    unsigned long mResumeTimePosition;
};

#endif
