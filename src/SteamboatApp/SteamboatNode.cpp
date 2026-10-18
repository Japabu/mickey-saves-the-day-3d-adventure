#include "SteamboatNode.h"
#include "SBItem.h"
#include "SBCursorItems.h"
#include "RenderManager.h"
#include "SBActorManager.h"
#include "BitmapBuffer.h"
#include "MBRuntime.h"
#include "MBSoundUtils.h"
#include "mss.h"
#include "SBDialoguePropertyManager.h"
#include "SBMotivateDriver.h"
#include "MfHookData.h"
#include "MBColor.h"
#include "SBMouseEvent.h"
#include "SBRenderObject.h"
#include "CSBRenderManager.h"
#include "MfScDataHandle.h"
#include "MfPoEvent.h"
#include "MfPoVector3.h"
#include "MfTransform3.h"
#include "MfAiActorNode.h"
#include "d3d7.h"
#include "SavedGameDB.h"
#include "MfProperty.h"
#include "MfStringValue.h"
#include "MfIntValue.h"
#include "ViewEventHandler.h"
#include "Gfx_Display.h"
#include "Graph_DataBase.h"
#include "Gfx_Node.h"
#include "DX_Exception.h"
#include "Gfx_TextureException.h"
#include "Gfx_TextureAttribute.h"
#include "DXGfx_ViewD3D.h"
#include "DX_Device.h"
#include "DX_Direct3D.h"
#include "DX_DirectDraw.h"
#include "DXGfx_DisplayD3D.h"
#include "DX_TextureMapManager.h"
#include "MfAiHook.h"
#include "MfCamera.h"
#include "MfRotation3.h"
#include "Gfx_Camera.h"
#include "Quaternion.h"
#include "DXGfx_NodeFactory.h"
#include "Graph_BasicRefreshTraverser.h"
#include "../MBEngine/MBLocale.h"
#include <windows.h>
#undef CreateFont                                       // SimpleNode::CreateFont, not the GDI call
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// 0x100a23e0 ??0SteamboatNode@@QAE@PAVIMBGameEngine@@PAVMfAiSpace@@@Z
SteamboatNode::SteamboatNode(IMBGameEngine *engine, MfAiSpace *mofaSpace)
    : ScriptHelpNode(engine), mShowGui(true), mMotivateSpace(mofaSpace), mMfCameraNode(0),
      mMotivateHotspot(MBIID_IMBScreenPoly), mDisableHotspots(false), mWaitingToPickUpItem(false),
      mEnableHotspotsDelayTimer(0), mCharacterDeactivatedInputEventChannel(false), mNextSceneIsUI(false),
      mRenderObject(0), m_pMfEvent(0), m_pMfVector(0), m_hpMfVector(0), m_hpMfEvent(0)
{
    sWantBackgroundMusic = true;
    mWorld.CreateObject(MBIID_CSBWorld);
    m_sbWorld = mWorld;
    mView->AttachToWorld(mWorld);
}

// EQUIV 0x100a2810 ??0SteamboatNode@@QAE@PAVIMBGameEngine@@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@Z: reviewed: 77% similar; the original calls mMotivateHotspot's CreateObject and m_sbWorld's _QueryInterface out of line here (inline in the other constructor)
SteamboatNode::SteamboatNode(IMBGameEngine *engine, MBString space)
    : ScriptHelpNode(engine), mShowGui(true), mMotivateSpace(0), mMfCameraNode(0), mSpaceCode(space),
      mMotivateHotspot(MBIID_IMBScreenPoly), mDisableHotspots(false), mWaitingToPickUpItem(false),
      mEnableHotspotsDelayTimer(0), mCharacterDeactivatedInputEventChannel(false), mNextSceneIsUI(false),
      mRenderObject(0), m_pMfEvent(0), m_pMfVector(0), m_hpMfVector(0), m_hpMfEvent(0)
{
    sWantBackgroundMusic = true;
    mWorld.CreateObject(MBIID_CSBWorld);
    m_sbWorld = mWorld;
    mView->AttachToWorld(mWorld);
}

// 0x100a2a60 ??1SteamboatNode@@UAE@XZ
SteamboatNode::~SteamboatNode()
{
    CleanupExitSceneEvent();
    if (mRenderObject) {
        mWorld->GetRoot().RemoveObject(mRenderObject);
        mRenderObject->Release();
    }
    RemoveDatabase(mSpaceCode.c_str());
    sCursorItem = 0;
    if (mWorld)
        mWorld->GetRoot().RemoveObject(mCursor);
}

// EQUIV 0x100a2ce0 ?Init@SteamboatNode@@UAEXXZ: reviewed: 63% similar; Graph::DataBase::Create is inline in the
// original (`sCurrentDataBase = new DataBase;` inlined here, no out-of-line copy in the binary), a
// declaration in G's Graph_DataBase.h. Tested with that inline: identical up to +0x30e, then only
// sTextControl's CreateObject differs (the original inlines its Release, VC6 calls it: inline budget;
// removing the later SetText line flips it)
void SteamboatNode::Init()
{
    // once per run: string table, display, scene graph, Motivate, the debug text line
    MBComPtr<ISBGameState> gs(MBIID_CSBGameState);
    SBDialoguePropertyManager::Create();
    ReadPrefs();
    AddDatabase("GD");
    AddDatabase("PR");
    AddDatabase("MV");
    MBComCreateInstance(0, MBIID_CMBStringTable, (IMBUnknown **)&sStringTable);
    if (sStringTable) {
        sStringTable->Load("steamboat.stl");
        sStringTable->SetLocale(MBLocale(sLocale));
    }
    MBDebugStr("INFO: Entering CreateDisplay\n");
    CreateDisplay();
    MBDebugStr("INFO: CreateDisplay finished\n");
    DXGfx::NodeFactory *factory = new DXGfx::NodeFactory;
    factory->SetValidFileHeader("*VisFly Project #00.25  bin*");
    Graph::DataBase::Create();
    Graph::DataBase::GetInstance()->SetFactory(factory);
    RenderManager::Create();
    RenderManager::GetInstance()->CreateView(0, RenderManager::cStandardView,
                                             Gfx::Display::GetInstance()->GetDisplaySurface());
    DeleteFile("Steamboat.log");
    DeleteFile("SteamboatD.log");
    SBMotivateDriver *driver = SBMotivateDriver::GetInstance();
    mMotivateSpace = driver->Init(mEngine, sMofaDir);
    sMotivateEventHandler = new ViewEventHandler(driver);
    sSceneRootNode = ((Gfx::NodeFactory *)Graph::DataBase::GetInstance()->GetFactory())->MakeNode();
    sSceneRootNode->Ref();
    SBActorManager::GetInstance()->SetSceneRoot(sSceneRootNode);
    Graph::BasicRefreshTraverser().Traverse(*sSceneRootNode, Graph::BasicRefreshTraverser::Partial);
    MBComPtr<IMBFont> AB12 = CreateFont("Arial_Black_12");
    sTextControl.CreateObject(MBIID_CMBTextControl);
    sTextControl->SetFont(AB12);
    sTextControl->Place(MBPoint3(100, 10, 2000));
    sTextControl->SetSize(420, 30);
    sTextControl->SetJustification(kTCJLeft);
    sTextControl->SetText(MBString(""));
    MBComPtr<IMBGameClock> clock(MBIID_IMBGameClock);
    MBsrand(clock->GetRealTime());
}

// EQUIV 0x100a33a0 ?SetCursorItem@SteamboatNode@@UAEXPAVSBItem@@@Z: reviewed: the original calls gs's Release and
// _QueryInterface out of line from the inlined CreateObject (VC6 inlines both here) and stores
// negativeMidpoint.y before computing x
void SteamboatNode::SetCursorItem(SBItem *item)
{
    sCursorItem = item;
    SetCursor(CreateAnim(item->GetClassNameA().c_str()), 0, 0);

    // Center every cast member of the item's cursor on the hot spot.
    IMBAnim *cursor = mCursor;
    cursor->SetCurrentSequence(1);
    unsigned long numCasts = cursor->GetNumCasts();
    for (unsigned long i = 1; i <= numCasts; i++) {
        IMBSurface *surface;
        cursor->GetCastMemberSurface(i, &surface);
        MBPoint negativeMidpoint(-((long)surface->GetWidth() / 2), -((long)surface->GetHeight() / 2));
        cursor->SetCastMemberRegistrationPoint(i, negativeMidpoint);
        surface->Release();
    }

    int cursorInt = 0;
    MBString cursorString = item->GetClassNameA();
    CursorItemStringToInt(cursorString, cursorInt);
    MBComPtr<ISBGameState> gs(MBIID_ISBGameState);
    gs->SetProperty("HC_cursorItemInt", cursorInt);
    gs->SetProperty("HC_cursorItem", item->GetInstanceName().c_str());
}

// 0x100a3630 ?SetCursor@SteamboatNode@@UAEXV?$MBComPtr@VIMBAnim@@@@FF@Z
void SteamboatNode::SetCursor(MBComPtr<IMBAnim> anim, short xOffset, short yOffset)
{
    HideCursor();
    MBShowSystemCursor(false);
    mCursor = anim;
    ShowCursor();
    if (!sCursorItem)
        SetCursorSequence(0);
}

// 0x100a3700 ?HideCursor@SteamboatNode@@UAEXXZ
void SteamboatNode::HideCursor()
{
    SimpleNode::HideCursor();
}

// 0x100a3720 ?ShowCursor@SteamboatNode@@UAEXXZ
void SteamboatNode::ShowCursor()
{
    SimpleNode::ShowCursor();
}

void UpdateCursorRegpoints(IMBAnim *cursor);

// 0x100a3740 ?ResetCursor@SteamboatNode@@QAEXXZ
void SteamboatNode::ResetCursor()
{
    // back to the plain cursor; Motivate learns that the player holds nothing
    if (sCursorItem) {
        sCursorItem = 0;
        SetCursor(CreateAnim("GDcursor"), 0, 0);
        UpdateCursorRegpoints(mCursor);
        MBString cursorString("none");
        MfProperty prop;
        if (sMfPrimaryActor->GetProperty("HC_cursorItem", prop)) {
            MfStringValue *value = new MfStringValue(cursorString.c_str());
            prop.SetValue(value);
            sMfPrimaryActor->SetProperty(prop);
        }
        if (sMfPrimaryActor->GetProperty("HC_cursorItemInt", prop)) {
            int cursorInt = 0;
            CursorItemStringToInt(cursorString, cursorInt);
            prop.SetValue(new MfIntValue(cursorInt));
            sMfPrimaryActor->SetProperty(prop);
        }
    }
}

// 0x100a3a90 ?UpdateCursorRegpoints@@YAXPAVIMBAnim@@@Z
void UpdateCursorRegpoints(IMBAnim *cursor)
{
    // where each cursor sequence has its hot spot: 0 at the left (top), 1 at the right (bottom), 2 in
    // the middle of each frame
    int alignment[11][2] = {
        { 0, 0 }, { 0, 0 }, { 0, 2 }, { 1, 2 }, { 2, 0 }, { 2, 1 }, { 0, 0 }, { 0, 1 }, { 1, 1 }, { 1, 0 }, { 0, 2 }
    };
    for (unsigned long sequence = 0; sequence < cursor->GetNumSequences(); sequence++) {
        unsigned long numFrames = cursor->GetNumFrames(sequence);
        for (unsigned long frame = 0; frame < numFrames; frame++) {
            long castID = cursor->GetCastID(sequence, frame);
            IMBSurface *surface;
            cursor->GetCastMemberSurface(castID, &surface);
            MBPoint regPoint = cursor->GetCastMemberRegistrationPoint(castID);
            switch (alignment[sequence][0]) {
            case 1:
                regPoint.x -= surface->GetWidth();
                break;
            case 2:
                regPoint.x += -((long)surface->GetWidth() / 2);
                break;
            }
            switch (alignment[sequence][1]) {
            case 1:
                regPoint.y -= surface->GetHeight();
                break;
            case 2:
                regPoint.y += -((long)surface->GetHeight() / 2);
                break;
            }
            surface->Release();
            cursor->SetCastMemberRegistrationPoint(castID, regPoint);
        }
    }
}

// 0x100a3cb0 ?SetCursorSequence@SteamboatNode@@UAEXK@Z
void SteamboatNode::SetCursorSequence(unsigned long seq)
{
    if (!sCursorItem)
        mCursor->SetCurrentSequence(seq);
}

// Set once the engine has been shut down (OnQuit may be called again).
static bool sDidQuit;

// 0x100a3ce0 ?OnQuit@SteamboatNode@@UAEXXZ
void SteamboatNode::OnQuit()
{
    if (!sDidQuit) {
        sDidQuit = true;
        delete sMotivateEventHandler;
        delete SBMotivateDriver::GetInstance();
        SBActorManager::Destroy();
        SBDialoguePropertyManager::Destroy();
        if (sSceneRootNode) {
            sSceneRootNode->Unref();
            sSceneRootNode = 0;
        }
        RenderManager::Destroy();
        Graph::DataBase::Destroy();
        Gfx::Display::Destroy();
        if (sBackgroundMusic) {
            AIL_close_stream(sBackgroundMusic);
            sBackgroundMusic = 0;
        }
        MBComPtr<IMBSoundManager> soundManager(MBIID_IMBSoundManager);
        if (soundManager)
            soundManager->StopAllSound();
        RemoveDatabase("GD");
        RemoveDatabase("PR");
        RemoveDatabase("MV");
        SimpleNode::OnQuit();
    }
}

// 3D characters drawn as a layer of their own (OnStart, OnPreRender, CalculateFrameRate)
static bool sUseLayered3DRender = true;

// OnStart prints its own source line when it disables the hotspots, so that statement sits on
// the original lines 637; the gap held code not reconstructed yet.




































































































































































































































































































