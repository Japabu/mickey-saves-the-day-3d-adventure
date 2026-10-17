// SoundEndedEvent.h - the data of a sound-ended event.
#ifndef MBENGINE_SOUNDENDEDEVENT_H
#define MBENGINE_SOUNDENDEDEVENT_H

class IMBSoundDescription;

struct SoundEndedEvent {
    IMBSoundDescription *mSoundDescription;
    bool mCompleted;
};

#endif
