// MfHookData: the common head of the data the runtime hands its hooks
// (declared in the SteamboatApp PDB). RuntimeLib's copy of the class whose
// canonical header is src/MfActorLib/MfHookData.h; kept here, out of
// MfHookManager.h, so that the RuntimeLib hook data headers can derive from it.
#ifndef RUNTIMELIB_MFHOOKDATA_H
#define RUNTIMELIB_MFHOOKDATA_H

struct MfHookData {
    int reserved[4];                                        // 0x00
};

#endif