// EQUIV 0x100a3e90 ?OnStart@SteamboatNode@@UAEXXZ: reviewed: 55% similar (94% of the instructions equal once stack
// offsets are ignored); inline budget: the original inlines animStream.Attach's and animation.CreateObject's
// Release in the first VL block and folds them away in the FI block (VC6 calls them out of line there; the
// later blocks match); the frame layout follows from that. Calls, constants and order are the original's
void SteamboatNode::OnStart()
{
    MBDebugStr("(INFO) Enter OnStart\n");
    ScriptHelpNode::OnStart();
    sCurrentActiveNode = this;
    MBDebugStr("getting ready to init the display\n");
    static bool sFirstTime = true;
    if (sFirstTime) {
        sFirstTime = false;
        MBDebugStr("calling steamboat::Init()\n");
        Init();
        VerifyCD();
    }
    SetSpaceName();
    int nTracePoint = 1;
    MBDebugStr("(INFO) OnStart (%d)\n", nTracePoint++);
    AddDatabase(mSpaceCode.c_str());
    MBDebugStr("(INFO) OnStart (%d)\n", nTracePoint++);
    SetCursor(CreateAnim("GDcursor"), 0, 0);
    MBDebugStr("(INFO) OnStart (%d)\n", nTracePoint++);
    UpdateCursorRegpoints(mCursor);
    MBDebugStr("(INFO) OnStart (%d)\n", nTracePoint++);
    SetCursorSequence(0);
    ShowCursor();
    MBComPtr<ISBGameState> gs(MBIID_CSBGameState);
    MBDebugStr("(INFO) OnStart (%d)\n", nTracePoint++);
    gs->VisitScene(mSpaceCode);
    MBDebugStr("(INFO) OnStart (%d)\n", nTracePoint++);
    sCursorItem = 0;
    sInventoryItemPicked = "";
    LoadHotspots();
    MBDebugStr("(INFO) OnStart (%d)\n", nTracePoint++);
    MBComPtr<ISBInventory> gui(MBIID_ISBInventory);
    gui->Enable(true);
    MBDebugStr("(INFO) OnStart (%d)\n", nTracePoint++);
    // the trek in the park: no hotspots until it is done
    if (mSpaceCode == "PE" && !gs->GetProperty("HC_act1_done", 0) && gs->GetProperty("PLtrek", 0) &&
        !gs->GetProperty("PLtrekDone", 0)) {
        mDisableHotspots = true;
        MBDebugStr("DisableHotspots: %s, line %d.\n", "D:\\tugboat\\code\\steamboat\\SteamboatNode.cpp", __LINE__);
        EnableHotspots(false);
    }
    LoadDialogue();
    MBDebugStr("(INFO) OnStart (%d)\n", nTracePoint++);
    LoadMiscellaneous();
    MBDebugStr("(INFO) OnStart (%d)\n", nTracePoint++);
    MfAiSpace *mofaSpace = SBMotivateDriver::GetMotivateSpace();
    MBDebugStr("SteamboatNode::OnStart(): mofaSpace 0x%08X is bad ptr? %d\n", mofaSpace, IsBadReadPtr(mofaSpace, 4));
    if (mofaSpace) {
        // the whole screen is Motivate's hotspot, behind everything else
        mMotivateHotspot->SetRect(MBRect(0, 0, 640, 480), true);
        mMotivateHotspot->SetBroadcastChannel(130);
        mMotivateHotspot->SetClickMessage(201, mMotivateHotspot);
        mMotivateHotspot->SetZ(-10);
        AddToWorld(MBComPtr<IMBUnknown>(mMotivateHotspot));
        LoadItems();
        MBDebugStr("(INFO) OnStart (%d)\n", nTracePoint++);
        MfAiViewEventHandler *pHandler;
        if (MfInterfaceRegistry::GetInterface(MfString("MfAiViewEventHandler"), (MfAi **)&pHandler)) {
            pHandler->SetSpace(mofaSpace);
            pHandler->Release();
            mofaSpace->StartUpdate();
        }
        MBDebugStr("(INFO) OnStart (%d)\n", nTracePoint++);
    }

    // the background, its z-buffer and the scene's animations; some scenes change with the story
    MBString prefix = mSpaceCode;
    if (mSpaceCode.compare("UI") != 0 && mSpaceCode.compare("CD") != 0) {
        if (mSpaceCode.compare("BK") == 0) {
            if (gs->GetProperty("PRbicycleRecovered", 0) || gs->GetProperty("PRbathtubRecovered", 0) ||
                gs->GetProperty("PRpropellerRecovered", 0))
                prefix += "2";
        } else if (mSpaceCode.compare("DH") == 0) {
            if (gs->GetProperty("ERmovie", 0))
                prefix += "3";
            else if (gs->GetProperty("PRcabinetInInventory", 0) || gs->GetProperty("PRcabinetUsed", 0))
                prefix += "2";
        } else if (mSpaceCode.compare("IY") == 0) {
            if (gs->GetProperty("PRbicycleRecovered", 0))
                prefix += "2";
        } else if (mSpaceCode.compare("FI") == 0) {
            if (gs->GetProperty("LLMaxLevel", 0) > 1)
                prefix += "2";
        } else if (gs->GetProperty("HC_act1_done", 0))
            prefix += "2";
    }
    MBDebugStr("(INFO) OnStart (%d)\n", nTracePoint++);
    MBString background = prefix + "bg";
    MBString zbuffer = prefix + "bgz";
    MBString defaultZbuffer = mSpaceCode + "bgz";
    IMBStream *stream = 0;
    long err = GetStream(MBHashString(background.c_str()), 'bmp ', stream);
    if (err != 0) {
        background = mSpaceCode + "bg";
        zbuffer = mSpaceCode + "bgz";
        GetStream(MBHashString(background.c_str()), 'bmp ', stream);
    }
    RenderManager::GetInstance()->GetBitmapBuffer(0)->LoadBitmap(*stream, cBackground);
    MBDebugStr("(INFO) OnStart (%d)\n", nTracePoint++);
    if (stream)
        stream->Release();

    MBComPtr<IMBResourceFileList> mgr(MBIID_CMBResourceMgr);
    MBComPtr<IMBStream> animStream;
    MBComPtr<IMBAnim> animation;
    IMBStream *tempStream;
    long id;
    if (mSpaceCode == MBString("VL")) {
        // the recovered vehicles stand in the village
        if (gs->GetProperty("PRbathtubRecovered", 0)) {
            id = MBHashString("VLSbathtub");
            mgr->GetStream(id, 'anm ', &tempStream);
            animStream.Attach(tempStream);
            if (animStream) {
                animation.CreateObject(MBIID_CMBAnim);
                if (animation) {
                    mWorld->AddObject(animation);
                    animation->Load(id, animStream, false);
                    animation->SetCurrentSequence(0);
                    animation->Play();
                }
            }
        }
        if (gs->GetProperty("PRbicycleRecovered", 0)) {
            id = MBHashString("VLSbike");
            mgr->GetStream(id, 'anm ', &tempStream);
            animStream.Attach(tempStream);
            if (animStream) {
                animation.CreateObject(MBIID_CMBAnim);
                if (animation) {
                    mWorld->AddObject(animation);
                    animation->Load(id, animStream, false);
                    animation->SetCurrentSequence(0);
                    animation->Play();
                }
            }
        }
        if (gs->GetProperty("PRpropellerRecovered", 0)) {
            id = MBHashString("VLSpropeller");
            mgr->GetStream(id, 'anm ', &tempStream);
            animStream.Attach(tempStream);
            if (animStream) {
                animation.CreateObject(MBIID_CMBAnim);
                if (animation) {
                    mWorld->AddObject(animation);
                    animation->Load(id, animStream, false);
                    animation->SetCurrentSequence(0);
                    animation->Play();
                }
            }
        }
    } else if (mSpaceCode.compare("FI") == 0) {
        id = MBHashString("FIchipper");
        mgr->GetStream(id, 'anm ', &tempStream);
        animStream.Attach(tempStream);
        if (animStream) {
            animation.CreateObject(MBIID_CMBAnim);
            if (animation) {
                mWorld->AddObject(animation);
                animation->Load(id, animStream, false);
                long sequence = 0;
                if (!gs->GetProperty("CRweaselsPresent", 1))
                    sequence = 1;
                animation->SetCurrentSequence(sequence);
                animation->Play();
            }
        }
    }
    MBDebugStr("(INFO) OnStart (%d)\n", nTracePoint++);
    err = GetStream(MBHashString(zbuffer.c_str()), 'bmp ', stream);
    if (err != 0)
        GetStream(MBHashString(defaultZbuffer.c_str()), 'bmp ', stream);
    RenderManager::GetInstance()->GetBitmapBuffer(0)->LoadBitmap(*stream, cZbuffer);
    if (stream)
        stream->Release();
    sBackgroundDirty = 0;

    mShowGui &= mSpaceCode.compare("ST") != 0;
    if (mShowGui) {
        MBComPtr<ISBInventory> gui(MBIID_ISBInventory);
        gui->Show(true);
    }
    mRenderObject = new SBRenderObject;
    mRenderObject->AddRef();
    mRenderObject->Place(MBPoint3(0, 0, 110));
    mWorld->AddObject(mRenderObject);
    mRenderObject->SetParentNode(this);
    if (!sUseLayered3DRender)
        mRenderObject->Hide();
    mWorld->AddObject(sTextControl);
    FeedBackgroundMusic();
    MBDebugStr("(INFO) OnStart (%d)\n", nTracePoint++);
    if (mSpaceCode == MBString("VL")) {
        // all three vehicles back: on to the movie
        MBComPtr<ISBGameState> gs(MBIID_CSBGameState);
        if (gs->GetProperty("PRbathtubRecovered", 0) && gs->GetProperty("PRbicycleRecovered", 0) &&
            gs->GetProperty("PRpropellerRecovered", 0))
            SendExitSceneEvent(MBString("MV"));
    }

    // a fresh scene: textures of the last one go
    ((DXGfx::ViewD3D *)RenderManager::GetInstance()->GetView(0))->GetDevice()->GetDirect3D()->GetTextureMapManager()->Release(false);
    IMBScreenManager *screenMgr;
    MBGetScreenManager(&screenMgr);
    IMBScreen *screen;
    screenMgr->GetScreenByIndex(0, &screen);
    MBComPtr<IMBDirectXScreen> dxScreen(screen);
    DDSCAPS2 ddsCaps2;
    memset(&ddsCaps2, 0, sizeof(ddsCaps2));
    ddsCaps2.dwCaps = DDSCAPS_TEXTURE;
    DWORD dwTotal;
    DWORD dwFree;
    dxScreen->GetDirectDraw7()->GetAvailableVidMem(&ddsCaps2, &dwTotal, &dwFree);
    screen->Release();
    screenMgr->Release();
    MEMORYSTATUS memstatus;
    memset(&memstatus, 0, sizeof(memstatus));
    GlobalMemoryStatus(&memstatus);
    MBDebugStr("INFO: Entering node \"%s\"\n", GetSpaceCode().c_str());
    MBDebugStr("      Texture mem: %dk/%dk free/total\n", dwFree / 1024, dwTotal / 1024);
    MBDebugStr("      RAM usage  : %dk/%dk free/total\n", memstatus.dwAvailPhys / 1024, memstatus.dwTotalPhys / 1024);
    MBDebugStr("      VM usage   : %dk/%dk free/total\n", memstatus.dwAvailPageFile / 1024, memstatus.dwTotalPageFile / 1024);
    if (mSpaceCode != MBString("ST")) {
        if (sBackgroundMusic)
            SetBackgroundMusicState(sWantBackgroundMusic);
        else if (sWantBackgroundMusic) {
            MBString musicname = gs->GetProperty("HCBackgroundMusic", MBString("Theme"));
            SetNewBackgroundMusic(musicname);
        }
    }
    MBDebugStr("(INFO) OnStart (%d)\n", nTracePoint++);
    if (sMfPrimaryActor) {
        MBComPtr<ISBGameState> gs(MBIID_CSBGameState);
        gs->SetProperty("hc_remote_control", 0L);
    }
    MBDebugStr("(INFO) Exit OnStart\n");
}

// 0x100a59a0 ?VerifyCD@SteamboatNode@@QAEXXZ
void SteamboatNode::VerifyCD()
{
    // The CD is in the drive when the background of the book opens.
    MBString16 title;
    MBString16 msg;
    IMBStream *testStream;
    AddDatabase("bk");
    GetStream(MBHashString("BK_window"), 'anm ', testStream);
    RemoveDatabase("bk");
    if (testStream)
        testStream->Release();
    else {
        GetNamedString("NO_CD", msg);
        GetNamedString("ERROR", title);
        MBMsgBox(msg, 9, title);
        exit(0);
    }
}

// 0x100a5b60 ?OpenPrefStream@SteamboatNode@@MAEPAVIMBStream@@XZ
IMBStream *SteamboatNode::OpenPrefStream()
{
    return OpenPrefStream(mSpaceCode + "scene.ini");
}

// 0x100a5c30 ?OpenPrefStream@SteamboatNode@@MAEPAVIMBStream@@ABV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@Z
IMBStream *SteamboatNode::OpenPrefStream(const MBString &filename)
{
    IMBStream *stream = 0;
    MBComPtr<IMBFileSpec> fileSpec(MBIID_IMBFileSpec);
    if (fileSpec) {
        fileSpec->SetPath(filename.c_str());
        MBOpenFile(fileSpec, 5, &stream);
    }
    return stream;
}

// where the scene files of Fly live (read from steamboat.ini)
MBString sFlyDir(".");

// 0x100a5d40 ?ReadPrefs@SteamboatNode@@MAEXXZ
void SteamboatNode::ReadPrefs()
{
    // steamboat.ini: install and CD directories, volumes, locale, the actor abbreviations
    MBComPtr<ISBGameState> gs(MBIID_CSBGameState);
    MBComPtr<IMBFileSpec> fileSpec(MBIID_IMBFileSpec);
    if (fileSpec) {
        fileSpec->SetPath("steamboat.ini");
        MBComPtr<IMBStream> stream;
        IMBStream *tempStream;
        MBOpenFile(fileSpec, 5, &tempStream);
        stream.Attach(tempStream);
        if (stream) {
            IMBFileSpec *cwd;
            MBGetCurrentDirectory(&cwd);
            MBComPtr<IMBPref> pref(MBIID_CMBPref);
            MBString installDir;
            MBString key("directories");
            pref->GetString(stream, key, MBString("hd"), installDir, cwd->GetPath());
            MBString cdDir;
            pref->GetString(stream, key, MBString("cd"), cdDir, cwd->GetPath());
            MBString tmpString;
            pref->GetString(stream, key, MBString("fly"), tmpString, MBString("fly"));
            sFlyDir = installDir + tmpString;
            pref->GetString(stream, key, MBString("mofa"), tmpString, MBString("mofa"));
            sMofaDir = installDir + tmpString;
            MBString muffinResources;
            pref->GetString(stream, key, MBString("muffin"), tmpString, MBString("resources"));
            muffinResources = installDir + tmpString;
            MBString cdResources;
            cdResources = cdDir + tmpString;
            key = MBString("sound");
            sSfxVolume = (short)pref->GetLong(stream, key, MBString("sfx volume"), sSfxVolume);
            sDialogVolume = (short)pref->GetLong(stream, key, MBString("dialog volume"), sDialogVolume);
            sMusicVolume = (short)pref->GetLong(stream, key, MBString("music volume"), sMusicVolume);
            SetResourceDirectories(cdResources, muffinResources);
            cwd->Release();
            pref->GetString(stream, MBString("i18n"), MBString("locale"), sLocale, MBString("eng_usa"));
            gs->LoadInventoryItemFromPref(stream, pref);
            gs->LoadRecycleItemsFromPref(stream, pref);
            int keyval = 0;
            char s[80];
            sprintf(s, "key%d", keyval);
            MBString abbrevString;
            while (pref->GetString(stream, MBString("AbbreviationMap"), MBString(s), abbrevString, MBString(""))) {
                MBString nameString;
                pref->GetString(stream, MBString("AbbreviationMap"), abbrevString, nameString, MBString(""));
                SBActorManager::GetInstance()->AddToAbbreviationMap(abbrevString, nameString);
                keyval++;
                sprintf(s, "key%d", keyval);
            }
        }
    }
}

