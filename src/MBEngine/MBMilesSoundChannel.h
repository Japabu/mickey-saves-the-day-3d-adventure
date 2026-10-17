// MBMilesSoundChannel.h - a sound channel playing through the Miles engines.
#ifndef MBENGINE_MBMILESSOUNDCHANNEL_H
#define MBENGINE_MBMILESSOUNDCHANNEL_H

#include "MBSoundChannel.h"

class IMBSoundDescription;
class MBSoundEngine;

class MBMilesSoundChannel : public MBSoundChannel {
public:
    MBMilesSoundChannel(long channelID);
    virtual ~MBMilesSoundChannel(void);
protected:
    virtual MBSoundEngine *CreateSoundEngine(IMBSoundDescription *soundDescription) const;
};

#endif
