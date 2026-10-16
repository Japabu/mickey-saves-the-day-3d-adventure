#pragma once
// SBdLineGroup.h: one group of dialogue lines of a dialogue block, with the properties it sets
// and the condition under which it plays (declaration from SteamboatApp.pdb).
#include "MBInterfaces.h"

class MfAiActor;

class SBdLineGroup : public IMBEventListener {
public:
    SBdLineGroup(const SBdLineGroup &);
    SBdLineGroup(MBString &lineGroupName);
    virtual ~SBdLineGroup();                            // deleting destructor at vftable+0x10
protected:
    unsigned long _mRefCnt;                             // +0x0004 size 0x4
public:
    virtual long QueryInterface(const MBGUID &iid, void **obj); // vftable+0x00
    virtual unsigned long AddRef();                     // vftable+0x04
    virtual unsigned long Release();                    // vftable+0x08
    virtual void ListenToEvent(long channel, long event, void *data); // vftable+0x0c
    void LoadFromPref(MBComPtr<IMBStream> stream, MBComPtr<IMBPref> pref);
    bool PlayDialogue(MfAiActor *mfPrimaryActor);
    void MakeActive() { mActive = true; }
    bool IsActive() { return mActive; }
    void AddLine(MBString newLine) { mDialogueLines.push_back(newLine); }
    void GetLineVector(MBVector<MBString> &lines) { lines = mDialogueLines; }
    void GetExitEvent(MBString &str) { str = mExitEvent; }
    void SetExitEvent(MBString str) { mExitEvent = str; }
    void GetExitEventData(MBString &str) { str = mExitEventData; }
    void SetExitEventData(MBString str) { mExitEventData = str; }
    void GetPropertyVectors(MBVector<unsigned long> &trueVector, MBVector<unsigned long> &falseVector)
    {
        trueVector = mTrueVector;
        falseVector = mFalseVector;
    }
    void SetPropertyVectors(MBVector<unsigned long> trueVector, MBVector<unsigned long> falseVector)
    {
        mTrueVector = trueVector;
        mFalseVector = falseVector;
    }
    MBString &GetLineGroupName() { return mLineGroupName; }
    bool EvaluateConditional();
protected:
    void ParseString(MBString &propString, MBVector<unsigned long> &propVec);
    void SetProperties(MBVector<unsigned long> &propVec, int val, MfAiActor *mfPrimaryActor);
    void ParseConditional();
private:
    MBString mLineGroupName;                            // +0x0008 size 0x10
    int mCurrentLine;                                   // +0x0018 size 0x4
    MBVector<MBString> mDialogueLines;                  // +0x001c size 0x10
    bool mActive;                                       // +0x002c size 0x1
    bool mSoundPlaying;                                 // +0x002d size 0x1
    MBString mExitEvent;                                // +0x0030 size 0x10
    MBString mExitEventData;                            // +0x0040 size 0x10
    MBString mTrueString;                               // +0x0050 size 0x10
    MBString mFalseString;                              // +0x0060 size 0x10
    MBVector<unsigned long> mTrueVector;                // +0x0070 size 0x10
    MBVector<unsigned long> mFalseVector;               // +0x0080 size 0x10
    bool mCheckConditional;                             // +0x0090 size 0x1
    MBString mConditionalString;                        // +0x0094 size 0x10
    MBVector<unsigned long> mConditionalVector;         // +0x00a4 size 0x10
};
