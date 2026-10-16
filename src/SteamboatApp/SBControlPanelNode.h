#pragma once
// SBControlPanelNode.h: the control panel scene, a code lock of ten buttons
// (SBControlPanelNode.obj, declaration from SteamboatApp.pdb).
#include "SBActivityNode.h"

struct MBKeyboardEvent;

class SBControlPanelNode : public SBActivityNode {
public:
    SBControlPanelNode(IMBGameEngine *engine, MBString spaceCode);
    virtual ~SBControlPanelNode();
    virtual void OnStart();
    virtual void OnEnd();
    virtual void StartListening();
    virtual void StopListening();
    virtual void OnKeyDown(MBKeyboardEvent &key);
    virtual void ListenToEvent(long channel, long event, void *data);
    virtual void Idle();
protected:
    virtual void OnActivityEvent(long event, void *data);
    MBComPtr<IMBAnim> mSavedCursor;                     // +0x00d0
    MBComPtr<ISBAnimButton> mButtons[10];               // +0x00d4
    MBComPtr<ISBAnimButton> mPrintButtons[10];          // +0x00fc
    bool mActive;                                       // +0x0124
    bool mHasFlour;                                     // +0x0125
    long mIncorrectTries;                               // +0x0128
    unsigned char mCodes[45];                           // +0x012c
    long mCorrectCode;                                  // +0x015c
    long mCurrentDigit;                                 // +0x0160
    long mCurrentCode[2];                               // +0x0164
    int ConvertDigitsIntoCode(long *digits);
};
