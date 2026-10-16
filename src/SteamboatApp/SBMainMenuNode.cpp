#include "SBMainMenuNode.h"
#include "SBCursorItems.h" // every node unit of the original has the cursor item strings
#include <locale> // evidence: the original unit ends with the std::ctype<unsigned short>::id and std::collate<char>::id initializers
#include "SBMotivateDriver.h"
#include "ViewEventHandler.h"
#include "MfAi.h"
#include "MfString.h"
#include "MBRuntime.h"
#include <stdio.h>
#include <string.h>

bool SBMainMenuNode::sIsGameInProgress;
short SBMainMenuNode::sCurrentGameIndex;

// 0x10081db0 ??0SBMainMenuNode@@QAE@PAVIMBGameEngine@@PAVMfAiSpace@@@Z
SBMainMenuNode::SBMainMenuNode(IMBGameEngine *engine, MfAiSpace *space)
    : SBActivityNode(engine, space), mCurrentState(0), mUIStarted(false), mTitleScreen(0),
      mNewButton(0), mNewBalloonString(0), mLoadButton(0), mLoadBalloonString(0), mExitButton(0),
      mReturnToGameButton(0), mLoadBoard(0), mLeftLoadArrow(0), mRightLoadArrow(0),
      mLoadGameIcons(0), mLoadGameThumbnails(0), mLoadGameSelectorIndex(0), mBackButton(0),
      mMickeyHotRect(0), mMinnieHotRect(0), mEasyButton(0), mHardButton(0), mGameButtonBoard(0),
      mLetsGoButton(0), mThoughtBubble(0), mCharacter(3), mDifficulty(0), mGameIcon(0),
      mMickeyState(kBackgroundIdle), mMinnieState(kBackgroundIdle), mEraseGameIndex(-1),
      mLoadingGame(false)
{
    for (int i = 0; i < 8; i++)
        mGameButtons[i] = 0;
    mSavedGameDB.Load();
}

// 0x10082040 ??0SBMainMenuNode@@QAE@PAVIMBGameEngine@@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@Z
SBMainMenuNode::SBMainMenuNode(IMBGameEngine *engine, MBString spaceCode)
    : SBActivityNode(engine, spaceCode), mCurrentState(0), mUIStarted(false), mTitleScreen(0),
      mNewButton(0), mNewBalloonString(0), mLoadButton(0), mLoadBalloonString(0), mExitButton(0),
      mReturnToGameButton(0), mLoadBoard(0), mLeftLoadArrow(0), mRightLoadArrow(0),
      mLoadGameIcons(0), mLoadGameThumbnails(0), mLoadGameSelectorIndex(0), mBackButton(0),
      mMickeyHotRect(0), mMinnieHotRect(0), mEasyButton(0), mHardButton(0), mGameButtonBoard(0),
      mLetsGoButton(0), mThoughtBubble(0), mCharacter(3), mDifficulty(0), mGameIcon(0),
      mMickeyState(kBackgroundIdle), mMinnieState(kBackgroundIdle), mEraseGameIndex(-1),
      mLoadingGame(false)
{
    for (int i = 0; i < 8; i++)
        mGameButtons[i] = 0;
    mSavedGameDB.Load();
}

// 0x100822a0 ??1SBMainMenuNode@@UAE@XZ
SBMainMenuNode::~SBMainMenuNode()
{
}

// TODO 0x10082390 ?OnStart@SBMainMenuNode@@UAEXXZ: the original calls broadcaster's CreateObject out of line and inlines gameState's; VC6 inlines both here (47% similar)
void SBMainMenuNode::OnStart()
{
    SetBackgroundMusicState(true);
    SteamboatNode::OnStart();
    MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
    broadcaster->AddListener(100, this);
    broadcaster->AddListener(210, this);
    AddDatabase("ui");
    MBComPtr<ISBGameState> gameState(MBIID_ISBGameState);
    CreateMainMenuButtons();
    mEraseGameDialog.SetBackgroundName("eraseboard");
    mEraseGameDialog.SetButtonNames("yes", "no");
    mLeaveGameDialog.SetBackgroundName("leaveboard");
    mLeaveGameDialog.SetButtonNames("yes", "no");
}

// 0x10082750 ?OnEnd@SBMainMenuNode@@UAEXXZ
void SBMainMenuNode::OnEnd()
{
    mEraseGameDialog.Destroy();
    mLeaveGameDialog.Destroy();
    RemoveDatabase("ui");
    MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
    broadcaster->RemoveListener(100, this);
    broadcaster->RemoveListener(210, this);
    SteamboatNode::OnEnd();
}

// 0x10082880 ?OnKeyDown@SBMainMenuNode@@UAEXAAUMBKeyboardEvent@@@Z
void SBMainMenuNode::OnKeyDown(MBKeyboardEvent &key)
{
    SteamboatNode::OnKeyDown(key);
}

