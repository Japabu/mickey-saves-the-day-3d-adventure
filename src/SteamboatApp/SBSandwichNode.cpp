#include "SBSandwichNode.h"
#include "SBCursorItems.h" // every node unit of the original has the cursor item strings
#include <locale> // evidence: the original unit ends with the std::ctype<unsigned short>::id and std::collate<char>::id initializers
#include "MBRuntime.h"
#include "MBSoundUtils.h"
#include <algorithm>
#include <math.h>
#include <stdio.h>

// Where the machine throws the ingredients from, Mickey's plate offsets and the plate's start.
MBPoint3 kLaunchPoint = MBPoint3(108, 206, 200);
MBPoint kMickeyPlateOffsets = MBPoint(286, 291);
MBPoint kStartingPlatePos = MBPoint(160, 360);

// The flight of the ingredients.
const float kIngredientYOffset = 10.0f;
const float kMaxSandwichSlideDistance = 10.0f;
const float kIngredientFriction = 250.0f;
const float kIngredientSpring = 280.0f;
const float kLaunchVelocity = 480.0f;
const float kSpeedPerLevelMultiplier = 1.1f;
const float kLaunchDelay = 3000.0f;
const float kLaunchDelayRandomness = 500.0f;
const float kFlightCeiling = -100.0f;
const float kFlightFloor = 434.0f;
const float kMaximumCatchDistance = 50.0f;
const float kMillisecondsToSeconds = 0.001;
const unsigned long kFallColumns[6] = { 121, 203, 284, 365, 444, 524 };

// 0x1008db90 ??0SBSandwichNode@@QAE@PAVIMBGameEngine@@PAVMfAiSpace@@@Z
SBSandwichNode::SBSandwichNode(IMBGameEngine *engine, MfAiSpace *space)
    : SBActivityNode(engine, space), mActive(false), mHotspotActive(true), mHighestLevelAvailable(1),
      mCurrentLevel(1), mNextLevel(1), mCurrentPoints(0), mPointGoal(0), mCharMoved(0),
      mPlatePos(kStartingPlatePos), mLastUpdate(0), mNextLaunch(0)
{
    SetBackgroundMusicState(true);
}

// 0x1008df10 ??0SBSandwichNode@@QAE@PAVIMBGameEngine@@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@Z
SBSandwichNode::SBSandwichNode(IMBGameEngine *engine, MBString spaceCode)
    : SBActivityNode(engine, spaceCode), mActive(false), mHotspotActive(true), mHighestLevelAvailable(1),
      mCurrentLevel(1), mNextLevel(1), mCurrentPoints(0), mPointGoal(0), mCharMoved(0),
      mPlatePos(kStartingPlatePos), mLastUpdate(0), mNextLaunch(0)
{
    SetBackgroundMusicState(true);
}

// 0x1008e1e0 ??1SBSandwichNode@@UAE@XZ
SBSandwichNode::~SBSandwichNode()
{
}

