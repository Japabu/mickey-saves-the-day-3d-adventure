// dialogue.cpp - improvement over the original: a click, Space or Enter skips the line being
// spoken (platform/win32/message.cpp calls port_skip_dialogue on the key or the click). The voices play on sound
// channel 20; stopping the sound playing there ends the line as if it had played out
// (MBSoundEngine reports the end, Actor passes it on as event 702), and the channel goes on
// with the lines queued after it. StopChannel would also drop those, and the activities wait
// for the end of a queued line (Beulah's after a lost sandwich round) to go on.
#include "CMBMilesSoundManager.h"

// FindSoundChannel is protected: reached through a pointer to member of a derived class
struct PortSoundManagerAccess : CMBBaseSoundManager {
    static MBSoundChannel *Find(CMBBaseSoundManager *m, long channelID)
    {
        return (m->*&PortSoundManagerAccess::FindSoundChannel)(channelID);
    }
};

extern "C" void port_skip_dialogue(void)
{
    MBComPtr<IMBSoundManager> soundManager(MBIID_IMBSoundManager);
    if (!soundManager)
        return;
    // the only sound manager is CMBMilesSoundManager, whose first base this interface is
    if (MBSoundChannel *channel = PortSoundManagerAccess::Find(static_cast<CMBBaseSoundManager *>((IMBSoundManager *)soundManager), 20))
        channel->StopSound();
}
