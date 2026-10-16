#include "SBSongbirdNode.h"
#include "SBCursorItems.h" // every node unit of the original has the cursor item strings
#include <locale> // evidence: the original unit ends with the std::ctype<unsigned short>::id and std::collate<char>::id initializers
#include "MfAi.h"
#include "MfString.h"
#include "MBRuntime.h"
#include "SBMouseEvent.h"
#include <stdio.h>

// Where the seven songbirds sit on their roost (z is the layer).
const MBPointStruct3 kRoostPoint[7] = {
    { 88, 348, 150 }, { 111, 347, 150 }, { 136, 347, 150 }, { 151, 347, 150 },
    { 170, 350, 150 }, { 189, 340, 150 }, { 210, 347, 150 }
};

// The song grid: a click inside it puts the picked bird on the nearest wire.
static MBRect kSongGridRect(0, 0, 640, 240);

typedef std::pair<const MBComPtr<ISBAnimButton>, char> BirdNotePair;

// 0x10093130 ??0SBSongbirdNode@@QAE@PAVIMBGameEngine@@PAVMfAiSpace@@@Z
SBSongbirdNode::SBSongbirdNode(IMBGameEngine *engine, MfAiSpace *space)
    : SBActivityNode(engine, space), mSongBook(0), mTrash(0), mNoteToPlay(0), mSongPtr(0), mTimeToPlay(0),
      mLastNotePos(0), mCnt(0), mMouseClicked(false), mClickHandled(false), mCurrentSong(0)
{
}

// 0x100934c0 ??0SBSongbirdNode@@QAE@PAVIMBGameEngine@@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@Z
SBSongbirdNode::SBSongbirdNode(IMBGameEngine *engine, MBString spaceCode)
    : SBActivityNode(engine, spaceCode), mSongBook(0), mTrash(0), mNoteToPlay(0), mSongPtr(0), mTimeToPlay(0),
      mLastNotePos(0), mCnt(0), mMouseClicked(false), mClickHandled(false), mCurrentSong(1)
{
}

// EQUIV 0x100936e0 ??1SBSongbirdNode@@UAE@XZ: reviewed: the original inlines MBComPtr::Release (if held, Release through the vftable, then clear) for the grid squares in the inlined vector destructor, VC6 here calls it; nothing else differs
SBSongbirdNode::~SBSongbirdNode()
{
}

// 0x10093980 ?OnStart@SBSongbirdNode@@UAEXXZ
void SBSongbirdNode::OnStart()
{
    SteamboatNode::OnStart();
    mSavedCursor = mCursor;
    PlaceNests();
    InitSongGrid();
    LoadSongs();

    MBComPtr<IMBAnim> playAnim = CreateAnim("SBSPplayButton");
    MBComPtr<ISBAnimButton> play(MBIID_CSBAnimButton);
    play->SetAnim(playAnim);
    play->SetClickMessage(412);
    play->SetBroadcastChannel(400);
    play->Place(MBPoint3(0, 0, 150));
    mWorld->AddObject(play);
    MBComPtr<IMBMouseable> playMouseable(play);
    mMouseMonitor->AddMouseable(playMouseable);

    // the song book comes with the sheet music found in the game
    MBComPtr<ISBGameState> gs(MBIID_CSBGameState);
    mFirstSong = 1;
    if (gs->GetProperty("SBhasSongbook", 1)) {
        RemoveItemFromInventory(MBString("PRsheetmusic0"));
        mFirstSong = 0;
    }

    MBComPtr<IMBAnim> bookAnim = CreateAnim("SBSPsongbook");
    if (bookAnim) {
        MBComPtr<ISBAnimButton> book(MBIID_CSBAnimButton);
        book->SetAnim(bookAnim);
        book->SetClickMessage(413);
        book->SetBroadcastChannel(400);
        mWorld->AddObject(book);
        MBComPtr<IMBMouseable> bookMouseable(book);
        mMouseMonitor->AddMouseable(bookMouseable);
    }

    MBComPtr<IMBAnim> trashAnim = CreateAnim("SBtrash");
    if (trashAnim) {
        MBComPtr<ISBAnimButton> trash(MBIID_CSBAnimButton);
        trash->SetAnim(trashAnim);
        trash->SetClickMessage(415);
        trash->SetBroadcastChannel(400);
        mWorld->AddObject(trash);
        MBComPtr<IMBMouseable> trashMouseable(trash);
        mMouseMonitor->AddMouseable(trashMouseable);
    }

    MBComPtr<IMBAnim> cameraAnim = CreateAnim("SBSPcamera");
    if (cameraAnim) {
        MBComPtr<ISBAnimButton> camera(MBIID_CSBAnimButton);
        camera->SetAnim(cameraAnim);
        camera->SetClickMessage(414);
        camera->SetRolloverMessage(416);
        camera->SetBroadcastChannel(400);
        mWorld->AddObject(camera);
        MBComPtr<IMBMouseable> cameraMouseable(camera);
        mMouseMonitor->AddMouseable(cameraMouseable);
    }

    MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
    broadcaster->AddListener(210, this);
    mEraseSongDialog.SetBackgroundName("SBerase");
    mEraseSongDialog.SetButtonNames("yes", "no");
}

