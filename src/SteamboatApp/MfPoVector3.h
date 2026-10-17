#pragma once
// MfPoVector3.h: a Piccolo vector object (PiccoloRT.dll), as the game declares it (SteamboatApp.pdb).
#include "MfScDataObject.h"
#include "MfVector3.h"

// own copy: the game calls MfPoLibrary through its import thunks (no dllimport) and emits its own copies of
// inline members; the owner's header declares the class MFPO_API, dllimport here,
// and it opens MfPoLibrary's MfScData/MfString/MfVector3 copies.
class MfPoVector3 : public MfPoBase {
public:
    MfPoVector3(const MfVector3 &v);
    MfPoVector3(MfVector3 *v);
    MfPoVector3();
    virtual ~MfPoVector3();
    MfVector3 &Vector();
private:
    MfVector3 *_v3;                                     // +0x0014
    int bOwner;                                         // +0x0018
};
