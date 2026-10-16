#include "SBControlPanelNode.h"
#include "SBCursorItems.h" // every node unit of the original has the cursor item strings
#include "MBRuntime.h"
#include "MBSoundUtils.h"
#include <stdio.h>
#include <string.h>

// 0x10042930 ??0SBControlPanelNode@@QAE@PAVIMBGameEngine@@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@Z
SBControlPanelNode::SBControlPanelNode(IMBGameEngine *engine, MBString spaceCode)
    : SBActivityNode(engine, spaceCode), mActive(false), mHasFlour(false), mIncorrectTries(0), mCurrentDigit(0)
{
    for (int i = 0; i < 45; i++)
        mCodes[i] = 0;
}

// 0x10042aa0 ??_GSBControlPanelNode@@UAEPAXI@Z

// 0x10042ae0 ??1SBControlPanelNode@@UAE@XZ
SBControlPanelNode::~SBControlPanelNode()
{
}

// EQUIV 0x10042bc0 ?OnStart@SBControlPanelNode@@UAEXXZ: reviewed: the differences are register choices (the original pushes and stores immediate zeros, VC6 uses esi), lea/inc for i + 1 and the order of an independent load; calls, arguments and order agree. Was: register allocation: the original keeps i in eax and i + 1 in ebx and no zero register (esi) in the loop; the statements match
void SBControlPanelNode::OnStart()
{
    SteamboatNode::OnStart();
    MBComPtr<ISBGameState> gs(MBIID_CSBGameState);
    mHasFlour = gs->GetProperty("GRFingerPrintsDusted", 0) != 0;
    long correctcode[2];
    correctcode[0] = MBrandom(0, 9);
    do {
        correctcode[1] = MBrandom(0, 9);
    } while (correctcode[0] == correctcode[1]);
    mCorrectCode = ConvertDigitsIntoCode(correctcode);
    MBComPtr<IMBAnim> anim;
    for (int i = 0; i < 10; i++) {
        char buf[64];
        if (mHasFlour && (correctcode[0] == i || correctcode[1] == i))
            sprintf(buf, "CPb%dthumb", i + 1);
        else
            sprintf(buf, "CPb%d", i + 1);
        anim = CreateAnim(buf);
        mWorld->GetRoot().RemoveObject(anim);
        mButtons[i].CreateObject(MBIID_CSBAnimButton);
        mButtons[i]->SetAnim(anim);
        mButtons[i]->SetClickMessage(i + 500);
        mButtons[i]->SetBroadcastChannel(400);
        mWorld->GetRoot().AddObject(mButtons[i]);
        MBComPtr<IMBMouseable> btnMouseable(mButtons[i]);
        mMouseMonitor->AddMouseable(btnMouseable);
    }
    mSavedCursor = mCursor;
}

// 0x10042ec0 ?OnEnd@SBControlPanelNode@@UAEXXZ
void SBControlPanelNode::OnEnd()
{
    SetCursor(mSavedCursor, 0, 0);
    ShowCursor();
    SteamboatNode::OnEnd();
}

// 0x10042f10 ?Idle@SBControlPanelNode@@UAEXXZ
void SBControlPanelNode::Idle()
{
    MBComPtr<IMBGameClock> clock(MBIID_IMBGameClock);
    clock->GetGameTime();
    SteamboatNode::Idle();
}

// 0x10043000 ?StartListening@SBControlPanelNode@@UAEXXZ
void SBControlPanelNode::StartListening()
{
    MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
    if (broadcaster)
        broadcaster->AddListener(-1, this);
    SBActivityNode::StartListening();
}

// 0x100430e0 ?StopListening@SBControlPanelNode@@UAEXXZ
void SBControlPanelNode::StopListening()
{
    MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
    if (broadcaster)
        broadcaster->RemoveListener(-1, this);
    SBActivityNode::StopListening();
}

// 0x100431c0 ?OnKeyDown@SBControlPanelNode@@UAEXAAUMBKeyboardEvent@@@Z
void SBControlPanelNode::OnKeyDown(MBKeyboardEvent &key)
{
    if (key.mCharCode == 27 || key.mCharCode == ' ')
        SendExitSceneEvent(MBString("GR"));
    else
        SteamboatNode::OnKeyDown(key);
}

// 0x10043320 ?ListenToEvent@SBControlPanelNode@@UAEXJJPAX@Z
void SBControlPanelNode::ListenToEvent(long channel, long event, void *data)
{
    SBActivityNode::ListenToEvent(channel, event, data);
}

