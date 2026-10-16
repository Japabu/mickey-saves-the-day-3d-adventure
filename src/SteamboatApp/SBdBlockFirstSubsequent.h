#pragma once
// SBdBlockFirstSubsequent.h: a dialogue block that plays its first line group the first time and
// its second one after that (declaration from SteamboatApp.pdb).
#include "SBdBlock.h"

class SBdBlockFirstSubsequent : public SBdBlock {
public:
    SBdBlockFirstSubsequent(const SBdBlockFirstSubsequent &);
    SBdBlockFirstSubsequent(MBString &blockName);
    virtual ~SBdBlockFirstSubsequent();                 // deleting destructor at vftable+0x10
    virtual void LoadFromPref(MBComPtr<IMBStream> stream, MBComPtr<IMBPref> pref); // vftable+0x14
    virtual SBdLineGroup *GetLineGroup(MfAiActor *mfPrimaryActor); // vftable+0x1c
private:
    int mCurrentLineGroup;                              // +0x00ac size 0x4
};