// TODO 0x1008e4a0 ?OnStart@SBSandwichNode@@UAEXXZ: inline budget: the original calls the CreateAnim
// temporaries' Release out of line up to SHStartButton and inlines it from SHMachine on, calls
// operator= for SHMinnie and inlines the last prototype loop's AddRef; VC6 here chooses otherwise,
// and ebx/ebp are swapped (83% similar)
void SBSandwichNode::OnStart()
{
    SetSceneBackgroundMusic(MBString("sandwichmaker"));
    MBComPtr<IMBAnim> anim;
    SteamboatNode::OnStart();
    mSavedCursor = mCursor;
    MBComPtr<ISBGameState> gs(MBIID_CSBGameState);
    bool player_is_mickey = gs->GetProperty("HCMickeyCaptive", 0) == 0;
    mHighestLevelAvailable = gs->GetHighestSandwichLevel();
    char buf[32];
    int i;
    for (i = 0; i < 10; i++) {
        sprintf(buf, "SHCounter%d", i + 1);
        mLevelCounters[i] = CreateAnim(buf);
        mLevelCounters[i]->Hide();
    }

    // the level buttons and the machine's start button
    anim = CreateAnim("SHLastLevel");
    mLastLevelBtn.CreateObject(MBIID_CSBAnimButton);
    mLastLevelBtn->SetAnim(anim);
    mLastLevelBtn->SetClickMessage(500);
    mLastLevelBtn->SetBroadcastChannel(400);
    mWorld->AddObject(mLastLevelBtn);
    mWorld->RemoveObject(anim);
    MBComPtr<IMBMouseable> lastLevelBtnMouseable(mLastLevelBtn);
    mMouseMonitor->AddMouseable(lastLevelBtnMouseable);

    anim = CreateAnim("SHNextLevel");
    mNextLevelBtn.CreateObject(MBIID_CSBAnimButton);
    mNextLevelBtn->SetAnim(anim);
    mNextLevelBtn->SetClickMessage(501);
    mNextLevelBtn->SetBroadcastChannel(400);
    mWorld->AddObject(mNextLevelBtn);
    mWorld->RemoveObject(anim);
    MBComPtr<IMBMouseable> nextLevelBtn(mNextLevelBtn);
    mMouseMonitor->AddMouseable(nextLevelBtn);

    anim = CreateAnim("SHStartButton");
    mMachineButton.CreateObject(MBIID_CSBAnimButton);
    mMachineButton->SetAnim(anim);
    mMachineButton->SetClickMessage(502);
    mMachineButton->SetBroadcastChannel(400);
    mWorld->AddObject(mMachineButton);
    mWorld->RemoveObject(anim);
    MBComPtr<IMBMouseable> startMouseable(mMachineButton);
    mMouseMonitor->AddMouseable(startMouseable);

    mMachine = CreateAnim("SHMachine");
    mMachine->Place(MBPoint3(0, 0, 50));
    mMachine->Hide();
    mSpinner = CreateAnim("SHTicketSpin");
    mSpinner->Place(MBPoint3(0, 0, 50));
    mSpinner->Hide();

    // the progress sandwich: bottom, twelve layers, top
    mProgressSandwich[0] = CreateAnim("SHSandwichCounter1");
    mProgressSandwich[1] = CreateAnim("SHSandwichCounter2");
    mProgressSandwich[2] = CreateAnim("SHSandwichCounter3");
    mProgressSandwich[3] = CreateAnim("SHSandwichCounter4");
    mProgressSandwich[4] = CreateAnim("SHSandwichCounter2");
    mProgressSandwich[5] = CreateAnim("SHSandwichCounter3");
    mProgressSandwich[6] = CreateAnim("SHSandwichCounter4");
    mProgressSandwich[7] = CreateAnim("SHSandwichCounter2");
    mProgressSandwich[8] = CreateAnim("SHSandwichCounter3");
    mProgressSandwich[9] = CreateAnim("SHSandwichCounter4");
    mProgressSandwich[10] = CreateAnim("SHSandwichCounter2");
    mProgressSandwich[11] = CreateAnim("SHSandwichCounter3");
    mProgressSandwich[12] = CreateAnim("SHSandwichCounter4");
    mProgressSandwich[13] = CreateAnim("SHSandwichCounter5");
    for (i = 0; i < 14; i++) {
        long offset = 10 - i * 10;
        mProgressSandwich[i]->Move(MBPoint3(0, offset < 0 && offset > -120 ? offset : 0, i - 50));
        mProgressSandwich[i]->Hide();
    }

    if (player_is_mickey)
        mCharacter = CreateAnim("SHMickey");
    else
        mCharacter = CreateAnim("SHMinnie");
    mCharacter->SetCurrentSequence(0);
    mCharacter->Place(MBPoint3(mPlatePos.x - kMickeyPlateOffsets.x, mPlatePos.y - kMickeyPlateOffsets.y, 100));

    // the prototypes the machine clones its ingredients from, centred on their registration points
    for (i = 0; i < 2; i++) {
        sprintf(buf, "SHBread%d", i + 1);
        mBreadPrototype[i] = CreateAnim(buf);
        mBreadPrototype[i]->Place(MBPoint3(320, 200, 100));
        MBRect bounds = mBreadPrototype[i]->GetBounds();
        int numCasts = mBreadPrototype[i]->GetNumCasts();
        for (int j = 1; j <= numCasts; j++)
            mBreadPrototype[i]->SetCastMemberRegistrationPoint(j, MBPoint(-(bounds.Width() / 2), -(bounds.Height() / 2)));
        mWorld->RemoveObject(mBreadPrototype[i]);
    }
    for (i = 0; i < 10; i++) {
        sprintf(buf, "SHIngredient%d", i + 1);
        mIngredientPrototype[i] = CreateAnim(buf);
        mIngredientPrototype[i]->Place(MBPoint3(320, 200, 100));
        MBRect bounds = mIngredientPrototype[i]->GetBounds();
        int numCasts = mIngredientPrototype[i]->GetNumCasts();
        for (int j = 1; j <= numCasts; j++)
            mIngredientPrototype[i]->SetCastMemberRegistrationPoint(j, MBPoint(-(bounds.Width() / 2), -(bounds.Height() / 2)));
        mWorld->RemoveObject(mIngredientPrototype[i]);
    }
    for (i = 0; i < 7; i++) {
        sprintf(buf, "SHNoningredient%d", i + 1);
        mNoningredientPrototype[i] = CreateAnim(buf);
        mNoningredientPrototype[i]->Place(MBPoint3(320, 200, 100));
        MBRect bounds = mNoningredientPrototype[i]->GetBounds();
        int numCasts = mNoningredientPrototype[i]->GetNumCasts();
        for (int j = 1; j <= numCasts; j++)
            mNoningredientPrototype[i]->SetCastMemberRegistrationPoint(j, MBPoint(-(bounds.Width() / 2), -(bounds.Height() / 2)));
        mWorld->RemoveObject(mNoningredientPrototype[i]);
    }

    InitializeLevel(1);
    mBeulah = CreateAnim("SHBeulah");
    mBeulah->Hide();
    mBeulahState = kBSNone;
}

// 0x1008f5d0 ?OnEnd@SBSandwichNode@@UAEXXZ
void SBSandwichNode::OnEnd()
{
    SetCursor(mSavedCursor, 0, 0);
    ShowCursor();
    MBComPtr<ISBGameState> gs(MBIID_CSBGameState);
    gs->SetHighestSandwichLevel(mHighestLevelAvailable);
    SteamboatNode::OnEnd();
}