// TODO 0x10094100 ?PlaceNests@SBSongbirdNode@@IAEXXZ: register allocation: the original keeps the bird letter in bl across the loop body and has a 4-byte smaller frame (60% similar)
void SBSongbirdNode::PlaceNests()
{
    MBComPtr<ISBGameState> gs(MBIID_CSBGameState);
    for (int i = 6; i >= 0; i--) {
        char name[10];
        sprintf(name, "SBSPbird%c", (char)('A' + i));
        MBComPtr<IMBAnim> birdAnim = CreateAnim(name);
        MBComPtr<IMBAnim> cursorBirdAnim = CreateAnim(name);

        // move the registration points of all casts to the bottom centre of the first one
        MBRect bounds = birdAnim->GetBounds();
        long yOffset = bounds.top - bounds.bottom;
        long xOffset = -((bounds.right - bounds.left) / 2);
        MBPoint c0Offset = birdAnim->GetCastMemberRegistrationPoint(1);
        unsigned long numCasts = birdAnim->GetNumCasts();
        for (unsigned long cast = 1; cast <= numCasts; cast++) {
            MBPoint reg = birdAnim->GetCastMemberRegistrationPoint(cast);
            birdAnim->SetCastMemberRegistrationPoint(cast,
                MBPoint(reg.x - c0Offset.x + xOffset, reg.y - c0Offset.y + yOffset));
        }

        mBirdNoteMap.insert(BirdNotePair(mSongbird[i], (char)('A' + i)));
        if (mSongbird[i].CreateObject(MBIID_ISBAnimButton) == 0) {
            mSongbird[i]->SetAnim(birdAnim);
            mSongbird[i]->Place(MBPoint3(kRoostPoint[i].x, kRoostPoint[i].y, kRoostPoint[i].z));
            mSongbird[i]->SetBroadcastChannel(400);
            mSongbird[i]->SetClickMessage(420 + i);
            mWorld->AddObject(mSongbird[i]);
            MBComPtr<IMBMouseable> mSongbirdMouseable(mSongbird[i]);
            mMouseMonitor->AddMouseable(mSongbirdMouseable);
        }
        if (mCursorSongbird[i].CreateObject(MBIID_ISBAnimButton) == 0) {
            mCursorSongbird[i]->SetAnim(cursorBirdAnim);
            mCursorSongbird[i]->Place(MBPoint3(0, 0, kRoostPoint[i].z));
            cursorBirdAnim->Hide();
            mWorld->AddObject(mCursorSongbird[i]);
        }
    }

    // Luciano, the high C, once he has flown in
    if (gs->GetProperty("IYLucianoHasFlown", 0)) {
        char name[10];
        sprintf(name, "SBSPbirdHigh%c", 'C');
        MBComPtr<IMBAnim> birdAnim = CreateAnim(name);

        MBRect bounds = birdAnim->GetBounds();
        long yOffset = bounds.top - bounds.bottom;
        long xOffset = -((bounds.right - bounds.left) / 2);
        MBPoint c0Offset = birdAnim->GetCastMemberRegistrationPoint(1);
        unsigned long numCasts = birdAnim->GetNumCasts();
        for (unsigned long cast = 1; cast <= numCasts; cast++) {
            MBPoint reg = birdAnim->GetCastMemberRegistrationPoint(cast);
            birdAnim->SetCastMemberRegistrationPoint(cast,
                MBPoint(reg.x - c0Offset.x + xOffset, reg.y - c0Offset.y + yOffset));
        }

        mBirdNoteMap.insert(BirdNotePair(mLuciano, 'C'));
        if (mLuciano.CreateObject(MBIID_ISBAnimButton) == 0) {
            mLuciano->SetAnim(birdAnim);
            mLuciano->Place(MBPoint3(228, 347, 1000));
            mLuciano->SetBroadcastChannel(400);
            mLuciano->SetClickMessage(429);
            mWorld->AddObject(mLuciano);
            MBComPtr<IMBMouseable> mSongbirdMouseable((IMBObject *)mLuciano);
            mMouseMonitor->AddMouseable(mSongbirdMouseable);
        }
    }
}

