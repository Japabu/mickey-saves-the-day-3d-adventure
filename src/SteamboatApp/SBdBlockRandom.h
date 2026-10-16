#pragma once
// SBdBlockRandom.h: a dialogue block that plays a random line group, never the same twice in a row (declaration from SteamboatApp.pdb).
#include "SBdBlock.h"

class SBdBlockRandom : public SBdBlock {
public:
    SBdBlockRandom(const SBdBlockRandom &);
    SBdBlockRandom(MBString &blockName);
    virtual ~SBdBlockRandom();
    virtual void LoadFromPref(MBComPtr<IMBStream> stream, MBComPtr<IMBPref> pref); // vftable+0x14
    virtual SBdLineGroup *GetLineGroup(MfAiActor *mfPrimaryActor); // vftable+0x1c
private:
    int mCurrentLineGroup;                              // +0x00ac size 0x4
};
