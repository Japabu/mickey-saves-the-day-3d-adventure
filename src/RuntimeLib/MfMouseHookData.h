// MfMouseHookData: a mouse event of a render view, as the runtime hands it to
// the view hooks (declared in the SteamboatApp PDB).
#ifndef RUNTIMELIB_MFMOUSEHOOKDATA_H
#define RUNTIMELIB_MFMOUSEHOOKDATA_H
#include "MfHookData.h"
#include "MfVector3.h"
class MfAiSpace;
class MfRenderView;

struct MfMouseHookData : public MfHookData {
    MfRenderView *eventView;                                // 0x10
    float x;                                                // 0x14
    float y;                                                // 0x18
    unsigned int flags;                                     // 0x1c
    short wheelDelta;                                       // 0x20
    MfVector3 pos;                                          // 0x24
    MfVector3 dir;                                          // 0x30
    MfAiSpace *pSpace;                                      // 0x3c
};

#endif