// TODO 0x100828a0 ?OnLMouseDown@SBMainMenuNode@@UAEXAAUMBMouseEvent@@@Z: the original calls the MfVector3 constructor of data.dir out of line (pickPoint and data.pos inline) and the MBString assign out of line; VC6 inlines all of them here (constructor forms tried: body, init list, setValue, chained assignment) (49% similar)
void SBMainMenuNode::OnLMouseDown(MBMouseEvent &evt)
{
    if (!mUIStarted) {
        CreateMainMenuButtons();
        mUIStarted = true;
    }
    MfPickData pickData;
    MfMouseHookData data;
    CopyMBMouseEventToMfMouseHookData(evt, data);
    MfAiActor *actor = sMotivateEventHandler->HandleLButtonUp(&data, pickData);
    if (actor) {
        // a click on Mickey or Minnie while both stand in front picks the character
        MBString mfActorName((char *)actor->GetClassNameA());
        if (mfActorName == "GDmickey" || mfActorName == "GDminnie") {
            if (mMickeyState != kForegroundIdle || mMinnieState != kForegroundIdle || GetMfPrimaryActor())
                return;
        }
    }
    SteamboatNode::OnLMouseDown(evt);
}

// TODO 0x10082b40 ?OnLMouseUp@SBMainMenuNode@@UAEXAAUMBMouseEvent@@@Z: as OnLMouseDown (42% similar)
void SBMainMenuNode::OnLMouseUp(MBMouseEvent &evt)
{
    MfPickData pickData;
    MfMouseHookData data;
    CopyMBMouseEventToMfMouseHookData(evt, data);
    MfAiActor *actor = sMotivateEventHandler->HandleLButtonUp(&data, pickData);
    if (actor) {
        // a click on Mickey or Minnie while both stand in front picks the character
        MBString mfActorName((char *)actor->GetClassNameA());
        if (mfActorName == "GDmickey" || mfActorName == "GDminnie") {
            if (mMickeyState != kForegroundIdle || mMinnieState != kForegroundIdle || GetMfPrimaryActor())
                return;
        }
    }
    SteamboatNode::OnLMouseUp(evt);
}

// 0x10082d70 ?OnAnimSeqStarted@SBMainMenuNode@@UAEXKK@Z
void SBMainMenuNode::OnAnimSeqStarted(unsigned long id, unsigned long seq)
{
    // the balloon strings follow the sequences of their buttons
    unsigned long newID = 0;
    unsigned long loadID = 0;
    IMBResource *animResource = 0;
    if (mNewButton) {
        mNewButton->GetAnim()->QueryInterface(MBIID_IMBResource, (void **)&animResource);
        if (animResource) {
            newID = animResource->GetID();
            animResource->Release();
        }
    }
    animResource = 0;
    if (mLoadButton) {
        mLoadButton->GetAnim()->QueryInterface(MBIID_IMBResource, (void **)&animResource);
        if (animResource) {
            loadID = animResource->GetID();
            animResource->Release();
        }
    }
    if (id) {
        if (id == newID && mNewBalloonString)
            mNewBalloonString->SetCurrentSequence(seq);
        else if (id == loadID && mLoadBalloonString)
            mLoadBalloonString->SetCurrentSequence(seq);
    }
    SimpleNode::OnAnimSeqStarted(id, seq);
}

