// MfContourFailDownData: what an actor reports about its last contour failure
// (struct from the SteamboatApp PDB).
#ifndef MFPOLIBRARY_MFCONTOURFAILDOWNDATA_H
#define MFPOLIBRARY_MFCONTOURFAILDOWNDATA_H
class MfAiActorNode;

struct MfContourFailDownData {
    MfAiActorNode *failNode;                                // 0x00
};

#endif