static inline float Abs(float value)
{
    return value < 0 ? -value : value;
}

static inline float Distance(float dx, float dy)
{
    return sqrt(dx * dx + dy * dy);
}

// 0x1008f700 ?Idle@SBSandwichNode@@UAEXXZ
void SBSandwichNode::Idle()
{
    MBComPtr<IMBSoundManager> sm(MBIID_IMBSoundManager);
    MBComPtr<IMBGameClock> clock(MBIID_IMBGameClock);
    unsigned long now = clock->GetGameTime();
    float elapsed = (now - mLastUpdate) * kMillisecondsToSeconds;
    if (mLastUpdate == 0)
        elapsed = 0.1f;
    mLastUpdate = now;

    // Mickey stands still again a few frames after the last move
    if (mCharMoved == 0)
        mCharacter->SetCurrentSequence(0);
    if (mCharMoved > 3)
        mCharMoved = 3;
    else if (mCharMoved > 0)
        mCharMoved--;

    if (now > mNextLaunch && mNextLaunch > 0 && mActive)
        LaunchIngredient(MBrandom(1, 20) <= mCurrentLevel ? 2 : 1);

    // the sandwich: the bottom slice sits on the plate, every layer is pulled towards the one below
    int i = 0;
    SHIngredientType *prev_ingr = 0;
    for (; i < mSandwichStack.size(); i++) {
        SHIngredientType *ingr = mSandwichStack[i];
        if (i == 0) {
            ingr->mVelocity[0] = mPlatePos.x - ingr->mPosition[0];
            ingr->mVelocity[1] = 0;
            ingr->mVelocity[2] = 0;
            ingr->mPosition[0] = mPlatePos.x;
            ingr->mPosition[1] = mPlatePos.y;
            ingr->mPosition[2] = 101;
        } else {
            float newpos[3];
            newpos[0] = ingr->mPosition[0] + ingr->mVelocity[0] * elapsed;
            newpos[1] = prev_ingr->mPosition[1] - kIngredientYOffset;
            newpos[2] = prev_ingr->mPosition[2] + 1;
            float diff = newpos[0] - prev_ingr->mPosition[0];
            if ((diff < 0 ? -diff : diff) > kMaxSandwichSlideDistance) {
                if (diff < 0)
                    newpos[0] = prev_ingr->mPosition[0] - kMaxSandwichSlideDistance;
                else
                    newpos[0] = prev_ingr->mPosition[0] + kMaxSandwichSlideDistance;
                ingr->mVelocity[0] = 0;
            }
            float accel = (prev_ingr->mPosition[0] - newpos[0]) * kIngredientSpring * elapsed;
            ingr->mVelocity[0] = prev_ingr->mVelocity[0] + accel;
            float friction = kIngredientFriction * elapsed;
            if (ingr->mVelocity[0] > 0)
                friction = -friction;
            if (friction * accel > 0)
                friction *= 2;
            if (Abs(ingr->mVelocity[0]) < Abs(friction))
                friction = -ingr->mVelocity[0];
            ingr->mVelocity[0] += friction;
            ingr->mPosition[0] = newpos[0];
            ingr->mPosition[1] = newpos[1];
            ingr->mPosition[2] = newpos[2];
        }
        {
            long x = ingr->mPosition[0];
            long y = ingr->mPosition[1];
            long z = ingr->mPosition[2];
            ingr->mImage->Place(MBPoint3(x, y, z));
        }
        prev_ingr = ingr;
    }

    // the flying ingredients
    for (i = 0; i < mFlyingStack.size(); i++) {
        SHIngredientType *ingr = mFlyingStack[i];
        ingr->mPosition[0] += ingr->mVelocity[0] * elapsed;
        ingr->mPosition[1] += ingr->mVelocity[1] * elapsed;
        ingr->mPosition[2] += ingr->mVelocity[2] * elapsed;
        {
            long x = ingr->mPosition[0];
            long y = ingr->mPosition[1];
            long z = ingr->mPosition[2];
            ingr->mImage->Place(MBPoint3(x, y, z));
        }
        if (ingr->mVelocity[1] < 0 && ingr->mPosition[1] < kFlightCeiling) {
            // over the top: it falls down one of the columns
            ingr->mVelocity[1] = -ingr->mVelocity[1];
            ingr->mPosition[0] = kFallColumns[MBrandom(0, 5)] + MBrandom(-5, 5);
            {
            long x = ingr->mPosition[0];
            long y = ingr->mPosition[1];
            long z = ingr->mPosition[2];
            ingr->mImage->Place(MBPoint3(x, y, z));
        }
        } else if (ingr->mVelocity[1] > 0 && ingr->mPosition[1] > 0) {
            if (Distance(ingr->mPosition[0] - prev_ingr->mPosition[0], ingr->mPosition[1] - prev_ingr->mPosition[1]) < kMaximumCatchDistance) {
                mFlyingStack.erase(std::find(mFlyingStack.begin(), mFlyingStack.end(), ingr));
                i--;
                CatchIngredient(ingr);
            }
            if (ingr->mPosition[1] >= kFlightFloor) {
                // missed: it lies on the floor
                ingr->mPosition[1] = kFlightFloor;
                ingr->mPosition[2] = 75;
                ingr->mVelocity[0] = 0;
                ingr->mVelocity[1] = 0;
                ingr->mVelocity[2] = 0;
                {
            long x = ingr->mPosition[0];
            long y = ingr->mPosition[1];
            long z = ingr->mPosition[2];
            ingr->mImage->Place(MBPoint3(x, y, z));
        }
                ingr->mImage->SetCurrentSequence(1);
                long channel = FindFreeChannel(30, 39);
                if (ingr->mType < 2) {
                    // a lost ingredient costs a broom
                    if (ingr->mType == 0)
                        MBForceSound("SHSEbreadHitFloor", channel, 1);
                    else if (ingr->mType == 1)
                        MBForceSound("SHSEingrdHitFloor", channel, 1);
                    if (mBrooms.size() == 0) {
                        LoseRound();
                    } else {
                        if (mNextLaunch == 0)
                            LaunchIngredient(1);
                        SetNumberBrooms(mBrooms.size() - 1);
                    }
                } else {
                    MBForceSound("SHSEbootHitFloor", channel, 1);
                }
                sm->SetChannelVolume(channel, sSfxVolume);
            }
        }
    }
    SteamboatNode::Idle();
}

