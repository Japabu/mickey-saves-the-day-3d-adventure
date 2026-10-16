#include "SBCreditsNode.h"
#include <locale> // evidence: the original unit ends with the std::ctype<unsigned short>::id and std::collate<char>::id initializers
#include "SBCursorItems.h" // every node unit of the original has the cursor item strings

// 0x10044200 ??0SBCreditsNode@@QAE@PAVIMBGameEngine@@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@Z
SBCreditsNode::SBCreditsNode(IMBGameEngine *engine, MBString spaceCode) : SBActivityNode(engine, spaceCode)
{
    MBComPtr<IMBNodeManager> nodemgr = mEngine->GetNodeMgr();
    IMBNode *node = nodemgr->GetActiveNode();
    mPrevNode = SteamboatNode::GetSpaceCode();
    if (node)
        node->Release();
    SetBackgroundMusicState(true);
}

// 0x10044560 ??_GSBCreditsNode@@UAEPAXI@Z

// 0x100445a0 ??1SBCreditsNode@@UAE@XZ
SBCreditsNode::~SBCreditsNode()
{
}

// 0x10044680 ?OnStart@SBCreditsNode@@UAEXXZ
void SBCreditsNode::OnStart()
{
    SetSceneBackgroundMusic("sandwichmaker");
    SteamboatNode::OnStart();
    mScreens = CreateAnim("CDScreens");
    mCursor->Hide();
}

// 0x100447e0 ?OnEnd@SBCreditsNode@@UAEXXZ
void SBCreditsNode::OnEnd()
{
    mCursor->Show();
    SteamboatNode::OnEnd();
}

// 0x10044810 ?Idle@SBCreditsNode@@UAEXXZ
void SBCreditsNode::Idle()
{
    SteamboatNode::Idle();
}

// 0x10044830 ?StartListening@SBCreditsNode@@UAEXXZ
void SBCreditsNode::StartListening()
{
    MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
    if (broadcaster) {
        broadcaster->AddListener(-1, this);
        broadcaster->AddListener(-2, this);
    }
    SBActivityNode::StartListening();
}

// 0x10044920 ?StopListening@SBCreditsNode@@UAEXXZ
void SBCreditsNode::StopListening()
{
    MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
    if (broadcaster) {
        broadcaster->RemoveListener(-1, this);
        broadcaster->RemoveListener(-2, this);
    }
    SBActivityNode::StopListening();
}

// 0x10044a10 ?OnKeyDown@SBCreditsNode@@UAEXAAUMBKeyboardEvent@@@Z
void SBCreditsNode::OnKeyDown(MBKeyboardEvent &key)
{
    SteamboatNode::OnKeyDown(key);
}

// 0x10044a30 ?ListenToEvent@SBCreditsNode@@UAEXJJPAX@Z
void SBCreditsNode::ListenToEvent(long channel, long event, void *data)
{
    if (channel == -2 && event == 6 && ((MBAnimEventStruct *)data)->anim == mScreens)
        Done();
    SBActivityNode::ListenToEvent(channel, event, data);
}

// 0x10044aa0 ?OnActivityEvent@SBCreditsNode@@MAEXJPAX@Z
void SBCreditsNode::OnActivityEvent(long event, void *data)
{
    SBActivityNode::OnActivityEvent(event, data);
}

// 0x10044ad0 ?Done@SBCreditsNode@@MAEXXZ
void SBCreditsNode::Done()
{
    MBComPtr<ISBNodeManager> mgr(MBIID_ISBNodeManager);
    mgr->AddNode("MV");
}

// Matched by this code (template and inline instances):
// 0x10044bc0 ??1?$MBComPtr@VISBNodeManager@@@@QAE@XZ
// 0x10044bf0 ?Release@?$MBComPtr@VISBNodeManager@@@@QAEXXZ

// Not claimed (static initializers and header instances of this object):
// 0x10043bc0 _$E50
// 0x10043be0 _$E47
// 0x100441b0 _$E49
// 0x100441d0 _$E48
// TODO 0x100443e0 ?GetSpaceCode@SteamboatNode@@SA?AV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@XZ: header/template instance that our object does not emit (VC6 inlines it or another unit provides it)
// 0x10044c20 _$E61
// 0x10044c60 _$E57
// 0x10044c80 _$E60
