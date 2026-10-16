// MfAiController: an application object that steers a live actor instead of
// its behaviour (MfLiveActor::SetController). Not declared in the SteamboatApp
// PDB; slots from the calls in MfLiveActor (GoAuthor stops it, ContinueLive
// resumes it, SetControlMode activates or deactivates it); it receives the
// application events of the actor (MfLiveActor::UpdateAppEventHandler).
#ifndef RUNTIMELIB_MFAICONTROLLER_H
#define RUNTIMELIB_MFAICONTROLLER_H
#include "MfAi.h"

class MfAiController : public MfAi {
public:
    virtual void HandleEvent(int *event) = 0;
    virtual void Activate() = 0;
    virtual void Deactivate() = 0;
    virtual void Resume() = 0;
};

#endif