// TODO 0x10082f00 ?ListenToEvent@SBMainMenuNode@@UAEXJJPAX@Z: same size and statements as the original (locals named and sized after the PDB); the locals sit in other stack slots (the string temporaries and the MBString allocator byte) (73% similar)
void SBMainMenuNode::ListenToEvent(long channel, long event, void *data)
{
    if (channel == 100) {
        // the buttons of the menu screens
        switch (event) {
        case 400: {
            // new game
            RemoveMainMenuButtons();
            PrepareCharacterSelection();
            MBComPtr<ISBGameState> gs(MBIID_ISBGameState);
            gs->ResetItems();
            gs->RandomizeRecycleItems(0x9de749f, gs->GetNumberOfRecycleItems(),
                                      gs->GetNumberOfPossibleLocationsForRecycleItems());
            break;
        }
        case 401:
            StartDialogue("ui02xmc0400", true);
            break;
        case 402:
            RemoveMainMenuButtons();
            CreateLoadGameSelector();
            break;
        case 403:
            StartDialogue("ui01xmc0020", true);
            break;
        case 404:
            mLeaveGameDialog.Create();
            StartDialogue("ui03xmc0100", true);
            break;
        case 405:
            StartDialogue("ui01xmc0040", true);
            break;
        case 406:
            // back to the game in progress
            if (!mLoadingGame) {
                IMBStream *pPrefStream = 0;
                IMBPref *pPref = 0;
                MBComPtr<ISBGameState> gs(MBIID_ISBGameState);
                mLoadingGame = true;
                char savedGameFilename[32];
                sprintf(savedGameFilename, "savedgame%d.ini", sCurrentGameIndex);
                if (CreatePrefAndStream(pPrefStream, pPref, MBString(savedGameFilename), true, true, 0)) {
                    MBComPtr<ISBInventory> iInv(MBIID_ISBInventory);
                    gs->Load(pPrefStream, pPref, 0);
                    iInv->Load(pPrefStream, pPref, 0);
                    if (pPref) {
                        pPref->Release();
                        pPref = 0;
                    }
                    if (pPrefStream) {
                        pPrefStream->Release();
                        pPrefStream = 0;
                    }
                }
                gs->SetProperty("SB_SavedGameID", sCurrentGameIndex);
                gs->SetProperty("SB_SavedGameIconID", sCurrentGameIndex);
                sIsGameInProgress = true;
                sMfPrimaryActor = gs->GetPrimaryActor();
                sMfPrimaryActor->SendMessageA(sMfPrimaryActor, "hc_set_player");
                MBString start = gs->GetProperty("HC_spaceCode", MBString("MX"));
                SendExitSceneEvent(MBString(start.c_str()));
            }
            break;
        case 450:
            mLoadGameSelectorIndex = --mLoadGameSelectorIndex % mLoadGameSelectorData.size();
            UpdateLoadGameSelector();
            break;
        case 451:
            mLoadGameSelectorIndex = ++mLoadGameSelectorIndex % mLoadGameSelectorData.size();
            UpdateLoadGameSelector();
            break;
        case 452:
            RemoveLoadGameSelector();
            CreateMainMenuButtons();
            break;
        case 453:
            // load the chosen saved game
            if (!mLoadingGame) {
                IMBStream *pPrefStream = 0;
                IMBPref *pPref = 0;
                MBComPtr<ISBGameState> gs(MBIID_ISBGameState);
                mLoadingGame = true;
                short savedGameIndex = mLoadGameSelectorData[mLoadGameSelectorIndex].gameIconIndex + 1;
                char savedGameFilename[32];
                sprintf(savedGameFilename, "savedgame%d.ini", savedGameIndex);
                if (CreatePrefAndStream(pPrefStream, pPref, MBString(savedGameFilename), true, true, 0)) {
                    MBComPtr<ISBInventory> iInv(MBIID_ISBInventory);
                    gs->Load(pPrefStream, pPref, 0);
                    iInv->Load(pPrefStream, pPref, 0);
                    if (pPref) {
                        pPref->Release();
                        pPref = 0;
                    }
                    if (pPrefStream) {
                        pPrefStream->Release();
                        pPrefStream = 0;
                    }
                }
                gs->SetProperty("SB_SavedGameID", savedGameIndex);
                gs->SetProperty("SB_SavedGameIconID", GetGameIcon());
                sIsGameInProgress = true;
                sCurrentGameIndex = savedGameIndex;
                sMfPrimaryActor = gs->GetPrimaryActor();
                sMfPrimaryActor->SendMessageA(sMfPrimaryActor, "hc_set_player");
                MBString start = gs->GetProperty("HC_spaceCode", MBString("MX"));
                SendExitSceneEvent(MBString(start.c_str()));
            }
            break;
        case 500:
            RemoveCharacterSelectionButtons();
            MoveCharactersToBackground();
            break;
        case 501:
            if (GetChosenCharacter() == 3)
                StartDialogue("ui02xmc0100", true);
            break;
        case 502:
            if (GetChosenCharacter() == 3)
                StartDialogue("ui02xmi0100", true);
            break;
        case 503:
        case 504:
            On3dLMouseUp();
            break;
        case 600:
            SetDifficulty(0);
            RemoveDifficultyButtons();
            CreateGameIcons();
            CreateIconSelectionButtons();
            break;
        case 601: {
            char easySoundName[16];
            sprintf(easySoundName, "ui02xm%c0200", GetCharacterCode(GetChosenCharacter()));
            StartDialogue(easySoundName, true);
            break;
        }
        case 602:
            SetDifficulty(1);
            RemoveDifficultyButtons();
            CreateGameIcons();
            CreateIconSelectionButtons();
            break;
        case 603: {
            char hardSoundName[16];
            sprintf(hardSoundName, "ui02xm%c0300", GetCharacterCode(GetChosenCharacter()));
            StartDialogue(hardSoundName, true);
            break;
        }
        case 604:
            // back to the choice of the character
            SetChosenCharacter(3);
            SetMfPrimaryActor(0);
            RemoveDifficultyButtons();
            PrepareCharacterSelection();
            break;
        case 700:
            RemoveGameIcons();
            RemoveIconSelectionButtons();
            CreateDifficultyButtons();
            break;
        case 800:
            RemoveSelectedGameIcon();
            RemoveLetsGoButtons();
            CreateGameIcons();
            CreateIconSelectionButtons();
            break;
        case 801: {
            // let's go: start the new game
            MBComPtr<IMBMouseable> letsGoMouseable(mLetsGoButton);
            if (letsGoMouseable)
                letsGoMouseable->Deactivate();
            MBComPtr<IMBMouseable> backMouseable(mBackButton);
            if (backMouseable)
                backMouseable->Deactivate();
            MBComPtr<ISBGameState> gs(MBIID_ISBGameState);
            gs->SetProperty("SB_SavedGameID", GetGameIcon());
            gs->SetProperty("SB_SavedGameIconID", GetGameIcon());
            sIsGameInProgress = true;
            long character = GetChosenCharacter();
            if (character == 1)
                StartDialogue("ui02xmc0410", true);
            else if (character == 2)
                StartDialogue("ui02xmi0410", true);
            MBComPtr<ISBInventory> iInv(MBIID_ISBInventory);
            iInv->RemoveAllItems();
            break;
        }
        }
        if (event >= 701 && event <= 708) {
            // a game icon: a free one starts the new game there, a used one asks to erase it
            short gameIcon = event - 700;
            if (!mSavedGameDB.IsGameUsed(gameIcon)) {
                SetGameIcon(gameIcon);
                RemoveUnselectedGameIcons();
                RemoveIconSelectionButtons();
                CreateLetsGoButtons();
            } else {
                mEraseGameIndex = gameIcon;
                mEraseGameDialog.Create();
            }
        }
    } else if (channel == -4) {
        // a voice line has ended
        if (event == 1) {
            unsigned long soundID = (*(IMBSoundDescription **)data)->GetSoundID();
            if (soundID == MBHashString("ui03xmc0110")) {
                // leave the game
                RemoveMainMenuButtons();
                RemoveGameIcons();
                if (SBMotivateDriver::GetMotivateSpace())
                    SBMotivateDriver::GetMotivateWorld()->CloseSpace(SBMotivateDriver::GetMotivateSpace());
                MBComPtr<ISBNodeManager> mgr(MBIID_ISBNodeManager);
                mgr->AddNode("CD");
            } else if (soundID == MBHashString("ui02xmc0410") || soundID == MBHashString("ui02xmi0410"))
                LetsGo();
        }
    } else if (channel == 210) {
        // the confirmation boxes
        switch (event) {
        case 211:
            if (mEraseGameDialog.IsActive()) {
                if (mEraseGameIndex >= 0) {
                    mSavedGameDB.Delete(mEraseGameIndex);
                    mSavedGameDB.Save();
                }
                mEraseGameDialog.Destroy();
                CreateGameIcons();
                mEraseGameIndex = -1;
            } else if (mLeaveGameDialog.IsActive()) {
                StartDialogue("ui03xmc0110", true);
                mLeaveGameDialog.Destroy();
            }
            break;
        case 212:
            if (mEraseGameDialog.IsActive())
                mEraseGameDialog.Destroy();
            else if (mLeaveGameDialog.IsActive())
                mLeaveGameDialog.Destroy();
            break;
        }
    }
    SBActivityNode::ListenToEvent(channel, event, data);
}

