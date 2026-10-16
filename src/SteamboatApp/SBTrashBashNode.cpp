#include "SBTrashBashNode.h"
#include "SBCursorItems.h" // every node unit of the original has the cursor item strings
#include <locale> // evidence: the original unit ends with the std::ctype<unsigned short>::id and std::collate<char>::id initializers
#include "ViewEventHandler.h"
#include "MfAi.h"
#include "MBRuntime.h"
#include "SBMouseEvent.h"
#include <stdio.h>

// Event channel of the level buttons (mLeveler).
const long kTrashBashLevelChannel = 1122;

// Where the two picked tiles fly to.
MBPoint3 kLeftCan(40, 332, 10000);
MBPoint3 kRightCan(90, 354, 10000);

// The board: 6 x 6 squares, the stacks higher towards the middle.
static const long kSlots[6] = { 1, 2, 3, 3, 2, 1 };
static const long kXCoords[36] = {
    87, 135, 178, 220, 259, 295, 121, 170, 213, 256, 295, 329, 160, 208, 253, 295, 333, 368,
    199, 248, 293, 335, 374, 410, 245, 294, 338, 379, 418, 460, 294, 343, 389, 428, 467, 499
};
static const long kYCoords[36] = {
    221, 193, 168, 144, 123, 102, 254, 223, 195, 169, 147, 125, 286, 255, 225, 199, 172, 148,
    326, 290, 258, 229, 201, 175, 366, 327, 292, 260, 230, 202, 411, 369, 329, 295, 262, 232
};

// What Goofy, Mickey and Minnie say during the game.
MBString kBadMatchVO("tb02bgf0010");
MBString kGoofyVO[] = { "tb02agf0010", "tb02agf0020", "tb02agf0030" };
unsigned long kGoofyVOID[] = { MBHashString(kGoofyVO[0].c_str()), MBHashString(kGoofyVO[1].c_str()),
                              MBHashString(kGoofyVO[2].c_str()) };
MBString kMickeyVO[] = { "tb10xmc0010", "tb11xmc0010", "tb11xmc0020", "tb11xmc0030", "tb10bmc0020" };
MBString kMinnieVO[] = { "tb10xmi0010", "tb11xmi0010", "tb11xmi0020", "tb11xmi0030", "tb10bmi0020" };

// 0x1009cb50 ??0SBTrashBashNode@@QAE@PAVIMBGameEngine@@PAVMfAiSpace@@@Z
SBTrashBashNode::SBTrashBashNode(IMBGameEngine *engine, MfAiSpace *space)
    : SBActivityNode(engine, space), mEmptyTrashAt(0), mLeftTile(0), mRightTile(0),
      mLeftTileSquare(0), mRightTileSquare(0), mReadyToPlay(true), mCansFilled(0),
      mGameStarted(false), mFirstMatch(true), mGameOverAt(0), mConsecutiveBadMatches(0),
      mLeveler(kTrashBashLevelChannel, 1)
{
    MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
    if (broadcaster)
        broadcaster->AddListener(kTrashBashLevelChannel, this);
    SetBackgroundMusicState(true);
}

// 0x1009cdb0 ??0SBTrashBashNode@@QAE@PAVIMBGameEngine@@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@Z
SBTrashBashNode::SBTrashBashNode(IMBGameEngine *engine, MBString spaceCode)
    : SBActivityNode(engine, spaceCode), mEmptyTrashAt(0), mLeftTile(0), mRightTile(0),
      mLeftTileSquare(0), mRightTileSquare(0), mReadyToPlay(true), mCansFilled(0),
      mGameStarted(false), mFirstMatch(true), mGameOverAt(0), mConsecutiveBadMatches(0),
      mLeveler(kTrashBashLevelChannel, 1)
{
    MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
    if (broadcaster)
        broadcaster->AddListener(kTrashBashLevelChannel, this);
    SetBackgroundMusicState(true);
}

// 0x1009cfe0 ??1SBTrashBashNode@@UAE@XZ
SBTrashBashNode::~SBTrashBashNode()
{
}

// 0x1009d0d0 ?OnStart@SBTrashBashNode@@UAEXXZ
void SBTrashBashNode::OnStart()
{
    SetSceneBackgroundMusic("trashbash");
    SteamboatNode::OnStart();
    MBComPtr<ISBGameState> gs(MBIID_CSBGameState);
    long maxLevel = gs->GetProperty(TB_MAX_LEVEL_PROP_NAME, 1);
    mLeveler.SetLevelRange(1, maxLevel);
    mLeveler.Create(MBPoint3(0, 0, 155), 1);
    mLeveler.SetLevel(maxLevel);
    LoadTrash();
}