// EQUIV 0x100a6f80 ?CreateDisplay@SteamboatNode@@MAEXXZ: reviewed: 54% similar; DisplayD3D::SetFrontBuffer/
// SetBackBuffer(IDirectDrawSurface7 *) are inline stores in the original (m_FrontBuffer at 0xa8,
// m_BackBuffer at 0xac; no out-of-line copies; G's DXGfx_DisplayD3D.h declares them only). Otherwise
// inline budget and registers: the original keeps 0 in esi and MBDebugStr in ebx, calls the string
// destructors (_Tidy) out of line where VC6 inlines them and inlines pref's Release
void SteamboatNode::CreateDisplay()
{
    // the 3D display on the engine's DirectX screen; prints what the card can do
    IMBScreenManager *screenManager = 0;
    MBGetScreenManager(&screenManager);
    if (screenManager) {
        MBDebugStr("      have a screenManager\n");
        IMBScreen *primaryScreen = 0;
        screenManager->GetScreenByID(1, &primaryScreen);
        if (primaryScreen) {
            MBDebugStr("      have a primary screen\n");
            IMBWinScreen *winScreen = 0;
            IMBDirectXScreen *directXScreen = 0;
            primaryScreen->QueryInterface(MBIID_IMBWinScreen, (void **)&winScreen);
            primaryScreen->QueryInterface(MBIID_IMBDirectXScreen, (void **)&directXScreen);
            if (directXScreen) {
                MBDebugStr("      have a directXScreen\n");
                DXGfx::DisplayD3D::Create(winScreen->GetHWND(), directXScreen->GetDirectDraw7(), directXScreen->GetDeviceGUID());
                if (!Gfx::Display::GetInstance()) {
                    MBDebugStr("      DisplayD3D::Create failed\n");
                    return;
                }
                MBDebugStr("      displayd3d create finished\n");
                DXGfx::DisplayD3D *display = (DXGfx::DisplayD3D *)Gfx::Display::GetInstance();
                MBDebugStr("      getting surface desc\n");
                DDSURFACEDESC2 ddsd2;
                memset(&ddsd2, 0, sizeof(ddsd2));
                ddsd2.dwSize = sizeof(ddsd2);
                MBDebugStr("      done getting backbuffer surfacedesc, hresult is %d\n",
                           directXScreen->GetBackBuffer()->GetSurfaceDesc(&ddsd2));
                bool isEmulation = false;
                if (ddsd2.ddsCaps.dwCaps & DDSCAPS_SYSTEMMEMORY)
                    isEmulation = true;
                MBComPtr<IMBFileSpec> fileSpec(MBIID_IMBFileSpec);
                if (fileSpec) {
                    // engine.ini can force the software renderer
                    MBComPtr<IMBStream> stream;
                    fileSpec->SetPath("engine.ini");
                    IMBStream *tempStream;
                    MBOpenFile(fileSpec, 5, &tempStream);
                    stream.Attach(tempStream);
                    if (stream) {
                        MBComPtr<IMBPref> pref(MBIID_CMBPref);
                        MBString key("ForceSoftware");
                        if (pref->GetLong(stream, MBString("Screen"), key, 0))
                            isEmulation = true;
                    }
                }
                try {
                    Gfx::SurfaceDesc surfaceDesc;
                    memset(&surfaceDesc, 0, sizeof(surfaceDesc));
                    surfaceDesc.width = 640;
                    surfaceDesc.height = 480;
                    surfaceDesc.depth = 16;
                    surfaceDesc.backBufferCount = 1;
                    surfaceDesc.systemMemory = isEmulation;
                    surfaceDesc.flip = true;
                    surfaceDesc.is3D = true;
                    display->SetSurfaceProperties(surfaceDesc);
                    display->SetEmulation(isEmulation);
                    display->SetFullScreen(directXScreen->IsFullscreen());
                    MBDebugStr("      setting buffers\n");
                    display->SetFrontBuffer(directXScreen->GetFrontBuffer());
                    display->SetBackBuffer(directXScreen->GetBackBuffer());
                    MBDebugStr("      initializing the display\n");
                    display->Initialize();
                    MBDebugStr("      done initializing, releasing directXScreen\n");
                    directXScreen->Release();
                    DX::Direct3D *d3d = display->GetDirect3D();
                    DX::D3DDriverInfo d3dinfo = d3d->DriverInfo();
                    DX::DDDeviceInfo ddinfo = d3d->GetCurrentDevice();
                    MBDebugStr("INFO: 3D Device selected:\n");
                    MBDebugStr("      Card       : %s\n", ddinfo.strDriverDesc);
                    MBDebugStr("      Driver     : %s\n", ddinfo.strDriverName);
                    MBDebugStr("      3D device  : %s\n", d3dinfo.strName);
                    MBDebugStr("                   %s\n", d3dinfo.strAbout);
                    MBDebugStr("      Device GUID: %08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X\n\n",
                               d3dinfo.guid.Data1, d3dinfo.guid.Data2, d3dinfo.guid.Data3,
                               d3dinfo.guid.Data4[0], d3dinfo.guid.Data4[1], d3dinfo.guid.Data4[2],
                               d3dinfo.guid.Data4[3], d3dinfo.guid.Data4[4], d3dinfo.guid.Data4[5],
                               d3dinfo.guid.Data4[6], d3dinfo.guid.Data4[7]);
                    MBDebugStr("      Video RAM  : %d bytes (%d MB)\n", d3d->GetTotalVideoMemory(),
                               d3d->GetTotalVideoMemory() / (1024 * 1024));
                    MBDebugStr("      AGP RAM    : %d bytes (%d MB)\n", d3d->GetTotalNonLocalVidMemory(d3dinfo),
                               d3d->GetTotalNonLocalVidMemory(d3dinfo) / (1024 * 1024));
                    MBDebugStr("      Texture RAM: %d bytes (%d MB)\n\n", d3d->GetTotalTextureMemory(d3dinfo),
                               d3d->GetTotalTextureMemory(d3dinfo) / (1024 * 1024));
                    MBDebugStr("      3D accelerated: %s\n", d3dinfo.bIsHardware ? "yes" : "no");
                    MBDebugStr("      Running in software: %s\n", display->IsEmulation() ? "yes" : "no");
                    MBDebugStr("      Compatible w/ desktop: %s\n", d3dinfo.bCompatibleWithDesktop ? "yes" : "no");
                    MBDebugStr("      Can render windowed: %s\n", d3dinfo.bCanDoWindow ? "yes" : "no");
                    MBDebugStr("      Can texture: %s\n", d3dinfo.bDoesTextures ? "yes" : "no");
                    MBDebugStr("      Can Z-buffer: %s\n", d3dinfo.bDoesZBuffer ? "yes" : "no");
                    MBDebugStr("      Can texture from AGP: %s\n", d3dinfo.bTextureNonLocalVidMem ? "yes" : "no");
                    MBDebugStr("      Can multitexture: %s\n", d3dinfo.bCanDoMultiTexture ? "yes" : "no");
                    MBDebugStr("      Has enough texture mem: %s\n", d3dinfo.bHasSufficientTextureMem ? "yes" : "no");
                } catch (DX::Exception &e) {
                    MBDebugStr("FATAL ERROR: DX::Exception thrown during display initialization.\n");
                    MBDebugStr("             %s\n", e.GetDetails());
                    throw e;
                }
            }
            winScreen->Release();
            primaryScreen->Release();
        }
        screenManager->Release();
    }
}

// 0x100a7a90 ?OnEnd@SteamboatNode@@UAEXXZ
void SteamboatNode::OnEnd()
{
    SaveGame();
    MBComPtr<IMBSoundManager> soundManager(MBIID_IMBSoundManager);
    if (soundManager) {
        // the scene's own sound channels
        for (long channel = 30; channel <= 39; channel++)
            soundManager->StopChannel(channel);
    }
    ClearHotspots();
    mCursor->Hide();
    bool noSceneMusic = sBackgroundMusicName == MBString("");
    if (noSceneMusic && sBackgroundMusic) {
        AIL_close_stream(sBackgroundMusic);
        sBackgroundMusic = 0;
    }
    if (mShowGui) {
        MBComPtr<ISBInventory> inventory(MBIID_ISBInventory);
        inventory->Show(false);
    }
    if (mEngine->IsQuitting())
        OnQuit();
    sCurrentActiveNode = this;
    ScriptHelpNode::OnEnd();
}

// 0x100a7d10 ?OnPause@SteamboatNode@@UAEXXZ
void SteamboatNode::OnPause()
{
    if (mShowGui) {
        MBComPtr<ISBInventory> inventory(MBIID_ISBInventory);
        inventory->Show(false);
    }
    ScriptHelpNode::OnPause();
}

// 0x100a7e20 ?OnResume@SteamboatNode@@UAEXXZ
void SteamboatNode::OnResume()
{
    if (mShowGui) {
        MBComPtr<ISBInventory> inventory(MBIID_ISBInventory);
        inventory->Show(true);
    }
    ScriptHelpNode::OnResume();
}

// OnPreRender prints its own source line when it switches the hotspots, so those two
// statements sit on the original lines 1373 and 1384; the gap held code not reconstructed yet.


















































































































































































































































const MBColorMask kCM_ARGB32(0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);

static bool sDrawDirtyRects;
static MBVector<tagRECT> sDirtyRectVector;
static const MBColor kYellow(255, 255, 0, 0);

// 0x100a7f30 ?OnPreRender@SteamboatNode@@UAEXV?$MBComPtr@VIMBScreen@@@@@Z
void SteamboatNode::OnPreRender(MBComPtr<IMBScreen> screen)
{
    if (sDrawDirtyRects) {
        // outline the dirty rectangles on the front buffer (windowed mode only)
        MBComPtr<IMBDirectXScreen> dxScreen(screen);
        if (dxScreen) {
            if (!dxScreen->IsFullscreen()) {
                IDirectDrawSurface7 *surface = dxScreen->GetFrontBuffer();
                if (surface) {
                    HDC hdc;
                    surface->GetDC(&hdc);
                    if (hdc) {
                        DrawDirtyRects(dxScreen->GetHWND(), hdc, kYellow);
                        surface->ReleaseDC(hdc);
                    }
                    surface->Release();
                }
            }
        }
    }
    if (sMfPrimaryActor) {
        SBDialoguePropertyManager::GetInstance()->UpdateProperties(sMfPrimaryActor);
        // no clicks while somebody talks
        bool talking = mDialogue.Update(sMfPrimaryActor);
        if (talking) {
            if (sInputEventChannelActive) {
                if (!mCharacterDeactivatedInputEventChannel) {
                    MBComPtr<IMBEventManager> eventManager(MBIID_IMBEventManager);
                    MBDebugStr("DisableHotspots: %s, line %d.\n", "D:\\tugboat\\code\\steamboat\\SteamboatNode.cpp", __LINE__);
                    EnableHotspots(false);
                }
                sInputEventChannelActive = false;
            }
        } else if (!sInputEventChannelActive) {
            MBComPtr<IMBEventManager> eventManager(MBIID_IMBEventManager);




            MBDebugStr("EnableHotspotsWithDelay: %s, line %d.\n", "D:\\tugboat\\code\\steamboat\\SteamboatNode.cpp", __LINE__);
            EnableHotspotsWithDelay();
            sInputEventChannelActive = true;
            mCharacterDeactivatedInputEventChannel = true;
        }
    }
    SBMotivateDriver::GetInstance()->AcquireLock();
    SBMotivateDriver::GetInstance()->Update();
    SBMotivateDriver::GetInstance()->ReleaseLock();
    if (sAppInForeground) {
        if (sUseLayered3DRender)
            RenderBackground();
        else
            RenderScene();
    }
    PlaceCursor();
}

// 0x100a8240 ?OnPostRender@SteamboatNode@@UAEXV?$MBComPtr@VIMBScreen@@@@@Z
void SteamboatNode::OnPostRender(MBComPtr<IMBScreen> screen)
{
    if (sDisplayFrameRate)
        CalculateFrameRate();
    if (++sBuffer > 1)
        sBuffer = 0;
    if (sDrawDirtyRects) {
        sDirtyRectVector.clear();
        RenderManager::GetInstance()->GetBitmapBuffer(0)->BuildDirtyRectList(sDirtyRectVector);
        MBComPtr<IMBDirectXScreen> dxScreen(screen);
        if (dxScreen) {
            if (!dxScreen->IsFullscreen()) {
                IDirectDrawSurface7 *surface;
                if (dxScreen->IsFullscreen())
                    surface = dxScreen->GetBackBuffer();
                else
                    surface = dxScreen->GetFrontBuffer();
                if (surface) {
                    HDC hdc;
                    surface->GetDC(&hdc);
                    if (hdc) {
                        DrawDirtyRects(dxScreen->GetHWND(), hdc, kYellow);
                        surface->ReleaseDC(hdc);
                    }
                    surface->Release();
                }
            }
        }
    }
    if (sRenderFlags & 8) {
        RenderManager::GetInstance()->GetBitmapBuffer(0)->SwapScreenRects();
        AddAllObjectDirtyRects();
    }
}

// 0x100a8530 ?OnKeyDown@SteamboatNode@@UAEXAAUMBKeyboardEvent@@@Z
void SteamboatNode::OnKeyDown(MBKeyboardEvent &key)
{
    SimpleNode::OnKeyDown(key);
}

// 0x100a8550 ?OnKeyUp@SteamboatNode@@UAEXAAUMBKeyboardEvent@@@Z
void SteamboatNode::OnKeyUp(MBKeyboardEvent &keyEvent)
{
    SimpleNode::OnKeyUp(keyEvent);
}

// 0x100a8570 ?OnLMouseDown@SteamboatNode@@UAEXAAUMBMouseEvent@@@Z
void SteamboatNode::OnLMouseDown(MBMouseEvent &evt)
{
    SimpleNode::OnLMouseDown(evt);
}

// 0x100a8590 ?OnLMouseUp@SteamboatNode@@UAEXAAUMBMouseEvent@@@Z
void SteamboatNode::OnLMouseUp(MBMouseEvent &evt)
{
    SimpleNode::OnLMouseUp(evt);
}

// On3dLMouseUp prints its own source line when it disables the hotspots, so those statements
// sit on the original lines 1631 and 1648; the gap held code not reconstructed yet.

























































































































// 0x100a85b0 ?On3dLMouseUp@SteamboatNode@@UAEXXZ
void SteamboatNode::On3dLMouseUp()
{
    // a click into the 3D scene: give the held item to the actor hit, or pick up a recyclable
    MBMouseEvent evt;
    if (MBGetScreenMouseLocation(evt.mMousePos, &evt.mScreen)) {
        MBComPtr<ISBInventory> gui(MBIID_ISBInventory);
        if (!gui->IsPointOnMenu(evt.mMousePos)) {
            MfPickData pickData;
            MfMouseHookData data;
            MBComPtr<ISBGameState> gs(MBIID_ISBGameState);
            CopyMBMouseEventToMfMouseHookData(evt, data);
            MfAiActor *actor = sMotivateEventHandler->HandleLButtonUp(&data, pickData);
            if (actor) {
                if (sCursorItem) {
                    int cursorInt = 0;
                    MBString mfActorName = (char *)actor->GetClassNameA();
                    bool receiveItem = false;
                    MfProperty receiveItemProperty;
                    if (actor->GetProperty(sCursorItem->GetClassNameA().c_str(), receiveItemProperty)) {
                        receiveItem = true;
                        MfIntValue iValue(1);
                        MfProperty pValue(sCursorItem->GetClassNameA().c_str(), iValue.Copy());
                        actor->SetProperty(pValue);
                    }
                    gs->SetProperty(sCursorItem->GetClassNameA().c_str(), 1L);
                    gs->SetProperty("HC_lastCursorItem", sCursorItem->GetClassNameA().c_str());
                    CursorItemStringToInt(sCursorItem->GetClassNameA(), cursorInt);
                    gs->SetProperty("HC_lastCursorItemInt", cursorInt);
                    if (receiveItem && sCursorItem->GetGiveTo() == mfActorName) {
                        sCursorItem->SetOnCharacter(false);
                        RemoveItemFromInventory(MBString(sCursorItem->GetInstanceName()));
                    }
                    ResetCursor();
                } else if (sMfPrimaryActor) {
                    int cursorInt = 0;
                    gs->SetProperty("HC_lastCursorItem", "none");
                    CursorItemStringToInt("none", cursorInt);
                    gs->SetProperty("HC_lastCursorItemInt", cursorInt);
                    MBString className = actor->GetClassNameA();
                    MBString recyclableStr = mSpaceCode + "can";
                    if (className.find(recyclableStr.c_str()) != -1) {
                        // a recyclable can: the dialogue says so, the hotspots wait for the pick-up
                        MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
                        broadcaster->BroadcastEvent(700, 712, "GDRecyclable");
                        mWaitingToPickUpItem = true;
                        MBDebugStr("DisableHotspots: %s, line %d.\n", "D:\\tugboat\\code\\steamboat\\SteamboatNode.cpp", __LINE__);
                        EnableHotspots(false);
                    }
                }
                if (sMfPrimaryActor) {
                    // Somebody else than the player's character is talking: no clicks until the
                    // line is done. The talking actor's script sets HCTalking for the length of
                    // the line; the hotspots come back on when the dialogue ends (OnPreRender
                    // and Idle switch them on again). The player's own character (sMfPrimaryActor)
                    // and a click with an item in the hand (handled above, the item goes first)
                    // never block the input.
                    //
                    //
                    //
                    MfProperty actorTalkingProperty;
                    if (sInputEventChannelActive && sMfPrimaryActor != actor &&
                        actor->GetProperty("HCTalking", actorTalkingProperty) && (int)actorTalkingProperty) {
                        MBDebugStr("DisableHotspots: %s, line %d.\n", "D:\\tugboat\\code\\steamboat\\SteamboatNode.cpp", __LINE__);
                        EnableHotspots(false);
                    }
                }
                actor->HandleLButtonUp(&pickData);
            }
        }
        if (evt.mScreen)
            evt.mScreen->Release();
    }
}

// 0x100a8ed0 ?CopyMBMouseEventToMfMouseHookData@SteamboatNode@@IAEXAAUMBMouseEvent@@AAUMfMouseHookData@@@Z
void SteamboatNode::CopyMBMouseEventToMfMouseHookData(MBMouseEvent &evt, MfMouseHookData &data)
{
    // Motivate wants the position as a fraction of the screen
    data.eventView = 0;
    data.x = (float)evt.mMousePos.x / evt.mScreen->GetWidth();
    data.y = (float)evt.mMousePos.y / evt.mScreen->GetHeight();
}

// 0x100a8f50 ?OnLMouseDblClk@SteamboatNode@@UAEXAAUMBMouseEvent@@@Z
void SteamboatNode::OnLMouseDblClk(MBMouseEvent &evt)
{
    SimpleNode::OnLMouseDblClk(evt);
}

// 0x100a8f70 ?OnMMouseDown@SteamboatNode@@UAEXAAUMBMouseEvent@@@Z
void SteamboatNode::OnMMouseDown(MBMouseEvent &evt)
{
    SimpleNode::OnMMouseDown(evt);
}

// 0x100a8f90 ?OnMMouseUp@SteamboatNode@@UAEXAAUMBMouseEvent@@@Z
void SteamboatNode::OnMMouseUp(MBMouseEvent &evt)
{
    SimpleNode::OnMMouseUp(evt);
}

// 0x100a8fb0 ?OnMMouseDblClk@SteamboatNode@@UAEXAAUMBMouseEvent@@@Z
void SteamboatNode::OnMMouseDblClk(MBMouseEvent &evt)
{
    SimpleNode::OnMMouseDblClk(evt);
}

// 0x100a8fd0 ?OnRMouseDown@SteamboatNode@@UAEXAAUMBMouseEvent@@@Z
void SteamboatNode::OnRMouseDown(MBMouseEvent &evt)
{
    SimpleNode::OnRMouseDown(evt);
}

// 0x100a8ff0 ?OnRMouseUp@SteamboatNode@@UAEXAAUMBMouseEvent@@@Z
void SteamboatNode::OnRMouseUp(MBMouseEvent &evt)
{
    SimpleNode::OnRMouseUp(evt);
}

// 0x100a9010 ?OnRMouseDblClk@SteamboatNode@@UAEXAAUMBMouseEvent@@@Z
void SteamboatNode::OnRMouseDblClk(MBMouseEvent &evt)
{
    SimpleNode::OnRMouseDblClk(evt);
}