// TODO 0x100948e0 ?InitSongGrid@SBSongbirdNode@@IAEXXZ: the original loads the two wire points into integer temporaries and computes both products before the sums; ours adds through the point addresses (38% similar)
void SBSongbirdNode::InitSongGrid()
{
    // four wires, each a polyline of four points; eight squares between two points
    const MBPointStruct kWirePos[4][4] = {
        { { 133, 67 }, { 261, 76 }, { 397, 76 }, { 517, 64 } },
        { { 135, 111 }, { 260, 123 }, { 393, 118 }, { 514, 111 } },
        { { 139, 167 }, { 263, 180 }, { 395, 177 }, { 511, 162 } },
        { { 142, 223 }, { 261, 231 }, { 394, 232 }, { 507, 220 } }
    };
    for (int wire = 0; wire < 4; wire++) {
        for (int i = 0; i < 24; i++) {
            float dx = (float)(i % 8) * 0.125f;
            const MBPointStruct &p0 = kWirePos[wire][i / 8];
            const MBPointStruct &p1 = kWirePos[wire][i / 8 + 1];
            MBPoint3 refpt((long)(p0.x + (p1.x - p0.x) * dx), (long)(p0.y + (p1.y - p0.y) * dx), 0);
            MBComPtr<ISBSongbirdNote> note(MBIID_CSBSongbirdNote);
            if (note) {
                note->SetReferencePoint(refpt);
                mGridSquares.push_back(note);
            }
        }
    }
}

// EQUIV 0x10094c20 ?DrawSong@SBSongbirdNode@@IAEXXZ: reviewed: this and the square pointer swap esi/edi; otherwise identical
void SBSongbirdNode::DrawSong()
{
    // a song is one letter per grid square: 'a'.. a songbird, 'C' Luciano, 'x' nothing
    ClearSongboard();
    MBComPtr<ISBGameState> gs(MBIID_CSBGameState);
    MBComPtr<ISBSongbirdNote> *square = mGridSquares.begin();
    for (int i = 0; i < mSongPtr->length() && square != mGridSquares.end(); i++, square++) {
        char note = (*mSongPtr)[i];
        switch (note) {
        case 'x':
            (*square)->SetBird(0);
            (*square)->SetNote('x');
            break;
        case 'C':
            if (gs->GetProperty("IYLucianoHasFlown", 0))
                (*square)->SetBird(CloneAnim(mLuciano->GetAnim()));
            (*square)->SetNote('C');
            break;
        default:
            (*square)->SetBird(CloneAnim(mSongbird[note - 'a']->GetAnim()));
            (*square)->SetNote(note);
            break;
        }
    }
}

