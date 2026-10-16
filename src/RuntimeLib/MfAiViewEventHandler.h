// MfAiViewEventHandler: routes view (mouse) events into a space.
// Declaration from the SteamboatApp PDB.
#ifndef RUNTIMELIB_MFAIVIEWEVENTHANDLER_H
#define RUNTIMELIB_MFAIVIEWEVENTHANDLER_H

#include "MfAi.h"
class MfAiSpace;

class MfAiViewEventHandler : public MfAi {
public:
    virtual void SetSpace(MfAiSpace *) = 0;
    virtual MfAiSpace *GetSpace() = 0;
};

#endif
