#include "SBMapNode.h"
#include "SBCursorItems.h" // every node unit of the original has the cursor item strings
#include "MBRuntime.h"
#include "MBSoundUtils.h"
#include <stdio.h>
#include <string.h>

// The map's buttons: animation, scene code and the number of the line that names the scene
// (the table has no name in the PDB).
static const char *sMapButtons[13][3] = {
    { "MPbutton1", "HL", "0080" },  { "MPbutton2", "MX", "0110" },  { "MPbutton3", "MS", "0100" },
    { "MPbutton4", "VD", "0170" },  { "MPbutton5", "PP", "0140" },  { "MPbutton6", "NB", "0120" },
    { "MPbutton7", "DA", "0030" },  { "MPbutton8", "PD", "0150" },  { "MPbutton9", "HH", "0070" },
    { "MPbutton10", "IY", "0090" }, { "MPbutton11", "PE", "0130" }, { "MPbutton12", "DH", "0040" },
    { "MPbutton13", "ER", "0050" },
};

// 0x10087d80 ??0SBMapNode@@QAE@PAVIMBGameEngine@@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@Z
SBMapNode::SBMapNode(IMBGameEngine *engine, MBString spaceCode) : SBActivityNode(engine, spaceCode), mLeaving(false)
{
    MBComPtr<IMBNodeManager> nodemgr = mEngine->GetNodeMgr();
    IMBNode *node = nodemgr->GetActiveNode();
    mPrevNode = SteamboatNode::GetSpaceCode();
    if (node)
        node->Release();
}

// 0x10087f50 ??_GSBMapNode@@UAEPAXI@Z

// 0x10087f90 ??1SBMapNode@@UAE@XZ
SBMapNode::~SBMapNode()
{
}

// TODO 0x10088020 ?OnStart@SBMapNode@@UAEXXZ: (80% similar) VC6 calls the temporaries' Release out of line in the loop, the original inlines it (frame 8 bytes smaller)
void SBMapNode::OnStart()
{
    static bool introplayed;
    MBComPtr<ISBGameState> gs(MBIID_CSBGameState);
    MBComPtr<ISBAnimButton> btn;
    MBComPtr<IMBAnim> anim;
    MBComPtr<IMBMouseable> btnMouseable;
    SteamboatNode::OnStart();
    bool player_is_mickey = gs->GetProperty("HCMickeyCaptive", 0) == 0;
    anim = CreateAnim("MPexit");
    btn.CreateObject(MBIID_CSBAnimButton);
    btn->SetAnim(anim);
    btn->SetClickMessage(300);
    btn->SetBroadcastChannel(400);
    mWorld->GetRoot().AddObject(btn);
    mWorld->GetRoot().RemoveObject(anim);
    btnMouseable = btn;
    mMouseMonitor->AddMouseable(btnMouseable);
    for (int i = 100; i < 113; i++) {
        anim = CreateAnim(sMapButtons[i - 100][0]);
        btn.CreateObject(MBIID_CSBAnimButton);
        btn->SetAnim(anim);
        btn->SetClickMessage(i + 100);
        btn->SetRolloverMessage(i);
        btn->SetBroadcastChannel(400);
        if (gs->GetProperty("ERmovie", 0) == 1 && strcmp(sMapButtons[i - 100][1], "ER") == 0) {
            btn->SetClickMessage(500);
            btn->SetRolloverMessage(500);
        }
        mWorld->GetRoot().AddObject(btn);
        mWorld->GetRoot().RemoveObject(anim);
        btnMouseable = btn;
        mMouseMonitor->AddMouseable(btnMouseable);
        if (!gs->HasVisitedScene(MBString(sMapButtons[i - 100][1])))
            btn->Hide();
    }
    if (!introplayed) {
        introplayed = true;
        StartSound(player_is_mickey ? "gd15xmc0010" : "gd15xmi0010");
    }
}