// 0x100a9030 ?OnMouseMove@SteamboatNode@@UAEXAAUMBMouseEvent@@@Z
void SteamboatNode::OnMouseMove(MBMouseEvent &evt)
{
    mMousePos = evt.mMousePos;
}

// 0x100a9060 ?RenderScene@SteamboatNode@@MAEXXZ
void SteamboatNode::RenderScene()
{
    RenderBackground();
    RenderCharacters();
}

// 0x100a9150 ?RenderCharacters@SteamboatNode@@MAEXXZ
void SteamboatNode::RenderCharacters()
{
    try {
        RenderManager::GetInstance()->EndRenderView(0, sSceneRootNode, 0, sRenderFlags);
    } catch (DX::Exception &e) {
        MBDebugStr("DX::Exception caught: '%s'\n", e.GetErrorString());
        MBDebugStr("details: '%s'\n", e.GetDetails());
    } catch (Gfx::TextureException &e) {
        MBDebugStr("Gfx::TextureException caught: texture file '%s'\n", e.GetTextureAttribute().GetFilename());
    }
}

// 0x100a9090 ?RenderBackground@SteamboatNode@@MAEXXZ
void SteamboatNode::RenderBackground()
{
    if (sRenderFlags & RenderManager::cDirtyRects) {
        // the first two frames redraw the whole background, one per buffer
        if (sBackgroundDirty > 1) {
            AddAllObjectDirtyRects();
        } else {
            MBRect rect(0, 0, 640, 480);
            AddDirtyRect(rect);
            SBActorManager::GetInstance()->SetCurrentBuffer(sBuffer);
            sBackgroundDirty++;
        }
    }
    RenderManager::GetInstance()->BeginRenderView(0, sSceneRootNode, 0, sRenderFlags);
}

// 0x100a9230 ?StartListening@SteamboatNode@@MAEXXZ
void SteamboatNode::StartListening()
{
    MBComPtr<IMBEventManager> eventManager(MBIID_IMBEventManager);
    if (eventManager) {
        eventManager->AddListener(200, this);
        eventManager->AddListener(300, this);
        eventManager->AddListener(600, this);
        eventManager->AddListener(-9, this);
        eventManager->AddListener(700, this);
        eventManager->AddListener(110, this);
        eventManager->AddListener(120, this);
        eventManager->AddListener(130, this);
    }
    ScriptHelpNode::StartListening();
}

// 0x100a9400 ?StopListening@SteamboatNode@@MAEXXZ
void SteamboatNode::StopListening()
{
    MBComPtr<IMBEventManager> eventManager(MBIID_IMBEventManager);
    if (eventManager) {
        eventManager->RemoveListener(600, this);
        eventManager->RemoveListener(200, this);
        eventManager->RemoveListener(300, this);
        eventManager->RemoveListener(-9, this);
        eventManager->RemoveListener(700, this);
        eventManager->RemoveListener(110, this);
        eventManager->RemoveListener(120, this);
        eventManager->RemoveListener(130, this);
    }
    ScriptHelpNode::StopListening();
}

// 0x100a95d0 ?SetSpaceName@SteamboatNode@@MAEXXZ
void SteamboatNode::SetSpaceName()
{
    // a scene's code is the first two letters of its Motivate space file's name
    if (mSpaceCode != "MV" && mSpaceCode != "CP" && mSpaceCode != "HK" && mSpaceCode != "MP" && mSpaceCode != "CD" &&
        mSpaceCode != "TB" && mSpaceCode != "GL" && mSpaceCode != "LL" && mSpaceCode != "SB" && mSpaceCode != "SH" &&
        mSpaceCode != "SM" && mSpaceCode != "UI" && SBMotivateDriver::GetMotivateSpace()) {
        MfString spacename = SBMotivateDriver::GetMotivateSpace()->GetSpaceName();
        const char *name = MBstrrchr(spacename, '\\');
        if (!name)
            name = (const char *)spacename - 1;
        name++;
        char spaceCode[3];
        spaceCode[0] = name[0];
        spaceCode[1] = name[1];
        spaceCode[2] = 0;
        mSpaceCode = spaceCode;
        if (sMfPrimaryActor) {
            MBComPtr<ISBGameState> gs(MBIID_ISBGameState);
            gs->SetProperty("HC_spaceCode", mSpaceCode.c_str());
        }
    }
}

// The game's angle constant. The original's is a const double with a run-time initializer
// (SteamboatNode.obj's $E82) that SetView loads from memory; VC6 folds a const one into the code.
static const double kPI = 3.141592653589793;
static double RadiansToDegrees = 180.0 / kPI;

// SetView is not no-alias code: the original reloads mMfCameraNode after every call and calls
// map::find out of line; only with "a" off does VC6 do the same (tested).
#pragma optimize("a", off)
// TODO 0x100a9a00 ?SetView@SteamboatNode@@UAEXABVMfString@@@Z: 58% similar (with G's inline SetFOV,
// SetWorldMatrix and toMat); the original's inlined matrix code indexes with operator[] and calls clamp
// out of line for each element (toMat's mat(i, j) has no clamp; written out with [] here it was 27%),
// and keeps space in edi and this in esi (VC6: esi, ebp), which shifts every instruction. The fov and
// near-clip tests keep the value on the FPU in the original (probably inside the inline setters)
void SteamboatNode::SetView(const MfString &actorName)
{
    // the scene camera follows the camera node of the given Motivate actor
    MfAiSpace *space = SBMotivateDriver::GetMotivateSpace();
    if (space) {
        mMfCameraNode = space->GetActor(actorName)->GetNodeByName("camera");
        Gfx::Camera *camera = RenderManager::GetInstance()->GetViewCamera(0);
        if (mMfCameraNode) {
            float fov = atan(mMfCameraNode->GetCamera()->fov) * RadiansToDegrees * 2;
            if (fov >= 0.0f)
                camera->SetFOV(fov);
            MfTransform3 global(mMfCameraNode->GetGlobalTransform());
            TMatrix4<float> worldMatrix(camera->GetWorldMatrix());
            worldMatrix[3][0] = global.translation().x();
            worldMatrix[3][1] = global.translation().y();
            worldMatrix[3][2] = global.translation().z();
            mCameraDir[0] = global.rotation().Axis().x();
            mCameraDir[1] = global.rotation().Axis().y();
            mCameraDir[2] = global.rotation().Axis().z();
            mCameraAngle = global.rotation().Angle();
            TMatrix4<float> newWorldMatrix;
            Quaternion<float> quat(mCameraDir, mCameraAngle);
            quat.toMat(newWorldMatrix);
            newWorldMatrix[3][0] = worldMatrix[3][0];
            newWorldMatrix[3][1] = worldMatrix[3][1];
            newWorldMatrix[3][2] = worldMatrix[3][2];
            camera->SetWorldMatrix(newWorldMatrix);
            float nearClip = mMfCameraNode->GetCamera()->nearClip;
            if (nearClip > 0.0f)
                camera->SetNearClip(nearClip);
            camera->SetFarClip(mMfCameraNode->GetCamera()->farClip);
        }
    }
}
#pragma optimize("a", on)

// UserEventHook prints its own source line when it switches the hotspots, so those statements
// sit on the original lines 2056, 2061 and 2066 (written with the original's brace style there).

































































// EQUIV 0x100aa170 ?UserEventHook@SteamboatNode@@UAEXPAUMfUserHookData@@@Z: reviewed: 82% similar; inline budget
// only: the original inlines both MBString constructors of `new SBMouseEvent` (VC6 calls the first),
// the _Eos of hc_hotspot_true's string, and builds hc_hotspot_false's string with an inline _Tidy and
// an out-of-line assign; the frame slot of the new-expression differs with it
void SteamboatNode::UserEventHook(MfUserHookData *hook)
{
    // the user events of the Motivate scripts
    MBComPtr<IMBSoundManager> soundManager(MBIID_IMBSoundManager);
    MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
    if (hook->event == "SetView")
    {
        SetView(hook->data);
    }
    else if (hook->event == "HCPlayDialogue")
    {
        broadcaster->BroadcastEvent(700, 712, (char *)hook->data);
    }
    else if (hook->event == "hc_play_line")
    {
        broadcaster->BroadcastEvent(700, 701, (char *)hook->data);
    }
    else if (hook->event == "HCPlaySound")
    {
        PlaySfx(hook->data);
    }
    else if (hook->event == "HCKillSound")
    {
        if (soundManager)
        {
            for (long channel = 30; channel <= 39; channel++)
                soundManager->StopChannel(channel);
        }
    }
    else if (hook->event == "HCPauseSfxChannel")
    {
        if (soundManager)
        {
            for (long channel = 30; channel <= 39; channel++)
                soundManager->PauseChannel(channel);
        }
    }
    else if (hook->event == "HCResumeSfxChannel")
    {
        if (soundManager)
        {
            for (long channel = 30; channel <= 39; channel++)
                soundManager->ResumeChannel(channel);
        }
    }
    else if (hook->event == "HCPlaySoundForever")
    {
        long channel = FindFreeChannel(30, 39);
        MBForceSound((char *)hook->data, channel, -1);
        soundManager->SetChannelVolume(channel, sSfxVolume);
    }
    else if (hook->event == "PlayBackgroundMusic")
    {
        SetNewBackgroundMusic((const char *)hook->data);
        SetBackgroundMusicState(true);
    }
    else if (hook->event == "hc_item_picked")
    {
        {
            MBString item = (char *)hook->data;
            sInventoryItemPicked = item;
        }
        AddItemToInventory(sInventoryItemPicked, true);
    }
    else if (hook->event == "HCLeaveScene")
    {
        SBMouseEvent *e = new SBMouseEvent;
        e->data = (char *)hook->data;
        e->action = "exit";
        broadcaster->BroadcastEvent(200, 201, e);
        delete e;
    }
    else if (hook->event == "hc_reset_player")
    {
        sMfPrimaryActor = 0;
    }
    else if (hook->event == "hc_remove_item")
    {
        MBComPtr<ISBInventory> gui(MBIID_ISBInventory);
        MBString instanceName = (char *)hook->data;
        RemoveItemFromInventory(instanceName);
    }
    else if (hook->event == "hc_change_head")
    {
        broadcaster->BroadcastEvent(700, 713, (char *)hook->data);
    }
    else if (hook->event == "hc_set_cursor")
    {
        MBString instanceName = (char *)hook->data;
        SetCursorItem(MBComPtr<ISBGameState>(MBIID_ISBGameState)->GetItem(instanceName));
    }
    else if (hook->event == "hc_toggle_hotspot")
    {
        MBString instanceName = (char *)hook->data;
        SBItem *item = MBComPtr<ISBGameState>(MBIID_ISBGameState)->GetItem(instanceName);
        if (item)
        {
            IMBScreenPoly *hotspot = item->GetHotspot();
            if (hotspot)
            {
                if (hotspot->IsActive())
                    hotspot->Deactivate();
                else
                    hotspot->Activate();
            }
        }
    }
    else if (hook->event == "hc_hotspot_true")
    {
        MBString instanceName = (char *)hook->data;
        SBItem *item = MBComPtr<ISBGameState>(MBIID_ISBGameState)->GetItem(instanceName);
        if (item && item->GetHotspot())
            item->GetHotspot()->Activate();
    }
    else if (hook->event == "hc_hotspot_false")
    {
        MBString instanceName = (char *)hook->data;
        SBItem *item = MBComPtr<ISBGameState>(MBIID_ISBGameState)->GetItem(instanceName);
        if (item && item->GetHotspot())
            item->GetHotspot()->Deactivate();
    }
    else if (hook->event == "hc_enable_exit_hotspots")
    {
        mWaitingToPickUpItem = false;
        MBDebugStr("UserEventHook calling EnableHotspots: %s, line %d.\n", "D:\\tugboat\\code\\steamboat\\SteamboatNode.cpp", __LINE__);
        EnableHotspots(true);
    }
    else if (hook->event == "hc_enable_hotspots_delayed")
    {
        MBDebugStr("UserEventHook calling EnableHotspotsWithDelay: %s, line %d.\n", "D:\\tugboat\\code\\steamboat\\SteamboatNode.cpp", __LINE__);
        EnableHotspotsWithDelay();
    }
    else if (hook->event == "hc_disable_exit_hotspots")
    {
        MBDebugStr("UserEventHook calling DisableHotspots: %s, line %d.\n", "D:\\tugboat\\code\\steamboat\\SteamboatNode.cpp", __LINE__);
        EnableHotspots(false);
    }
}

// Idle prints its own source line when it switches the hotspots back on, so that statement
// statements sit on the original lines 2086; the gap held code not reconstructed yet.

// 0x100aaee0 ?Idle@SteamboatNode@@UAEXXZ
void SteamboatNode::Idle()
{
    // The hotspots come back two seconds after EnableHotspotsWithDelay.
    unsigned long delayStart = mEnableHotspotsDelayTimer;
    if (delayStart > 0) {
        MBComPtr<IMBGameClock> clock(MBIID_IMBGameClock);
        unsigned long now = clock->GetGameTime();
        if (now < delayStart)
            mEnableHotspotsDelayTimer = 1;
        if (now - mEnableHotspotsDelayTimer >= 2000) {
            mWaitingToPickUpItem = false;
            MBDebugStr("EnableHotspots: %s, line %d.\n", "D:\\tugboat\\code\\steamboat\\SteamboatNode.cpp", __LINE__);
            EnableHotspots(true);
            mEnableHotspotsDelayTimer = 0;
        }
    }
    SimpleNode::Idle();
}

// 0x100ab050 ?AddItemToInventory@SteamboatNode@@QAEXAAV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@_N@Z
void SteamboatNode::AddItemToInventory(MBString &itemInstanceName, bool bPopUp)
{
    MBComPtr<ISBInventory> inventory(MBIID_ISBInventory);
    if (itemInstanceName == "PRbird0") {
        // The bird replaces the whistle.
        MBString whistle("PRwhistle0");
        RemoveItemFromInventory(whistle);
    }
    inventory->AddItem(itemInstanceName, bPopUp);
}

// 0x100ab1e0 ?RemoveItemFromInventory@SteamboatNode@@QAEXAAV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@Z
void SteamboatNode::RemoveItemFromInventory(MBString &itemInstanceName)
{
    MBComPtr<ISBInventory> inventory(MBIID_ISBInventory);
    MBComPtr<ISBGameState> gs(MBIID_CSBGameState);
    if (itemInstanceName == "PRbow0" && gs->GetProperty("HCMickeyCaptive", 0))
        itemInstanceName = "PRbutton0";
    inventory->RemoveItem(itemInstanceName);
}

// 0x100ab3b0 ?KillSound@SteamboatNode@@UAEXJ@Z
void SteamboatNode::KillSound(const long channelID)
{
    MBComPtr<IMBSoundManager> soundManager(MBIID_IMBSoundManager);
    if (soundManager)
        soundManager->StopChannel(channelID);
}

// TODO 0x100ab480 ?PlaySfx@SteamboatNode@@UAEXPBD@Z: (81% similar) as KillSound
void SteamboatNode::PlaySfx(const char *name)
{
    MBComPtr<IMBSoundManager> soundManager(MBIID_IMBSoundManager);
    long channel = FindFreeChannel(30, 39);
    MBForceSound(name, channel, 1);
    soundManager->SetChannelVolume(channel, sSfxVolume);
}

// EQUIV 0x100ab570 ?PlayMovie@SteamboatNode@@UAE?AV?$MBComPtr@VIMBMovie@@@@PBDK@Z: reviewed: 66% similar; the original keeps one more stack slot (the created IMBUnknown apart from the name argument) and flags in memory, ebp holding the id
MBComPtr<IMBMovie> SteamboatNode::PlayMovie(const char *name, unsigned long flags)
{
    // a Smacker movie of that name, else a Bink one
    MBComPtr<IMBMovie> movie;
    unsigned long id = MBHashString(name);
    IMBStream *stream;
    if (GetStream(id, 'smk ', stream) == 0) {
        movie.CreateObject(MBIID_CMBSmackerMovie);
        if (movie) {
            if (movie->Load(id, stream, flags)) {
                movie->Play();
                MBComPtr<IMBObject> object(movie);
                mWorld->GetRoot().AddObject(object);
            }
        }
        if (stream) {
            stream->Release();
            stream = 0;
        }
    }
    if (GetStream(id, 'bik ', stream) == 0) {
        movie.CreateObject(MBIID_CMBBinkMovie);
        if (movie) {
            if (movie->Load(id, stream, flags)) {
                movie->Play();
                MBComPtr<IMBObject> object(movie);
                mWorld->GetRoot().AddObject(object);
            }
        }
        if (stream) {
            stream->Release();
            stream = 0;
        }
    }
    return movie;
}