// 0x1008ff00 ?StartListening@SBSandwichNode@@UAEXXZ
void SBSandwichNode::StartListening()
{
    MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
    if (broadcaster) {
        broadcaster->AddListener(-2, this);
        broadcaster->AddListener(-1, this);
        broadcaster->AddListener(-4, this);
    }
    SBActivityNode::StartListening();
}

// 0x10090020 ?StopListening@SBSandwichNode@@UAEXXZ
void SBSandwichNode::StopListening()
{
    MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
    if (broadcaster) {
        broadcaster->RemoveListener(-2, this);
        broadcaster->RemoveListener(-1, this);
        broadcaster->RemoveListener(-4, this);
    }
    SBActivityNode::StopListening();
}

// 0x10090140 ?OnKeyDown@SBSandwichNode@@UAEXAAUMBKeyboardEvent@@@Z
void SBSandwichNode::OnKeyDown(MBKeyboardEvent &key)
{
    SteamboatNode::OnKeyDown(key);
}

// 0x10090160 ?ListenToEvent@SBSandwichNode@@UAEXJJPAX@Z
void SBSandwichNode::ListenToEvent(long channel, long event, void *data)
{
    MBComPtr<IMBSoundManager> sm(MBIID_IMBSoundManager);
    MBComPtr<ISBGameState> gs(MBIID_CSBGameState);
    bool player_is_mickey = gs->GetProperty("HCMickeyCaptive", 0) == 0;
    float levelmult = pow(kSpeedPerLevelMultiplier, (float)(mCurrentLevel - 1));
    if (channel == -2) {
        MBAnimEventStruct *animEvent = (MBAnimEventStruct *)data;
        if (event == 2) {
            if (animEvent->anim == mMachine) {
                // the machine spits the ingredient out
                SHIngredientType *ingredient = mFlyingStack.back();
                ingredient->mVelocity[0] = 0;
                ingredient->mVelocity[1] = -(kLaunchVelocity * levelmult);
                ingredient->mVelocity[2] = 0;
                ingredient->mImage->Show();
                long sfxChannel = FindFreeChannel(30, 39);
                MBForceSound("SHSEingrdSpitOut", sfxChannel, 1);
                sm->SetChannelVolume(sfxChannel, sSfxVolume);
            } else {
                // a level counter finished flipping
                animEvent->anim->Hide();
                InitializeLevel(mNextLevel);
            }
        } else if (event == 6) {
            if (animEvent->anim == mCharacter) {
                if (animEvent->sequence == 4)
                    mCharacter->SetCurrentSequence(3);
                else if (animEvent->sequence == 5)
                    mCharacter->SetCurrentSequence(2);
            } else if (animEvent->anim == mBeulah && animEvent->sequence == 1) {
                switch (mBeulahState) {
                case kBSIntro:
                    DisableHotspots();
                    if (mHighestLevelAvailable > 1) {
                        MBForceSound("sh02xbl0010", 20, 1);
                    } else {
                        MBForceSound("sh01xbl0010", 20, 1);
                        MBQueueSound(player_is_mickey ? "sh01xmc0020" : "sh01xmi0020", 20, 1);
                        MBQueueSound("sh01xbl0030", 20, 1);
                        MBQueueSound(player_is_mickey ? "sh01xmc0040" : "sh01xmi0040", 20, 1);
                        MBQueueSound("sh01xbl0050", 20, 1);
                        MBQueueSound(player_is_mickey ? "sh01xmc0060" : "sh01xmi0060", 20, 1);
                        MBQueueSound("sh01xbl0070", 20, 1);
                        MBQueueSound(player_is_mickey ? "sh01xmc0080" : "sh01xmi0080", 20, 1);
                        MBQueueSound("sh01xbl0090", 20, 1);
                        MBQueueSound(player_is_mickey ? "sh01xmc0100" : "sh01xmi0100", 20, 1);
                    }
                    sm->SetChannelVolume(20, sDialogVolume);
                    break;
                case kBSCleanUp:
                    DisableHotspots();
                    switch (MBrandom(1, 5)) {
                    case 1:
                        MBForceSound("sh08abl0010", 20, 1);
                        break;
                    case 2:
                        MBForceSound("sh08abl0020", 20, 1);
                        break;
                    case 3:
                        MBForceSound("sh08abl0030", 20, 1);
                        break;
                    case 4:
                        MBForceSound("sh08abl0050", 20, 1);
                        break;
                    case 5:
                        MBForceSound("sh08abl0060", 20, 1);
                        break;
                    }
                    MBQueueSound("sh08abl0065", 20, 1);
                    break;
                case kBSAllClean:
                    DisableHotspots();
                    MBForceSound("sh08abl0070", 20, 1);
                    break;
                case kBSFinished:
                    DisableHotspots();
                    MBForceSound("sh08bbl0010", 20, 1);
                    MBQueueSound(player_is_mickey ? "sh08bmc0020" : "sh08bmi0020", 20, 1);
                    break;
                default:
                    mBeulahState = kBSNone;
                    mBeulah->SetCurrentSequence(3);
                    if (!mActive)
                        EnableHotspots();
                    break;
                }
                mBeulah->SetCurrentSequence(2);
            }
        } else if (event == 5) {
            if (animEvent->sequence == 0 && mLevelCounters[mCurrentLevel - 1] == animEvent->anim) {
                mNextLevelBtn->Show();
                mLastLevelBtn->Show();
            }
        }
    } else if (channel == -1) {
        if (mActive) {
            if (event == 8) {
                // Mickey follows the mouse with the plate
                long x = ((MBMouseEvent *)data)->mMousePos.x;
                if (x > 540)
                    x = 540;
                else if (x < 110)
                    x = 110;
                int seq;
                long dx = x - mPlatePos.x;
                if (dx < 0)
                    seq = 1;
                else if (dx > 0)
                    seq = 2;
                mCharMoved += 2;
                mPlatePos.x = x;
                mCharacter->Place(MBPoint3(x - kMickeyPlateOffsets.x, mPlatePos.y - kMickeyPlateOffsets.y, 100));
                if (mCharacter->GetCurrentSequenceIndex() != seq)
                    mCharacter->SetCurrentSequence(seq);
            } else if (event == 5) {
                if (mNextLaunch > 0 && LaunchIngredient(0))
                    mNextLaunch = 0;
            }
        }
    } else if (channel == -4) {
        if (event == 1) {
            IMBSoundDescription **desc = (IMBSoundDescription **)data;
            if ((*desc)->GetSoundID() == MBHashString("sh01xbl0090") || (*desc)->GetSoundID() == MBHashString("sh02xbl0010") ||
                (*desc)->GetSoundID() == MBHashString("sh08abl0070") || (*desc)->GetSoundID() == MBHashString("sh08abl0065")) {
                mBeulah->SetCurrentSequence(3);
                if (!mActive)
                    EnableHotspots();
            }
            if ((*desc)->GetSoundID() == MBHashString("sh08bmc0020") || (*desc)->GetSoundID() == MBHashString("sh08bmi0020")) {
                // the finished sandwich: back to the steamboat
                mBeulah->SetCurrentSequence(3);
                SendExitSceneEvent(MBString("SW"));
            }
        }
    }
    SBActivityNode::ListenToEvent(channel, event, data);
}

