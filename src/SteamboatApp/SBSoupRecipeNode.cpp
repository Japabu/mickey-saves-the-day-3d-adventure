#include "SBSoupRecipeNode.h"
#include "SBCursorItems.h" // every node unit of the original has the cursor item strings

// 0x1009a9c0 ??0SBSoupRecipeNode@@QAE@PAVIMBGameEngine@@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@Z
SBSoupRecipeNode::SBSoupRecipeNode(IMBGameEngine *engine, MBString spaceCode) : SBActivityNode(engine, spaceCode)
{
    MBComPtr<IMBNodeManager> nodemgr = mEngine->GetNodeMgr();
    IMBNode *node = nodemgr->GetActiveNode();
    mPrevNode = SteamboatNode::GetSpaceCode();
    if (node)
        node->Release();
    SetBackgroundMusicState(true);
}

// 0x1009ab90 ??_GSBSoupRecipeNode@@UAEPAXI@Z

// 0x1009abd0 ??1SBSoupRecipeNode@@UAE@XZ
SBSoupRecipeNode::~SBSoupRecipeNode()
{
}

// 0x1009ac60 ?OnStart@SBSoupRecipeNode@@UAEXXZ
void SBSoupRecipeNode::OnStart()
{
    SteamboatNode::OnStart();
    MBComPtr<ISBGameState> gs(MBIID_CSBGameState);
    gs->GetProperty("HCMickeyCaptive", 0);
    MBComPtr<IMBScreenPoly> poly;
    MBComPtr<IMBMouseable> mouseable;
    poly.CreateObject(MBIID_CMBScreenPoly);
    poly->SetRect(MBRect(0, 0, 640, 40), 1);
    poly->SetBroadcastChannel(400);
    poly->SetClickMessage(100);
    poly->SetEnterMessage(204);
    poly->SetLeaveMessage(101);
    mouseable = poly;
    mMouseMonitor->AddMouseable(mouseable);
    poly.CreateObject(MBIID_CMBScreenPoly);
    poly->SetRect(MBRect(0, 0, 40, 480), 1);
    poly->SetBroadcastChannel(400);
    poly->SetClickMessage(100);
    poly->SetEnterMessage(202);
    poly->SetLeaveMessage(101);
    mouseable = poly;
    mMouseMonitor->AddMouseable(mouseable);
    poly.CreateObject(MBIID_CMBScreenPoly);
    poly->SetRect(MBRect(0, 440, 640, 480), 1);
    poly->SetBroadcastChannel(400);
    poly->SetClickMessage(100);
    poly->SetEnterMessage(205);
    poly->SetLeaveMessage(101);
    mouseable = poly;
    mMouseMonitor->AddMouseable(mouseable);
    poly.CreateObject(MBIID_CMBScreenPoly);
    poly->SetRect(MBRect(600, 0, 640, 480), 1);
    poly->SetBroadcastChannel(400);
    poly->SetClickMessage(100);
    poly->SetEnterMessage(203);
    poly->SetLeaveMessage(101);
    mouseable = poly;
    mMouseMonitor->AddMouseable(mouseable);
}

// 0x1009b0b0 ?OnEnd@SBSoupRecipeNode@@UAEXXZ
void SBSoupRecipeNode::OnEnd()
{
    SteamboatNode::OnEnd();
}

// 0x1009b0d0 ?Idle@SBSoupRecipeNode@@UAEXXZ
void SBSoupRecipeNode::Idle()
{
    SteamboatNode::Idle();
}

// 0x1009b0f0 ?StartListening@SBSoupRecipeNode@@UAEXXZ
void SBSoupRecipeNode::StartListening()
{
    MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
    if (broadcaster)
        broadcaster->AddListener(-1, this);
    SBActivityNode::StartListening();
}

// 0x1009b1d0 ?StopListening@SBSoupRecipeNode@@UAEXXZ
void SBSoupRecipeNode::StopListening()
{
    MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
    if (broadcaster)
        broadcaster->RemoveListener(-1, this);
    SBActivityNode::StopListening();
}

// 0x1009b2b0 ?OnKeyDown@SBSoupRecipeNode@@UAEXAAUMBKeyboardEvent@@@Z
void SBSoupRecipeNode::OnKeyDown(MBKeyboardEvent &key)
{
    if (key.mCharCode == 27 || key.mCharCode == ' ')
        SendExitSceneEvent(mPrevNode);
    else
        SteamboatNode::OnKeyDown(key);
}

// 0x1009b2f0 ?ListenToEvent@SBSoupRecipeNode@@UAEXJJPAX@Z
void SBSoupRecipeNode::ListenToEvent(long channel, long event, void *data)
{
    SBActivityNode::ListenToEvent(channel, event, data);
}

// 0x1009b320 ?OnActivityEvent@SBSoupRecipeNode@@MAEXJPAX@Z
void SBSoupRecipeNode::OnActivityEvent(long event, void *data)
{
    if (event == 100) {
        SetCursorSequence(1);
        SendExitSceneEvent(mPrevNode);
    } else if (event >= 200 && event <= 210)
        SetCursorSequence(event - 200);
    else if (event == 101)
        SetCursorSequence(0);
    SBActivityNode::OnActivityEvent(event, data);
}

// Matched by this code (template and inline instances):
// 0x1009b3c0 ?_QueryInterface@?$MBComPtr@VIMBScreenPoly@@@@AAEJABQAVIMBUnknown@@@Z
// 0x1009b460 ?_QueryInterface@?$MBComPtr@VIMBMouseable@@@@AAEJABV?$MBComPtr@VIMBScreenPoly@@@@@Z

// Not claimed (static initializers and header instances of this object):
// 0x1009a380 _$E49
// 0x1009a3a0 _$E46
// 0x1009a970 _$E48
// 0x1009a990 _$E47
// TODO 0x1009b500 $E342: static initializer from the headers (calls ??_B?1???id@?$ctype@G@std@@$D@@9@51, $E335); its $E number depends on the original header set
// 0x1009b540 _$E55
// TODO 0x1009b560 $E338: static initializer from the headers (registers ?id@?$collate@D@std@@$E with atexit); its $E number depends on the original header set