// 0x1009d240 ?OnEnd@SBTrashBashNode@@UAEXXZ
void SBTrashBashNode::OnEnd()
{
    ClearBoard();
    if (mLeftTile)
        mLeftTile->Release();
    if (mRightTile)
        mRightTile->Release();
    mLeveler.Destroy();
    MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
    if (broadcaster)
        broadcaster->RemoveListener(kTrashBashLevelChannel, this);
    SteamboatNode::OnEnd();
}

// TODO 0x1009d360 ?LoadTrash@SBTrashBashNode@@IAEXXZ: the loop runs while count <= numTrash - 1 and the random square index is taken before indexing, as in the original; left: the original reads the object pointer before the GetRoot call (VC6 here after it), and the locals sit in other stack slots (76% similar)
void SBTrashBashNode::LoadTrash()
{
    ClearBoard();
    InitGameboard();
    mPairs = 0;
    MBComPtr<IMBFileSpec> fileSpec(MBIID_IMBFileSpec);
    if (fileSpec) {
        MBString prefFile = mSpaceCode + "scene.ini";
        IMBStream *tempStream;
        fileSpec->SetPath(prefFile.c_str());
        MBComPtr<IMBStream> stream;
        MBOpenFile(fileSpec, 5, &tempStream);
        stream.Attach(tempStream);
        if (stream) {
            // the trash of this level: "trash0" .. in [trash items]
            MBComPtr<IMBPref> pref(MBIID_CMBPref);
            IMBUnknown *unknown;
            IMBObject *object;
            ISBAnimButton *button;
            MBVector<MBComPtr<IMBAnim> > anims;
            int count = 0;
            long numTrash = mLeveler.GetLevel() + 2;
            char trashName[12] = "trash0";
            MBString animName;
            MBComPtr<IMBAnim> anim;
            while (count <= numTrash - 1) {
                bool found = pref->GetString(stream, MBString("trash items"), MBString(trashName), animName, MBString("")) > 0;
                if (!found)
                    break;
                unsigned long id = MBHashString(animName.c_str());
                IMBStream *animStream;
                if (GetStream(id, 'anm ', animStream) == 0) {
                    anim.CreateObject(MBIID_CMBAnim);
                    anim->Load(id, animStream, false);
                    anims.push_back(anim);
                    if (animStream) {
                        animStream->Release();
                        animStream = 0;
                    }
                }
                sprintf(trashName, "trash%d", ++count);
            }

            // two tiles of each kind, each on a random open square
            MBVector<MBComPtr<IMBAnim> >::iterator it = anims.begin();
            while (mOpenSquares.size()) {
                for (int i = 2; i; i--) {
                    MBComCreateInstance(0, MBIID_CSBTrashTile, &unknown);
                    unknown->QueryInterface(MBIID_ISBAnimButton, (void **)&button);
                    IMBAnim *tileAnim;
                    (*it)->Clone(&tileAnim);
                    if (tileAnim) {
                        button->SetAnim(tileAnim);
                        bool placed = false;
                        while (mOpenSquares.size() && !placed) {
                            int index = MBrandom(0, mOpenSquares.size() - 1);
                            SBTrashSquare **square = &mOpenSquares[index];
                            placed = (*square)->PushTile((CSBTrashTile *)button);
                            if (!placed)
                                mOpenSquares.erase(square);
                        }
                        if (placed) {
                            button->QueryInterface(MBIID_IMBObject, (void **)&object);
                            mWorld->GetRoot().AddObject(object);
                            MBComPtr<IMBMouseable> mouseable(object);
                            mMouseMonitor->AddMouseable(mouseable);
                            object->Release();
                        }
                    }
                    button->Release();
                }
                mPairs++;
                if (++it == anims.end())
                    it = anims.begin();
            }
        }
    }
}

// TODO 0x1009db70 ?InitGameboard@SBTrashBashNode@@IAEXXZ: the original walks kSlots with pointers and keeps &mLeveler and &mTrashSquares on the stack (index forms tried) (50% similar)
void SBTrashBashNode::InitGameboard()
{
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 6; j++) {
            MBPoint3 pos(kXCoords[i * 6 + j], kYCoords[i * 6 + j], 0);
            MBPoint coord(i, j);
            long layers = kSlots[i] < kSlots[j] ? kSlots[i] : kSlots[j];
            // the higher levels pile the trash higher
            long height = 1;
            switch (mLeveler.GetLevel()) {
            case 1:
            case 2:
            case 3:
                height = 1;
                break;
            case 4:
            case 5:
            case 6:
                height = 2;
                break;
            case 7:
            case 8:
            case 9:
                height = 3;
                break;
            case 10:
                height = 4;
                break;
            }
            SBTrashSquare *sqr = new SBTrashSquare(pos, coord, height * layers);
            mTrashSquares.push_back(sqr);
            mOpenSquares.insert(mOpenSquares.end(), 1, sqr);
        }
    }
}