// EQUIV 0x10090b00 ?OnActivityEvent@SBSandwichNode@@MAEXJPAX@Z: reviewed: the data argument is loaded into another register before the this move; same call and arguments
// and pushes both arguments before setting this for the base call; VC6 uses eax here (88% similar)
void SBSandwichNode::OnActivityEvent(long event, void *data)
{
    if (!mActive && mHotspotActive) {
        switch (event) {
        case 500:
            // the level buttons wrap around
            SwitchToLevel(mCurrentLevel > 1 ? mCurrentLevel - 1 : mHighestLevelAvailable);
            break;
        case 501:
            if (mCurrentLevel < mHighestLevelAvailable)
                SwitchToLevel(mCurrentLevel + 1);
            else
                SwitchToLevel(1);
            break;
        case 502:
            ActivateGame();
            break;
        }
    }
    SBActivityNode::OnActivityEvent(event, data);
}

// 0x10090bc0 ?SwitchToLevel@SBSandwichNode@@IAEXH@Z
void SBSandwichNode::SwitchToLevel(int level)
{
    DeactivateGame();
    mNextLevel = level;
    if (level > mHighestLevelAvailable)
        mHighestLevelAvailable = level;
    mLevelCounters[mCurrentLevel - 1]->SetCurrentSequence(1);
    mNextLevelBtn->Hide();
    mLastLevelBtn->Hide();
}

// 0x10090c40 ?InitializeLevel@SBSandwichNode@@IAEXH@Z
void SBSandwichNode::InitializeLevel(int level)
{
    DeactivateGame();
    mCurrentLevel = level;
    if (mCurrentLevel <= 0)
        mCurrentLevel = 10;
    if (mCurrentLevel > 10)
        mCurrentLevel = 1;
    IMBAnim *counter = mLevelCounters[mCurrentLevel - 1];
    counter->SetCurrentSequence(2);
    counter->Play();
    counter->Show();
    mPointGoal = level * 3 + 15;
    mCurrentPoints = 0;
    AddPoints(0);
    SetNumberBrooms(5);
}