// 0x100ab820 ?StartDialogue@SteamboatNode@@UAEXPBD_N@Z
void SteamboatNode::StartDialogue(const char *line, bool force)
{
    MBComPtr<IMBSoundManager> soundManager(MBIID_IMBSoundManager);
    if (force || !soundManager->IsChannelPlaying(20)) {
        MBComPtr<IMBEventManager> eventManager(MBIID_IMBEventManager);
        eventManager->BroadcastEvent(700, 701, (void *)line);
    }
}

// 0x100ab960 ?SetMfPrimaryActor@SteamboatNode@@UAEXVMfString@@@Z
void SteamboatNode::SetMfPrimaryActor(const MfString mfActorName)
{
    if (SBMotivateDriver::GetMotivateSpace())
        sMfPrimaryActor = SBMotivateDriver::GetMotivateSpace()->GetActor(mfActorName);
}

// 0x100ab9e0 ?GetMfPrimaryActor@SteamboatNode@@UAEPAVMfAiActor@@XZ
MfAiActor *SteamboatNode::GetMfPrimaryActor()
{
    return sMfPrimaryActor;
}

// TODO 0x100aba00 ?LoadItems@SteamboatNode@@MAEXXZ: 93% similar; the original gives stream the lower stack slot than fileSpec (here the other way round) and is 2 bytes longer
void SteamboatNode::LoadItems()
{
    // scene.ini lists the scene's items (item0, item1, ...); each one is loaded as a Motivate
    // actor where the floor has a node of its name
    MfAiSpace *space = SBMotivateDriver::GetMotivateSpace();
    if (space) {
        MBComPtr<IMBFileSpec> fileSpec(MBIID_IMBFileSpec);
        if (fileSpec) {
            MBString prefFile = mSpaceCode + "scene.ini";
            fileSpec->SetPath(prefFile.c_str());
            MBComPtr<IMBStream> stream;
            IMBStream *tempStream;
            MBOpenFile(fileSpec, 5, &tempStream);
            stream.Attach(tempStream);
            if (stream) {
                MBComPtr<IMBPref> pref(MBIID_CMBPref);
                MBString itemName;
                char key[16];
                int item = 0;
                sprintf(key, "item%d", item);
                while (pref->GetString(stream, MBString("Items"), MBString(key), itemName, MBString("")) > 0) {
                    MfAiWorld *world = space->GetWorld();
                    MBString floorName = mSpaceCode + "floor0";
                    MfAiActor *floor = space->GetActor(MfString(floorName.c_str()));
                    MfAiActorNode *node = floor->GetNodeByName(MfString(itemName.c_str()));
                    if (node) {
                        MfTransform3 itemTransform = node->GetGlobalTransform();
                        MfAiActor *actor = world->LoadActor(MfString(itemName.c_str()), space, itemTransform, 1);
                        if (actor)
                            SBActorManager::GetInstance()->LoadActor(itemName + "0", actor);
                    }
                    sprintf(key, "item%d", ++item);
                }
            }
        }
    }
}

// 0x100abfd0 ?LoadDialogue@SteamboatNode@@MAEXXZ
void SteamboatNode::LoadDialogue()
{
    MBComPtr<IMBFileSpec> fileSpec(MBIID_IMBFileSpec);
    if (fileSpec) {
        MBString prefFile = mSpaceCode + "dialogue.ini";
        fileSpec->SetPath(prefFile.c_str());
        MBComPtr<IMBPref> pref(MBIID_CMBPref);
        MBComPtr<IMBStream> stream;
        IMBStream *tempStream;
        MBOpenFile(fileSpec, 5, &tempStream);
        stream.Attach(tempStream);
        if (stream) {
            SBDialoguePropertyManager::GetInstance()->LoadFromPref(stream, pref);
            mDialogue.LoadFromPref(stream, pref);
        }
    }
}

// 0x100ac240 ?LoadMiscellaneous@SteamboatNode@@MAEXXZ
void SteamboatNode::LoadMiscellaneous()
{
    // the scene's ambient sound loops on channel 10
    MBComPtr<ISBGameState> gameState(MBIID_CSBGameState);
    MBComPtr<IMBSoundManager> soundManager(MBIID_IMBSoundManager);
    MBComPtr<IMBStream> stream;
    stream.Attach(OpenPrefStream());
    if (stream) {
        MBComPtr<IMBPref> pref(MBIID_CMBPref);
        MBString ambientName;
        pref->GetString(stream, MBString("miscellaneous"), MBString("ambient"), ambientName, MBString(""));
        MBForceSound(ambientName.c_str(), 10, -1);
        soundManager->SetChannelVolume(10, sMusicVolume);
    }
}

// 0x100ac510 ?LoadHotspots@SteamboatNode@@MAEXXZ
void SteamboatNode::LoadHotspots()
{
    // the scene's hotspots, buttons and clickables from <space>scene.ini, "<space>hotspot<n>" etc.
    MBComPtr<ISBGameState> gs(MBIID_CSBGameState);
    MBComPtr<IMBFileSpec> fileSpec(MBIID_IMBFileSpec);
    if (fileSpec) {
        MBString prefFile = mSpaceCode + "scene.ini";
        fileSpec->SetPath(prefFile.c_str());
        MBComPtr<IMBStream> stream;
        IMBStream *tempStream;
        MBOpenFile(fileSpec, 5, &tempStream);
        stream.Attach(tempStream);
        if (stream) {
            MBComPtr<IMBPref> pref(MBIID_CMBPref);
            MBComPtr<IMBScreenPoly> poly(MBIID_CMBScreenPoly);
            int iPoly = 0;
            char key[16];
            sprintf(key, "%shotspot0", mSpaceCode.c_str());
            while (poly->LoadFromStream(stream, pref, key) > 0) {
                long afterAct = pref->GetLong(stream, MBString(key), MBString("after act"), 0);
                if (afterAct <= gs->GetProperty("HC_act1_done", 0)) {
                    MBString nextScene;
                    MBString triggerType;
                    pref->GetString(stream, MBString(key), MBString("next scene"), nextScene, MBString(""));
                    pref->GetString(stream, MBString(key), MBString("trigger type"), triggerType, MBString(""));
                    long cursorType = pref->GetLong(stream, MBString(key), MBString("cursor"), 0);
                    long zDepth = pref->GetLong(stream, MBString(key), MBString("z depth"), 0);
                    SBMouseEvent *mouseEvent = new SBMouseEvent;
                    mouseEvent->cursorID = cursorType;
                    mouseEvent->data = nextScene;
                    mouseEvent->action = triggerType;
                    if (triggerType == "exit") {
                        poly->SetBroadcastChannel(200);
                        poly->SetClickMessage(201, mouseEvent);
                        poly->SetEnterMessage(202, mouseEvent);
                        poly->SetLeaveMessage(203);
                        poly->SetZ(zDepth);
                    } else if (triggerType == "dialog") {
                        MBString motivateMessageHandler;
                        pref->GetString(stream, MBString(key), MBString("motivateMessage Handler"), motivateMessageHandler,
                                        MBString(""));
                        mouseEvent->data = motivateMessageHandler;
                        poly->SetBroadcastChannel(700);
                        poly->SetClickMessage(710, mouseEvent);
                        poly->SetEnterMessage(202, mouseEvent);
                        poly->SetLeaveMessage(203);
                        poly->SetZ(zDepth);
                    } else if (triggerType == "sound") {
                        MBString sfx;
                        pref->GetString(stream, MBString(key), MBString("data"), sfx, MBString(""));
                        mouseEvent->data = sfx;
                        poly->SetBroadcastChannel(120);
                        poly->SetClickMessage(201, mouseEvent);
                        poly->SetEnterMessage(202, mouseEvent);
                        poly->SetLeaveMessage(203);
                        poly->SetZ(zDepth);
                    }
                    bool addToWorld = true;
                    if (poly->GetIfProperty().compare("null") != 0 && !gs->GetProperty(poly->GetIfProperty().c_str(), 0))
                        addToWorld = false;
                    if (poly->GetIfNotProperty().compare("null") != 0 && gs->GetProperty(poly->GetIfNotProperty().c_str(), 0) == 1)
                        addToWorld = false;
                    if (addToWorld) {
                        MBComPtr<IMBMouseable> polyMouseable(poly);
                        mMouseMonitor->AddMouseable(polyMouseable);
                        mHotspots.push_back(poly);
                    }
                }
                poly.CreateObject(MBIID_CMBScreenPoly);
                iPoly++;
                sprintf(key, "%shotspot%d", mSpaceCode.c_str(), iPoly);
            }
            MBDebugStr("SteamboatNode::LoadHotspots() loaded %d hotspots\n", iPoly);

            iPoly = 0;
            sprintf(key, "%sbutton0", mSpaceCode.c_str());
            MBComPtr<ISBAnimButton> button(MBIID_CSBAnimButton);
            if (button) {
                while (button->LoadFromStream(stream, pref, key) > 0) {
                    mWorld->AddObject(button);
                    MBComPtr<IMBMouseable> buttonMouseable(button);
                    mMouseMonitor->AddMouseable(buttonMouseable);
                    mButtons.push_back(button);
                    button.CreateObject(MBIID_CSBAnimButton);
                    iPoly++;
                    sprintf(key, "%sbutton%d", mSpaceCode.c_str(), iPoly);
                }
                MBDebugStr("SteamboatNode::LoadHotspots() loaded %d buttons\n", iPoly);
            }

            iPoly = 0;
            sprintf(key, "%sclickable0", mSpaceCode.c_str());
            MBComPtr<ISBClickable> clickable(MBIID_CSBClickable);
            if (clickable) {
                while (clickable->LoadFromStream(stream, pref, key) > 0) {
                    bool addToWorld = true;
                    if (clickable->GetIfProperty().compare("null") != 0 && !gs->GetProperty(clickable->GetIfProperty().c_str(), 0))
                        addToWorld = false;
                    if (clickable->GetIfNotProperty().compare("null") != 0 &&
                        gs->GetProperty(clickable->GetIfNotProperty().c_str(), 0) == 1)
                        addToWorld = false;
                    if (addToWorld) {
                        mWorld->AddObject(clickable);
                        MBComPtr<IMBMouseable> clickableMouseable(clickable);
                        mMouseMonitor->AddMouseable(clickableMouseable);
                        mClickables.push_back(clickable);
                        clickable.CreateObject(MBIID_CSBClickable);
                    }
                    iPoly++;
                    sprintf(key, "%sclickable%d", mSpaceCode.c_str(), iPoly);
                }
                MBDebugStr("SteamboatNode::LoadHotspots() loaded %d clickables\n", iPoly);
            }
        }
    }
}

// TODO 0x100adc00 ?ClearHotspots@SteamboatNode@@MAEXXZ: first draft (89% similar; the original inlines the SBMouseEvent destructor in the second and third loops and the third clear)
void SteamboatNode::ClearHotspots()
{
    // the click data of each hotspot, button and clickable belongs to the scene
    for (MBComPtr<IMBScreenPoly> *hotspot = mHotspots.begin(); hotspot != mHotspots.end(); hotspot++) {
        MBComPtr<IMBMouseable> mouseable(hotspot);
        mMouseMonitor->RemoveMouseable(mouseable);
        delete (SBMouseEvent *)(*hotspot)->GetClickData();
    }
    mHotspots.clear();
    for (MBComPtr<ISBAnimButton> *button = mButtons.begin(); button != mButtons.end(); button++) {
        mWorld->GetRoot().RemoveObject(*button);
        MBComPtr<IMBMouseable> mouseable(button);
        mMouseMonitor->RemoveMouseable(mouseable);
        delete (SBMouseEvent *)(*button)->GetClickData();
    }
    mButtons.clear();
    for (MBComPtr<ISBClickable> *clickable = mClickables.begin(); clickable != mClickables.end(); clickable++) {
        mWorld->GetRoot().RemoveObject(*clickable);
        MBComPtr<IMBMouseable> mouseable(clickable);
        mMouseMonitor->RemoveMouseable(mouseable);
        delete (SBMouseEvent *)(*clickable)->GetClickData();
    }
    mClickables.clear();
}

// 0x100ae010 ?EnableHotspots@SteamboatNode@@UAEX_N@Z
void SteamboatNode::EnableHotspots(bool enable)
{
    mEnableHotspotsDelayTimer = 0;
    if (enable) {
        if (mDisableHotspots || mWaitingToPickUpItem)
            return;
        mMotivateHotspot->Activate();
    } else
        mMotivateHotspot->Deactivate();
    for (MBComPtr<IMBScreenPoly> *hotspot = mHotspots.begin(); hotspot != mHotspots.end(); hotspot++) {
        if (enable) {
            (*hotspot)->Activate();
            if (mShowGui) {
                MBComPtr<ISBInventory> inventory(MBIID_ISBInventory);
                inventory->Enable(true);
            }
        } else {
            (*hotspot)->Deactivate();
            if (mShowGui) {
                MBComPtr<ISBInventory> inventory(MBIID_ISBInventory);
                inventory->Enable(false);
            }
        }
    }
    MBComPtr<ISBGameState> gs(MBIID_ISBGameState);
    gs->EnableItemHotspots(mSpaceCode, enable);
    if (!enable && !sCursorItem)
        SetCursorSequence(0);
}

// 0x100ae2c0 ?EnableHotspotsWithDelay@SteamboatNode@@IAEXXZ
void SteamboatNode::EnableHotspotsWithDelay()
{
    MBComPtr<IMBGameClock> clock(MBIID_IMBGameClock);
    mEnableHotspotsDelayTimer = clock->GetGameTime();
}

// EQUIV 0x100ae3b0 ?SendExitSceneEvent@SteamboatNode@@UAEXABV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@Z: reviewed: 85% similar; the original calls gs's CreateObject out of line and keeps one EH flag word for the two new-expressions
void SteamboatNode::SendExitSceneEvent(const MBString &toSpace)
{
    MBComPtr<ISBGameState> gs(MBIID_CSBGameState);
    mNextSceneIsUI = toSpace == "UI";
    // the background music follows the story: act 2 starts with the paddle wheel (PP), the
    // village (VL) plays the act the recovered parts have reached
    if (toSpace == MBString("PP")) {
        if (gs->GetProperty("HC_act1_done", 0) == 1)
            SetNewBackgroundMusic(MBString("act2"));
    } else if (mSpaceCode == MBString("VL")) {
        int partsRecovered = 0;
        if (gs->GetProperty("PRbathtubRecovered", 0))
            partsRecovered = 1;
        if (gs->GetProperty("PRbicycleRecovered", 0))
            partsRecovered++;
        if (gs->GetProperty("PRpropellerRecovered", 0))
            partsRecovered++;
        if (gs->GetProperty("HC_act1_done", 0) == 1) {
            switch (partsRecovered) {
            case 0:
                SetNewBackgroundMusic(MBString("act2"));
                break;
            case 1:
                SetNewBackgroundMusic(MBString("act3"));
                break;
            case 2:
                SetNewBackgroundMusic(MBString("act4"));
                break;
            default:
                SetNewBackgroundMusic(MBString("act5"));
                break;
            }
        }
    }
    // Mickey walks to the scene's exit for the next scene, or leaves at once if there is none
    if (sMfPrimaryActor) {
        MfTransform3 exitTransform;
        MfAiActorNode *exitNode = 0;
        MfAiSpace *space = SBMotivateDriver::GetMotivateSpace();
        if (space) {
            MfAiActor *floor = space->GetActor(MfString(mSpaceCode.c_str()) + MfString("floor0"));
            if (!floor) {
                MBDebugStr("SteamboatNode::SendExitSceneEvent() space but no floor; probably double-clicked exit hotspot\n");
                return;
            }
            exitNode = floor->GetNodeByName(MfString(toSpace.c_str()) + MfString("exit"));
        }
        if (exitNode) {
            exitTransform = exitNode->GetGlobalTransform();
            m_pMfEvent = new MfPoEvent(MfString("hc_loco"), sMfPrimaryActor);
        } else
            m_pMfEvent = new MfPoEvent(MfString("hc_exit_scene"), sMfPrimaryActor);
        m_pMfVector = new MfPoVector3(exitTransform.translation());
        m_hpMfVector = MfScDataHandle::CreateData(m_pMfVector);
        m_pMfEvent->SetProperty(0, MfString("nextScene"), MfScDataHandle::CreateData(MfString(toSpace.c_str()), 0), 1);
        m_pMfEvent->SetProperty(0, MfString("point"), m_hpMfVector, 1);
        m_hpMfEvent = MfScDataHandle::CreateData(m_pMfEvent);
        sMfPrimaryActor->SendEvent(0, m_hpMfEvent, 0);
        SBMotivateDriver::GetInstance()->Update();
    }
}