// 0x10094e90 ?ClearSongboard@SBSongbirdNode@@IAEXXZ
void SBSongbirdNode::ClearSongboard()
{
    // take every bird off the grid
    MBComPtr<ISBSongbirdNote> *note = mGridSquares.begin();
    while (note != mGridSquares.end()) {
        MBComPtr<IMBAnim> bird((*note)->GetBird());
        if (bird) {
            bird->Stop();
            mWorld->GetRoot().RemoveObject(bird);
            (*note)->SetBird(0);
        }
        note++;
    }
}

// 0x10094f80 ?LoadSongs@SBSongbirdNode@@IAEXXZ
void SBSongbirdNode::LoadSongs()
{
    IMBStream *stream = OpenPrefStream();
    if (stream) {
        MBComPtr<IMBPref> pref(MBIID_CMBPref);
        int iSong = 0;
        char key[8] = "song0";
        MBString song;
        while (pref->GetString(stream, "songs", key, song, "") > 0) {
            if (song.length())
                mSongs.push_back(song);
            iSong++;
            sprintf(key, "song%d", iSong);
        }
        stream->Release();
    }
    MBDebugStr("[SBSongbirdNode::LoadSongs] songs loaded: %d\n", mSongs.size());
}

// TODO 0x100952a0 ?SaveSong@SBSongbirdNode@@IAEXXZ: inline budget: the original calls append(1, c) and the single-element insert out of line and has a larger frame (69% similar)
void SBSongbirdNode::SaveSong()
{
    // the song is the note of every grid square
    MBString song;
    for (MBComPtr<ISBSongbirdNote> *note = mGridSquares.begin(); note != mGridSquares.end(); note++)
        song += (*note)->GetNote();
    mSongs.insert(mSongs.end(), song);
    MBDebugStr("(INFO) no. of songs: %d\n", mSongs.size());
    mSongPtr = mSongs.end();
    mCurrentSong = mSongs.size();

    IMBStream *stream = OpenPrefStream();
    if (stream) {
        char key[8];
        sprintf(key, "song%d", mSongs.size() - 1);
        MBComPtr<IMBPref> pref(MBIID_CMBPref);
        pref->SetString(stream, "songs", key, MBString(song));
        stream->Release();
    }
}

// 0x10095630 ?Idle@SBSongbirdNode@@UAEXXZ
void SBSongbirdNode::Idle()
{
    MBComPtr<ISBGameState> gs(MBIID_CSBGameState);
    MBComPtr<IMBGameClock> clock(MBIID_IMBGameClock);
    long now = clock->GetGameTime();
    if (mTimeToPlay && now > mTimeToPlay) {
        // play the next square of the song
        (*mNoteToPlay)->Play();
        mCnt++;
        mNoteToPlay++;
        if (mNoteToPlay != mGridSquares.end()) {
            if (mLastNotePos == mCnt) {
                {
                    MfString strMessage("hc_stop_conducting");
                    GetMfPrimaryActor()->SendMessageA(0, strMessage);
                }
                // the lullaby is the first song, sung with Luciano
                if (mCurrentSong == 1 && gs->GetProperty("IYLucianoHasFlown", 0))
                    gs->SetProperty("IYlullaby", 1);
            }
            mTimeToPlay = now + 200;
        } else
            mTimeToPlay = 0;
    }
    if (mMouseClicked && !mClickHandled) {
        // a click beside the grid drops the bird
        mBirdPicked = 0;
        SetCursor(mSavedCursor, 0, 0);
    }
    mMouseClicked = false;
    mClickHandled = false;
    SteamboatNode::Idle();
}

// 0x100958f0 ?OnEnd@SBSongbirdNode@@UAEXXZ
void SBSongbirdNode::OnEnd()
{
    mEraseSongDialog.Destroy();
    MBComPtr<IMBEventManager> eventManager(MBIID_IMBEventManager);
    eventManager->RemoveListener(210, this);
    SteamboatNode::OnEnd();
}

// 0x10095a10 ?StartListening@SBSongbirdNode@@UAEXXZ
void SBSongbirdNode::StartListening()
{
    MBComPtr<IMBEventManager> eventManager(MBIID_IMBEventManager);
    if (eventManager)
        eventManager->AddListener(-1, this);
    SBActivityNode::StartListening();
}