// 0x10084550 ?Idle@SBMainMenuNode@@UAEXXZ
void SBMainMenuNode::Idle()
{
    SteamboatNode::Idle();
}

// 0x10084570 ?UserEventHook@SBMainMenuNode@@UAEXPAUMfUserHookData@@@Z
void SBMainMenuNode::UserEventHook(MfUserHookData *hook)
{
    if (hook->event == "hc_ready_to_choose_player") {
        // both characters in front: the choice can be made
        if (hook->data == "GDmickey0") {
            mMickeyState = kForegroundIdle;
            if (mMinnieState == kForegroundIdle)
                CreateCharacterSelectionButtons();
        } else if (hook->data == "GDminnie0") {
            mMinnieState = kForegroundIdle;
            if (mMickeyState == kForegroundIdle)
                CreateCharacterSelectionButtons();
        }
    } else if (hook->event == "hc_player_ready_in_background") {
        if (hook->data == "GDmickey0") {
            mMickeyState = kBackgroundIdle;
            if (mMinnieState == kBackgroundIdle || GetChosenCharacter() == 3)
                CreateMainMenuButtons();
            else if (GetChosenCharacter() == 2)
                CreateDifficultyButtons();
        } else if (hook->data == "GDminnie0") {
            mMinnieState = kBackgroundIdle;
            if (mMickeyState == kBackgroundIdle || GetChosenCharacter() == 3)
                CreateMainMenuButtons();
            else if (GetChosenCharacter() == 1)
                CreateDifficultyButtons();
        }
    } else if (hook->event == "hc_player_chosen") {
        IMBStream *iStream = 0;
        IMBPref *iPref = 0;
        if (CreatePrefAndStream(iStream, iPref, MBString("gamestate.ini"), true, true, 0)) {
            RemoveCharacterSelectionButtons();
            SetMfPrimaryActor(hook->data);
            MBComPtr<ISBGameState> gs(MBIID_ISBGameState);
            gs->Load(iStream, iPref, sMfPrimaryActor);
            MfAiSpace *space = SBMotivateDriver::GetMotivateSpace();
            MfAiActor *mickey = space->GetActor("GDmickey0");
            MfAiActor *minnie = space->GetActor("GDminnie0");
            if (hook->data == "GDmickey0") {
                SetChosenCharacter(1);
                StartDialogue("ui02xmc0110", true);
                mickey->SendMessageA(minnie, "UIgotoBackground");
            } else if (hook->data == "GDminnie0") {
                SetChosenCharacter(2);
                StartDialogue("ui02xmi0110", true);
                mickey->SendMessageA(mickey, "UIgotoBackground");
            }
            if (iPref) {
                iPref->Release();
                iPref = 0;
            }
            if (iStream) {
                iStream->Release();
                iStream = 0;
            }
        }
    }
    SteamboatNode::UserEventHook(hook);
}

