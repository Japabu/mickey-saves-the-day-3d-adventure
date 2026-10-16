#pragma once
// SBdBlockFirstSubsequentRandom.h: a dialogue block that plays its first line group the first time and a random other one after that (declaration from SteamboatApp.pdb).
#include "SBdBlock.h"

class SBdBlockFirstSubsequentRandom : public SBdBlock {
public:
    SBdBlockFirstSubsequentRandom(const SBdBlockFirstSubsequentRandom &);
    SBdBlockFirstSubsequentRandom(MBString &blockName);
    virtual ~SBdBlockFirstSubsequentRandom();
    virtual void LoadFromPref(MBComPtr<IMBStream> stream, MBComPtr<IMBPref> pref); // vftable+0x14
    virtual SBdLineGroup *GetLineGroup(MfAiActor *mfPrimaryActor); // vftable+0x1c
private:
    int mCurrentLineGroup;                              // +0x00ac size 0x4
};
