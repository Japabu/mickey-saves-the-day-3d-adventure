#include "SBActorManager.h"
#include "SBCursorItems.h" // every node unit of the original has the cursor item strings
#include "Actor.h"
#include "BitmapBuffer.h"
#include "MBRuntime.h"
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "Graph_BasicRefreshTraverser.h"
#include "SteamboatNode.h"
#include "SBItem.h"
#include "MfAi.h"
#include "MfStream.h"
#include "MfHookData.h"
#include "SBMotivateDriver.h"
#include <locale> // the ctype/collate id initializers at the end of the original object

// 0x1003b970 ??0SBActorManager@@IAE@XZ
SBActorManager::SBActorManager() : mSceneRootNode(0)
{
    _mRefCnt = 0;
    MBComPtr<IMBEventManager> eventManager(MBIID_IMBEventManager);
    if (eventManager) {
        eventManager->AddListener(300, this);
        eventManager->AddListener(700, this);
    }
}

// 0x1003bae0 ?QueryInterface@SBActorManager@@UAEJABUMBGUID@@PAPAX@Z
long SBActorManager::QueryInterface(const MBGUID &iid, void **obj)
{
    if (!obj)
        return -2;
    *obj = 0;
    IMBUnknown *p = 0;
    if (iid == MBIID_IMBEventListener)
        p = (IMBEventListener *)this;
    if (p) {
        p->AddRef();
        *obj = p;
        return 0;
    }
    return -3;
}

// 0x1003bb80 ?AddRef@SBActorManager@@UAEKXZ
unsigned long SBActorManager::AddRef()
{
    return ++_mRefCnt;
}

// 0x1003bba0 ?Release@SBActorManager@@UAEKXZ
unsigned long SBActorManager::Release()
{
    unsigned long r = --_mRefCnt;
    if (!_mRefCnt)
        delete this;
    return r;
}

// 0x1003bdf0 ??1SBActorManager@@UAE@XZ
SBActorManager::~SBActorManager()
{
    ActorMap::iterator iActor = mActorMap.begin();
    while (iActor != mActorMap.end())
        iActor = DeleteActorByIterator(iActor);
    if (mSceneRootNode) {
        mSceneRootNode->Unref();
        mSceneRootNode = 0;
    }
}

// 0x1003c040 ?GetInstance@SBActorManager@@SAPAV1@XZ
SBActorManager *__cdecl SBActorManager::GetInstance()
{
    if (!_instance) {
        _instance = new SBActorManager;
        _instance->AddRef();
    }
    return _instance;
}

// 0x1003c0d0 ?Destroy@SBActorManager@@SAXXZ
void __cdecl SBActorManager::Destroy()
{
    if (_instance) {
        MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
        if (broadcaster) {
            broadcaster->RemoveListener(300, _instance);
            broadcaster->RemoveListener(700, _instance);
        }
        _instance->Release();
    }
}

// 0x1003c1e0 ?SetSceneRoot@SBActorManager@@UAEXPAVNode@Gfx@@@Z
void SBActorManager::SetSceneRoot(Gfx::Node *rootNode)
{
    mSceneRootNode = rootNode;
    mSceneRootNode->Ref();
}

#pragma optimize("a", off)
// 0x1003c210 ?DeleteActor@SBActorManager@@UAEXV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@Z
void SBActorManager::DeleteActor(MBString actorName)
{
    MBDebugStr("(INFO) deleting actor %s...\n", actorName.c_str());
    DeleteActorByIterator(mActorMap.find(actorName));
    MBComPtr<ISBGameState> gs(MBIID_ISBGameState);
    SBItem *item = gs->GetItem(MBString(actorName.c_str()));
    if (item)
        SteamboatNode::RemoveFromWorld(MBComPtr<IMBUnknown>(item->GetHotspot()));
    if (actorName.find("camera") != -1) {
        // the camera actor went: SteamboatNode forgets its camera node
        MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
        if (broadcaster)
            broadcaster->BroadcastEvent(300, 209, 0);
    }
}