// TODO 0x10090d00 ?SetNumberBrooms@SBSandwichNode@@IAEXH@Z: inline choices differ (the original
// inlines the size test and calls the two-argument insert out of line; tried size()/empty(),
// insert/push_back, the IMBWorld helpers) (78% similar)
void SBSandwichNode::SetNumberBrooms(int num)
{
    // the brooms are the lives: clones of the first one, 30 pixels apart
    MBComPtr<IMBAnim> proto;
    if (num > mBrooms.size()) {
        if (!mBrooms.empty()) {
            proto = mBrooms[0];
        } else {
            proto = CreateAnim("SHBroomCounter");
            mBrooms.push_back(proto);
        }
        while (num > mBrooms.size()) {
            MBComPtr<IMBAnim> broomptr;
            IMBAnim *broom;
            proto->Clone(&broom);
            broomptr.Attach(broom);
            mWorld->GetRoot().AddObject(broom);
            broomptr->Play();
            broomptr->Move(MBPoint3(mBrooms.size() * 30, 0, 0));
            mBrooms.push_back(broomptr);
        }
    } else {
        while (mBrooms.size() > num) {
            MBComPtr<IMBAnim> broom(mBrooms.back());
            mBrooms.pop_back();
            mWorld->GetRoot().RemoveObject(broom);
        }
    }
}

// 0x10090fd0 ?AddPoints@SBSandwichNode@@IAEXH@Z
void SBSandwichNode::AddPoints(int num)
{
    // the progress sandwich grows with the points: 13 layers and the top
    mCurrentPoints += num;
    int layers = 0;
    if (mPointGoal > 0) {
        layers = mCurrentPoints * 12 / mPointGoal;
        if (layers > 13)
            layers = 13;
    }
    int i;
    for (i = 0; i <= layers; i++)
        while (mProgressSandwich[i]->Show())
            ;
    for (; i < 13; i++)
        mProgressSandwich[i]->Hide();
    while (mProgressSandwich[13]->Show())
        ;
}

// 0x100910a0 ?LaunchIngredient@SBSandwichNode@@IAE_NH@Z
bool SBSandwichNode::LaunchIngredient(int type)
{
    // the machine throws faster on the higher levels
    float levelmult = pow(kSpeedPerLevelMultiplier, (float)(mCurrentLevel - 1));
    MBComPtr<IMBGameClock> clock(MBIID_IMBGameClock);
    mNextLaunch = clock->GetGameTime() + kLaunchDelay / levelmult;
    mNextLaunch += MBrandom((long)-(kLaunchDelayRandomness / levelmult), (long)(kLaunchDelayRandomness / levelmult));
    if (mMachine->GetCurrentSequenceIndex() != 0)
        return false;
    SHIngredientType *ingredient = new SHIngredientType;
    ingredient->mType = type;
    ingredient->mPosition[0] = kLaunchPoint.x;
    ingredient->mPosition[1] = kLaunchPoint.y;
    ingredient->mPosition[2] = kLaunchPoint.z;
    ingredient->mVelocity[0] = 0;
    ingredient->mVelocity[1] = 0;
    ingredient->mVelocity[2] = 0;
    IMBAnim *clone = 0;
    switch (type) {
    case 0:
        mBreadPrototype[1]->Clone(&clone);
        break;
    case 1:
        mIngredientPrototype[MBrandom(0, 9)]->Clone(&clone);
        break;
    case 2:
        mNoningredientPrototype[MBrandom(0, 6)]->Clone(&clone);
        break;
    }
    ingredient->mImage.Attach(clone);
    mWorld->AddObject(clone);
    clone->SetCurrentSequence(0);
    clone->Play();
    clone->Hide();
    mFlyingStack.push_back(ingredient);
    mMachine->SetCurrentSequence(1);
    return true;
}

// 0x10091370 ?CatchIngredient@SBSandwichNode@@IAEXPAUSHIngredientType@1@@Z
void SBSandwichNode::CatchIngredient(SHIngredientType *i)
{
    i->mImage->SetCurrentSequence(1);
    MBComPtr<IMBSoundManager> sm(MBIID_IMBSoundManager);
    long channel = FindFreeChannel(30, 39);
    switch (i->mType) {
    case 0:
        // the top slice of bread ends the round
        i->mVelocity[0] = 0;
        i->mVelocity[1] = 0;
        i->mVelocity[2] = 0;
        i->mPosition[0] = mSandwichStack.back()->mPosition[0];
        i->mPosition[1] = mSandwichStack.back()->mPosition[1] - kIngredientYOffset;
        i->mPosition[2] = mSandwichStack.back()->mPosition[2] + 1;
        i->mImage->Place(MBPoint3(i->mPosition[0], i->mPosition[1], i->mPosition[2]));
        mSandwichStack.push_back(i);
        MBForceSound("SHSEbreadHitFloor", channel, 1);
        WinRound();
        break;
    case 1:
        i->mVelocity[0] = 0;
        i->mVelocity[1] = 0;
        i->mVelocity[2] = 0;
        i->mPosition[0] = mSandwichStack.back()->mPosition[0];
        i->mPosition[1] = mSandwichStack.back()->mPosition[1] - kIngredientYOffset;
        i->mPosition[2] = mSandwichStack.back()->mPosition[2] + 1;
        i->mImage->Place(MBPoint3(i->mPosition[0], i->mPosition[1], i->mPosition[2]));
        mSandwichStack.push_back(i);
        MBForceSound("SHSEingrdHitPlate", channel, 1);
        break;
    case 2:
        // a boot loses the round
        i->mVelocity[0] = 0;
        i->mVelocity[1] = 0;
        i->mVelocity[2] = 0;
        i->mPosition[0] = mSandwichStack.back()->mPosition[0];
        i->mPosition[1] = mSandwichStack.back()->mPosition[1] - kIngredientYOffset;
        i->mPosition[2] = mSandwichStack.back()->mPosition[2] + 1;
        i->mImage->Place(MBPoint3(i->mPosition[0], i->mPosition[1], i->mPosition[2]));
        mSandwichStack.push_back(i);
        MBForceSound("SHSEbootHitFloor", channel, 1);
        LoseRound();
        break;
    }
    sm->SetChannelVolume(channel, sSfxVolume);
}