#pragma optimize("a", off)
// 0x100aeda0 ?CleanupExitSceneEvent@SteamboatNode@@IAEXXZ
void SteamboatNode::CleanupExitSceneEvent()
{
    // the event of the last exit is done: free the handles and the objects behind them
    if (m_hpMfEvent) {
        m_hpMfEvent->SetPointer(0);
        m_hpMfVector->SetPointer(0);
        m_hpMfEvent->SetType(0x12);
        m_hpMfVector->SetType(0x12);
        m_hpMfEvent->Release();
        m_pMfVector->~MfPoVector3();
        m_pMfEvent->~MfPoEvent();
        MBfree_release(m_pMfVector);
        MBfree_release(m_pMfEvent);
    }
}
#pragma optimize("a", on)

#define SAFE_RELEASE(p) if (p) { p->Release(); p = 0; }

// EQUIV 0x100aee40 ?SaveGame@SteamboatNode@@UAEXXZ: reviewed: 86% similar; the original gives pPrefStream the higher stack slot (pPref here) and clears spaceCode inline after the loop-free tail (38 bytes more)
void SteamboatNode::SaveGame()
{
    // movies, the user interface, the credits and the main menu are not saved
    if (mSpaceCode != "MV" && mSpaceCode != "UI" && mSpaceCode != "CD" && mSpaceCode != "RR") {
        MBComPtr<ISBGameState> gs(MBIID_CSBGameState);
        CSavedGameDB objDavedGameDB;
        MBComPtr<ISBInventory> gui(MBIID_ISBInventory);
        IMBPref *pPref = 0;
        IMBStream *pPrefStream = 0;
        long nGameID = gs->GetProperty("SB_SavedGameID", 1);
        char pszSavedGameFileName[32];
        sprintf(pszSavedGameFileName, "savedgame%d.ini", nGameID, 1);
        bool bOpened = CreatePrefAndStream(pPrefStream, pPref, MBString(pszSavedGameFileName), false, false, 0);
        if (bOpened) {
            objDavedGameDB.Load();
            MBString spaceCode;
            // the activities return to the scene they were started from
            if (mSpaceCode == "CP" || mSpaceCode == "HK" || mSpaceCode == "MP" || mSpaceCode == "TB" || mSpaceCode == "GL" ||
                mSpaceCode == "LL" || mSpaceCode == "SB" || mSpaceCode == "SH" || mSpaceCode == "SM")
                objDavedGameDB.GetGameLastSean(nGameID, spaceCode);
            else
                spaceCode = mSpaceCode;
            objDavedGameDB.SetGameLastSean(nGameID, spaceCode);
            objDavedGameDB.SetGameIcon(nGameID, gs->GetProperty("SB_SavedGameIconID", 1));
            objDavedGameDB.Save();
            gui->Save(pPrefStream, pPref, mNextSceneIsUI);
            gs->Save(pPrefStream, pPref, mNextSceneIsUI);
            SAFE_RELEASE(pPrefStream);
            SAFE_RELEASE(pPref);
        }
    }
}

// ListenToEvent prints its own source line when it switches the hotspots on, so those statements
// sit on the original lines 2867 and 3020; the gaps held code not reconstructed yet.

























































































































































































// EQUIV 0x100af3c0 ?ListenToEvent@SteamboatNode@@UAEXJJPAX@Z: reviewed: 45% similar; same size, same dispatch and
// case layout (the case bodies are in the original's order, which differs from the event order); the
// original's frame is 4 bytes smaller and keeps this in ebx, 0 in edi and data in ebp (VC6 here: edi,
// ebp, esi), which shifts every instruction. Which temporary shares a slot in the original is open
void SteamboatNode::ListenToEvent(long channel, long event, void *data)
{
    MBComPtr<ISBGameState> gs(MBIID_CSBGameState);
    switch (channel) {
    case -1:
        if (event == 8)
            OnMouseMove(*(MBMouseEvent *)data);
        break;
    case -9:
        // the application window lost or got the focus
        if (event == 1) {
            sAppInForeground = false;
            ((DXGfx::ViewD3D *)RenderManager::GetInstance()->GetView(0))->GetDevice()->GetDirect3D()->GetTextureMapManager()->Release(false);
            MBComPtr<IMBPeriodicalManager> _periodicalManager(MBIID_IMBPeriodicalManager);
            if (_periodicalManager)
                _periodicalManager->RemovePeriodical(MBComPtr<IMBPeriodical>(MBComPtr<IMBViewManager>(MBIID_CMBViewManager)));
            MBDebugStr("INFO: moving to background\n");
        } else if (event == 0) {
            sAppInForeground = true;
            sBackgroundDirty = 0;
            MBComPtr<IMBPeriodicalManager> _periodicalManager(MBIID_IMBPeriodicalManager);
            if (_periodicalManager)
                _periodicalManager->AddPeriodical(MBComPtr<IMBPeriodical>(MBComPtr<IMBViewManager>(MBIID_CMBViewManager)));
            MBDebugStr("INFO: moving to foreground\n");
        }
        break;
    case 110:
        // an item hotspot
        switch (event) {
        case 203:
            if (!sCursorItem)
                SetCursorSequence(0);
            break;
        case 202:
            if (!sCursorItem)
                SetCursorSequence(((SBItem *)data)->GetRolloverCursor());
            break;
        case 201:
            OnItemClick((SBItem *)data);
            break;
        }
        break;
    case 120:
        // a hotspot that plays a sound
        switch (event) {
        case 203:
            if (!sCursorItem)
                SetCursorSequence(0);
            break;
        case 201: {
            MBString sound = ((SBMouseEvent *)data)->data;
            PlaySfx(sound.c_str());
            break;
        }
        case 202:
            if (!sCursorItem)
                SetCursorSequence(((SBMouseEvent *)data)->cursorID);
            break;
        }
        break;
    case 130:
        // Motivate's hotspot: a click into the 3D scene
        if (event == 201)
            On3dLMouseUp();
        break;
    case 300:
        switch (event) {
        case 209:
            mMfCameraNode = 0;
            break;
        case 303:
            UserEventHook((MfUserHookData *)data);
            break;
        case 309:
            sMfPrimaryActor->SendMessageA(0, (const char *)data);
            break;
        case 202:
            if (!sCursorItem)
                SetCursorSequence(((SBMouseEvent *)data)->cursorID);
            break;
        case 203:
            if (!sCursorItem)
                SetCursorSequence(0);
            break;
        }
        break;
    case 200:
        // an exit hotspot
        if (!sCursorItem) {
            switch (event) {
            case 202:
                SetCursorSequence(((SBMouseEvent *)data)->cursorID);
                break;
            case 203:
                SetCursorSequence(0);
                break;
            case 201:
                SendExitSceneEvent(((SBMouseEvent *)data)->data);
                break;
            }
        }
        break;
    case 600:
        switch (event) {
        case 610:
            if (*(long *)data == 0 && mWaitingToPickUpItem) {
                mWaitingToPickUpItem = false;
                MBDebugStr("EnableHotspots: %s, line %d.\n", "D:\\tugboat\\code\\steamboat\\SteamboatNode.cpp", __LINE__);
                EnableHotspots(true);
            }
            break;
        case 604:
            SendExitSceneEvent(MBString("MP"));
            break;
        case 606:
            SendExitSceneEvent(MBString("UI"));
            break;
        case 603:
            // an item given from the inventory
            if (((SBItem *)data)->GetInstanceName().compare("PRmglassAndHankie0") == 0)
                SendExitSceneEvent(MBString("HK"));
            else if (((SBItem *)data)->GetInstanceName().compare("PRwhistle0") == 0) {
                sMfPrimaryActor->SendMessageA(0, MfString("hc_play_whistle"));
                MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
                broadcaster->BroadcastEvent(600, 609, 0);
            } else if (((SBItem *)data)->IsItemActiveInSpace(GetSpaceCode()))
                SetCursorItem((SBItem *)data);
            break;
        }
        break;
    // The original has more lines here: the dialogue case below sits on the original lines 3020 ff.











































































































    case 700:
        // the dialogue system
        if (!sCursorItem) {
            switch (event) {
            case 202:
                SetCursorSequence(((SBMouseEvent *)data)->cursorID);
                break;
            case 203:
                SetCursorSequence(0);
                break;
            case 207:
                EnableHotspots(false);
                break;
            case 206:
                EnableHotspots(true);
                break;
            case 208:
                // the end of the trek in the park
                if (mSpaceCode.compare("PE") == 0) {
                    MBComPtr<ISBGameState> gs(MBIID_CSBGameState);
                    if (gs->GetProperty("HC_act1_done", 0)) {
                        mDisableHotspots = false;
                        MBDebugStr("EnableHotspots: %s, line %d.\n", "D:\\tugboat\\code\\steamboat\\SteamboatNode.cpp", __LINE__);
                        EnableHotspots(true);
                    }
                }
                break;
            }
        }
        break;
    }
    ScriptHelpNode::ListenToEvent(channel, event, data);
}

// OnItemClick prints its own source line when it switches the hotspots off, so those two
// statements sit on the original lines 3118 and 3133; the gap held code not reconstructed yet.

































































// EQUIV 0x100b0260 ?OnItemClick@SteamboatNode@@UAEXPAVSBItem@@@Z: reviewed: 29% similar; the original zeroes the new-expression flag word up front and keeps this in ebp, the flag in ebx (register choice shifts everything)
void SteamboatNode::OnItemClick(SBItem *item)
{
    if (!sMfPrimaryActor)
        return;
    if (sCursorItem)
        OnClickWithCursorItem(item);
    // tell Mickey which item hotspot was clicked
    MfPoEvent *event = new MfPoEvent(MfString("hc_item_hotspot_lup"), sMfPrimaryActor);
    event->SetProperty(0, MfString("item"), MfScDataHandle::CreateData(MfString(item->GetInstanceName().c_str()), 0), 1);
    sMfPrimaryActor->SendEvent(0, MfScDataHandle::CreateData(event), 0);
    SBMotivateDriver::GetInstance()->Update();
    MfAiActor *actor = 0;
    if (SBMotivateDriver::GetMotivateSpace())
        actor = SBMotivateDriver::GetMotivateSpace()->GetActor(MfString(item->GetInstanceName().c_str()));
    MfProperty prop;
    if (actor && sInputEventChannelActive && sMfPrimaryActor) {
        // an item that talks keeps the hotspots off until it is done
        if (actor->GetProperty(MfString("HCTalking"), prop) && (int)prop) {
            MBDebugStr("DisableHotspots: %s, line %d.\n", "D:\\tugboat\\code\\steamboat\\SteamboatNode.cpp", __LINE__);
            EnableHotspots(false);
            if (!sCursorItem) {
                // a recyclable can of this scene goes to the recycle bin
                MBString className = item->GetClassNameA();
                MBString canName = mSpaceCode + "can";
                if (className.find(canName) != MBString::npos) {
                    MBComPtr<IMBEventManager> eventManager(MBIID_IMBEventManager);
                    eventManager->BroadcastEvent(700, 712, "GDRecyclable");
                    mWaitingToPickUpItem = true;





                    MBDebugStr("DisableHotspots: %s, line %d.\n", "D:\\tugboat\\code\\steamboat\\SteamboatNode.cpp", __LINE__);
                    EnableHotspots(false);
                }
            }
        }
    }
}

// EQUIV 0x100b07d0 ?OnClickWithCursorItem@SteamboatNode@@IAEXPAVSBItem@@@Z: reviewed: 76% similar; the original keeps bGiven in the item argument's slot and the MfString temporaries in the frame
void SteamboatNode::OnClickWithCursorItem(SBItem *item)
{
    // the item under the cursor is used on the clicked item: the clicked item's Motivate actor
    // learns about it, and an item given to the one it is meant for leaves the inventory
    MBComPtr<ISBGameState> gs(MBIID_CSBGameState);
    int cursorInt = 0;
    MBString className = item->GetClassNameA();
    MfAiActor *actor = SBMotivateDriver::GetMotivateSpace()->GetActor(MfString(item->GetInstanceName().c_str()));
    MfProperty prop;
    bool bGiven = false;
    if (actor->GetProperty(MfString(sCursorItem->GetClassNameA().c_str()), prop)) {
        bGiven = true;
        MfIntValue trueValue(1);
        MfProperty newProp(MfString(sCursorItem->GetClassNameA().c_str()), trueValue.Copy());
        actor->SetProperty(newProp);
    }
    gs->SetProperty(sCursorItem->GetClassNameA().c_str(), 1);
    gs->SetProperty("HC_lastCursorItem", sCursorItem->GetClassNameA().c_str());
    CursorItemStringToInt(sCursorItem->GetClassNameA(), cursorInt);
    gs->SetProperty("HC_lastCursorItemInt", cursorInt);
    if (bGiven && sCursorItem->GetGiveTo() == className) {
        sCursorItem->SetOnCharacter(false);
        MBString instanceName = sCursorItem->GetInstanceName();
        RemoveItemFromInventory(instanceName);
        ResetCursor();
    }
}

// 0x100b0c00 ?ClipDirtyRect@SteamboatNode@@KAXABVMBRect@@AAUtagRECT@@@Z
void __cdecl SteamboatNode::ClipDirtyRect(const MBRect &bounds, tagRECT &newRect)
{
    newRect.left = 640;
    newRect.top = 480;
    newRect.right = 0;
    newRect.bottom = 0;
    if (bounds.left < 640)
        newRect.left = bounds.left;
    if (newRect.left < 0)
        newRect.left = 0;
    if (bounds.right > 0)
        newRect.right = bounds.right;
    if (newRect.right > 640)
        newRect.right = 640;
    if (bounds.top < 480)
        newRect.top = bounds.top;
    if (newRect.top < 0)
        newRect.top = 0;
    if (bounds.bottom > 0)
        newRect.bottom = bounds.bottom;
    if (newRect.bottom > 480)
        newRect.bottom = 480;
}

// Whether the 3D scene is drawn in layers (shown in the frame-rate line).

// 0x100b0cc0 ?CalculateFrameRate@SteamboatNode@@IAEXXZ
void SteamboatNode::CalculateFrameRate()
{
    sNumFrames++;
    if (timeGetTime() - sLastFrameTime > 500) {
        float fFrameRate = sNumFrames * 1000.0f / (timeGetTime() - sLastFrameTime);
        sLastFrameTime = timeGetTime();
        sNumFrames = 0;
        sprintf(sS, "FPS: %.1f layer: %s useDirty: %s     (%d,%d)\n", fFrameRate, sUseLayered3DRender ? "yes" : "no",
                (sRenderFlags & RenderManager::cDirtyRects) ? "yes" : "no", mMousePos.x, mMousePos.y);
        sTextControl->SetText(MBString(sS));
    }
}

// 0x100b0ec0 ?GetActiveNode@SteamboatNode@@SAPAV1@XZ
SteamboatNode *__cdecl SteamboatNode::GetActiveNode()
{
    return sCurrentActiveNode;
}

// 0x100b0ee0 ?GetActiveCursor@SteamboatNode@@SAPAVIMBAnim@@XZ
IMBAnim *__cdecl SteamboatNode::GetActiveCursor()
{
    return sCurrentActiveNode->mCursor;
}

// 0x100b0f00 ?GetActiveWorld@SteamboatNode@@SAPAVIMBWorld@@XZ
IMBWorld *__cdecl SteamboatNode::GetActiveWorld()
{
    return sCurrentActiveNode->mWorld;
}

// 0x100b0f20 ?GetActiveMouseMonitor@SteamboatNode@@SAPAVIMBMouseMonitor@@XZ
IMBMouseMonitor *__cdecl SteamboatNode::GetActiveMouseMonitor()
{
    return sCurrentActiveNode->mMouseMonitor;
}

// 0x100b0f40 ?SetActiveMouseMonitor@SteamboatNode@@SAXPAVIMBMouseMonitor@@@Z
void __cdecl SteamboatNode::SetActiveMouseMonitor(IMBMouseMonitor *monitor)
{
    sCurrentActiveNode->mMouseMonitor = monitor;
}

// TODO 0x100b0f80 ?CreateAndLoadAnim@SteamboatNode@@SAJABV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@AAPAVIMBAnim@@@Z: (92% similar) the inlined CreateObject keeps unk->Release in both branches and tests the -2 result again (VC6 merges and folds them)
long __cdecl SteamboatNode::CreateAndLoadAnim(const MBString &strAnimName, IMBAnim *&pAnim)
{
    MBComPtr<IMBAnim> iAnim;
    IMBStream *pStream;
    unsigned long id = MBHashString(strAnimName.c_str());
    pAnim = 0;
    long err = GetStream(id, 'anm ', pStream);
    if (err == 0) {
        err = iAnim.CreateObject(MBIID_CMBAnim);
        if (err == 0) {
            err = iAnim->Load(id, pStream, false);
            if (err == 0) {
                iAnim.AddRef();
                pAnim = iAnim;
            }
        }
        if (pStream) {
            pStream->Release();
            pStream = 0;
        }
    }
    return err;
}

