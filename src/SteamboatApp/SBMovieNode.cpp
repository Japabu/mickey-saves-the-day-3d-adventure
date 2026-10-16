#include "SBMovieNode.h"
#include "SBCursorItems.h" // every node unit of the original has the cursor item strings
#include "SBMotivateDriver.h"
#include "MBRuntime.h"

// EQUIV 0x1008a960 ??0SBMovieNode@@QAE@PAVIMBGameEngine@@PAVMfAiSpace@@@Z: reviewed: the original calls basic_string::assign(const char *, n) out of line, VC6 inlines it (_Grow, copy, _Eos), and keeps currentNode in its own slot; an EH state store moves past an independent call setup. Was: the original keeps currentNode in the unused space parameter slot; VC6 gives it its own
SBMovieNode::SBMovieNode(IMBGameEngine *engine, MfAiSpace *space) : SteamboatNode(engine, "MV")
{
    SetBackgroundMusicState(false);
    mShowGui = false;
    MBComPtr<IMBNodeManager> nodemgr = mEngine->GetNodeMgr();
    SteamboatNode *currentNode = (SteamboatNode *)nodemgr->GetActiveNode();
    if (currentNode)
        mPrevNode = SteamboatNode::GetSpaceCode();
    else
        mPrevNode = MBString("none");
    mMovieName = MBString("none");
    mQueuedMovie = MBString("none");
    mNextNode = MBString("MX");
    if (currentNode)
        currentNode->Release();
}

// 0x1008adb0 ??_GSBMovieNode@@UAEPAXI@Z

// 0x1008adf0 ??1SBMovieNode@@UAE@XZ
SBMovieNode::~SBMovieNode()
{
}

// 0x1008afb0 ?StartListening@SBMovieNode@@UAEXXZ
void SBMovieNode::StartListening()
{
    MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
    if (broadcaster)
        broadcaster->AddListener(-5, this);
    SteamboatNode::StartListening();
}

// 0x1008b090 ?StopListening@SBMovieNode@@UAEXXZ
void SBMovieNode::StopListening()
{
    MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
    if (broadcaster)
        broadcaster->RemoveListener(-5, this);
    SteamboatNode::StopListening();
}

// 0x1008b170 ?OnPreRender@SBMovieNode@@UAEXV?$MBComPtr@VIMBScreen@@@@@Z
void SBMovieNode::OnPreRender(MBComPtr<IMBScreen> screen)
{
}

// 0x1008b190 ?OnPostRender@SBMovieNode@@UAEXV?$MBComPtr@VIMBScreen@@@@@Z
void SBMovieNode::OnPostRender(MBComPtr<IMBScreen> screen)
{
    SteamboatNode::OnPostRender(screen);
}

// 0x1008b220 ?ListenToEvent@SBMovieNode@@UAEXJJPAX@Z
void SBMovieNode::ListenToEvent(long channel, long event, void *data)
{
    if (channel == -5 && event == 3 && *(IMBMovie **)data == mMovie) {
        if (mQueuedMovie == MBString("none"))
            Done();
        else {
            mMovie = PlayMovie(mQueuedMovie.c_str(), 0);
            if (mMovie == 0)
                Done();
            else
                MBComPtr<IMBObject>(mMovie)->Move(MBPoint3(0, 0, 150));
            mQueuedMovie = MBString("none");
        }
    }
    SteamboatNode::ListenToEvent(channel, event, data);
}

// EQUIV 0x1008b520 ?OnKeyDown@SBMovieNode@@UAEXAAUMBKeyboardEvent@@@Z: reviewed: the original calls MBComPtr::Release on the temporary, VC6 inlines it (the inlined null store is dead, overwritten by the next assignment); nothing else differs. Was: VC6 inlines the second Release of the temporary, the original calls it
void SBMovieNode::OnKeyDown(MBKeyboardEvent &key)
{
    if (key.mCharCode == 27 || key.mCharCode == ' ') {
        mMovie->Stop();
        mWorld->GetRoot().RemoveObject(MBComPtr<IMBObject>(mMovie));
    }
    SteamboatNode::OnKeyDown(key);
}