// EQUIV 0x1003c5d0 ?LoadActor@SBActorManager@@UAEXV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@Z: reviewed: register choice in the inlined destructor of the by-value parameter only. Was:
// 91% similar; only the inlined destructor of the by-value parameter at the end differs (the original
// keeps the pointer in eax and the count byte in cl, VC6 the other way round)
void SBActorManager::LoadActor(const MBString mbActorName)
{
    MBDebugStr("SBActorManager::LoadActor %s\n", mbActorName.c_str());
    MfAiSpace *mofaSpace = SBMotivateDriver::GetMotivateSpace();
    MBDebugStr("SBActorManager::LoadActor: mofaSpace 0x%08X is bad ptr? %d\n", mofaSpace, IsBadReadPtr(mofaSpace, 4));
    if (mofaSpace) {
        MfAiActor *actor = mofaSpace->GetActor(mbActorName.c_str());
        LoadActor(mbActorName, actor);
    }
}

// 0x1003c4b0 ?DeleteActorByIterator@SBActorManager@@IAE?AViterator@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVActor@@@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVActor@@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAVActor@@@@@2@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAVActor@@@@@std@@V3412@@Z
SBActorManager::ActorMap::iterator SBActorManager::DeleteActorByIterator(ActorMap::iterator iActor)
{
    if (iActor != mActorMap.end()) {
        Actor *pActor = (*iActor).second;
        pActor->AddRef();
        mSceneRootNode->RemoveChild(mSceneRootNode->FindChild(pActor));
        MBDebugStr("(INFO) actor found. ref count: %d\n", pActor->GetRefCount());
        pActor->Release();
        pActor->Release();
        mCharacterMap.erase((*iActor).first);
        return mActorMap.erase(iActor);
    }
    return mActorMap.end();
}

// where the scene files of Fly live (SteamboatNode.cpp)
extern MBString sFlyDir;

// EQUIV 0x1003c730 ?LoadActor@SBActorManager@@UAEXV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVMfAiActor@@@Z: reviewed: frame layout, the operand order of an independent compare, and GetItem's MBString assign called in the original and inlined by VC6; calls and their order agree. Was:
// 66% similar; the original's frame is 4 bytes smaller, it compares the find result with end() the
// other way round (cmp eax, ecx), adds the index to c_str() before a shared load in the name loop
// and builds GetItem's MBString with an out-of-line assign (VC6 inlines it). Calls and order are the
// original's
void SBActorManager::LoadActor(const MBString mbActorName, MfAiActor *pActor)
{
    // the 3D model of a Motivate actor: <fly dir><name without prefix and number>.fly
    if (mCharacterMap.end() == mCharacterMap.find(mbActorName)) {
        MBString name;
        int i = 2;
        char curChar = mbActorName.c_str()[i];
        while ((curChar >= 'a' && curChar <= 'z') || (curChar >= 'A' && curChar <= 'Z')) {
            name += curChar;
            i++;
            curChar = mbActorName.c_str()[i];
        }
        mCharacterMap[mbActorName] = pActor;
        MBString filename = sFlyDir + name + ".fly";
        Actor *actor = new Actor(pActor, mSceneRootNode, filename);
        actor->AddRef();
        mActorMap[mbActorName] = actor;
        actor->SetName(mbActorName.c_str());
    }
    MBDebugStr("SBActorManager::LoadActor: loaded actor %s\n", mbActorName.c_str());
    Graph::BasicRefreshTraverser().Traverse(*mSceneRootNode, Graph::BasicRefreshTraverser::Partial);
    MBComPtr<ISBGameState> gs(MBIID_ISBGameState);
    SBItem *item = gs->GetItem(MBString(mbActorName.c_str()));
    if (item)
        SteamboatNode::AddToWorld(MBComPtr<IMBUnknown>(item->GetHotspot()));
}
#pragma optimize("a", on)

// 0x1003cbe0 ?GetScreenRects@SBActorManager@@UAEXHPAVBitmapBuffer@@@Z
void SBActorManager::GetScreenRects(const int iBuffer, BitmapBuffer *bitmapBuffer)
{
    // every character that drew something in this buffer dirties its rectangle
    for (ActorMap::iterator it = mActorMap.begin(); it != mActorMap.end(); ++it) {
        tagRECT rect = (*it).second->GetScreenRect(iBuffer);
        if (rect.left != rect.right && rect.top != rect.bottom)
            bitmapBuffer->SetScreenRect((*it).second->GetScreenRect(iBuffer));
        (*it).second->SetCurrentBuffer(iBuffer);
    }
}

// 0x1003cc80 ?SetCurrentBuffer@SBActorManager@@UAEXH@Z
void SBActorManager::SetCurrentBuffer(const int iBuffer)
{
    for (ActorMap::iterator it = mActorMap.begin(); it != mActorMap.end(); ++it)
        (*it).second->SetCurrentBuffer(iBuffer);
}

