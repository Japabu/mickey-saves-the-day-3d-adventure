#pragma once
// SBdBlockOrdered.h: a dialogue block that plays its line groups in order, then (optionally) at
// random from a range (declaration from SteamboatApp.pdb).
#include "SBdBlock.h"

class SBdBlockOrdered : public SBdBlock {
public:
    SBdBlockOrdered(const SBdBlockOrdered &);
    SBdBlockOrdered(MBString &blockName);
    virtual ~SBdBlockOrdered();
    virtual void LoadFromPref(MBComPtr<IMBStream> stream, MBComPtr<IMBPref> pref); // vftable+0x14
    virtual SBdLineGroup *GetLineGroup(MfAiActor *mfPrimaryActor); // vftable+0x1c
private:
    int mFirstLineGroup;                                // +0x00ac size 0x4
    int mLastLineGroup;                                 // +0x00b0 size 0x4
    int mRandomLow;                                     // +0x00b4 size 0x4
    int mRandomHigh;                                    // +0x00b8 size 0x4
    int mSubsequentLineGroups;                          // +0x00bc size 0x4
    int mCurrentLineGroup;                              // +0x00c0 size 0x4
    int mPreviousLineGroup;                             // +0x00c4 size 0x4
    bool mInOrder;                                      // +0x00c8 size 0x1
    bool mRandomly;                                     // +0x00c9 size 0x1
};