// 0x1009df20 ?ClearBoard@SBTrashBashNode@@IAEXXZ
void SBTrashBashNode::ClearBoard()
{
    SBTrashSquare **square = mTrashSquares.begin();
    while (square != mTrashSquares.end()) {
        delete *square;
        mTrashSquares.erase(square);
    }
    square = mOpenSquares.begin();
    while (square != mOpenSquares.end()) {
        delete *square;
        mOpenSquares.erase(square);
    }
}

// 0x1009e000 ?OnActivityEvent@SBTrashBashNode@@MAEXJPAX@Z
void SBTrashBashNode::OnActivityEvent(long event, void *data)
{
    switch (event) {
    case 401:
        // a square was clicked
        StartSound("TBSEpickupTrash");
        PlayTile((SBTrashSquare *)data);
        break;
    case 402:
        // a tile reached its trash can
        if (data == mLeftTile) {
            mLeftTile = 0;
            mLeftTileSquare = 0;
        } else if (data == mRightTile) {
            mRightTile = 0;
            mRightTileSquare = 0;
        }
        mReadyToPlay = --mCansFilled == 0;
        break;
    }
}

// 0x1009e0a0 ?PlayTile@SBTrashBashNode@@IAEXPAVSBTrashSquare@@@Z
void SBTrashBashNode::PlayTile(SBTrashSquare *square)
{
    if (!mReadyToPlay || mCansFilled >= 2)
        return;
    if (!mLeftTile) {
        mCansFilled = 1;
        mLeftTile = square->PopTile();
        mLeftTileSquare = square;
        mLeftTile->Place(kLeftCan);
        mLeftTile->SetClickMessage(-1);
        mGameStarted = true;
    } else {
        // the second tile: compare the pair a moment later
        mCansFilled = 2;
        mReadyToPlay = false;
        mRightTile = square->PopTile();
        mRightTileSquare = square;
        mRightTile->Place(kRightCan);
        mRightTile->SetClickMessage(-1);
        MBComPtr<IMBGameClock> clock(MBIID_IMBGameClock);
        mEmptyTrashAt = clock->GetGameTime() + 500;
    }
}

// TODO 0x1009e270 ?Idle@SBTrashBashNode@@UAEXXZ: MBComPtr inlining: the original calls clock's CreateObject out of line and inlines the later two GUID constructors; VC6 inlines all three (also with an explicit CreateObject call) (60% similar)
void SBTrashBashNode::Idle()
{
    SteamboatNode::Idle();
    MBComPtr<IMBGameClock> clock(MBIID_IMBGameClock);
    if (mEmptyTrashAt && clock->GetGameTime() > mEmptyTrashAt) {
        // the two picked tiles have arrived at the cans
        if (mRightTile->Compare(mLeftTile)) {
            StartSound("TBSEmatchSuccess");
            mWorld->GetRoot().RemoveObject((ISBAnimButton *)mLeftTile);
            mWorld->GetRoot().RemoveObject((ISBAnimButton *)mRightTile);
            mLeftTile->Release();
            mRightTile->Release();
            mLeftTileSquare->Enable();
            mRightTileSquare->Enable();
            mLeftTile = 0;
            mLeftTileSquare = 0;
            mRightTile = 0;
            mRightTileSquare = 0;
            mPairs--;
            mCansFilled = 0;
            if (IsBoardCleared()) {
                StartSound("TBSEwinLevel");
                MBComPtr<IMBGameClock> gameClock(MBIID_IMBGameClock);
                mGameOverAt = gameClock->GetGameTime() + 1000;
                MBComPtr<ISBGameState> gs(MBIID_CSBGameState);
                long maxLevel = gs->GetProperty(TB_MAX_LEVEL_PROP_NAME, 1);
                long nextLevel = mLeveler.GetLevel() + 1;
                if (nextLevel > maxLevel)
                    maxLevel = nextLevel;
                if (maxLevel > 10)
                    maxLevel = 10;
                gs->SetProperty(TB_MAX_LEVEL_PROP_NAME, maxLevel);
                mLeveler.SetLevelRange(mLeveler.GetMinLevel(), maxLevel);
            } else {
                mReadyToPlay = true;
                if (mFirstMatch) {
                    StartDialogue(kGoofyVO[0].c_str(), false);
                    mFirstMatch = false;
                } else {
                    long chance = MBrandom(1, 15);
                    if (chance > 0 && chance <= 3)
                        StartDialogue(kGoofyVO[MBrandom(0, 2)].c_str(), false);
                }
            }
        } else {
            // no match: every fourth bad match gets a comment, the tiles fly back
            mConsecutiveBadMatches = (mConsecutiveBadMatches + 1) % 4;
            if (mConsecutiveBadMatches == 1)
                StartDialogue(kBadMatchVO.c_str(), false);
            mLeftTile->Translate(mLeftTileSquare, 500);
            mRightTile->Translate(mRightTileSquare, 500);
        }
        mEmptyTrashAt = 0;
    }
    if (mGameOverAt && clock->GetGameTime() > mGameOverAt) {
        mGameOverAt = 0;
        MBComPtr<ISBGameState> gs(MBIID_ISBGameState);
        gs->SetProperty("PPTrashBashingComplete", 1);
        gs->SetProperty("PPEnterFromTB", 1);
        mLeveler.SetLevel(mLeveler.GetMaxLevel());
        ResetGame();
        LoadTrash();
        mReadyToPlay = true;
    }
}

