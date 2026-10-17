// MfProximityData: what an actor reports about its last proximity event
// (struct from the SteamboatApp PDB).
#ifndef MFPOLIBRARY_MFPROXIMITYDATA_H
#define MFPOLIBRARY_MFPROXIMITYDATA_H
class MfAiActor;

struct MfProximityData {
    MfAiActor *plaOther;                                    // 0x00
};

#endif