// 0x10095af0 ?StopListening@SBSongbirdNode@@UAEXXZ
void SBSongbirdNode::StopListening()
{
    MBComPtr<IMBEventManager> eventManager(MBIID_IMBEventManager);
    if (eventManager)
        eventManager->RemoveListener(-1, this);
    SBActivityNode::StopListening();
}

// EQUIV 0x10095bd0 ?ListenToEvent@SBSongbirdNode@@UAEXJJPAX@Z: reviewed: in the inlined string assignment of the vector erase the original limits the length against npos loaded from memory, VC6 here against the constant -1 (npos is -1), and keeps the end pointer in esi instead of reloading it from its slot; same calls and arguments
void SBSongbirdNode::ListenToEvent(long channel, long event, void *data)
{
    if (channel == -1) {
        if (event == 6) {
            MBMouseEvent *click = (MBMouseEvent *)data;
            if (kSongGridRect.ContainsPoint(click->mMousePos))
                OnGridClick(click);
            else
                mMouseClicked = true;
        }
    } else if (channel == 210) {
        switch (event) {
        case 211:
            // "yes": erase the song shown
            if (mEraseSongDialog.IsActive()) {
                mSongPtr = mSongs.erase(--mSongPtr);
                OnSongbookClick(0);
                mEraseSongDialog.Destroy();
            }
            break;
        case 212:
            if (mEraseSongDialog.IsActive())
                mEraseSongDialog.Destroy();
            break;
        }
    }
    SBActivityNode::ListenToEvent(channel, event, data);
}

// 0x10095ee0 ?OnActivityEvent@SBSongbirdNode@@MAEXJPAX@Z
void SBSongbirdNode::OnActivityEvent(long event, void *data)
{
    MBComPtr<ISBGameState> gameState(MBIID_CSBGameState);
    switch (event) {
    case 420:
    case 421:
    case 422:
    case 423:
    case 424:
    case 425:
    case 426:
    case 427:
        // the eight nests
        OnNestClick(event - 420, data);
        break;
    case 412:
        OnPlaySong(data);
        break;
    case 413:
        OnSongbookClick(data);
        break;
    case 415:
        OnTrashClick(data);
        break;
    case 414:
        SaveSong();
        if (gameState->GetProperty("HCMickeyCaptive", 1))
            StartDialogue("sb15xmi0060", true);
        else
            StartDialogue("sb15xmc0060", true);
        break;
    case 416:
        if (gameState->GetProperty("HCMickeyCaptive", 1))
            StartDialogue("sb15xmi0030", true);
        else
            StartDialogue("sb15xmc0030", true);
        break;
    }
}

// 0x100960e0 ?OnNestClick@SBSongbirdNode@@IAEXJPAX@Z
void SBSongbirdNode::OnNestClick(long birdID, void *data)
{
    // the bird taken from a nest becomes the cursor
    mBirdPicked = (ISBAnimButton *)data;
    mNotePicked = 'a' + (char)birdID;
    mCursorSongbird[birdID]->GetAnim()->Show();
    SetCursor(mCursorSongbird[birdID]->GetAnim(), 0, 0);
    mClickHandled = true;
}

