#pragma once
// The engine's sound helpers (MBEngine.dll, called through the import thunks).
class IMBFileSpec;

extern "C" {
void MBForceSound(const char *soundName, long channelID, long loopCount);
void MBForceSoundID(unsigned long id, long channelID, long loopCount);
void MBQueueSound(const char *soundName, long channelID, long loopCount);
}
