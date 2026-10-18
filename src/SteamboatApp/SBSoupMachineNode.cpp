#include "SBSoupMachineNode.h"
#include "SBCursorItems.h" // every node unit of the original has the cursor item strings
#include "MBRuntime.h"
#include "MBSoundUtils.h"
#include <algorithm>
#include <stdio.h>

// 0x10098910 ??0SBSoupMachineNode@@QAE@PAVIMBGameEngine@@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@Z
SBSoupMachineNode::SBSoupMachineNode(IMBGameEngine *engine, MBString spaceCode) : SBActivityNode(engine, spaceCode)
{
    mMachineState = kSMClosed;
    SetBackgroundMusicState(true);
}

// 0x10098aa0 ??_GSBSoupMachineNode@@UAEPAXI@Z

// 0x10098b60 ??1SBSoupMachineNode@@UAE@XZ
SBSoupMachineNode::~SBSoupMachineNode()
{
}

// TODO 0x10098cf0 ?OnStart@SBSoupMachineNode@@UAEXXZ: 92% similar: frame layout differs (gs and the CreateAnim temporaries sit 4 bytes lower in the original), and the original tests the loop as i - 100 < 9 where VC6 compares i < 109 (writing i - 100 < 9 makes VC6 count from 0, 80% but further off)
void SBSoupMachineNode::OnStart()
{
    MBComPtr<ISBGameState> gs(MBIID_CSBGameState);
    MBComPtr<IMBAnim> anim;
    MBComPtr<ISBAnimButton> btn;
    MBComPtr<IMBMouseable> btnMouseable;
    SteamboatNode::OnStart();
    anim = CreateAnim("SMRedButton");
    mBigRedButton.CreateObject(MBIID_CSBAnimButton);
    mBigRedButton->SetAnim(anim);
    mBigRedButton->SetClickMessage(200);
    mBigRedButton->SetBroadcastChannel(400);
    mWorld->GetRoot().AddObject(mBigRedButton);
    mWorld->GetRoot().RemoveObject(anim);
    btnMouseable = mBigRedButton;
    mMouseMonitor->AddMouseable(btnMouseable);
    mDoors = CreateAnim("SMMachineDoors");
    mIndicator = CreateAnim("SMIndicator");
    for (int i = 100; i < 109; i++) {
        char buf[64];
        sprintf(buf, "SMbutton%d", i - 99);
        mButtonAnims[i - 100] = CreateAnim(buf);
        mButtons[i - 100].CreateObject(MBIID_CSBAnimButton);
        mButtons[i - 100]->SetAnim(mButtonAnims[i - 100]);
        mButtons[i - 100]->SetClickMessage(i);
        mButtons[i - 100]->SetBroadcastChannel(400);
        mWorld->GetRoot().AddObject(mButtons[i - 100]);
        mWorld->GetRoot().RemoveObject(mButtonAnims[i - 100]);
        btnMouseable = (ISBAnimButton *)mButtons[i - 100];
        mMouseMonitor->AddMouseable(btnMouseable);
    }
}

// 0x10099120 ?OnEnd@SBSoupMachineNode@@UAEXXZ
void SBSoupMachineNode::OnEnd()
{
    SteamboatNode::OnEnd();
}

// 0x10099140 ?Idle@SBSoupMachineNode@@UAEXXZ
void SBSoupMachineNode::Idle()
{
    SteamboatNode::Idle();
}

// 0x10099160 ?StartListening@SBSoupMachineNode@@UAEXXZ
void SBSoupMachineNode::StartListening()
{
    MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
    if (broadcaster) {
        broadcaster->AddListener(-1, this);
        broadcaster->AddListener(-2, this);
        broadcaster->AddListener(-4, this);
    }
    SBActivityNode::StartListening();
}

// 0x10099280 ?StopListening@SBSoupMachineNode@@UAEXXZ
void SBSoupMachineNode::StopListening()
{
    MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
    if (broadcaster) {
        broadcaster->RemoveListener(-2, this);
        broadcaster->RemoveListener(-1, this);
        broadcaster->RemoveListener(-4, this);
    }
    SBActivityNode::StopListening();
}

// 0x100993a0 ?OnKeyDown@SBSoupMachineNode@@UAEXAAUMBKeyboardEvent@@@Z
void SBSoupMachineNode::OnKeyDown(MBKeyboardEvent &key)
{
    if (mMachineState == kSMOpenRight) {
        mMachineState = kSMClosing;
        mDoors->SetCurrentSequence(4);
    } else if (mMachineState == kSMOpenWrong) {
        mMachineState = kSMClosing;
        mDoors->SetCurrentSequence(2);
    }
    SteamboatNode::OnKeyDown(key);
}