// TODO 0x100961f0 ?OnGridClick@SBSongbirdNode@@IAEXPAUMBMouseEvent@@@Z: register allocation: the original keeps this in ebp and the square pointer in ebx from the start; ours uses edi/ebp (+4 frame) (59% similar)
void SBSongbirdNode::OnGridClick(MBMouseEvent *click)
{
    MBPoint mousepos = click->mMousePos;
    MBComPtr<ISBSongbirdNote> *wire = mGridSquares.begin();
    long closest = 10000;
    ISBSongbirdNote *closestWire = 0;
    if (mBirdPicked) {
        // the bird hangs from the cursor: aim with its feet
        MBRect birdRect = mBirdPicked->GetAnim()->GetBounds();
        long width = birdRect.right - birdRect.left;
        mousepos.x += width / 2 + width;
        mousepos.y += birdRect.bottom - birdRect.top;
    }
    for (; wire != mGridSquares.end(); wire++) {
        MBPoint3 wirepos = (*wire)->GetReferencePoint();
        long dx = wirepos.x - mousepos.x;
        if (dx < 0)
            dx = -dx;
        long dy = wirepos.y - mousepos.y;
        if (dy < 0)
            dy = -dy;
        long distance = dy * dy + dx * dx;
        if (distance < closest) {
            closest = distance;
            closestWire = *wire;
        }
    }
    if (closestWire) {
        mClickHandled = true;
        if (mBirdPicked == 0) {
            // no bird in hand: take the one off the wire
            MBComPtr<IMBAnim> bird(closestWire->GetBird());
            if (bird) {
                closestWire->Clear();
                mWorld->GetRoot().RemoveObject(bird);
            }
        } else {
            MBComPtr<IMBAnim> oldbird(closestWire->GetBird());
            if (oldbird) {
                closestWire->Clear();
                mWorld->GetRoot().RemoveObject(oldbird);
            }
            MBComPtr<IMBAnim> bird = CloneAnim(mBirdPicked->GetAnim());
            closestWire->SetBird(bird);
            closestWire->SetNote(mNotePicked);
            mBirdPicked = 0;
            SetCursor(mSavedCursor, 0, 0);
        }
    }
}

// 0x10096540 ?OnPlaySong@SBSongbirdNode@@IAEXPAX@Z
void SBSongbirdNode::OnPlaySong(void *)
{
    // the song ends after the last square with a bird
    int nPos = 0;
    for (MBComPtr<ISBSongbirdNote> *note = mGridSquares.begin(); note != mGridSquares.end(); note++, nPos++) {
        if ((*note)->GetNote() != 'x')
            mLastNotePos = nPos;
    }
    if (mLastNotePos < nPos)
        mLastNotePos++;
    {
        MfString strMessage("hc_start_conducting");
        GetMfPrimaryActor()->SendMessageA(0, strMessage);
    }
    mCnt = 0;
    mNoteToPlay = mGridSquares.begin();
    if (mNoteToPlay != mGridSquares.end()) {
        MBComPtr<IMBGameClock> clock(MBIID_IMBGameClock);
        mTimeToPlay = clock->GetGameTime() + 200;
    }
}

// 0x10096700 ?OnSongbookClick@SBSongbirdNode@@IAEXPAX@Z
void SBSongbirdNode::OnSongbookClick(void *)
{
    // turn the page: the next song, after the last one an empty board
    if (!mSongPtr) {
        mSongPtr = mSongs.begin();
        mCurrentSong = mFirstSong;
        if (mFirstSong > 0)
            mSongPtr++;
    }
    if (mSongPtr == mSongs.end()) {
        ClearSongboard();
        mSongPtr = mSongs.begin();
        mCurrentSong = mFirstSong;
        if (mFirstSong > 0)
            mSongPtr++;
    } else {
        DrawSong();
        mSongPtr++;
        mCurrentSong++;
    }
    SBMouseEvent *trigger = new SBMouseEvent;
    char data_str[32];
    sprintf(data_str, "SBSongTrigger%d", mCurrentSong);
    trigger->data = data_str;
    MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
    if (mFirstSong != 1 || mCurrentSong != 1)
        broadcaster->BroadcastEvent(700, 710, trigger);
    mTimeToPlay = 0;
}

// 0x10096980 ?OnTrashClick@SBSongbirdNode@@IAEXPAX@Z
void SBSongbirdNode::OnTrashClick(void *)
{
    // the first songs come with the game; a song of the player's own may be erased
    MBComPtr<ISBGameState> gameState(MBIID_CSBGameState);
    if (mCurrentSong > 13) {
        if (gameState->GetProperty("HCMickeyCaptive", 1))
            StartDialogue("sb15xmi0010", true);
        else
            StartDialogue("sb15xmc0010", true);
        mEraseSongDialog.Create();
    }
}

