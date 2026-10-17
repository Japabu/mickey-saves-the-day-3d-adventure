// dialogue.cpp - improvement over the original: Space or Enter skips the line being spoken
// (platform/win32/message.cpp calls port_skip_dialogue on the key). The voices play on sound
// channel 20; stopping it ends the line as if it had played out (MBSoundEngine reports the
// end, Actor passes it on as event 702), so the dialogue goes on with the next line.
#include "SteamboatNode.h"
#include "MBRuntime.h"

extern "C" void port_skip_dialogue(void)
{
    MBComPtr<IMBSoundManager> soundManager(MBIID_IMBSoundManager);
    soundManager->StopChannel(20);
}
