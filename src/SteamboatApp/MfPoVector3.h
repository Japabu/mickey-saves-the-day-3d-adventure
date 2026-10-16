#pragma once
// MfPoVector3.h: a Piccolo vector object (PiccoloRT.dll), as the game declares it (SteamboatApp.pdb).
#include "MfScDataObject.h"
#include "MfVector3.h"

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