// 0x10084ae0 ?RemoveAnim@SBMainMenuNode@@IAEXAAPAVIMBAnim@@@Z
void SBMainMenuNode::RemoveAnim(IMBAnim *&anim)
{
    if (anim) {
        RemoveFromWorld(anim);
        anim = 0;
    }
}

// 0x10084b10 ?CreateButton@SBMainMenuNode@@IAEPAVISBAnimButton@@PBDJJ@Z
ISBAnimButton *SBMainMenuNode::CreateButton(const char *buttonName, long clickMessage, long rolloverMessage)
{
    ISBAnimButton *button;
    IMBUnknown *unknown;
    MBComCreateInstance(0, MBIID_CSBAnimButton, &unknown);
    if (unknown) {
        unknown->QueryInterface(MBIID_ISBAnimButton, (void **)&button);
        if (button) {
            button->SetAnim(buttonName);
            button->SetBroadcastChannel(100);
            button->SetClickMessage(clickMessage, 0);
            button->SetRolloverMessage(rolloverMessage, 0);
            AddToWorld(unknown);
            button->Release();
        }
        unknown->Release();
    }
    return button;
}

// 0x10084be0 ?RemoveButton@SBMainMenuNode@@IAEXAAPAVISBAnimButton@@@Z
void SBMainMenuNode::RemoveButton(ISBAnimButton *&button)
{
    if (button) {
        RemoveFromWorld(button);
        button = 0;
    }
}

// 0x10084c10 ?CreateHotRect@SBMainMenuNode@@IAEPAVIMBScreenPoly@@ABVMBRect@@J@Z
IMBScreenPoly *SBMainMenuNode::CreateHotRect(const MBRect &rect, long rolloverMessage)
{
    IMBScreenPoly *hotRect;
    IMBUnknown *unknown;
    MBComCreateInstance(0, MBIID_CMBScreenPoly, &unknown);
    if (unknown) {
        unknown->QueryInterface(MBIID_IMBScreenPoly, (void **)&hotRect);
        if (hotRect) {
            hotRect->SetRect(rect, 150);
            hotRect->SetBroadcastChannel(100);
            hotRect->SetEnterMessage(rolloverMessage, 0);
            AddToWorld(unknown);
            hotRect->Release();
        }
        unknown->Release();
    }
    return hotRect;
}

// 0x10084cd0 ?RemoveHotRect@SBMainMenuNode@@IAEXAAPAVIMBScreenPoly@@@Z
void SBMainMenuNode::RemoveHotRect(IMBScreenPoly *&hotRect)
{
    if (hotRect) {
        RemoveFromWorld(hotRect);
        hotRect = 0;
    }
}

// 0x10084d00 ?PrepareCharacterSelection@SBMainMenuNode@@IAEXXZ
void SBMainMenuNode::PrepareCharacterSelection()
{
    MfAiSpace *space = SBMotivateDriver::GetMotivateSpace();
    if (space) {
        MfAiActor *mickey = space->GetActor("GDmickey0");
        if (mickey)
            mickey->SendMessageA(0, "UIchooseCharacter");
    }
}

// 0x10084de0 ?MoveCharactersToBackground@SBMainMenuNode@@IAEXXZ
void SBMainMenuNode::MoveCharactersToBackground()
{
    MfAiSpace *space = SBMotivateDriver::GetMotivateSpace();
    if (space) {
        MfAiActor *mickey = space->GetActor("GDmickey0");
        if (mickey)
            mickey->SendMessageA(0, "UIgoBack");
    }
}

// 0x10084ec0 ?SetChosenCharacter@SBMainMenuNode@@IAEXJ@Z
void SBMainMenuNode::SetChosenCharacter(long character)
{
    mCharacter = character;
}

// 0x10084ee0 ?GetChosenCharacter@SBMainMenuNode@@IBEJXZ
long SBMainMenuNode::GetChosenCharacter() const
{
    return mCharacter;
}

// 0x10084f00 ?GetCharacterCode@SBMainMenuNode@@IAEDJ@Z
char SBMainMenuNode::GetCharacterCode(long character)
{
    switch (character) {
    case 1:
        return 'c';
    case 2:
        return 'i';
    }
    return ' ';
}

// 0x10084f30 ?SetDifficulty@SBMainMenuNode@@IAEXJ@Z
void SBMainMenuNode::SetDifficulty(long difficulty)
{
    mDifficulty = difficulty;
    MBComPtr<ISBGameState> gameState(MBIID_CSBGameState);
    if (gameState)
        gameState->SetProperty("difficulty", difficulty);
}

// 0x10085000 ?GetDifficulty@SBMainMenuNode@@IBEJXZ
long SBMainMenuNode::GetDifficulty() const
{
    return mDifficulty;
}

// 0x10085020 ?SetGameIcon@SBMainMenuNode@@IAEXJ@Z
void SBMainMenuNode::SetGameIcon(long gameIcon)
{
    mGameIcon = gameIcon;
}

// 0x10085040 ?GetGameIcon@SBMainMenuNode@@IBEJXZ
long SBMainMenuNode::GetGameIcon() const
{
    return mGameIcon;
}