// 0x10088500 ?OnEnd@SBMapNode@@UAEXXZ
void SBMapNode::OnEnd()
{
    SteamboatNode::OnEnd();
}

// 0x10088520 ?Idle@SBMapNode@@UAEXXZ
void SBMapNode::Idle()
{
    SteamboatNode::Idle();
}

// 0x10088540 ?StartListening@SBMapNode@@UAEXXZ
void SBMapNode::StartListening()
{
    MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
    if (broadcaster)
        broadcaster->AddListener(-1, this);
    SBActivityNode::StartListening();
}

// 0x10088620 ?StopListening@SBMapNode@@UAEXXZ
void SBMapNode::StopListening()
{
    MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
    if (broadcaster)
        broadcaster->RemoveListener(-1, this);
    SBActivityNode::StopListening();
}

// 0x10088700 ?OnKeyDown@SBMapNode@@UAEXAAUMBKeyboardEvent@@@Z
void SBMapNode::OnKeyDown(MBKeyboardEvent &key)
{
    if (key.mCharCode == 27 || key.mCharCode == ' ')
        SendExitSceneEvent(mPrevNode);
    else
        SteamboatNode::OnKeyDown(key);
}

// 0x10088740 ?ListenToEvent@SBMapNode@@UAEXJJPAX@Z
void SBMapNode::ListenToEvent(long channel, long event, void *data)
{
    SBActivityNode::ListenToEvent(channel, event, data);
}

// EQUIV 0x10088770 ?OnActivityEvent@SBMapNode@@MAEXJPAX@Z: reviewed: the original inlines _Eos in the string construction (two stores), VC6 calls it; nothing else differs. Was: VC6 calls _Eos in the inlined string construction, the original inlines it
void SBMapNode::OnActivityEvent(long event, void *data)
{
    MBComPtr<ISBGameState> gs(MBIID_CSBGameState);
    bool player_is_mickey = gs->GetProperty("HCMickeyCaptive", 0) == 0;
    MBComPtr<IMBSoundManager> soundManagerPtr(MBIID_IMBSoundManager);
    if (!mLeaving) {
        if (event == 300) {
            mLeaving = true;
            SendExitSceneEvent(mPrevNode);
        } else if (event >= 200 && event < 213) {
            const char *scene = sMapButtons[event - 200][1];
            if (MBString(scene) == MBString("PE")) {
                if (gs->GetProperty("PRbathtubRecovered", 0) && gs->GetProperty("PRbicycleRecovered", 0) &&
                    gs->GetProperty("PRpropellerRecovered", 0))
                    scene = "CE";
            }
            mLeaving = true;
            SendExitSceneEvent(MBString(scene));
        } else if (event >= 100 && event < 113) {
            char buf[64];
            sprintf(buf, "gd15x%s%s", player_is_mickey ? "mc" : "mi", sMapButtons[event - 100][2]);
            if (!soundManagerPtr->IsSoundPlaying(MBHashString("gd15xmc0010")) &&
                !soundManagerPtr->IsSoundPlaying(MBHashString("gd15xmi0010")))
                MBForceSound(buf, 31, 1);
        }
    }
    SBActivityNode::OnActivityEvent(event, data);
}

// Matched by this code (template and inline instances):
// 0x10088bb0 ?Release@?$MBComPtr@VISBAnimButton@@@@QAEXXZ

// Not claimed (static initializers and header instances of this object):
// 0x10087740 _$E49
// 0x10087760 _$E46
// 0x10087d30 _$E48
// 0x10087d50 _$E47
// TODO 0x10088be0 $E342: static initializer from the headers (calls ??_B?1???id@?$ctype@G@std@@$D@@9@51, $E335); its $E number depends on the original header set
// 0x10088c20 _$E55
// TODO 0x10088c40 $E338: static initializer from the headers (registers ?id@?$collate@D@std@@$E with atexit); its $E number depends on the original header set