// 0x10099430 ?OnLMouseDown@SBSoupMachineNode@@UAEXAAUMBMouseEvent@@@Z
void SBSoupMachineNode::OnLMouseDown(MBMouseEvent &evt)
{
    if (mMachineState == kSMOpenRight) {
        mMachineState = kSMClosing;
        mDoors->SetCurrentSequence(4);
    } else if (mMachineState == kSMOpenWrong) {
        mMachineState = kSMClosing;
        mDoors->SetCurrentSequence(2);
    }
    SteamboatNode::OnLMouseDown(evt);
}


// EQUIV 0x100994c0 ?ListenToEvent@SBSoupMachineNode@@UAEXJJPAX@Z: reviewed: VC6 calls MBComPtr::Release out of line in the inlined gs constructor, the original folds it; the rest are register choices, compare operand order and the two-way select of the sound name (push/push vs mov/mov). Was: VC6 calls MBComPtr::Release out of line in the inlined gs constructor, the original folds it
void SBSoupMachineNode::ListenToEvent(long channel, long event, void *data)
{
    static int dnum = MBrandom(1, 9);
    MBComPtr<ISBGameState> gs(MBIID_CSBGameState);
    bool player_is_mickey = gs->GetProperty("HCMickeyCaptive", 0) == 0;
    if (channel == -2) {
        if (event == 6 && ((MBAnimEventStruct *)data)->anim == mDoors) {
            long sequence = ((MBAnimEventStruct *)data)->sequence;
            if (sequence == 1) {
                mMachineState = kSMOpenWrong;
                mIndicator->SetCurrentSequence(0);
                if (player_is_mickey) {
                    switch (dnum) {
                case 1:
                    StartSound("sm04bmc0010");
                    break;
                case 2:
                    StartSound("sm04bmc0020");
                    break;
                case 3:
                    StartSound("sm04bmc0030");
                    break;
                case 4:
                    StartSound("sm04bmc0050");
                    break;
                case 5:
                    StartSound("sm04bmc0060");
                    break;
                case 6:
                    StartSound("sm04bmc0070");
                    break;
                case 7:
                    StartSound("sm04bmc0090");
                    break;
                case 8:
                    StartSound("sm04bmc0110");
                    break;
                case 9:
                    StartSound("sm04bmc0120");
                    break;
                    }
                } else {
                    switch (dnum) {
                case 1:
                    StartSound("sm04bmi0010");
                    break;
                case 2:
                    StartSound("sm04bmi0020");
                    break;
                case 3:
                    StartSound("sm04bmi0030");
                    break;
                case 4:
                    StartSound("sm04bmi0050");
                    break;
                case 5:
                    StartSound("sm04bmi0060");
                    break;
                case 6:
                    StartSound("sm04bmi0070");
                    break;
                case 7:
                    StartSound("sm04bmi0090");
                    break;
                case 8:
                    StartSound("sm04bmi0110");
                    break;
                case 9:
                    StartSound("sm04bmi0120");
                    break;
                    }
                }
                if (++dnum > 9)
                    dnum = 1;
            } else if (sequence == 2) {
                mMachineState = kSMClosed;
                ResetButtons();
            } else if (sequence == 3) {
                mMachineState = kSMOpenRight;
                mIndicator->SetCurrentSequence(0);
                MBQueueSound(player_is_mickey ? "sm04amc0010" : "sm04ami0010", 20, 1);
                RemoveItemFromInventory(MBString("PRhankie0"));
                RemoveItemFromInventory(MBString("PRmglassAndHankie0"));
                if (gs->GetProperty("PRgoodsoupInInventory", 0) == 0)
                    AddItemToInventory(MBString("PRgoodsoup0"), false);
            } else if (sequence == 4) {
                mMachineState = kSMClosed;
                ResetButtons();
            }
        }
    } else if (channel == -4 && event == 1) {
        IMBSoundDescription *desc = *(IMBSoundDescription **)data;
        if (desc->GetSoundID() == MBHashString("sm04amc0010") || desc->GetSoundID() == MBHashString("sm04ami0010"))
            SendExitSceneEvent(MBString("VL"));
    }
    SBActivityNode::ListenToEvent(channel, event, data);
}

// the ingredients of the right soup
bool kCorrectButtons[9] = { false, true, false, true, false, false, false, false, true };