// Template and inline instances this unit also emits:

// 0x10096ae0 ??C?$MBComPtr@VISBAnimButton@@@@QBEPAVISBAnimButton@@XZ

// 0x10096bd0 ??C?$MBComPtr@VISBSongbirdNote@@@@QBEPAVISBSongbirdNote@@XZ

// 0x10093360 ??_GSBSongbirdNode@@UAEPAXI@Z
// 0x100933a0 ??1?$MBVector@V?$MBComPtr@VISBSongbirdNote@@@@@@QAE@XZ
// 0x10093420 ??1?$MBMap@V?$MBComPtr@VISBAnimButton@@@@DU?$less@V?$MBComPtr@VISBAnimButton@@@@@std@@@@QAE@XZ
// 0x100948b0 ??1?$pair@$$CBV?$MBComPtr@VISBAnimButton@@@@D@std@@QAE@XZ
// 0x10096b00 ?CreateObject@?$MBComPtr@VISBAnimButton@@@@QAEJABUMBGUID@@@Z
// 0x10096ba0 ??1?$MBComPtr@VISBSongbirdNote@@@@QAE@XZ
// 0x10096bf0 ??0?$MBComPtr@VISBAnimButton@@@@QAE@ABV0@@Z
// 0x10096c20 ?begin@?$_Tree@V?$MBComPtr@VISBAnimButton@@@@U?$pair@$$CBV?$MBComPtr@VISBAnimButton@@@@D@std@@U_Kfn@?$map@V?$MBComPtr@VISBAnimButton@@@@DU?$less@V?$MBComPtr@VISBAnimButton@@@@@std@@V?$MBAllocator@D@@@3@U?$less@V?$MBComPtr@VISBAnimButton@@@@@3@V?$M6b896e57
// 0x10096c40 ?insert@?$_Tree@V?$MBComPtr@VISBAnimButton@@@@U?$pair@$$CBV?$MBComPtr@VISBAnimButton@@@@D@std@@U_Kfn@?$map@V?$MBComPtr@VISBAnimButton@@@@DU?$less@V?$MBComPtr@VISBAnimButton@@@@@std@@V?$MBAllocator@D@@@3@U?$less@V?$MBComPtr@VISBAnimButton@@@@@3@V?$5d2a8842
// 0x10096dd0 ?erase@?$_Tree@V?$MBComPtr@VISBAnimButton@@@@U?$pair@$$CBV?$MBComPtr@VISBAnimButton@@@@D@std@@U_Kfn@?$map@V?$MBComPtr@VISBAnimButton@@@@DU?$less@V?$MBComPtr@VISBAnimButton@@@@@std@@V?$MBAllocator@D@@@3@U?$less@V?$MBComPtr@VISBAnimButton@@@@@3@V?$MBAllocator@D@@@std@@QAE?AViterator@12@V312@0@Z
// 0x10096f60 ?insert@?$vector@V?$MBComPtr@VISBSongbirdNote@@@@V?$MBAllocator@V?$MBComPtr@VISBSongbirdNote@@@@@@@std@@QAEXPAV?$MBComPtr@VISBSongbirdNote@@@@IABV3@@Z
// 0x100972c0 ?Release@?$MBComPtr@VISBSongbirdNote@@@@QAEXXZ
// 0x100972f0 ?erase@?$_Tree@V?$MBComPtr@VISBAnimButton@@@@U?$pair@$$CBV?$MBComPtr@VISBAnimButton@@@@D@std@@U_Kfn@?$map@V?$MBComPtr@VISBAnimButton@@@@DU?$less@V?$MBComPtr@VISBAnimButton@@@@@std@@V?$MBAllocator@D@@@3@U?$less@V?$MBComPtr@VISBAnimButton@@@@@3@V?$MBAllocator@D@@@std@@QAE?AViterator@12@V312@@Z
// 0x10097950 ?_Erase@?$_Tree@V?$MBComPtr@VISBAnimButton@@@@U?$pair@$$CBV?$MBComPtr@VISBAnimButton@@@@D@std@@U_Kfn@?$map@V?$MBComPtr@VISBAnimButton@@@@DU?$less@V?$MBComPtr@VISBAnimButton@@@@@std@@V?$MBAllocator@D@@@3@U?$less@V?$MBComPtr@VISBAnimButton@@@@@3@V?$3e7fdce6
// 0x10097a20 ?_Init@?$_Tree@V?$MBComPtr@VISBAnimButton@@@@U?$pair@$$CBV?$MBComPtr@VISBAnimButton@@@@D@std@@U_Kfn@?$map@V?$MBComPtr@VISBAnimButton@@@@DU?$less@V?$MBComPtr@VISBAnimButton@@@@@std@@V?$MBAllocator@D@@@3@U?$less@V?$MBComPtr@VISBAnimButton@@@@@3@V?$Ma4080d8b
// 0x10097ac0 ?_Insert@?$_Tree@V?$MBComPtr@VISBAnimButton@@@@U?$pair@$$CBV?$MBComPtr@VISBAnimButton@@@@D@std@@U_Kfn@?$map@V?$MBComPtr@VISBAnimButton@@@@DU?$less@V?$MBComPtr@VISBAnimButton@@@@@std@@V?$MBAllocator@D@@@3@U?$less@V?$MBComPtr@VISBAnimButton@@@@@3@V?efebe762
// 0x10097e60 ?_Buynode@?$_Tree@V?$MBComPtr@VISBAnimButton@@@@U?$pair@$$CBV?$MBComPtr@VISBAnimButton@@@@D@std@@U_Kfn@?$map@V?$MBComPtr@VISBAnimButton@@@@DU?$less@V?$MBComPtr@VISBAnimButton@@@@@std@@V?$MBAllocator@D@@@3@U?$less@V?$MBComPtr@VISBAnimButton@@@@@3@V366fc954
// 0x10097e90 ?_Dec@const_iterator@?$_Tree@V?$MBComPtr@VISBAnimButton@@@@U?$pair@$$CBV?$MBComPtr@VISBAnimButton@@@@D@std@@U_Kfn@?$map@V?$MBComPtr@VISBAnimButton@@@@DU?$less@V?$MBComPtr@VISBAnimButton@@@@@std@@V?$MBAllocator@D@@@3@U?$less@V?$MBComPtr@VISBAnimBut365aa87b
// 0x10097f80 ?_Inc@const_iterator@?$_Tree@V?$MBComPtr@VISBAnimButton@@@@U?$pair@$$CBV?$MBComPtr@VISBAnimButton@@@@D@std@@U_Kfn@?$map@V?$MBComPtr@VISBAnimButton@@@@DU?$less@V?$MBComPtr@VISBAnimButton@@@@@std@@V?$MBAllocator@D@@@3@U?$less@V?$MBComPtr@VISBAnimBut80b47951
// 0x10098060 ?_QueryInterface@?$MBComPtr@VISBSongbirdNote@@@@AAEJABQAVIMBUnknown@@@Z
// 0x10098100 ?_Construct@std@@YAXPAU?$pair@$$CBV?$MBComPtr@VISBAnimButton@@@@D@1@ABU21@@Z
// 0x10098180 ?_Construct@std@@YAXPAV?$MBComPtr@VISBSongbirdNote@@@@ABV2@@Z
// 0x100981f0 ??4?$MBComPtr@VISBSongbirdNote@@@@QAEAAV0@PAVISBSongbirdNote@@@Z
// 0x10098230 ?AddRef@?$MBComPtr@VISBSongbirdNote@@@@QAEXXZ

// Static initializers and their atexit stubs: the PDB numbers them by the original header set, which
// ours does not reproduce yet, so they cannot be claimed by name.
// 0x10092a90 _$E51
// 0x10092ab0 _$E48
// 0x10093080 _$E50
// 0x100930a0 _$E49
// 0x100930d0 _$E55
// 0x100930f0 _$E54
// 0x10098250 _$E67
// 0x10098290 _$E63
// 0x100982b0 _$E66
