#pragma once
// SBDialogue: the dialogue blocks of a scene (declaration from SteamboatApp.pdb).
#include "MBInterfaces.h"

class MfAiActor;
class SBdBlock;
class SBdLineGroup;

class SBDialogue {
public:
    SBDialogue(const SBDialogue &);
    SBDialogue();
    virtual ~SBDialogue();                              // deleting destructor at vftable+0x00
    void LoadFromPref(MBComPtr<IMBStream> stream, MBComPtr<IMBPref> pref);
    bool Update(MfAiActor *mfPrimaryActor);
private:
    MBVector<SBdBlock *> mDBlockVector;                 // +0x0004 size 0x10
    SBdLineGroup *mActiveLineGroup;                     // +0x0014 size 0x4
};

