#pragma once
// SBSandwichNode.h: the sandwich activity: catch the ingredients the machine throws and stack
// them into a sandwich (SBSandwichNode.obj, declaration from SteamboatApp.pdb).
#include "SBActivityNode.h"

struct MBKeyboardEvent;

class SBSandwichNode : public SBActivityNode {
public:
    SBSandwichNode(IMBGameEngine *engine, MfAiSpace *space);
    SBSandwichNode(IMBGameEngine *engine, MBString spaceCode);
    virtual ~SBSandwichNode();
    virtual void OnStart();
    virtual void OnEnd();
    virtual void StartListening();
    virtual void StopListening();
    virtual void OnKeyDown(MBKeyboardEvent &key);
    virtual void ListenToEvent(long channel, long event, void *data);
    virtual void Idle();
    struct SHIngredientType {
        MBComPtr<IMBAnim> mImage;
        float mPosition[3];
        float mVelocity[3];
        unsigned long mType;
    };
protected:
    virtual void OnActivityEvent(long event, void *data);
    void InitializeLevel(int level);
    void SwitchToLevel(int level);
    void LoseRound();
    void WinRound();
    bool LaunchIngredient(int type);
    void CatchIngredient(SHIngredientType *i);
    void SetNumberBrooms(int num);
    void AddPoints(int num);
    void ActivateGame();
    void DeactivateGame();
    void DisableHotspots();
    void EnableHotspots();
    MBComPtr<IMBAnim> mSavedCursor;                     // +0x00d0
    bool mActive;                                       // +0x00d4
    bool mHotspotActive;                                // +0x00d5
    MBComPtr<ISBAnimButton> mNextLevelBtn;              // +0x00d8
    MBComPtr<ISBAnimButton> mLastLevelBtn;              // +0x00dc
    MBComPtr<IMBAnim> mLevelCounters[10];               // +0x00e0
    unsigned long mHighestLevelAvailable;               // +0x0108
    MBComPtr<ISBAnimButton> mMachineButton;             // +0x010c
    MBComPtr<IMBAnim> mMachine;                         // +0x0110
    MBComPtr<IMBAnim> mSpinner;                         // +0x0114
    long mCurrentLevel;                                 // +0x0118
    long mNextLevel;                                    // +0x011c
    MBVector<MBComPtr<IMBAnim> > mBrooms;               // +0x0120
    MBComPtr<IMBAnim> mProgressSandwich[14];            // +0x0130
    long mCurrentPoints;                                // +0x0168
    long mPointGoal;                                    // +0x016c
    MBComPtr<IMBAnim> mBeulah;                          // +0x0170
    enum {
        kBSNone = 0,
        kBSIntro = 1,
        kBSCleanUp = 2,
        kBSAllClean = 3,
        kBSFinished = 4
    } mBeulahState;                                     // +0x0174
    MBComPtr<IMBAnim> mCharacter;                       // +0x0178
    bool mCharFacingLeft;                               // +0x017c
    long mCharMoved;                                    // +0x0180
    MBPoint mPlatePos;                                  // +0x0184
    MBVector<SHIngredientType *> mSandwichStack;        // +0x018c
    unsigned long mLastUpdate;                          // +0x019c
    MBVector<SHIngredientType *> mFlyingStack;          // +0x01a0
    unsigned long mNextLaunch;                          // +0x01b0
    MBComPtr<IMBAnim> mBreadPrototype[2];               // +0x01b4
    MBComPtr<IMBAnim> mIngredientPrototype[10];         // +0x01bc
    MBComPtr<IMBAnim> mNoningredientPrototype[7];       // +0x01e4
};