// 0x10099b80 ?OnActivityEvent@SBSoupMachineNode@@MAEXJPAX@Z
void SBSoupMachineNode::OnActivityEvent(long event, void *data)
{
    MBComPtr<ISBGameState> gs(MBIID_CSBGameState);
    bool player_is_mickey = gs->GetProperty("HCMickeyCaptive", 0) == 0;
    if (mMachineState == kSMClosed) {
        if (event >= 100 && event < 109)
            PushButton(event - 99);
        else if (event == 200) {
            if (player_is_mickey) {
                switch (MBrandom(1, 5)) {
                case 1:
                    StartSound("sm03xmc0010");
                    break;
                case 2:
                    StartSound("sm03xmc0020");
                    break;
                case 3:
                    StartSound("sm03xmc0030");
                    break;
                case 4:
                    StartSound("sm03xmc0040");
                    break;
                case 5:
                    StartSound("sm03xmc0050");
                    break;
                }
            } else {
                switch (MBrandom(1, 5)) {
                case 1:
                    StartSound("sm03xmi0010");
                    break;
                case 2:
                    StartSound("sm03xmi0020");
                    break;
                case 3:
                    StartSound("sm03xmi0030");
                    break;
                case 4:
                    StartSound("sm03xmi0040");
                    break;
                case 5:
                    StartSound("sm03xmi0050");
                    break;
                }
            }
            bool correct = true;
            for (int i = 0; i < 9; i++) {
                if ((!kCorrectButtons[i] && ButtonPressed(i + 1)) || (kCorrectButtons[i] && !ButtonPressed(i + 1)))
                    correct = false;
            }
            mIndicator->SetCurrentSequence(1);
            if (correct) {
                mMachineState = kSMWorkingRight;
                mDoors->SetCurrentSequence(3);
            } else {
                mMachineState = kSMWorkingWrong;
                mDoors->SetCurrentSequence(1);
            }
        }
    }
    SBActivityNode::OnActivityEvent(event, data);
}

// 0x10099e20 ?PushButton@SBSoupMachineNode@@IAEXH@Z
void SBSoupMachineNode::PushButton(int num)
{
    if (!ButtonPressed(num)) {
        MBForceSound("SMSEsoupbutton", num + 29, 1);
        MBQueueSound("SMSEaddIngrdient", num + 29, 1);
        mMouseMonitor->RemoveMouseable(MBComPtr<IMBMouseable>(mButtons[num - 1]));
        mWorld->GetRoot().RemoveObject(mButtons[num - 1]);
        mButtons[num - 1]->Disable();
        mWorld->GetRoot().AddObject(mButtonAnims[num - 1]);
        mButtonAnims[num - 1]->SetCurrentSequence(1);
        mButtonsPressed.push_back(num - 1);
        while (mButtonsPressed.size() > 3) {
            int button = mButtonsPressed.front();
            mButtonsPressed.pop_front();
            mWorld->GetRoot().RemoveObject(mButtonAnims[button]);
            mButtonAnims[button]->SetCurrentSequence(0);
            mMouseMonitor->AddMouseable(MBComPtr<IMBMouseable>(mButtons[button]));
            mWorld->GetRoot().AddObject(mButtons[button]);
            mButtons[button]->Enable();
        }
    }
}

// 0x1009a070 ?ButtonPressed@SBSoupMachineNode@@IAE_NH@Z
bool SBSoupMachineNode::ButtonPressed(int num)
{
    return std::find(mButtonsPressed.begin(), mButtonsPressed.end(), num - 1) != mButtonsPressed.end();
}


// 0x1009a0b0 ?ResetButtons@SBSoupMachineNode@@IAEXXZ
void SBSoupMachineNode::ResetButtons()
{
    for (int i = 0; i < 9; i++) {
        if (ButtonPressed(i + 1)) {
            mWorld->GetRoot().RemoveObject(mButtonAnims[i]);
            mButtonAnims[i]->SetCurrentSequence(0);
            mMouseMonitor->AddMouseable(MBComPtr<IMBMouseable>(mButtons[i]));
            mWorld->GetRoot().AddObject(mButtons[i]);
            mButtons[i]->Enable();
        }
    }
    mButtonsPressed.erase(mButtonsPressed.begin(), mButtonsPressed.end());
}

// Matched by this code (template and inline instances):
// 0x10098ae0 ??1?$MBList@H@@QAE@XZ
// 0x1009a290 ?erase@?$list@HV?$MBAllocator@H@@@std@@QAE?AViterator@12@V312@@Z
// 0x1009a2e0 ??Eiterator@?$list@HV?$MBAllocator@H@@@std@@QAE?AV012@H@Z

// Matched by this code (template and inline instances):
// 0x1009a210 ?insert@?$list@HV?$MBAllocator@H@@@std@@QAE?AViterator@12@V312@ABH@Z

// Not claimed (static initializers and header instances of this object):
// 0x100982d0 _$E49
// 0x100982f0 _$E46
// 0x100988c0 _$E48
// 0x100988e0 _$E47
// TODO 0x1009a300 $E343: static initializer from the headers (calls ??_B?1???id@?$ctype@G@std@@$D@@9@51, $E336); its $E number depends on the original header set
// 0x1009a340 _$E56
// TODO 0x1009a360 $E339: static initializer from the headers (registers ?id@?$collate@D@std@@$E with atexit); its $E number depends on the original header set