// 0x1003ccd0 ?ListenToEvent@SBActorManager@@UAEXJJPAX@Z
void SBActorManager::ListenToEvent(long channel, long event, void *data)
{
    switch (channel) {
    case 300:
        ListenToMotivateEvent(event, data);
        break;
    case 700:
        ListenToDialogueEvent(event, data);
        break;
    }
}

// EQUIV 0x1003cd20 ?ListenToDialogueEvent@SBActorManager@@MAEXJPAX@Z: reviewed: the same instructions with other stack slots (the original shares the case slots). Was: 73% similar; same instructions
// (99% once stack offsets are ignored), but the original's frame is 12 bytes smaller: it puts case 713's
// abbreviatedName and the temporaries into slots shared with case 701 (PDB: name and the second
// abbreviatedName both at esp0-0xb4); which declaration order does that is open
void SBActorManager::ListenToDialogueEvent(long event, void *data)
{
    // dialogue lines start with the speaker's abbreviation: "<aa>..." for a head, "....<aa>" for a line
    switch (event) {
    case 701: {
        MBString abbreviatedName;
        char *line = (char *)data;
        abbreviatedName += line[5];
        abbreviatedName += line[6];
        MBString &actorName = mAbbreviationMap[abbreviatedName];
        ActorMap::iterator actorIterator2 = mActorMap.find(actorName);
        if (actorIterator2 == mActorMap.end()) {
            char serror[80];
            sprintf(serror, "actor %s not present in current scene\n", actorName.c_str());
            ActorMap::iterator it = mActorMap.begin();
            while (it != mActorMap.end()) {
                MBString name = (*it).first;
                it++;
            }
            MBDebugStr("\nERROR: SBActorManager::ListenToDialogueEvent: %s\n", serror);
        } else {
            mTalkingCharacterName = actorName;
            Actor *actor = (*actorIterator2).second;
            actor->PlayDialog(MBString(line));
        }
        break;
    }
    case 713: {
        MBString abbreviatedName;
        char *line = (char *)data;
        abbreviatedName += line[0];
        abbreviatedName += line[1];
        Actor *actor = (*mActorMap.find(mAbbreviationMap[abbreviatedName])).second;
        actor->SetActiveHead(atoi(line + 2));
        break;
    }
    }
}

// TODO 0x1003d1a0 ?ListenToMotivateEvent@SBActorManager@@MAEXJPAX@Z: 85% similar; the original's jump
// table covers events 302..310 (306..310 lead to the end: some label up to 310 whose code is gone; an
// empty `case 310:` is dropped by VC6), and it keeps data in esi and &event in edi (VC6: ebp, esi)
void SBActorManager::ListenToMotivateEvent(long event, void *data)
{
    // actors appear and go in the Motivate space; user events show, hide and load them
    MBComPtr<ISBGameState> gs(MBIID_CSBGameState);
    switch (event) {
    case 302:
        LoadActor((char *)((MfActorHookData *)data)->name, ((MfActorHookData *)data)->pActor);
        break;
    case 305:
        DeleteActor((char *)((MfActorHookData *)data)->pActor->GetInstanceName());
        break;
    case 304: {
        MBString mbActorName = (const char *)((MfActorHookData *)data)->pActor->GetInstanceName();
        ActorMap::iterator it = mActorMap.find(mbActorName);
        if (it != mActorMap.end())
            (*it).second->SetVisible(true);
        break;
    }
    case 303:
        if (((MfUserHookData *)data)->event == "HCLoadActor") {
            SBItem *item = gs->GetItem(MBString((char *)((MfUserHookData *)data)->data));
            if (item && !item->GetInScene()) {
                if (SBMotivateDriver::GetMotivateSpace()) {
                    MfAiActor *actor = SBMotivateDriver::GetMotivateSpace()->GetActor(((MfUserHookData *)data)->data);
                    SBMotivateDriver::GetMfRuntime()->GetWorld()->DeleteActor(actor);
                }
            } else
                LoadActor((char *)((MfUserHookData *)data)->data);
        } else if (((MfUserHookData *)data)->event == "HCPlaySound") {
        } else if (((MfUserHookData *)data)->event == "HCMakeVisible") {
            ActorMap::iterator it = mActorMap.find(MBString((char *)((MfUserHookData *)data)->data));
            if (it != mActorMap.end())
                (*it).second->SetVisible(true);
        } else if (((MfUserHookData *)data)->event == "HCMakeInvisible") {
            ActorMap::iterator it = mActorMap.find(MBString((char *)((MfUserHookData *)data)->data));
            if (it != mActorMap.end())
                (*it).second->SetVisible(false);
        }
        break;
    }
}

