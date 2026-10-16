#include "SBActivityNode.h"
#include "SBCursorItems.h" // every node unit of the original has the cursor item strings

// Event channel of the activity events (OnActivityEvent).
const long kActivityEventChannel = 400;

// 0x1003aab0 ??0SBActivityNode@@QAE@PAVIMBGameEngine@@PAVMfAiSpace@@@Z
SBActivityNode::SBActivityNode(IMBGameEngine *engine, MfAiSpace *space) : SteamboatNode(engine, space)
{
    SetBackgroundMusicState(false);
    mShowGui = false;
}

// Inline members of SteamboatNode.h, emitted with the vftable:
// 0x1003ab40 ?GetMfCameraNode@SteamboatNode@@UAEPAVMfAiActorNode@@XZ
// 0x1003ab60 ?GetCameraDir@SteamboatNode@@UAEAAV?$Vec3@M@@XZ
// 0x1003ab80 ?GetCameraAngle@SteamboatNode@@UAEMXZ
// 0x1003aba0 ?GetCursor@SteamboatNode@@UAEPAVIMBAnim@@XZ
// 0x1003abc0 ??_GSBActivityNode@@UAEPAXI@Z

// 0x1003ac00 ??0SBActivityNode@@QAE@PAVIMBGameEngine@@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@Z
SBActivityNode::SBActivityNode(IMBGameEngine *engine, MBString spaceCode) : SteamboatNode(engine, spaceCode)
{
    SetBackgroundMusicState(false);
    mShowGui = false;
}

// 0x1003add0 ??1SBActivityNode@@UAE@XZ
SBActivityNode::~SBActivityNode()
{
}

// 0x1003ae00 ?StartListening@SBActivityNode@@UAEXXZ
void SBActivityNode::StartListening()
{
    MBComPtr<IMBEventManager> eventManager(MBIID_IMBEventManager);
    if (eventManager)
        eventManager->AddListener(kActivityEventChannel, this);
    SteamboatNode::StartListening();
}

// 0x1003aee0 ?StopListening@SBActivityNode@@UAEXXZ
void SBActivityNode::StopListening()
{
    MBComPtr<IMBEventManager> eventManager(MBIID_IMBEventManager);
    if (eventManager)
        eventManager->RemoveListener(kActivityEventChannel, this);
    SteamboatNode::StopListening();
}

// 0x1003afc0 ?ListenToEvent@SBActivityNode@@UAEXJJPAX@Z
void SBActivityNode::ListenToEvent(long channel, long event, void *data)
{
    if (channel == kActivityEventChannel)
        OnActivityEvent(event, data);
    SteamboatNode::ListenToEvent(channel, event, data);
}

// 0x1003b010 ?RenderScene@SBActivityNode@@MAEXXZ
void SBActivityNode::RenderScene()
{
    SteamboatNode::sBackgroundDirty = 0;
    SteamboatNode::RenderScene();
}

// 0x1003b030 ?OnActivityEvent@SBActivityNode@@MAEXJPAX@Z
void SBActivityNode::OnActivityEvent(long event, void *data)
{
}

// Inline members of SimpleNode.h, emitted with the vftable:
// 0x1003b130 ?QueryInterface@SimpleNode@@UAEJABUMBGUID@@PAPAX@Z
// 0x1003b2e0 ?AddRef@SimpleNode@@UAEKXZ
// 0x1003b300 ?Release@SimpleNode@@UAEKXZ

// Adjustor thunks (emitted with the vftables):
// 0x1003b0d0 ?QueryInterface@SimpleNode@@W3AEJABUMBGUID@@PAPAX@Z
// 0x1003b0e0 ?AddRef@SimpleNode@@W3AEKXZ
// 0x1003b0f0 ?Release@SimpleNode@@W3AEKXZ
// 0x1003b100 ?QueryInterface@SimpleNode@@W7AEJABUMBGUID@@PAPAX@Z
// 0x1003b110 ?AddRef@SimpleNode@@W7AEKXZ
// 0x1003b120 ?Release@SimpleNode@@W7AEKXZ

// Instances the linker took from other objects (this unit emits them too):
// 0x10018b10 ?erase@?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@QAEAAV12@II@Z

// Not claimed (static initializers and header instances of this object):
// 0x1003a470 _$E49
// 0x1003a490 _$E46
// 0x1003aa60 _$E48
// 0x1003aa80 _$E47
// TODO 0x1003b050 $E342: static initializer from the headers (calls ??_B?1???id@?$ctype@G@std@@$D@@9@51, $E335); its $E number depends on the original header set
// 0x1003b090 _$E55
// TODO 0x1003b0b0 $E338: static initializer from the headers (registers ?id@?$collate@D@std@@$E with atexit); its $E number depends on the original header set
