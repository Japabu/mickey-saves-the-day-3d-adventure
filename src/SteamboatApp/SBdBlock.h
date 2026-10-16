#pragma once
// SBdBlock.h: a dialogue block: a set of line groups, the clicks that trigger it and the
// condition under which it plays (declaration from SteamboatApp.pdb).
#include "MBInterfaces.h"

class MfAiActor;
class SBdLineGroup;

class SBdBlock : public IMBEventListener {
public:
    SBdBlock(const SBdBlock &);
    SBdBlock(MBString &blockName);
    virtual ~SBdBlock();                                // deleting destructor at vftable+0x10
protected:
    unsigned long _mRefCnt;                             // +0x0004 size 0x4
public:
    virtual long QueryInterface(const MBGUID &iid, void **obj); // vftable+0x00
    virtual unsigned long AddRef();                     // vftable+0x04
    virtual unsigned long Release();                    // vftable+0x08
    virtual void ListenToEvent(long channel, long event, void *data); // vftable+0x0c
    virtual void LoadFromPref(MBComPtr<IMBStream> stream, MBComPtr<IMBPref> pref); // vftable+0x14
    virtual bool EvaluateBlock();                       // vftable+0x18
    virtual SBdLineGroup *GetLineGroup(MfAiActor *mfPrimaryActor); // vftable+0x1c
    MBString &GetBlockName() { return mBlockName; }
    void ClearClick() { mClicked = false; }
protected:
    void ParseTrigger();
    void ParseConditional();
    bool EvaluateConditional();
    MBString mBlockName;                                // +0x0008 size 0x10
    MBString mConditionalString;                        // +0x0018 size 0x10
    MBVector<unsigned long> mConditionalVector;         // +0x0028 size 0x10
    MBString mSectionIDstring;                          // +0x0038 size 0x10
    MBString mTriggerString;                            // +0x0048 size 0x10
    MBVector<unsigned long> mTriggerVector;             // +0x0058 size 0x10
    MBString mBlockType;                                // +0x0068 size 0x10
    MBString mLastActorClicked;                         // +0x0078 size 0x10
    MBVector<SBdLineGroup *> mLineGroups;               // +0x0088 size 0x10
    int mCurrentLine;                                   // +0x0098 size 0x4
    unsigned long mSectionID;                           // +0x009c size 0x4
    int mSectionIDstartValue;                           // +0x00a0 size 0x4
    int mNull;                                          // +0x00a4 size 0x4
    bool mClicked;                                      // +0x00a8 size 0x1
    bool mActive;                                       // +0x00a9 size 0x1
    bool mSoundPlaying;                                 // +0x00aa size 0x1
    bool mCheckConditional;                             // +0x00ab size 0x1
};