// 0x1003d790 ?SetVisible@SBActorManager@@UAEX_N@Z
void SBActorManager::SetVisible(bool show)
{
    for (ActorMap::iterator it = mActorMap.begin(); it != mActorMap.end(); ++it)
        (*it).second->SetVisible(show);
}

// 0x1003d7e0 ?AddToAbbreviationMap@SBActorManager@@QAEXV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@0@Z
void SBActorManager::AddToAbbreviationMap(MBString abbrev, MBString actorName)
{
    mAbbreviationMap[abbrev] = actorName;
}

// EQUIV 0x1003d9a0 ?CreateActorPrototype@SBActorManager@@SAPAVMfProtoActor@@VMfString@@@Z: reviewed: block order only: the failure exit is placed earlier in the original. Was: 82% similar;
// block order only: the original places the failure exit (destroy the file stream, return 0) right
// after the ReadObject test and the skill loop behind it; VC6 puts the exit last (early-return and
// if/else forms tried). The stream and the IMBWinFile are not released in the original either
MfProtoActor *__cdecl SBActorManager::CreateActorPrototype(MfString actorName)
{
    // the actor's prototype and skills from its 'mfa ' resource
    IMBStream *stream = 0;
    IMBWinFile *winFile = 0;
    SteamboatNode::GetStream(MBHashString((char *)actorName), 'mfa ', stream);
    if (stream) {
        stream->QueryInterface(MBIID_IMBWinFile, (void **)&winFile);
        if (winFile) {
            MfFileStream fileStream(winFile->GetFileHandle(), MF_FILE_READ, true);
            if (fileStream.Status() == 0) {
                MfProtoActor *proto = (MfProtoActor *)fileStream.ReadObject(0);
                if (proto) {
                    int numSkills;
                    fileStream >> numSkills;
                    for (int i = 0; i < numSkills; i++) {
                        MfProtoSkill *skill = (MfProtoSkill *)fileStream.ReadObject(0);
                        if (skill)
                            proto->AddSkill(skill);
                    }
                    return proto;
                }
            }
            return 0;
        }
    }
    return 0;
}

