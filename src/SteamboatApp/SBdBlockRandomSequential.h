#pragma once
// SBdBlockRandomSequential.h: a dialogue block that builds a new line group from one random line
// of each of its line groups (declaration from SteamboatApp.pdb).
#include "SBdBlock.h"

class SBdBlockRandomSequential : public SBdBlock {
public:
    SBdBlockRandomSequential(const SBdBlockRandomSequential &);
    SBdBlockRandomSequential(MBString &blockName);
    virtual ~SBdBlockRandomSequential();
    virtual void LoadFromPref(MBComPtr<IMBStream> stream, MBComPtr<IMBPref> pref); // vftable+0x14
    virtual SBdLineGroup *GetLineGroup(MfAiActor *mfPrimaryActor); // vftable+0x1c
private:
    MBVector<SBdLineGroup *> mGeneratedLineGroups;      // +0x00ac size 0x10
};