// 0x10043350 ?OnActivityEvent@SBControlPanelNode@@MAEXJPAX@Z
void SBControlPanelNode::OnActivityEvent(long event, void *data)
{
    MBComPtr<ISBGameState> gs(MBIID_CSBGameState);
    bool player_is_mickey = gs->GetProperty("HCMickeyCaptive", 0) == 0;
    if (event >= 500 && event <= 509) {
        gs->SetProperty("GRkeypadClicked", 1);
        StartSound("CPbuttonsnd");
        mCurrentCode[mCurrentDigit] = event - 500;
        if (++mCurrentDigit == 2) {
            int code = ConvertDigitsIntoCode(mCurrentCode);
            mCodes[code] = 1;
            bool correct;
            if (mHasFlour)
                correct = code == mCorrectCode;
            else {
                int untried = 0;
                for (int i = 0; i < 45; i++)
                    if (mCodes[i] == 0)
                        untried++;
                correct = untried == 0;
            }
            if (correct) {
                switch (MBrandom(0, 1)) {
                case 0:
                    MBQueueSound(player_is_mickey ? "cp04bmc0010" : "cp04bmi0010", 20, 1);
                    MBQueueSound("cp04bgf0020", 20, 1);
                    MBQueueSound(player_is_mickey ? "cp04bmc0030" : "cp04bmi0030", 20, 1);
                    break;
                case 1:
                    MBQueueSound(player_is_mickey ? "cp05xmc0010" : "cp05xmi0010", 20, 1);
                    MBQueueSound("cp05xgf0020", 20, 1);
                    MBQueueSound(player_is_mickey ? "cp05xmc0030" : "cp05xmi0030", 20, 1);
                    break;
                }
                gs->SetProperty("GRcorrectCodeEntered", 1);
                SendExitSceneEvent(MBString("RR"));
            } else {
                mIncorrectTries++;
                if (mHasFlour) {
                    switch (mIncorrectTries) {
                    case 1:
                        MBQueueSound(player_is_mickey ? "cp04amc0010" : "cp04ami0010", 20, 1);
                        break;
                    case 3:
                        MBQueueSound(player_is_mickey ? "cp04amc0020" : "cp04ami0020", 20, 1);
                        break;
                    case 5:
                        MBQueueSound(player_is_mickey ? "cp04amc0030" : "cp04ami0030", 20, 1);
                        break;
                    case 7:
                        MBQueueSound(player_is_mickey ? "cp04amc0040" : "cp04ami0040", 20, 1);
                        break;
                    }
                } else {
                    switch (mIncorrectTries) {
                    case 1:
                        MBQueueSound(player_is_mickey ? "cp02xmc0010" : "cp02xmi0010", 20, 1);
                        break;
                    case 3:
                        MBQueueSound(player_is_mickey ? "cp02xmc0030" : "cp02xmi0030", 20, 1);
                        break;
                    case 5:
                        MBQueueSound(player_is_mickey ? "cp02xmc0050" : "cp02xmi0050", 20, 1);
                        break;
                    case 7:
                        MBQueueSound(player_is_mickey ? "cp02xmc0070" : "cp02xmi0070", 20, 1);
                        break;
                    }
                }
                gs->SetProperty("GRcorrectCodeEntered", (long)0);
                mCurrentDigit = 0;
            }
        }
    }
    SBActivityNode::OnActivityEvent(event, data);
}

// 0x10043800 ?ConvertDigitsIntoCode@SBControlPanelNode@@IAEHPAJ@Z
int SBControlPanelNode::ConvertDigitsIntoCode(long *digits)
{
    int code;
    int i;
    if (digits[0] < digits[1]) {
        code = digits[0] * 10;
        for (i = 0; i <= digits[0]; i++)
            code -= i;
        code += digits[1] - digits[0] - 1;
    } else {
        code = digits[1] * 10;
        for (i = 0; i <= digits[1]; i++)
            code -= i;
        code += digits[0] - digits[1] - 1;
    }
    return code;
}

// Matched by this code (template and inline instances):
// 0x100438a0 ??0?$MBComPtr@VISBAnimButton@@@@QAE@XZ
// 0x10043960 ?_QueryInterface@?$MBComPtr@VISBAnimButton@@@@AAEJABQAVIMBUnknown@@@Z
// 0x10043aa0 ?_QueryInterface@?$MBComPtr@VIMBMouseable@@@@AAEJABV?$MBComPtr@VISBAnimButton@@@@@Z

// Not claimed (static initializers and header instances of this object):
// 0x100422f0 _$E49
// 0x10042310 _$E46
// 0x100428e0 _$E48
// 0x10042900 _$E47
// TODO 0x10043b40 $E342: static initializer from the headers (calls ??_B?1???id@?$ctype@G@std@@$D@@9@51, $E335); its $E number depends on the original header set
// 0x10043b80 _$E55
// TODO 0x10043ba0 $E338: static initializer from the headers (registers ?id@?$collate@D@std@@$E with atexit); its $E number depends on the original header set