// Template and inline instances this unit also emits:
// 0x1003bbd0 ??_GSBActorManager@@UAEPAXI@Z
// 0x1003bc10 ??1?$MBMap@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVMfAiActor@@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@@@QAE@XZ
// 0x1003bcb0 ??1?$MBMap@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVActor@@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@@@QAE@XZ
// 0x1003bd50 ??1?$MBMap@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@V12@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@@@QAE@XZ
// 0x1003dc70 ??1?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@V12@@std@@QAE@XZ
// 0x1003de10 ??0?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVActor@@@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVActor@@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAVActor@@@@@2@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAVActor@@@@@std@@QAE@ABU?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@1@_NABV?$MBAllocator@PAVActor@@@@@Z
// 0x1003de70 ?erase@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVActor@@@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVActor@@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAVActor@@@@@2@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAVActor@@@@@std@@QAE?AViterator@12@V312@@Z
// 0x1003e4f0 ?erase@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVActor@@@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVActor@@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAVActor@@@@@2@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAVActor@@@@@std@@QAE?AViterator@12@V312@0@Z
// 0x1003e8d0 ?_Inc@const_iterator@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVActor@@@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVActor@@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAVActor@@@@@2@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAVActor@@@@@std@@QAEXXZ
// 0x1003e9f0 ??0?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVMfAiActor@@@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVMfAiActor@@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAVMfAiActor@@@@@2@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAVMfAiActor@@@@@std@@QAE@ABU?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@1@_NABV?$MBAllocator@PAVMfAiActor@@@@@Z
// 0x1003ea30 ?erase@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVMfAiActor@@@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVMfAiActor@@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAVMfAiActor@@@@@2@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAVMfAiActor@@@@@std@@QAE?AViterator@12@V312@0@Z
// 0x1003ee50 ??0?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@V12@@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@V12@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@@@2@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@@@std@@QAE@ABU?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@1@_NABV?$MBAllocator@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@@@Z
// 0x1003ee90 ?erase@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@V12@@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@V12@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@@@2@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@@@std@@QAE?AViterator@12@V312@0@Z
// 0x1003f430 ?_Erase@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVActor@@@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVActor@@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAVActor@@@@@2@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAVActor@@@@@std@@IAEXPAU_Node@12@@Z
// 0x1003f520 ?_Init@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVActor@@@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVActor@@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAVActor@@@@@2@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAVActor@@@@@std@@IAEXXZ
// 0x1003f790 ?erase@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVMfAiActor@@@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVMfAiActor@@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAVMfAiActor@@@@@2@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAVMfAiActor@@@@@std@@QAE?AViterator@12@V312@@Z
// 0x1003fe10 ?_Erase@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVMfAiActor@@@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVMfAiActor@@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAVMfAiActor@@@@@2@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAVMfAiActor@@@@@std@@IAEXPAU_Node@12@@Z
// 0x1003ff00 ?_Init@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVMfAiActor@@@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVMfAiActor@@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAVMfAiActor@@@@@2@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAVMfAiActor@@@@@std@@IAEXXZ
// 0x10040170 ?erase@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@V12@@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@V12@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@@@2@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@@@std@@QAE?AViterator@12@V312@@Z
// 0x10040800 ?_Erase@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@V12@@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@V12@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@@@2@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@@@std@@IAEXPAU_Node@12@@Z
// 0x10040910 ?_Init@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@V12@@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@V12@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@@@2@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@@@std@@IAEXXZ
// 0x10041250 ?_Lbound@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVMfAiActor@@@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVMfAiActor@@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAVMfAiActor@@@@@2@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAVMfAiActor@@@@@std@@IBEPAU_Node@12@ABV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@2@@Z
// 0x10041810 ?_Ubound@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVMfAiActor@@@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVMfAiActor@@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAVMfAiActor@@@@@2@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAVMfAiActor@@@@@std@@IBEPAU_Node@12@ABV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@2@@Z
// 0x10041a10 ?_Inc@const_iterator@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVMfAiActor@@@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVMfAiActor@@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAVMfAiActor@@@@@2@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAVMfAiActor@@@@@std@@QAEXXZ
// 0x10041be0 ?_Inc@const_iterator@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@V12@@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@V12@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@@@2@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@@@std@@QAEXXZ
// 0x1003db60 ??1?$MBComPtr@VIMBUnknown@@@@QAE@XZ
// 0x1003db90 ??1?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVActor@@@std@@QAE@XZ
// 0x1003dc00 ??1?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVMfAiActor@@@std@@QAE@XZ
// 0x1003ddd0 ?insert@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVActor@@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAVActor@@@@@std@@QAE?AU?$pair@Viterator@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVActor@@@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVActor@@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAVActor@@@@@2@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAVActor@@@@@std@@_N@2@ABU?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVActor@@@2@@Z
// 0x1003de50 ?begin@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVActor@@@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVActor@@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAVActor@@@@@2@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAVActor@@@@@std@@QAE?AViterator@12@XZ
// 0x1003e680 ?find@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVActor@@@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVActor@@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAVActor@@@@@2@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAVActor@@@@@std@@QAE?AViterator@12@ABV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@2@@Z
// 0x1003e740 ??0?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVActor@@@std@@QAE@ABV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@1@ABQAVActor@@@Z
// 0x1003e9b0 ?insert@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVMfAiActor@@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAVMfAiActor@@@@@std@@QAE?AU?$pair@Viterator@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVMfAiActor@@@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVMfAiActor@@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAVMfAiActor@@@@@2@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAVMfAiActor@@@@@std@@_N@2@ABU?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVMfAiActor@@@2@@Z
// 0x1003ebc0 ?find@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVMfAiActor@@@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVMfAiActor@@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAVMfAiActor@@@@@2@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAVMfAiActor@@@@@std@@QAE?AViterator@12@ABV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@2@@Z
// 0x1003ec80 ??0?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVMfAiActor@@@std@@QAE@ABV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@1@ABQAVMfAiActor@@@Z
// 0x1003ee10 ?insert@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@V12@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@@@std@@QAE?AU?$pair@Viterator@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@V12@@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@V12@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@@@2@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@@@std@@_N@2@ABU?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@V12@@2@@Z
// 0x1003f020 ??0?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@V12@@std@@QAE@ABV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@1@0@Z
// 0x1003f230 ?insert@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVActor@@@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVActor@@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAVActor@@@@@2@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAVActor@@@@@std@@QAE?AU?$pair@Viterator@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVActor@@@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVActor@@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAVActor@@@@@2@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAVActor@@@@@std@@_N@2@ABU?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVActor@@@2@@Z
// 0x1003f400 ?lower_bound@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVActor@@@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVActor@@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAVActor@@@@@2@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAVActor@@@@@std@@QAE?AViterator@12@ABV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@2@@Z
// 0x1003f5c0 ?insert@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVMfAiActor@@@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVMfAiActor@@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAVMfAiActor@@@@@2@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAVMfAiActor@@@@@std@@QAE?AU?$pair@Viterator@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVMfAiActor@@@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVMfAiActor@@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAVMfAiActor@@@@@2@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAVMfAiActor@@@@@std@@_N@2@ABU?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVMfAiActor@@@2@@Z
// 0x1003ffa0 ?insert@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@V12@@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@V12@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@@@2@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@@@std@@QAE?AU?$pair@Viterator@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@V12@@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@V12@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@@@2@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@@@std@@_N@2@ABU?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@V12@@2@@Z
// 0x100409b0 ??0?$pair@Viterator@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVActor@@@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVActor@@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAVActor@@@@@2@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAVActor@@@@@std@@_N@std@@QAE@ABViterator@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVActor@@@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVActor@@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAVActor@@@@@2@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAVActor@@@@@1@AB_N@Z
// 0x100409e0 ??0?$pair@Viterator@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVMfAiActor@@@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVMfAiActor@@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAVMfAiActor@@@@@2@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAVMfAiActor@@@@@std@@_N@std@@QAE@ABViterator@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVMfAiActor@@@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVMfAiActor@@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAVMfAiActor@@@@@2@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAVMfAiActor@@@@@1@AB_N@Z
// 0x10040a10 ??0?$pair@Viterator@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@V12@@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@V12@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@@@2@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@@@std@@_N@std@@QAE@ABViterator@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@V12@@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@V12@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@@@2@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@@@1@AB_N@Z
// 0x10040a40 ?_Insert@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVActor@@@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVActor@@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAVActor@@@@@2@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAVActor@@@@@std@@IAE?AViterator@12@PAU_Node@12@0ABU?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVActor@@@2@@Z
// 0x10040dd0 ?_Lbound@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVActor@@@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVActor@@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAVActor@@@@@2@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAVActor@@@@@std@@IBEPAU_Node@12@ABV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@2@@Z
// 0x10040ec0 ?_Insert@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVMfAiActor@@@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVMfAiActor@@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAVMfAiActor@@@@@2@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAVMfAiActor@@@@@std@@IAE?AViterator@12@PAU_Node@12@0ABU?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVMfAiActor@@@2@@Z
// 0x10041340 ?_Insert@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@V12@@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@V12@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@@@2@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@@@std@@IAE?AViterator@12@PAU_Node@12@0ABU?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@V12@@2@@Z
// 0x100416d0 ??4Text@@QAEAAV0@ABV0@@Z
// 0x10041720 ?_Dec@const_iterator@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVActor@@@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVActor@@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAVActor@@@@@2@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAVActor@@@@@std@@QAEXXZ
// 0x10041920 ?_Dec@const_iterator@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVMfAiActor@@@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVMfAiActor@@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAVMfAiActor@@@@@2@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAVMfAiActor@@@@@std@@QAEXXZ
// 0x10041af0 ?_Dec@const_iterator@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@V12@@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@V12@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@@@2@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@@@std@@QAEXXZ
// 0x10041cc0 ?_Construct@std@@YAXPAU?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVActor@@@1@ABU21@@Z
// 0x10041e70 ?_Construct@std@@YAXPAU?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVMfAiActor@@@1@ABU21@@Z
// 0x10042020 ?_Construct@std@@YAXPAU?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@V12@@1@ABU21@@Z
// 0x100421d0 ?_QueryInterface@?$MBComPtr@VIMBUnknown@@@@AAEJABQAVIMBScreenPoly@@@Z

// Not claimed yet (static initializers and header instances of this object):
// 0x1003b330 _$E25
// 0x1003b350 _$E22
// 0x1003b920 _$E24
// 0x1003b940 _$E23
// 0x10042270 _$E113
// 0x100422b0 _$E109
// 0x100422d0 _$E112

// static data (zero in the original, .bss)
SBActorManager *SBActorManager::_instance = 0;