// 0x100b1130 ?AddAnimToWorld@SteamboatNode@@SAJABV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@AAPAVIMBAnim@@@Z
long __cdecl SteamboatNode::AddAnimToWorld(const MBString &strAnimName, IMBAnim *&pAnim)
{
    IMBUnknown *iUnknown = 0;
    pAnim = 0;
    long err = CreateAndLoadAnim(strAnimName, pAnim);
    if (err == 0) {
        err = pAnim->QueryInterface(MBIID_IMBUnknown, (void **)&iUnknown);
        if (err == 0) {
            AddToWorld(iUnknown);
            if (iUnknown)
                iUnknown->Release();
        }
    }
    return err;
}

// 0x100b11b0 ?AddToWorld@SteamboatNode@@SAXPAVIMBUnknown@@@Z
void __cdecl SteamboatNode::AddToWorld(IMBUnknown *iUnknown)
{
    IMBObject *iObject = 0;
    IMBMouseable *iMouseable = 0;
    if (iUnknown) {
        if (iUnknown->QueryInterface(MBIID_IMBObject, (void **)&iObject) == 0) {
            IMBObject *object = iObject;
            GetActiveWorld()->GetRoot().AddObject(object);
            if (iObject) {
                iObject->Release();
                iObject = 0;
            }
        }
        if (iUnknown->QueryInterface(MBIID_IMBMouseable, (void **)&iMouseable) == 0) {
            GetActiveMouseMonitor()->AddMouseable(iMouseable);
            if (iMouseable) {
                iMouseable->Release();
                iMouseable = 0;
            }
        }
    }
}

// 0x100b1280 ?RemoveFromWorld@SteamboatNode@@SAXPAVIMBUnknown@@@Z
void __cdecl SteamboatNode::RemoveFromWorld(IMBUnknown *iUnknown)
{
    IMBObject *iObject = 0;
    IMBMouseable *iMouseable = 0;
    if (iUnknown) {
        if (iUnknown->QueryInterface(MBIID_IMBMouseable, (void **)&iMouseable) == 0) {
            GetActiveMouseMonitor()->RemoveMouseable(iMouseable);
            if (iMouseable) {
                iMouseable->Release();
                iMouseable = 0;
            }
        }
        if (iUnknown->QueryInterface(MBIID_IMBObject, (void **)&iObject) == 0) {
            if (sRenderFlags & 8)
                AddDirtyRect(iObject);
            IMBObject *object = iObject;
            GetActiveWorld()->GetRoot().RemoveObject(object);
            if (iObject) {
                iObject->Release();
                iObject = 0;
            }
        }
    }
}

// EQUIV 0x100b1360 ?GetStream@SteamboatNode@@SAJKKAAPAVIMBStream@@@Z: reviewed: the original calls mgr's _QueryInterface
// out of line (VC6 inlines it here, as in SBActivityNode::StartListening)
long __cdecl SteamboatNode::GetStream(unsigned long id, unsigned long type, IMBStream *&pStream)
{
    IMBStream *streamPtr = 0;
    MBComPtr<IMBResourceFileList> mgr(MBIID_CMBResourceMgr);
    long err = -3;
    pStream = 0;
    if (mgr) {
        mgr->GetStream(id, type, &streamPtr);
        if (streamPtr) {
            pStream = streamPtr;
            err = 0;
        }
    }
    return err;
}

// The original's CreatePrefAndStream throws its source line from a failing step, so its throws
// sit on the original lines 3390 .. 3402; the gap before it held code not reconstructed yet.

























#define THROW_ON_ERROR(expr) if ((expr) != 0) throw (long)__LINE__
#define THROW_IF(cond) if (cond) throw (long)__LINE__

// 0x100b1450 ?CreatePrefAndStream@SteamboatNode@@SA_NAAPAVIMBStream@@AAPAVIMBPref@@ABV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@_N3K@Z
bool __cdecl SteamboatNode::CreatePrefAndStream(IMBStream *&iStream, IMBPref *&iPref, const MBString &strFile, bool bReadOnly, bool bMustExist, unsigned long flags)
{
    IMBFileSpec *iFileSpec = 0;
    IMBUnknown *iUnknown = 0;
    iPref = 0;
    iStream = 0;
    unsigned long mode = (bReadOnly ? 0 : 2) | (bMustExist ? 4 : 0) | flags | 1;
    try {
        THROW_ON_ERROR(MBComCreateInstance(0, MBIID_IMBFileSpec, &iUnknown));
        THROW_ON_ERROR(iUnknown->QueryInterface(MBIID_IMBFileSpec, (void **)&iFileSpec));
        SAFE_RELEASE(iUnknown);
        iFileSpec->SetPath(strFile.c_str());
        MBOpenFile(iFileSpec, mode, &iStream);


        THROW_IF(!iStream);
        SAFE_RELEASE(iFileSpec);


        THROW_ON_ERROR(MBComCreateInstance(0, MBIID_IMBPref, &iUnknown));
        THROW_ON_ERROR(iUnknown->QueryInterface(MBIID_IMBPref, (void **)&iPref));
        if (iUnknown)
            iUnknown->Release();
        return true;
    } catch (long nLine) {
        MBDebugStr("%s(%d) : ERROR! Failed to create stream and pref for file %s, Access:%s Open:%s\n",
                   "D:\\tugboat\\code\\steamboat\\SteamboatNode.cpp", nLine, strFile.c_str(),
                   bReadOnly ? "Read" : "Read & Write", bMustExist ? "Existing" : "Create | Existing");
        MBDebugStr("Is the file read only?\n");
        SAFE_RELEASE(iUnknown);
        SAFE_RELEASE(iFileSpec);
        SAFE_RELEASE(iStream);
        SAFE_RELEASE(iPref);
    }
    return false;
}

// EQUIV 0x100b1710 ?FindFreeChannel@SteamboatNode@@IAEJJJ@Z: reviewed: the original calls sm's Release and
// _QueryInterface out of line from the inlined CreateObject (VC6 inlines both here)
long SteamboatNode::FindFreeChannel(long start, long end)
{
    MBComPtr<IMBSoundManager> sm;
    sm.CreateObject(MBIID_IMBSoundManager);
    if (sm) {
        for (long channel = start; channel <= end; channel++) {
            if (!sm->IsChannelPaused(channel) && !sm->IsChannelPlaying(channel))
                return channel;
        }
    }
    return -1;
}

// 0x100b1850 ?SetNewBackgroundMusic@SteamboatNode@@IAEXV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@Z
void SteamboatNode::SetNewBackgroundMusic(MBString name)
{
    // stream the wave file of that name (unless it already plays) and remember it in the game state
    MBComPtr<ISBGameState> gameState(MBIID_CSBGameState);
    IMBStream *stream = 0;
    if (name != sBackgroundMusicName) {
        if (GetStream(MBHashString(name.c_str()), 'wav ', stream) == 0) {
            if (sBackgroundMusic) {
                AIL_close_stream(sBackgroundMusic);
                sBackgroundMusic = 0;
            }
            IMBFileSpec *fileSpec = 0;
            MBComPtr<IMBFile> file(stream);
            stream->Release();
            if (file) {
                file->GetFileSpec(&fileSpec);
                file.Release();
            }
            if (fileSpec) {
                _DIG_DRIVER *dig;
                AIL_quick_handles(&dig, 0, 0);
                char path[256];
                strcpy(path, fileSpec->GetPath().c_str());
                sBackgroundMusic = AIL_open_stream(dig, path, 0);
                AIL_last_error();
                if (sBackgroundMusic) {
                    AIL_auto_service_stream(sBackgroundMusic, 1);
                    AIL_start_stream(sBackgroundMusic);
                    AIL_set_stream_loop_count(sBackgroundMusic, 0);
                }
                fileSpec->Release();
            }
            SetBackgroundMusicState(sWantBackgroundMusic);
            sBackgroundMusicName = name;
            if (gameState)
                gameState->SetProperty("HCBackgroundMusic", name.c_str());
        }
    }
}

// 0x100b1b90 ?SetSceneBackgroundMusic@SteamboatNode@@IAEXV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@Z
void SteamboatNode::SetSceneBackgroundMusic(MBString name)
{
    // as SetNewBackgroundMusic, but the scene's music is not remembered: the name is cleared
    MBComPtr<ISBGameState> gameState(MBIID_CSBGameState);
    IMBStream *stream = 0;
    if (name != sBackgroundMusicName) {
        if (GetStream(MBHashString(name.c_str()), 'wav ', stream) == 0) {
            if (sBackgroundMusic) {
                AIL_close_stream(sBackgroundMusic);
                sBackgroundMusic = 0;
            }
            IMBFileSpec *fileSpec = 0;
            MBComPtr<IMBFile> file(stream);
            stream->Release();
            if (file) {
                file->GetFileSpec(&fileSpec);
                file.Release();
            }
            if (fileSpec) {
                _DIG_DRIVER *dig;
                AIL_quick_handles(&dig, 0, 0);
                char path[256];
                strcpy(path, fileSpec->GetPath().c_str());
                sBackgroundMusic = AIL_open_stream(dig, path, 0);
                AIL_last_error();
                if (sBackgroundMusic) {
                    AIL_auto_service_stream(sBackgroundMusic, 1);
                    AIL_start_stream(sBackgroundMusic);
                    AIL_set_stream_loop_count(sBackgroundMusic, 0);
                }
                fileSpec->Release();
            }
            SetBackgroundMusicState(sWantBackgroundMusic);
            sBackgroundMusicName = MBString("");
        }
    }
}

// 0x100b1f40 ?SetBackgroundMusicState@SteamboatNode@@IAEX_N@Z
void SteamboatNode::SetBackgroundMusicState(bool playing)
{
    sWantBackgroundMusic = playing;
    if (sBackgroundMusic) {
        if (playing)
            AIL_pause_stream(sBackgroundMusic, 0);
        else
            AIL_pause_stream(sBackgroundMusic, 1);
    }
}

// 0x100b1f80 ?FeedBackgroundMusic@SteamboatNode@@IAEXXZ
void SteamboatNode::FeedBackgroundMusic()
{
}

// 0x100b1fa0 ?AddDirtyRect@SteamboatNode@@SAXAAVMBRect@@@Z
void __cdecl SteamboatNode::AddDirtyRect(MBRect &rect)
{
    if (sRenderFlags & RenderManager::cDirtyRects) {
        tagRECT winRect;
        ClipDirtyRect(rect, winRect);
        if (winRect.left < winRect.right && winRect.top < winRect.bottom)
            RenderManager::GetInstance()->GetBitmapBuffer(0)->SetScreenRect(winRect);
    }
}

// 0x100b20c0 ?AddDirtyRect@SteamboatNode@@SAXPAVIMBObject@@@Z
void __cdecl SteamboatNode::AddDirtyRect(IMBObject *obj)
{
    if (obj)
        AddDirtyRect(obj->GetBounds());
}

// Every scene reads its strings from two databases: the shared one under "universal" and the one of
// the current language.
const char *kSBUniversal = "universal";

// The rectangles redrawn this frame, outlined for debugging.

// 0x100b2100 ?DrawDirtyRects@SteamboatNode@@SAXPAUHWND__@@PAUHDC__@@ABVMBColor@@@Z
void __cdecl SteamboatNode::DrawDirtyRects(HWND__ *hwnd, HDC__ *hdc, const MBColor &color)
{
    HPEN hPen = CreatePen(PS_SOLID, 1, RGB(color.GetRed(), color.GetGreen(), color.GetBlue()));
    HGDIOBJ hOldPen = SelectObject(hdc, hPen);
    int nOldROP = SetROP2(hdc, R2_XORPEN);
    RECT rectWindow;
    GetWindowRect(hwnd, &rectWindow);
    POINT ptOffset = { 0, 0 };
    ClientToScreen(hwnd, &ptOffset);
    ptOffset.x -= rectWindow.left;
    ptOffset.y -= rectWindow.top;
    for (tagRECT *it = sDirtyRectVector.begin(); it != sDirtyRectVector.end(); it++) {
        RECT rect = *it;
        OffsetRect(&rect, ptOffset.x, ptOffset.y);
        if (rect.left < rect.right) {
            MoveToEx(hdc, rect.left, rect.top, 0);
            LineTo(hdc, rect.right - 1, rect.top);
            LineTo(hdc, rect.right - 1, rect.bottom - 1);
            LineTo(hdc, rect.left, rect.bottom - 1);
            LineTo(hdc, rect.left, rect.top);
        }
    }
    SetROP2(hdc, nOldROP);
    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);
}

// EQUIV 0x100b22c0 ?AddAllObjectDirtyRects@SteamboatNode@@IAEXXZ: reviewed: only stack slots differ (same instructions and size); the original leaves an unused 4-byte slot after each IMBUnknown pointer (0x14, 0x1c) and keeps the object count above them
void SteamboatNode::AddAllObjectDirtyRects()
{
    // every visible object of the world but the scene renderer itself
    if (m_sbWorld) {
        unsigned long nObjects = m_sbWorld->GetNObjects();
        for (unsigned long i = 0; i < nObjects; i++) {
            MBComPtr<IMBObject> object = m_sbWorld->GetObjectA(i);
            IMBUnknown *pObjectUnknown = 0;
            IMBUnknown *pRenderUnknown = 0;
            if (object) {
                object->QueryInterface(MBIID_IMBUnknown, (void **)&pObjectUnknown);
                if (pObjectUnknown)
                    pObjectUnknown->Release();
            }
            if (mRenderObject) {
                // SBRenderObject is an IMBUnknown first
                ((IMBUnknown *)mRenderObject)->QueryInterface(MBIID_IMBUnknown, (void **)&pRenderUnknown);
                pRenderUnknown->Release();
            }
            if (!(pObjectUnknown - pRenderUnknown))
                continue;
            if (object->IsVisible())
                AddDirtyRect(object);
        }
    }
    SBActorManager::GetInstance()->GetScreenRects(sBuffer, RenderManager::GetInstance()->GetBitmapBuffer(0));
}

// 0x100b24e0 ?AddDatabase@SteamboatNode@@QAEXPBD@Z
void SteamboatNode::AddDatabase(const char *name)
{
    MBString dbname(kSBUniversal);
    dbname += "\\";
    dbname += name;
    SimpleNode::AddDatabase(dbname.c_str());
    dbname = sLocale + "\\" + name;
    SimpleNode::AddDatabase(dbname.c_str());
}

// 0x100b2710 ?RemoveDatabase@SteamboatNode@@QAEXPBD@Z
void SteamboatNode::RemoveDatabase(const char *name)
{
    MBString dbname(kSBUniversal);
    dbname += "\\";
    dbname += name;
    SimpleNode::RemoveDatabase(dbname.c_str());
    dbname = sLocale + "\\" + name;
    SimpleNode::RemoveDatabase(dbname.c_str());
}

// Template instances emitted by the code above:

// 0x100a3610 ?GetClassNameA@SBItem@@QAEABV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@XZ

// 0x100b2990 ?c_str@?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@QBEPBDXZ
// MBString members the linker took from ActivityLeveler.obj (every unit using MBString emits them):

// 0x100084f0 ??1?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@QAE@XZ

// 0x10008590 ?_Tidy@?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@AAEX_N@Z

// 0x10008700 ?_Grow@?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@AAE_NI_N@Z

// 0x10008880 ?_Copy@?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@AAEXI@Z

// Instances the linker took from other objects (this unit emits them too):

// 0x1000bd80 ??1?$MBComPtr@VIMBSoundManager@@@@QAE@XZ

// 0x1000bdb0 ??1?$MBComPtr@VIMBResourceFileList@@@@QAE@XZ


// 0x1000c900 ?_Eos@?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@AAEXI@Z

// 0x10014710 ?_QueryInterface@?$MBComPtr@VIMBSoundManager@@@@AAEJABQAVIMBUnknown@@@Z

// 0x10018840 ?assign@?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@QAEAAV12@ABV12@II@Z

// 0x10018c80 ?max_size@?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@QBEIXZ

// 0x10018ca0 ?_Split@?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@AAEXXZ

// 0x10024f20 ??0?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@QAE@ABV01@@Z

// 0x1003db30 ??1?$MBComPtr@VISBGameState@@@@QAE@XZ

// 0x10043a00 ?_QueryInterface@?$MBComPtr@VISBGameState@@@@AAEJABQAVIMBUnknown@@@Z

// 0x1005ea50 ?Release@?$MBComPtr@VIMBAnim@@@@QAEXXZ

// 0x10092150 ?Release@?$MBComPtr@VIMBSoundManager@@@@QAEXXZ

// Template and inline instances this unit also emits:
// 0x10086f10 ??1?$MBComPtr@VISBInventory@@@@QAE@XZ
// 0x10087210 ?Release@?$MBComPtr@VISBInventory@@@@QAEXXZ
// 0x100874d0 ?_QueryInterface@?$MBComPtr@VISBInventory@@@@AAEJABQAVIMBUnknown@@@Z
// 0x100ac220 ?GetInstance@SBDialoguePropertyManager@@SAPAV1@XZ
// 0x100b29b0 ??1?$basic_string@GU?$char_traits@G@std@@V?$MBAllocator@G@@@std@@QAE@XZ

