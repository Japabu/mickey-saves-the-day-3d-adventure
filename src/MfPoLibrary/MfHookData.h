// MfHookData: the base of the data passed to the application's hooks (struct
// from the SteamboatApp PDB; MfActorLib's class, declared here until that
// module's canonical header replaces it). MfPoLibrary sends user hooks
// (MfUserHookData.h, Actor.DisplayText) and miscellaneous-data hooks
// (MfMiscHookData.h, ActorNode.colType).
#ifndef MFPOLIBRARY_MFHOOKDATA_H
#define MFPOLIBRARY_MFHOOKDATA_H

struct MfHookData {
    int reserved[4];                                        // 0x00
};

#endif