// 0x1009edd0 ?IsBoardCleared@SBTrashBashNode@@IAE_NXZ
bool SBTrashBashNode::IsBoardCleared()
{
    for (SBTrashSquare **square = mTrashSquares.begin(); square != mTrashSquares.end(); square++) {
        if (!(*square)->IsEmpty())
            return false;
    }
    return true;
}

// 0x1009ee10 ?ListenToEvent@SBTrashBashNode@@UAEXJJPAX@Z
void SBTrashBashNode::ListenToEvent(long channel, long event, void *data)
{
    if (channel == kTrashBashLevelChannel) {
        // a new level was chosen
        mReadyToPlay = false;
        ResetGame();
        LoadTrash();
        mReadyToPlay = true;
    }
    SBActivityNode::ListenToEvent(channel, event, data);
}

// 0x1009ee80 ?ResetGame@SBTrashBashNode@@IAEXXZ
void SBTrashBashNode::ResetGame()
{
    mCansFilled = 0;
    if (mLeftTile) {
        mWorld->GetRoot().RemoveObject((ISBAnimButton *)mLeftTile);
        mLeftTile->Release();
    }
    if (mRightTile) {
        mWorld->GetRoot().RemoveObject((ISBAnimButton *)mRightTile);
        mRightTile->Release();
    }
    mLeftTile = 0;
    mLeftTileSquare = 0;
    mRightTile = 0;
    mRightTileSquare = 0;
    mEmptyTrashAt = 0;
    mGameOverAt = 0;
    mConsecutiveBadMatches = 0;
}

// Template and inline instances this unit also emits:
// 0x1009cd20 ??_GSBTrashBashNode@@UAEPAXI@Z
// 0x1009cd60 ??1?$MBVector@PAVSBTrashSquare@@@@QAE@XZ

// 0x1009e800 ?OnLMouseUp@SBTrashBashNode@@UAEXAAUMBMouseEvent@@@Z
void SBTrashBashNode::OnLMouseUp(MBMouseEvent &evt)
{
    SteamboatNode::OnLMouseUp(evt);
    MfPickData pickData;
    MfMouseHookData data;
    CopyMBMouseEventToMfMouseHookData(evt, data);
    MfAiActor *actor = sMotivateEventHandler->HandleLButtonUp(&data, pickData);
    if (!sCursorItem && actor) {
        // a click on Mickey or Minnie: a hint before the game, a cheer during it
        MBString actorName((char *)actor->GetClassNameA());
        if (actorName == "GDmickey") {
            if (!mGameStarted)
                StartDialogue(kMickeyVO[0].c_str(), false);
            else
                StartDialogue(kMickeyVO[MBrandom(1, 4)].c_str(), false);
        } else if (actorName == "GDminnie") {
            if (!mGameStarted)
                StartDialogue(kMinnieVO[0].c_str(), false);
            else
                StartDialogue(kMinnieVO[MBrandom(1, 4)].c_str(), false);
        }
    }
}