// 0x10085060 ?CreateTitleScreen@SBMainMenuNode@@IAEXXZ
void SBMainMenuNode::CreateTitleScreen()
{
    if (!mTitleScreen) {
        mTitleScreen = CreateAnim("titleScreen");
        mTitleScreen->Place(MBPoint3(0, 0, 10));
    }
}

// 0x100850e0 ?RemoveTitleScreen@SBMainMenuNode@@IAEXXZ
void SBMainMenuNode::RemoveTitleScreen()
{
    RemoveAnim(mTitleScreen);
}

// 0x10085100 ?CreateMainMenuButtons@SBMainMenuNode@@IAEXXZ
void SBMainMenuNode::CreateMainMenuButtons()
{
    CreateTitleScreen();
    if (!mNewButton) {
        mNewButton = CreateButton("new", 400, 401);
        if (mNewButton)
            mNewButton->Place(MBPoint3(0, 0, 200));
    }
    if (!mNewBalloonString)
        mNewBalloonString = CreateAnim("newstring");
    if (!mLoadButton) {
        mLoadButton = CreateButton("load", 402, 403);
        if (mLoadButton)
            mLoadButton->Place(MBPoint3(0, 0, 200));
    }
    if (!mLoadBalloonString)
        mLoadBalloonString = CreateAnim("loadstring");
    if (!mExitButton)
        mExitButton = CreateButton("exit", 404, 405);
    if (sIsGameInProgress && !mReturnToGameButton)
        mReturnToGameButton = CreateButton("return", 406, -1);
    // no saved game: the load button stays, but does not react
    for (long game = 1; game <= 8; game++) {
        if (mSavedGameDB.IsGameUsed(game))
            return;
    }
    if (mLoadButton) {
        MBComPtr<IMBMouseable> loadMouseable(mLoadButton);
        if (loadMouseable)
            loadMouseable->Deactivate();
    }
}

// 0x100853a0 ?RemoveMainMenuButtons@SBMainMenuNode@@IAEXXZ
void SBMainMenuNode::RemoveMainMenuButtons()
{
    RemoveButton(mNewButton);
    RemoveAnim(mNewBalloonString);
    RemoveButton(mLoadButton);
    RemoveAnim(mLoadBalloonString);
    RemoveButton(mExitButton);
    RemoveButton(mReturnToGameButton);
}

// 0x10085420 ?CreateLoadGameSelector@SBMainMenuNode@@IAEXXZ
void SBMainMenuNode::CreateLoadGameSelector()
{
    RemoveTitleScreen();
    GetLoadGameSelectorData(mLoadGameSelectorData);
    mLoadBoard = CreateAnim("loadboard");
    mLeftLoadArrow = CreateButton("leftLoadGameArrow", 450, -1);
    if (mLeftLoadArrow)
        mLeftLoadArrow->Place(MBPoint3(0, 0, 200));
    mRightLoadArrow = CreateButton("rightLoadGameArrow", 451, -1);
    if (mRightLoadArrow)
        mRightLoadArrow->Place(MBPoint3(0, 0, 200));
    mLoadGameIcons = CreateAnim("loadGameIcons");
    if (mLoadGameIcons)
        mLoadGameIcons->Place(MBPoint3(0, 0, 200));
    mLoadGameThumbnails = CreateAnim("gameThumbnails");
    if (mLoadGameThumbnails)
        mLoadGameThumbnails->Place(MBPoint3(0, 0, 200));
    UpdateLoadGameSelector();
    mBackButton = CreateButton("back", 452, -1);
    mLetsGoButton = CreateButton("letsgo", 453, -1);
}

// 0x10085600 ?RemoveLoadGameSelector@SBMainMenuNode@@IAEXXZ
void SBMainMenuNode::RemoveLoadGameSelector()
{
    RemoveAnim(mLoadBoard);
    RemoveButton(mLeftLoadArrow);
    RemoveButton(mRightLoadArrow);
    RemoveAnim(mLoadGameIcons);
    RemoveAnim(mLoadGameThumbnails);
    RemoveButton(mBackButton);
    RemoveButton(mLetsGoButton);
}

// 0x10085690 ?UpdateLoadGameSelector@SBMainMenuNode@@IAEXXZ
void SBMainMenuNode::UpdateLoadGameSelector()
{
    if (mLoadGameIcons) {
        SetGameIcon(mLoadGameSelectorData[mLoadGameSelectorIndex].gameIconIndex + 1);
        mLoadGameIcons->SetCurrentSequence(mLoadGameSelectorData[mLoadGameSelectorIndex].gameIconIndex);
    }
    if (mLoadGameThumbnails)
        mLoadGameThumbnails->SetCurrentSequence(mLoadGameSelectorData[mLoadGameSelectorIndex].thumbnailIndex);
}