// 0x10091720 ?LoseRound@SBSandwichNode@@IAEXXZ
void SBSandwichNode::LoseRound()
{
    DeactivateGame();
    mBeulah->Play();
    mBeulah->SetCurrentSequence(1);
    mBeulahState = kBSCleanUp;
}

// 0x10091770 ?WinRound@SBSandwichNode@@IAEXXZ
void SBSandwichNode::WinRound()
{
    MBComPtr<ISBGameState> gs(MBIID_CSBGameState);
    DeactivateGame();
    // the two slices of bread do not count
    AddPoints(mSandwichStack.size() - 2);
    if (mCurrentPoints >= mPointGoal) {
        if (mCurrentLevel < 10)
            SwitchToLevel(mCurrentLevel + 1);
        else
            SwitchToLevel(10);
        if (!gs->GetProperty("PRsandwichWInInventory", 0)) {
            // the first finished sandwich goes into the inventory
            mBeulah->Play();
            mBeulah->SetCurrentSequence(1);
            mBeulahState = kBSFinished;
            AddItemToInventory(MBString("PRsandwichW0"), false);
            gs->SetProperty("SHlevelOne_Done", 1);
        }
    } else {
        ActivateGame();
    }
}

// 0x100919a0 ?ActivateGame@SBSandwichNode@@IAEXXZ
void SBSandwichNode::ActivateGame()
{
    MBComPtr<ISBGameState> gs(MBIID_CSBGameState);
    bool player_is_mickey = gs->GetProperty("HCMickeyCaptive", 0) == 0;
    SHIngredientType **it;
    for (it = mSandwichStack.begin(); it != mSandwichStack.end(); it++) {
        mWorld->RemoveObject((*it)->mImage);
        delete *it;
    }
    mSandwichStack.clear();
    for (it = mFlyingStack.begin(); it != mFlyingStack.end(); it++) {
        mWorld->RemoveObject((*it)->mImage);
        delete *it;
    }
    mFlyingStack.clear();
    // the bottom slice of bread
    SHIngredientType *ingredient = new SHIngredientType;
    IMBAnim *piece;
    mBreadPrototype[0]->Clone(&piece);
    mWorld->AddObject(piece);
    piece->Show();
    piece->SetCurrentSequence(0);
    piece->Play();
    ingredient->mImage.Attach(piece);
    ingredient->mType = 1;
    ingredient->mVelocity[0] = 0;
    ingredient->mVelocity[1] = 0;
    ingredient->mVelocity[2] = 0;
    ingredient->mPosition[0] = 0;
    ingredient->mPosition[1] = 0;
    ingredient->mPosition[2] = 0;
    mSandwichStack.push_back(ingredient);
    mMachineButton->Hide();
    while (mMachine->Show())
        ;
    mSpinner->Play();
    mSpinner->SetCurrentSequence(2);
    mSpinner->Hide();
    mActive = true;
    HideCursor();
    LaunchIngredient(1);
    DisableHotspots();
    if (mBeulahState == kBSCleanUp) {
        SetNumberBrooms(5);
        mBeulah->Play();
        mBeulah->SetCurrentSequence(1);
        mBeulahState = kBSAllClean;
    } else if (mCurrentPoints == 0) {
        MBForceSound(player_is_mickey ? "sh08amc0080" : "sh08ami0080", 20, 1);
    }
}

// 0x10091d40 ?DeactivateGame@SBSandwichNode@@IAEXXZ
void SBSandwichNode::DeactivateGame()
{
    mMachine->Hide();
    while (mMachineButton->Show())
        ;
    if (mActive)
        EnableHotspots();
    mNextLaunch = 0;
    mActive = false;
    ShowCursor();
}

// 0x10091db0 ?DisableHotspots@SBSandwichNode@@IAEXXZ
void SBSandwichNode::DisableHotspots()
{
    mHotspotActive = false;
    for (MBComPtr<IMBScreenPoly> *hotspot = mHotspots.begin(); hotspot != mHotspots.end(); hotspot++)
        (*hotspot)->Deactivate();
}