// 0x1009eac0 ?OnLMouseDown@SBTrashBashNode@@UAEXAAUMBMouseEvent@@@Z
void SBTrashBashNode::OnLMouseDown(MBMouseEvent &evt)
{
    SteamboatNode::OnLMouseDown(evt);
    MBPoint worldPoint;
    IMBWorld *world;
    if (!sCursorItem && MBGetWorldMouseLocation(worldPoint, &world)) {
        // a click on a tile that cannot be picked yet gets a hint
        for (SBTrashSquare **i = mTrashSquares.begin(); i != mTrashSquares.end(); i++) {
            CSBTrashTile *tile = (*i)->PeekTopTile();
            if (tile && tile->GetAnim() != 0) {
                MBRect animBounds = tile->GetAnim()->GetBounds();
                if (animBounds.ContainsPoint(worldPoint) && !tile->IsEnabled()) {
                    SBMouseEvent *blocked = new SBMouseEvent;
                    blocked->data = "TBBlockedTile";
                    MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
                    broadcaster->BroadcastEvent(700, 710, blocked);
                    return;
                }
            }
        }
    }
}

// Template and inline instances this unit also emits:
// 0x1000ba20 ??0MfVector3@@QAE@XZ
// 0x1000ba40 ??1MfVector3@@QAE@XZ
// 0x10010110 ??3@YAXPAX0@Z
// 0x1001b430 ?Release@?$MBComPtr@VIMBStream@@@@QAEXXZ
// 0x1009ef50 ??0?$vector@V?$MBComPtr@VIMBAnim@@@@V?$MBAllocator@V?$MBComPtr@VIMBAnim@@@@@@@std@@QAE@ABV?$MBAllocator@V?$MBComPtr@VIMBAnim@@@@@@@Z
// 0x1009ef80 ?_Destroy@?$vector@PAVSBTrashSquare@@V?$MBAllocator@PAVSBTrashSquare@@@@@std@@IAEXPAPAVSBTrashSquare@@0@Z
// 0x1009efa0 ?insert@?$vector@PAVSBTrashSquare@@V?$MBAllocator@PAVSBTrashSquare@@@@@std@@QAEXPAPAVSBTrashSquare@@IABQAV3@@Z
// 0x1009f240 ?_Ucopy@?$vector@PAVSBTrashSquare@@V?$MBAllocator@PAVSBTrashSquare@@@@@std@@IAEPAPAVSBTrashSquare@@PBQAV3@0PAPAV3@@Z
// 0x1009f280 ?_Ufill@?$vector@PAVSBTrashSquare@@V?$MBAllocator@PAVSBTrashSquare@@@@@std@@IAEXPAPAVSBTrashSquare@@IABQAV3@@Z
// 0x1009f2c0 ??_G?$MBComPtr@VIMBAnim@@@@QAEPAXI@Z
// 0x1009f300 ?_QueryInterface@?$MBComPtr@VIMBMouseable@@@@AAEJABQAVIMBObject@@@Z

// TODO 0x1009ef20 ?size@?$vector@PAVSBTrashSquare@@V?$MBAllocator@PAVSBTrashSquare@@@@@std@@QBEIXZ: template/inline instance, not emitted: our compile inlines every use

// Static initializers and their atexit stubs: the PDB numbers them by the original header set, which
// ours does not reproduce yet, so they cannot be claimed by name.
// 0x1009bb40 _$E49
// 0x1009bb60 _$E46
// 0x1009c130 _$E48
// 0x1009c150 _$E47
// 0x1009c180 _$E54
// 0x1009c1a0 _$E53
// 0x1009c1d0 _$E57
// 0x1009c1f0 _$E56
// 0x1009c220 _$E62
// EQUIV 0x1009c240 _$E59: reviewed: kBadMatchVO's string construction; the original reloads the capacity only on the path that tests it (the block after the never-returning _Xlen), VC6 here reloads it with the data pointer; same tests, calls and stores
// 0x1009c360 _$E61
// 0x1009c380 _$E60
// 0x1009c3f0 _$E67
// 0x1009c410 _$E64
// 0x1009c5c0 _$E66
// 0x1009c5e0 _$E65
// 0x1009c610 _$E70
// 0x1009c630 _$E69
// 0x1009c6b0 _$E75
// 0x1009c6d0 _$E72
// 0x1009c8b0 _$E74
// 0x1009c8d0 _$E73
// 0x1009c900 _$E80
// 0x1009c920 _$E77
// 0x1009cb00 _$E79
// 0x1009cb20 _$E78
// 0x1009f3a0 _$E92
// 0x1009f3e0 _$E88
// 0x1009f400 _$E91