// 0x10085720 ?GetLoadGameSelectorData@SBMainMenuNode@@IAEXAAV?$MBVector@ULoadGameSelectorData@@@@@Z
void SBMainMenuNode::GetLoadGameSelectorData(MBVector<LoadGameSelectorData> &data)
{
    // The scene codes in thumbnail order.
    MBString sceneCodes[27] = {
        "BK", "CE", "CR", "CS", "DA", "DH", "ER", "FI", "GR", "HH", "HL", "IY", "MH", "MS",
        "MX", "NB", "NH", "PD", "PE", "PK", "PP", "PP", "RR", "SW", "VL", "VD", "DH"
    };
    for (long game = 1; game <= 8; game++) {
        if (mSavedGameDB.IsGameUsed(game)) {
            LoadGameSelectorData dataItem;
            MBString scene;
            long iconIndex;
            short thumbnailIndex = 0;
            mSavedGameDB.GetGameIcon(game, iconIndex);
            dataItem.gameIconIndex = (short)(iconIndex - 1);
            mSavedGameDB.GetGameLastSean(game, scene);
            for (int i = 0; i < 27; i++) {
                if (sceneCodes[i] == scene) {
                    thumbnailIndex = i;
                    break;
                }
            }
            dataItem.thumbnailIndex = thumbnailIndex;
            data.push_back(dataItem);
        }
    }
}

// 0x100863d0 ?CreateCharacterSelectionButtons@SBMainMenuNode@@IAEXXZ
void SBMainMenuNode::CreateCharacterSelectionButtons()
{
    if (!mBackButton)
        mBackButton = CreateButton("back", 500, -1);
    if (!mMickeyHotRect)
        mMickeyHotRect = CreateHotRect(MBRect(84, 267, 166, 387), 501);
    else
        mMickeyHotRect->Activate();
    if (!mMinnieHotRect)
        mMinnieHotRect = CreateHotRect(MBRect(449, 263, 523, 401), 502);
    else
        mMinnieHotRect->Activate();
    mMickeyHotRect->SetClickMessage(503);
    mMinnieHotRect->SetClickMessage(504);
}

// 0x100864f0 ?RemoveCharacterSelectionButtons@SBMainMenuNode@@IAEXXZ
void SBMainMenuNode::RemoveCharacterSelectionButtons()
{
    if (mMinnieHotRect)
        mMinnieHotRect->Deactivate();
    if (mMickeyHotRect)
        mMickeyHotRect->Deactivate();
    RemoveButton(mBackButton);
}

// TODO 0x10086540 ?CreateDifficultyButtons@SBMainMenuNode@@IAEXXZ: the original inlines the release of the first CreateAnim temporary (minnie) and calls MBComPtr<IMBAnim>::Release for the second; VC6 calls it for both here (switch order, if/else and a default case tried) (89% similar)
void SBMainMenuNode::CreateDifficultyButtons()
{
    CreateTitleScreen();
    RemoveCharacterSelectionButtons();
    MBString characterName;
    if (!mThoughtBubble) {
        switch (GetChosenCharacter()) {
        case 2:
            characterName = "minnie";
            mThoughtBubble = CreateAnim("minniebubble");
            break;
        case 1:
            characterName = "mickey";
            mThoughtBubble = CreateAnim("mickeybubble");
            break;
        }
        mThoughtBubble->Place(MBPoint3(0, 0, 150));
    }
    MBString buttonName;
    if (!mEasyButton) {
        buttonName = "easy" + characterName;
        mEasyButton = CreateButton(buttonName.c_str(), 600, 601);
        if (mEasyButton)
            MBComPtr<IMBObject>(mEasyButton)->Move(MBPoint3(0, 0, 160));
    }
    if (!mHardButton) {
        buttonName = "hard" + characterName;
        mHardButton = CreateButton(buttonName.c_str(), 602, 603);
        if (mHardButton)
            MBComPtr<IMBObject>(mHardButton)->Move(MBPoint3(0, 0, 160));
    }
    if (!mBackButton)
        mBackButton = CreateButton("back", 604, -1);
}

// 0x10086990 ?RemoveDifficultyButtons@SBMainMenuNode@@IAEXXZ
void SBMainMenuNode::RemoveDifficultyButtons()
{
    RemoveButton(mEasyButton);
    RemoveButton(mHardButton);
    RemoveButton(mBackButton);
    RemoveAnim(mThoughtBubble);
}

// 0x100869e0 ?CreateGameIcons@SBMainMenuNode@@IAEXXZ
void SBMainMenuNode::CreateGameIcons()
{
    RemoveTitleScreen();
    if (!mGameButtonBoard)
        mGameButtonBoard = CreateAnim("gameboard");
    for (int i = 0; i < 8; i++) {
        if (!mGameButtons[i]) {
            char buttonName[16];
            sprintf(buttonName, "icon%d", i + 1);
            if (mSavedGameDB.IsGameUsed(i + 1))
                sprintf(buttonName, "%sbusy", buttonName);
            mGameButtons[i] = CreateButton(buttonName, i + 701, -1);
            mGameButtons[i]->Place(MBPoint3(0, 0, 200));
        } else if (mEraseGameIndex == i + 1 && !mSavedGameDB.IsGameUsed(i + 1)) {
            // the game was just erased: the icon is free again
            RemoveButton(mGameButtons[i]);
            char buttonName[16];
            sprintf(buttonName, "icon%d", i + 1);
            mGameButtons[i] = CreateButton(buttonName, i + 701, -1);
            mGameButtons[i]->Place(MBPoint3(0, 0, 200));
        }
    }
}