// EQUIV 0x1008b640 ?OnStart@SBMovieNode@@UAEXXZ: reviewed: the original inlines the inventory MBComPtr::Release, VC6 calls it; the other differences are EH state stores and pushes moved past independent instructions. Was: all but the RR branch: the original inlines the inventory's MBComPtr::Release at its end of scope, VC6 calls it (the register choices of the CD and none branches follow); the inliner budget of this function decides (writing compare() for == changes nothing)
void SBMovieNode::OnStart()
{
    SteamboatNode::OnStart();
    IMBObject *renderObject = (IMBObject *)mRenderObject;
    mWorld->GetRoot().RemoveObject(renderObject);
    HideCursor();
    MBComPtr<ISBGameState> gs(MBIID_CSBGameState);
    bool player_is_mickey = gs->GetProperty("HCMickeyCaptive", 0) == 0;
    if (mPrevNode == MBString("UI")) {
        if (player_is_mickey) {
            mMovieName = MBString("intro_mic");
            mNextNode = MBString("MX");
        } else {
            mMovieName = MBString("intro_min");
            mNextNode = MBString("HL");
        }
    } else if (mPrevNode == MBString("VL")) {
        if (player_is_mickey)
            mMovieName = MBString("rescue_mic");
        else
            mMovieName = MBString("rescue_min");
        mNextNode = MBString("CE");
    } else if (mPrevNode == MBString("ER")) {
        mMovieName = MBString("steamroll");
        mNextNode = MBString("DH");
        MBComPtr<ISBGameState> gs(MBIID_ISBGameState);
        gs->SetProperty("ERmovie", 1);
    } else if (mPrevNode == MBString("RR")) {
        if (player_is_mickey)
            mMovieName = MBString("endmov_mic");
        else
            mMovieName = MBString("endmov_min");
        MBComPtr<ISBInventory> gui(MBIID_ISBInventory);
        if (gui->GetRecycleItemCount() >= 15) {
            if (player_is_mickey)
                mQueuedMovie = MBString("endmov_mic_b");
            else
                mQueuedMovie = MBString("endmov_min_b");
        }
        mNextNode = MBString("CD");
    } else if (mPrevNode == MBString("CD")) {
        mMovieName = MBString("radtools");
        mQueuedMovie = MBString("mofa");
        mNextNode = "";
    } else if (mPrevNode == MBString("none")) {
        mMovieName = MBString("disneybumper");
        mNextNode = MBString("UI");
    }
    MBDebugStr("SBMovieNode::OnStart() determined movie to play: PrevNode='%s', Recycle Count=%d, Player=%s, NextNode='%s', Movie='%s', QMovie='%s'\n",
               mPrevNode.c_str(), gs->GetNumberItemsInRecycleBin(), player_is_mickey ? "mickey" : "minnie",
               mNextNode.c_str(), mMovieName.c_str(), mQueuedMovie.c_str());
    mMovie = PlayMovie(mMovieName.c_str(), 0);
    if (mMovie == 0)
        Done();
    else
        MBComPtr<IMBObject>(mMovie)->Move(MBPoint3(0, 0, 150));
}

// 0x1008c520 ?OnEnd@SBMovieNode@@UAEXXZ
void SBMovieNode::OnEnd()
{
    SteamboatNode::OnEnd();
}

// 0x1008c540 ?Done@SBMovieNode@@MAEXXZ
void SBMovieNode::Done()
{
    if (mNextNode.empty()) {
        mEngine->Quit();
        return;
    }
    SendExitSceneEvent(MBString(mNextNode.c_str()));
    SBMotivateDriver::GetInstance()->Update();
}


// Matched by this code (template and inline instances):
// 0x1008c6a0 ??1?$MBComPtr@VIMBMovie@@@@QAE@XZ
// 0x1008c6d0 ?CreateObject@?$MBComPtr@VISBInventory@@@@QAEJABUMBGUID@@@Z
// 0x1008c770 ?AddRef@?$MBComPtr@VIMBMovie@@@@QAEXXZ
// 0x1008c790 ?_QueryInterface@?$MBComPtr@VIMBObject@@@@AAEJABV?$MBComPtr@VIMBMovie@@@@@Z

// Not claimed (static initializers and header instances of this object):
// 0x1008a320 _$E49
// 0x1008a340 _$E46
// 0x1008a910 _$E48
// 0x1008a930 _$E47
// TODO 0x1008c830 $E372: static initializer from the headers (calls ??_B?1???id@?$ctype@G@std@@$D@@9@51, $E365); its $E number depends on the original header set
// 0x1008c870 _$E63
// TODO 0x1008c890 $E368: static initializer from the headers (registers ?id@?$collate@D@std@@$E with atexit); its $E number depends on the original header set
