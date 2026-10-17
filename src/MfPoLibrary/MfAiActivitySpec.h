// MfAiActivitySpec: the application's plugin behind an activity (RuntimeLib's
// MfPluginActivity; reference counted, streamable). MfPoLibrary only keeps
// references (MfPoActivityWrapper). Slots from MfPluginActivity's calls
// (RuntimeLib.dll 1004c230-1004c5c0); the names are invented.
#ifndef MFPOLIBRARY_MFAIACTIVITYSPEC_H
#define MFPOLIBRARY_MFAIACTIVITYSPEC_H
#include "MfAi.h"
#include "MfStreamObject.h"
class MfAiActivity;
class MfAiActor;

class MfAiActivitySpec : public MfAi, public MfStreamObject {
public:
    // binds the plugin to the activity; 0: refused
    virtual int Start(MfAiActor *actor, MfAiActivity *activity) = 0;
    virtual void Stop() = 0;
    // 1: finished
    virtual int Update(unsigned long time) = 0;
};

#endif
