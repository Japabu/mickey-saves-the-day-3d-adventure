#pragma once
// SBSoupMachineNode.h: the soup machine: nine ingredient buttons and a big red button
// (SBSoupMachineNode.obj, declaration from SteamboatApp.pdb).
#include "SBActivityNode.h"

struct MBMouseEvent;

class SBSoupMachineNode : public SBActivityNode {
public:
    SBSoupMachineNode(const SBSoupMachineNode &);
    SBSoupMachineNode(IMBGameEngine *engine, MBString spaceCode);
    virtual ~SBSoupMachineNode();
    virtual void OnStart();
    virtual void OnEnd();
    virtual void StartListening();
    virtual void StopListening();
    virtual void OnLMouseDown(MBMouseEvent &evt);
    virtual void OnKeyDown(MBKeyboardEvent &key);
    virtual void ListenToEvent(long channel, long event, void *data);
    virtual void Idle();
protected:
    virtual void OnActivityEvent(long event, void *data);
    void PushButton(int num);
    bool ButtonPressed(int num);
    void ResetButtons();
    MBComPtr<ISBAnimButton> mButtons[9];                // +0x00d0 size 0x24
    MBComPtr<ISBAnimButton> mBigRedButton;              // +0x00f4 size 0x4
    MBComPtr<IMBAnim> mIndicator;                       // +0x00f8 size 0x4
    MBComPtr<IMBAnim> mDoors;                           // +0x00fc size 0x4
    MBComPtr<IMBAnim> mButtonAnims[9];                  // +0x0100 size 0x24
    MBList<int> mButtonsPressed;                        // +0x0124 size 0xc
    unsigned long mEndDialog;                           // +0x0130 size 0x4
    enum {
        kSMClosed = 0,
        kSMWorkingRight = 1,
        kSMWorkingWrong = 2,
        kSMOpenRight = 3,
        kSMOpenWrong = 4,
        kSMClosing = 5
    } mMachineState;                                    // +0x0134 size 0x4
};