// 0x10091df0 ?EnableHotspots@SBSandwichNode@@IAEXXZ
void SBSandwichNode::EnableHotspots()
{
    mHotspotActive = true;
    for (MBComPtr<IMBScreenPoly> *hotspot = mHotspots.begin(); hotspot != mHotspots.end(); hotspot++)
        (*hotspot)->Activate();
}

// Template and inline instances this unit also emits:
// 0x10043870 ??0?$MBComPtr@VIMBAnim@@@@QAE@ABV0@@Z
// 0x100438c0 ??1?$MBComPtr@VISBAnimButton@@@@QAE@XZ
// 0x10043920 ??4?$MBComPtr@VIMBAnim@@@@QAEAAV0@PAVIMBAnim@@@Z
// 0x1008de00 ??_GSBSandwichNode@@UAEPAXI@Z
// 0x1008de40 ??1?$MBVector@V?$MBComPtr@VIMBAnim@@@@@@QAE@XZ
// 0x1008dec0 ??1?$MBVector@PAUSHIngredientType@SBSandwichNode@@@@QAE@XZ
// 0x10091e30 ??0?$MBComPtr@VIMBAnim@@@@QAE@XZ
// 0x10091e50 ??1?$vector@PAUSHIngredientType@SBSandwichNode@@V?$MBAllocator@PAUSHIngredientType@SBSandwichNode@@@@@std@@QAE@XZ
// 0x10091ea0 ?insert@?$vector@V?$MBComPtr@VIMBAnim@@@@V?$MBAllocator@V?$MBComPtr@VIMBAnim@@@@@@@std@@QAEPAV?$MBComPtr@VIMBAnim@@@@PAV3@ABV3@@Z
// 0x10092080 ?size@?$vector@V?$MBComPtr@VIMBAnim@@@@V?$MBAllocator@V?$MBComPtr@VIMBAnim@@@@@@@std@@QBEIXZ
// 0x100920b0 ?_Destroy@?$vector@V?$MBComPtr@VIMBAnim@@@@V?$MBAllocator@V?$MBComPtr@VIMBAnim@@@@@@@std@@IAEXPAV?$MBComPtr@VIMBAnim@@@@0@Z
// 0x100920f0 ?erase@?$vector@PAUSHIngredientType@SBSandwichNode@@V?$MBAllocator@PAUSHIngredientType@SBSandwichNode@@@@@std@@QAEPAPAUSHIngredientType@SBSandwichNode@@PAPAU34@0@Z
// 0x10092130 ?_Destroy@?$vector@PAUSHIngredientType@SBSandwichNode@@V?$MBAllocator@PAUSHIngredientType@SBSandwichNode@@@@@std@@IAEXPAPAUSHIngredientType@SBSandwichNode@@0@Z
// 0x10092180 ?AddRef@?$MBComPtr@VIMBAnim@@@@QAEXXZ
// 0x100921a0 ?insert@?$vector@V?$MBComPtr@VIMBAnim@@@@V?$MBAllocator@V?$MBComPtr@VIMBAnim@@@@@@@std@@QAEXPAV?$MBComPtr@VIMBAnim@@@@IABV3@@Z
// 0x10092500 ?insert@?$vector@PAUSHIngredientType@SBSandwichNode@@V?$MBAllocator@PAUSHIngredientType@SBSandwichNode@@@@@std@@QAEXPAPAUSHIngredientType@SBSandwichNode@@IABQAU34@@Z
// 0x100927a0 ?_Ucopy@?$vector@V?$MBComPtr@VIMBAnim@@@@V?$MBAllocator@V?$MBComPtr@VIMBAnim@@@@@@@std@@IAEPAV?$MBComPtr@VIMBAnim@@@@PBV3@0PAV3@@Z
// 0x100927f0 ?_Ufill@?$vector@V?$MBComPtr@VIMBAnim@@@@V?$MBAllocator@V?$MBComPtr@VIMBAnim@@@@@@@std@@IAEXPAV?$MBComPtr@VIMBAnim@@@@IABV3@@Z
// 0x10092830 ?copy@std@@YAPAPAUSHIngredientType@SBSandwichNode@@PAPAU23@00@Z
// 0x10092870 ?copy@std@@YAPAV?$MBComPtr@VIMBAnim@@@@PAV2@00@Z
// 0x100928e0 ?fill@std@@YAXPAV?$MBComPtr@VIMBAnim@@@@0ABV2@@Z
// 0x10092930 ?copy_backward@std@@YAPAV?$MBComPtr@VIMBAnim@@@@PAV2@00@Z
// 0x100929a0 ?_Construct@std@@YAXPAV?$MBComPtr@VIMBAnim@@@@ABV2@@Z

// Static initializers and their atexit stubs: the PDB numbers them by the original header set, which
// ours does not reproduce yet, so they cannot be claimed by name.
// 0x1008d450 _$E49
// 0x1008d470 _$E46
// 0x1008da40 _$E48
// 0x1008da60 _$E47
// 0x1008da90 _$E53
// 0x1008dab0 _$E52
// 0x1008daf0 _$E56
// 0x1008db10 _$E55
// 0x1008db40 _$E59
// 0x1008db60 _$E58
// 0x10092a10 _$E70
// 0x10092a50 _$E66
// 0x10092a70 _$E69
