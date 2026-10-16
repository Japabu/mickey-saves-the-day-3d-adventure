#include "CSBNodeManager.h"
#include "SBCursorItems.h" // every node unit of the original has the cursor item strings
#include <locale> // evidence: the original unit ends with the std::ctype<unsigned short>::id and std::collate<char>::id initializers
#include "MfAi.h"
#include "MfHookData.h"
#include "SBMovieNode.h"
#include "SBControlPanelNode.h"
#include "SBSoupRecipeNode.h"
#include "SBMapNode.h"
#include "SBCreditsNode.h"
#include "SBTrashBashNode.h"
#include "SBGridlockNode.h"
#include "SBLinkingLogsNode.h"
#include "SBSongbirdNode.h"
#include "SBSandwichNode.h"
#include "SBSoupMachineNode.h"
#include "SBMainMenuNode.h"

// 0x10028ce0 ?MakeCSBNodeManager@@YAPAVIMBUnknown@@XZ
IMBUnknown *MakeCSBNodeManager()
{
    static IMBUnknown *instance = MBComUtilAddSingleton((IMBEventListener *)new CSBNodeManager);
    if (instance)
        instance->AddRef();
    return instance;
}

// 0x10028da0 ??0CSBNodeManager@@QAE@XZ
CSBNodeManager::CSBNodeManager() : mEngine(0)
{
    _mRefCnt = 0;
    MBComPtr<IMBEventManager> eventManager(MBIID_IMBEventManager);
    if (eventManager)
        eventManager->AddListener(300, this);
}

// 0x10028e80 ?QueryInterface@CSBNodeManager@@UAEJABUMBGUID@@PAPAX@Z
long CSBNodeManager::QueryInterface(const MBGUID &iid, void **obj)
{
    if (!obj)
        return -2;
    *obj = 0;
    IMBUnknown *p = 0;
    if (iid == MBIID_IMBEventListener)
        p = (IMBEventListener *)this;
    if (iid == MBIID_ISBNodeManager)
        p = (ISBNodeManager *)this;
    if (iid == MBIID_IMBUnknown)
        p = (ISBNodeManager *)this;
    if (p) {
        p->AddRef();
        *obj = p;
        return 0;
    }
    return -3;
}

// 0x10028ff0 ?AddRef@CSBNodeManager@@UAEKXZ
unsigned long CSBNodeManager::AddRef()
{
    return ++_mRefCnt;
}

// 0x10029010 ?Release@CSBNodeManager@@UAEKXZ
unsigned long CSBNodeManager::Release()
{
    unsigned long r = --_mRefCnt;
    if (!_mRefCnt)
        delete this;
    return r;
}

// 0x10029050 ?SetEngine@CSBNodeManager@@UAEXPAVIMBGameEngine@@@Z
void CSBNodeManager::SetEngine(IMBGameEngine *engine)
{
    mEngine = engine;
}

// 0x10029070 ??1CSBNodeManager@@IAE@XZ
CSBNodeManager::~CSBNodeManager()
{
    MBComPtr<IMBEventManager> eventManager(MBIID_IMBEventManager);
    if (eventManager)
        eventManager->RemoveListener(this);
}

// 0x10029140 ?AddNode@CSBNodeManager@@UAEXPBD@Z
void CSBNodeManager::AddNode(const char *pszSpaceCode)
{
    // the node of a space is chosen by the first two letters of its code
    MBString spaceCode(pszSpaceCode);
    IMBNode *node;
    if (spaceCode == "MV")
        node = new SBMovieNode(mEngine, 0);
    else if (spaceCode == "CP")
        node = new SBControlPanelNode(mEngine, spaceCode);
    else if (spaceCode == "HK")
        node = new SBSoupRecipeNode(mEngine, spaceCode);
    else if (spaceCode == "MP")
        node = new SBMapNode(mEngine, spaceCode);
    else if (spaceCode == "CD")
        node = new SBCreditsNode(mEngine, spaceCode);
    else if (spaceCode == "TB")
        node = new SBTrashBashNode(mEngine, spaceCode);
    else if (spaceCode == "GL")
        node = new SBGridlockNode(mEngine, spaceCode);
    else if (spaceCode == "LL")
        node = new SBLinkingLogsNode(mEngine, spaceCode);
    else if (spaceCode == "SB")
        node = new SBSongbirdNode(mEngine, spaceCode);
    else if (spaceCode == "SH")
        node = new SBSandwichNode(mEngine, spaceCode);
    else if (spaceCode == "SM")
        node = new SBSoupMachineNode(mEngine, spaceCode);
    else if (spaceCode == "UI")
        node = new SBMainMenuNode(mEngine, spaceCode);
    else
        node = new SteamboatNode(mEngine, spaceCode);
    MBComPtr<IMBNodeManager> nodeManager = mEngine->GetNodeMgr();
    nodeManager->QueueSwap(node);
    IMBNode *activeNode = nodeManager->GetActiveNode();
    if (activeNode)
        activeNode->Release();
}

// 0x10029a70 ?AddNode@CSBNodeManager@@UAEXPAX@Z
void CSBNodeManager::AddNode(void *eventData)
{
    MfAiSpace *space = ((MfSpaceHookData *)eventData)->pSpace;
    if (space) {
        MfString spaceName(space->GetSpaceName());
        MBString spaceCode = (char *)spaceName.LeftString(2);
        AddNode(spaceCode.c_str());
    }
}

// 0x10029c00 ?GetActiveNode@CSBNodeManager@@UAEPAVSteamboatNode@@XZ
SteamboatNode *CSBNodeManager::GetActiveNode()
{
    MBComPtr<IMBNodeManager> nodeManager = mEngine->GetNodeMgr();
    return (SteamboatNode *)nodeManager->GetActiveNode();
}

// 0x10029c80 ?ListenToEvent@CSBNodeManager@@UAEXJJPAX@Z
void CSBNodeManager::ListenToEvent(long channel, long event, void *data)
{
    if (channel == 300) {
        switch (event) {
        case 301:
            AddNode(data);
            break;
        case 303:
            MfUserHookData *hookData = (MfUserHookData *)data;
            if (hookData->event == "hc_load_mb_scene")
                AddNode((const char *)hookData->data);
            break;
        }
    }
}

// Adjustor thunks (emitted with the vftables):
// 0x10029e10 ?QueryInterface@CSBNodeManager@@W3AEJABUMBGUID@@PAPAX@Z
// 0x10029e20 ?AddRef@CSBNodeManager@@W3AEKXZ
// 0x10029e30 ?Release@CSBNodeManager@@W3AEKXZ

// Template and inline instances this unit also emits:
// 0x10029d10 ??1?$MBComPtr@VIMBNode@@@@QAE@XZ
// 0x10029d40 ??1?$MBComPtr@VIMBNodeManager@@@@QAE@XZ
// 0x10029d70 ??C?$MBComPtr@VIMBNodeManager@@@@QBEPAVIMBNodeManager@@XZ

// 0x100286a0 _$E18
// 0x100286c0 _$E15
// 0x10028c90 _$E17
// 0x10028cb0 _$E16
// 0x10029d90 _$E68
// 0x10029dd0 _$E64
// 0x10029df0 _$E67