// 0x10086b70 ?RemoveGameIcons@SBMainMenuNode@@IAEXXZ
void SBMainMenuNode::RemoveGameIcons()
{
    RemoveAnim(mGameButtonBoard);
    for (int i = 0; i < 8; i++)
        RemoveButton(mGameButtons[i]);
}

// 0x10086bb0 ?RemoveUnselectedGameIcons@SBMainMenuNode@@IAEXXZ
void SBMainMenuNode::RemoveUnselectedGameIcons()
{
    long gameIcon = GetGameIcon();
    for (int i = 0; i < 8; i++) {
        if (mGameButtons[i] && gameIcon != i + 1)
            RemoveButton(mGameButtons[i]);
    }
    mGameButtons[gameIcon - 1]->SetButtonState(kButtonUp);
    MBComPtr<IMBMouseable> mouseable(mGameButtons[gameIcon - 1]);
    if (mouseable)
        mouseable->Deactivate();
}

// 0x10086ce0 ?RemoveSelectedGameIcon@SBMainMenuNode@@IAEXXZ
void SBMainMenuNode::RemoveSelectedGameIcon()
{
    RemoveButton(mGameButtons[GetGameIcon() - 1]);
}

// 0x10086d10 ?CreateIconSelectionButtons@SBMainMenuNode@@IAEXXZ
void SBMainMenuNode::CreateIconSelectionButtons()
{
    mBackButton = CreateButton("back", 700, -1);
}

// 0x10086d40 ?RemoveIconSelectionButtons@SBMainMenuNode@@IAEXXZ
void SBMainMenuNode::RemoveIconSelectionButtons()
{
    RemoveButton(mBackButton);
}

// 0x10086d60 ?CreateLetsGoButtons@SBMainMenuNode@@IAEXXZ
void SBMainMenuNode::CreateLetsGoButtons()
{
    mBackButton = CreateButton("back", 800, -1);
    mLetsGoButton = CreateButton("letsgo", 801, -1);
}

// 0x10086db0 ?RemoveLetsGoButtons@SBMainMenuNode@@IAEXXZ
void SBMainMenuNode::RemoveLetsGoButtons()
{
    RemoveButton(mBackButton);
    RemoveButton(mLetsGoButton);
}

// 0x10086de0 ?LetsGo@SBMainMenuNode@@IAEXXZ
void SBMainMenuNode::LetsGo()
{
    sCurrentGameIndex = (short)GetGameIcon();
    SendExitSceneEvent("MV");
}

// Template and inline instances this unit also emits:
// 0x100438f0 ??1?$MBComPtr@VIMBMouseable@@@@QAE@XZ
// 0x10081fb0 ??_GSBMainMenuNode@@UAEPAXI@Z
// 0x10081ff0 ??1?$MBVector@ULoadGameSelectorData@@@@QAE@XZ
// 0x10087240 ?Release@?$MBComPtr@VIMBMouseable@@@@QAEXXZ
// 0x100876c0 ??4?$MBComPtr@VIMBMouseable@@@@QAEAAV0@PAVIMBMouseable@@@Z

// Template and inline instances this unit also emits:
// 0x10086f40 ??1?$MBComPtr@VIMBObject@@@@QAE@XZ
// 0x10087320 ??Hstd@@YA?AV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@0@PBDABV10@@Z
// 0x100875a0 ?_QueryInterface@?$MBComPtr@VIMBObject@@@@AAEJABQAVISBAnimButton@@@Z

// Template and inline instances this unit also emits:
// 0x10082b00 ??1MfPickData@@QAE@XZ
// 0x10082b20 ??1MfMouseHookData@@QAE@XZ
// 0x10086f70 ?insert@?$vector@ULoadGameSelectorData@@V?$MBAllocator@ULoadGameSelectorData@@@@@std@@QAEPAULoadGameSelectorData@@PAU3@ABU3@@Z
// 0x100871c0 ?size@?$vector@ULoadGameSelectorData@@V?$MBAllocator@ULoadGameSelectorData@@@@@std@@QBEIXZ
// 0x100871f0 ?_Destroy@?$vector@ULoadGameSelectorData@@V?$MBAllocator@ULoadGameSelectorData@@@@@std@@IAEXPAULoadGameSelectorData@@0@Z
// 0x100872a0 ?_Ucopy@?$vector@ULoadGameSelectorData@@V?$MBAllocator@ULoadGameSelectorData@@@@@std@@IAEPAULoadGameSelectorData@@PBU3@0PAU3@@Z
// 0x100872e0 ?_Ufill@?$vector@ULoadGameSelectorData@@V?$MBAllocator@ULoadGameSelectorData@@@@@std@@IAEXPAULoadGameSelectorData@@IABU3@@Z
// 0x10087570 ?_Construct@std@@YAXPAULoadGameSelectorData@@ABU2@@Z

// Static initializers and their atexit stubs: the PDB numbers them by the original header set, which
// ours does not reproduce yet, so they cannot be claimed by name.
// 0x10081770 _$E53
// 0x10081790 _$E50
// 0x10081d60 _$E52
// 0x10081d80 _$E51
// 0x10087640 _$E74
// 0x10087680 _$E70
// 0x100876a0 _$E73