// Template and inline instances this unit also emits:
// 0x100126b0 ?_Lbound@?$_Tree@HU?$pair@$$CBHPAUViewInfo@RenderManager@@@std@@U_Kfn@?$map@HPAUViewInfo@RenderManager@@U?$less@H@std@@V?$allocator@PAUViewInfo@RenderManager@@@4@@2@U?$less@H@2@V?$allocator@PAUViewInfo@RenderManager@@@2@@std@@IBEPAU_Node@12@ABH@Z
// 0x100a19c0 ??0?$MBVector@UtagRECT@@@@QAE@XZ
// 0x100adf70 ??1SBMouseEvent@@QAE@XZ
// 0x100b2f20 ??C?$MBComPtr@VISBGameState@@@@QBEPAVISBGameState@@XZ
// 0x100b3340 ?erase@?$vector@V?$MBComPtr@VIMBScreenPoly@@@@V?$MBAllocator@V?$MBComPtr@VIMBScreenPoly@@@@@@@std@@QAEPAV?$MBComPtr@VIMBScreenPoly@@@@PAV3@0@Z
// 0x100b3440 ?erase@?$vector@V?$MBComPtr@VISBAnimButton@@@@V?$MBAllocator@V?$MBComPtr@VISBAnimButton@@@@@@@std@@QAEPAV?$MBComPtr@VISBAnimButton@@@@PAV3@0@Z
// 0x100b4b70 ?AddRef@?$MBComPtr@VISBClickable@@@@QAEXXZ
// 0x1000c750 ?CreateObject@?$MBComPtr@VIMBSoundManager@@@@QAEJABUMBGUID@@@Z
// 0x10011c00 ?_Destroy@?$vector@UtagRECT@@V?$MBAllocator@UtagRECT@@@@@std@@IAEXPAUtagRECT@@0@Z
// 0x10011cd0 ?find@?$_Tree@HU?$pair@$$CBHPAUViewInfo@RenderManager@@@std@@U_Kfn@?$map@HPAUViewInfo@RenderManager@@U?$less@H@std@@V?$allocator@PAUViewInfo@RenderManager@@@4@@2@U?$less@H@2@V?$allocator@PAUViewInfo@RenderManager@@@2@@std@@QAE?AViterator@12@ABH@Z
// 0x100361a0 ??A?$map@HPAUViewInfo@RenderManager@@U?$less@H@std@@V?$allocator@PAUViewInfo@RenderManager@@@4@@std@@QAEAAPAUViewInfo@RenderManager@@ABH@Z
// 0x100a8e70 ??0MfTypedValue@@QAE@XZ
// 0x100a8ea0 ??0MfIntValue@@QAE@H@Z
// 0x100b2a90 ?insert@?$vector@UtagRECT@@V?$MBAllocator@UtagRECT@@@@@std@@QAEXPAUtagRECT@@PBU3@1@Z
// 0x100b2f70 ??1?$MBComPtr@VIMBDirectXScreen@@@@QAE@XZ
// 0x100b30f0 ?erase@?$vector@UtagRECT@@V?$MBAllocator@UtagRECT@@@@@std@@QAEPAUtagRECT@@PAU3@0@Z
// 0x100b3590 ?Release@?$MBComPtr@VIMBMovie@@@@QAEXXZ
// 0x100b44d0 ?_QueryInterface@?$MBComPtr@VIMBMovie@@@@AAEJABQAVIMBUnknown@@@Z
// 0x100b4610 ?copy@std@@YAPAUtagRECT@@PAU2@00@Z
// 0x100b5010 ?_QueryInterface@?$MBComPtr@VIMBFile@@@@AAEJABQAVIMBStream@@@Z
// 0x100a2650 ??_GSteamboatNode@@UAEPAXI@Z
// 0x100a2690 ??1?$MBVector@V?$MBComPtr@VIMBScreenPoly@@@@@@QAE@XZ
// 0x100a2710 ??1?$MBVector@V?$MBComPtr@VISBAnimButton@@@@@@QAE@XZ
// 0x100a2790 ??1?$MBVector@V?$MBComPtr@VISBClickable@@@@@@QAE@XZ
// 0x100a3300 ??1MBLocale@@QAE@XZ
// 0x100b2e80 ??1?$MBComPtr@VCSBRenderManager@@@@QAE@XZ
// 0x100b2ef0 ??1?$MBComPtr@VISBWorld@@@@QAE@XZ
// 0x100b3500 ?Release@?$MBComPtr@VISBWorld@@@@QAEXXZ
// 0x100b3530 ?Release@?$MBComPtr@VIMBUnknown@@@@QAEXXZ
// 0x100b3560 ?Release@?$MBComPtr@VIMBDirectXScreen@@@@QAEXXZ
// 0x100b4030 ?SetValidFileHeader@NodeFactory@Graph@@QAEXV?$_SS@D@@@Z
// 0x100b4260 ?_QueryInterface@?$MBComPtr@VIMBWorld@@@@AAEJABQAVIMBUnknown@@@Z
// 0x100b4300 ?_QueryInterface@?$MBComPtr@VIMBTextControl@@@@AAEJABQAVIMBUnknown@@@Z
// 0x100b4a10 ??_G?$MBComPtr@VIMBScreenPoly@@@@QAEPAXI@Z
// 0x100b4a50 ??_G?$MBComPtr@VISBAnimButton@@@@QAEPAXI@Z
// 0x100b4a90 ??_G?$MBComPtr@VISBClickable@@@@QAEPAXI@Z
// 0x100b4bf0 ?_QueryInterface@?$MBComPtr@VISBWorld@@@@AAEJABV?$MBComPtr@VIMBWorld@@@@@Z
// 0x100b4c90 ?_QueryInterface@?$MBComPtr@VIMBUnknown@@@@AAEJABV?$MBComPtr@VIMBScreenPoly@@@@@Z
// 0x100b5130 ??4?$MBComPtr@VISBWorld@@@@QAEAAV0@PAVISBWorld@@@Z
// 0x100b5170 ??4?$MBComPtr@VIMBDirectXScreen@@@@QAEAAV0@PAVIMBDirectXScreen@@@Z
// 0x100b2a20 ?find@?$map@HPAUViewInfo@RenderManager@@U?$less@H@std@@V?$allocator@PAUViewInfo@RenderManager@@@4@@std@@QAE?AViterator@?$_Tree@HU?$pair@$$CBHPAUViewInfo@RenderManager@@@std@@U_Kfn@?$map@HPAUViewInfo@RenderManager@@U?$less@H@std@@V?$allocator@PAUViewInfo@RenderManager@@@4@@2@U?$less@H@2@V?$allocator@PAUViewInfo@RenderManager@@@2@@2@ABH@Z
// 0x100b2eb0 ?multScal@?$FixedVec@M$02@@QAEAAV1@M@Z
// 0x100b2fa0 ??1?$MBComPtr@VISBClickable@@@@QAE@XZ
// 0x100b2fd0 ??C?$MBComPtr@VISBClickable@@@@QBEPAVISBClickable@@XZ
// 0x100b3020 ??1?$MBComPtr@VIMBPeriodical@@@@QAE@XZ
// 0x100b3160 ?insert@?$vector@V?$MBComPtr@VIMBScreenPoly@@@@V?$MBAllocator@V?$MBComPtr@VIMBScreenPoly@@@@@@@std@@QAEPAV?$MBComPtr@VIMBScreenPoly@@@@PAV3@ABV3@@Z
// 0x100b3400 ?_Destroy@?$vector@V?$MBComPtr@VIMBScreenPoly@@@@V?$MBAllocator@V?$MBComPtr@VIMBScreenPoly@@@@@@@std@@IAEXPAV?$MBComPtr@VIMBScreenPoly@@@@0@Z
// 0x100b35c0 ?Release@?$MBComPtr@VISBClickable@@@@QAEXXZ
// 0x100b35f0 ?Release@?$MBComPtr@VIMBPeriodicalManager@@@@QAEXXZ
// 0x100b3620 ?CreateObject@?$MBComPtr@VIMBPeriodicalManager@@@@QAEJABUMBGUID@@@Z
// 0x100b36f0 ?CreateObject@?$MBComPtr@VIMBViewManager@@@@QAEJABUMBGUID@@@Z
// 0x100b3790 ??1DDDeviceInfo@DX@@QAE@XZ
// 0x100b3880 ?insert@?$vector@V?$MBComPtr@VISBAnimButton@@@@V?$MBAllocator@V?$MBComPtr@VISBAnimButton@@@@@@@std@@QAEXPAV?$MBComPtr@VISBAnimButton@@@@IABV3@@Z
// 0x100b3be0 ?insert@?$vector@V?$MBComPtr@VISBClickable@@@@V?$MBAllocator@V?$MBComPtr@VISBClickable@@@@@@@std@@QAEXPAV?$MBComPtr@VISBClickable@@@@IABV3@@Z
// 0x100b3f40 ?size@?$vector@V?$MBComPtr@VIMBScreenPoly@@@@V?$MBAllocator@V?$MBComPtr@VIMBScreenPoly@@@@@@@std@@QBEIXZ
// 0x100b3f70 ?_Ucopy@?$vector@V?$MBComPtr@VIMBScreenPoly@@@@V?$MBAllocator@V?$MBComPtr@VIMBScreenPoly@@@@@@@std@@IAEPAV?$MBComPtr@VIMBScreenPoly@@@@PBV3@0PAV3@@Z
// 0x100b3fc0 ?_Ufill@?$vector@V?$MBComPtr@VIMBScreenPoly@@@@V?$MBAllocator@V?$MBComPtr@VIMBScreenPoly@@@@@@@std@@IAEXPAV?$MBComPtr@VIMBScreenPoly@@@@IABV3@@Z
// 0x100b41a0 ??1?$ValList@UDXDisplayModeInfo@DX@@@@QAE@XZ
// TODO 0x100b4210 ??A?$ValList@UDDDeviceInfo@DX@@@@QAEAAUDDDeviceInfo@DX@@H@Z: inline in ValList.h; with DisplayD3D's inline buffer setters CreateDisplay's inline budget changes and this unit inlines every call, so no out-of-line copy is emitted here
// 0x100b4570 ?_QueryInterface@?$MBComPtr@VISBClickable@@@@AAEJABQAVIMBUnknown@@@Z
// 0x100b4760 ?_QueryInterface@?$MBComPtr@VIMBViewManager@@@@AAEJABQAVIMBUnknown@@@Z
// 0x100b4800 ?fill@std@@YAXPAV?$MBComPtr@VIMBScreenPoly@@@@0ABV2@@Z
// 0x100b4850 ?copy_backward@std@@YAPAV?$MBComPtr@VIMBScreenPoly@@@@PAV2@00@Z
// 0x100b48c0 ?_Construct@std@@YAXPAV?$MBComPtr@VIMBScreenPoly@@@@ABV2@@Z
// 0x100b4930 ?_Construct@std@@YAXPAV?$MBComPtr@VISBAnimButton@@@@ABV2@@Z
// 0x100b49a0 ?_Construct@std@@YAXPAV?$MBComPtr@VISBClickable@@@@ABV2@@Z
// 0x100b4ad0 ??4?$MBComPtr@VISBAnimButton@@@@QAEAAV0@PAVISBAnimButton@@@Z
// 0x100b4b10 ??4?$MBComPtr@VISBClickable@@@@QAEAAV0@PAVISBClickable@@@Z
// 0x100b4b50 ?AddRef@?$MBComPtr@VISBAnimButton@@@@QAEXXZ
// 0x100b4b90 ?mag2@?$FixedVec@M$02@@QBEMXZ
// 0x100b4f70 ?_QueryInterface@?$MBComPtr@VIMBPeriodical@@@@AAEJABV?$MBComPtr@VIMBViewManager@@@@@Z
// 0x100b51b0 ?Add@?$ValList@UDXDisplayModeInfo@DX@@@@QAEHUDXDisplayModeInfo@DX@@@Z

// the node's shared state (defined after the code: the line numbers above are the original ones)
MBString SteamboatNode::sMofaDir(".");
MBComPtr<IMBTextControl> SteamboatNode::sTextControl;
MBString SteamboatNode::sInventoryItemPicked("");
MBString SteamboatNode::sBackgroundMusicName("");
MBString SteamboatNode::sLocale("");
// the plain statics (initial values from the original's data section; zero ones are in .bss)
MfAiActor *SteamboatNode::sMfPrimaryActor = 0;
short SteamboatNode::sSfxVolume = 196;
short SteamboatNode::sDialogVolume = 255;
short SteamboatNode::sMusicVolume = 196;
Gfx::Node *SteamboatNode::sSceneRootNode = 0;
ViewEventHandler *SteamboatNode::sMotivateEventHandler = 0;
SBItem *SteamboatNode::sCursorItem = 0;
unsigned long SteamboatNode::sLastFrameTime = 0;
char SteamboatNode::sS[80];
int SteamboatNode::sNumFrames = 0;
bool SteamboatNode::sDisplayFrameRate = false;
unsigned long SteamboatNode::sRenderFlags = 11;
int SteamboatNode::sBackgroundDirty = 0;
int SteamboatNode::sBuffer = 0;
SteamboatNode *SteamboatNode::sCurrentActiveNode = 0;
bool SteamboatNode::sAppInForeground = true;
bool SteamboatNode::sInputEventChannelActive = true;
_STREAM *SteamboatNode::sBackgroundMusic = 0;
bool SteamboatNode::sWantBackgroundMusic = true;

// the code of the scene being played
MBString __cdecl SteamboatNode::GetSpaceCode()
{
    return sCurrentActiveNode->mSpaceCode;
}

// the unit includes this after its code (the line numbers above are the original ones)
#include <locale> // evidence: the original unit ends with the std::ctype<unsigned short>::id and std::collate<char>::id initializers

// 0x100a1300 _$E152
// 0x100a1320 _$E46
// 0x100a18f0 _$E151
// 0x100a1910 _$E47
// 0x100a1940 _$E160
// 0x100a1960 _$E157
// 0x100a1980 _$E159
// 0x100a19a0 _$E158
// 0x100a19f0 _$E172
// 0x100a1a10 _$E149
// 0x100a1b30 _$E171
// 0x100a1b50 _$E150
// 0x100a1bc0 _$E177
// 0x100a1be0 _$E168
// 0x100a1d00 _$E176
// 0x100a1d20 _$E169
// 0x100a1da0 _$E182
// 0x100a1dc0 _$E174
// 0x100a1de0 _$E181
// 0x100a1e00 _$E175
// 0x100a1e40 _$E187
// 0x100a1e60 _$E179
// 0x100a1f80 _$E186
// 0x100a1fa0 _$E180
// 0x100a2020 _$E192
// 0x100a2040 _$E184
// 0x100a2160 _$E191
// 0x100a2180 _$E185
// 0x100a2200 _$E49
// 0x100a2220 _$E189
// 0x100a2340 _$E48
// 0x100a2360 _$E190
// 0x100b2940 _$E155
// 0x100b2960 _$E165
// 0x100b43a0 _$E163
// 0x100b43c0 _$E154
// 0x100b4660 _$E166
// EQUIV 0x100b4680 _$E162: reviewed: kYellow's initializer; the original reads each kCM_ARGB32 shift byte just before its use, VC6 here loads them up front as one dword and one word (distinct object from the kYellow stores); same stores, shifts, masks and order
// 0x100b50b0 _$E204
// 0x100b50f0 _$E200
// 0x100b5110 _$E203
// 0x100b4eb0 ?_QueryInterface@?$MBComPtr@VIMBMouseable@@@@AAEJABQAV?$MBComPtr@VISBClickable@@@@@Z
// 0x100b4df0 ?_QueryInterface@?$MBComPtr@VIMBMouseable@@@@AAEJABQAV?$MBComPtr@VISBAnimButton@@@@@Z
// 0x100b4d30 ?_QueryInterface@?$MBComPtr@VIMBMouseable@@@@AAEJABQAV?$MBComPtr@VIMBScreenPoly@@@@@Z
// TODO 0x100b5250 ?Add@?$ValList@UD3DDriverInfo@DX@@@@QAEHUD3DDriverInfo@DX@@@Z: header instance; this unit inlines every use, so VC6 here emits no out-of-line copy
// EQUIV 0x100aa140 ??0MfRotation3@@QAE@ABV0@@Z: reviewed: the copy stores the third component before loading the fourth, VC6 here loads first; distinct members of distinct objects, same loads and stores
// EQUIV 0x100aa110 ??0MfVector3@@QAE@ABV0@@Z: reviewed: the copy stores y before loading z, VC6 here loads first; distinct members of distinct objects, same loads and stores
