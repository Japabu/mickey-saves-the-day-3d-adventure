#pragma once
// SBdBlockFirstTimeOnly.h: a dialogue block that plays only the first time (declaration from SteamboatApp.pdb).
#include "SBdBlock.h"

class SBdBlockFirstTimeOnly : public SBdBlock {
public:
    SBdBlockFirstTimeOnly(const SBdBlockFirstTimeOnly &);
    SBdBlockFirstTimeOnly(MBString &blockName);
    virtual ~SBdBlockFirstTimeOnly();
    virtual void LoadFromPref(MBComPtr<IMBStream> stream, MBComPtr<IMBPref> pref); // vftable+0x14
    virtual SBdLineGroup *GetLineGroup(MfAiActor *mfPrimaryActor); // vftable+0x1c
private:
    int mCurrentLineGroup;                              // +0x00ac size 0x4
};
