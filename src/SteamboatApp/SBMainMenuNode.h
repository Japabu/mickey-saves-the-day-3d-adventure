#pragma once
// SBMainMenuNode.h: the main menu: title screen, new game (character, difficulty and game icon
// choice), load game selector, erase and leave confirmation (SBMainMenuNode.obj, declaration
// from SteamboatApp.pdb).
#include "SBActivityNode.h"
#include "ConfirmationBox.h"
#include "SavedGameDB.h"

struct MBKeyboardEvent;
struct MBMouseEvent;
struct MfUserHookData;

struct LoadGameSelectorData {
    short gameIconIndex;
    short thumbnailIndex;
};

enum UIPlayerState {
    kBackgroundIdle = 0,
    kForegroundIdle = 1
};

class SBMainMenuNode : public SBActivityNode {
public:
    SBMainMenuNode(IMBGameEngine *engine, MBString spaceCode);
    SBMainMenuNode(IMBGameEngine *engine, MfAiSpace *space);
    virtual ~SBMainMenuNode();
    virtual void OnStart();
    virtual void OnEnd();
    virtual void OnKeyDown(MBKeyboardEvent &key);
    virtual void OnLMouseDown(MBMouseEvent &evt);
    virtual void OnLMouseUp(MBMouseEvent &evt);
    virtual void OnAnimSeqStarted(unsigned long id, unsigned long seq);
    virtual void ListenToEvent(long channel, long event, void *data);
    virtual void UserEventHook(MfUserHookData *hook);
    virtual void Idle();
protected:
    void RemoveAnim(IMBAnim *&anim);
    ISBAnimButton *CreateButton(const char *buttonName, long clickMessage, long rolloverMessage);
    void RemoveButton(ISBAnimButton *&button);
    IMBScreenPoly *CreateHotRect(const MBRect &rect, long rolloverMessage);
    void RemoveHotRect(IMBScreenPoly *&hotRect);
    void PrepareCharacterSelection();
    void MoveCharactersToBackground();
    void SetChosenCharacter(long character);
    long GetChosenCharacter() const;
    char GetCharacterCode(long character);
    void SetDifficulty(long difficulty);
    long GetDifficulty() const;
    void SetGameIcon(long gameIcon);
    long GetGameIcon() const;
    void CreateTitleScreen();
    void RemoveTitleScreen();
    void CreateMainMenuButtons();
    void RemoveMainMenuButtons();
    void CreateLoadGameSelector();
    void RemoveLoadGameSelector();
    void GetLoadGameSelectorData(MBVector<LoadGameSelectorData> &data);
    void UpdateLoadGameSelector();
    void CreateCharacterSelectionButtons();
    void RemoveCharacterSelectionButtons();
    void CreateDifficultyButtons();
    void RemoveDifficultyButtons();
    void CreateGameIcons();
    void RemoveGameIcons();
    void RemoveUnselectedGameIcons();
    void RemoveSelectedGameIcon();
    void CreateIconSelectionButtons();
    void RemoveIconSelectionButtons();
    void CreateLetsGoButtons();
    void RemoveLetsGoButtons();
    void LetsGo();
    static bool sIsGameInProgress;
    static short sCurrentGameIndex;
    long mCurrentState;                                 // +0x00d0
    CSavedGameDB mSavedGameDB;                          // +0x00d4
    bool mUIStarted;                                    // +0x00e8
    IMBAnim *mTitleScreen;                              // +0x00ec
    ISBAnimButton *mNewButton;                          // +0x00f0
    IMBAnim *mNewBalloonString;                         // +0x00f4
    ISBAnimButton *mLoadButton;                         // +0x00f8
    IMBAnim *mLoadBalloonString;                        // +0x00fc
    ISBAnimButton *mExitButton;                         // +0x0100
    ISBAnimButton *mReturnToGameButton;                 // +0x0104
    IMBAnim *mLoadBoard;                                // +0x0108
    ISBAnimButton *mLeftLoadArrow;                      // +0x010c
    ISBAnimButton *mRightLoadArrow;                     // +0x0110
    IMBAnim *mLoadGameIcons;                            // +0x0114
    IMBAnim *mLoadGameThumbnails;                       // +0x0118
    MBVector<LoadGameSelectorData> mLoadGameSelectorData; // +0x011c
    long mLoadGameSelectorIndex;                        // +0x012c
    ISBAnimButton *mBackButton;                         // +0x0130
    IMBScreenPoly *mMickeyHotRect;                      // +0x0134
    IMBScreenPoly *mMinnieHotRect;                      // +0x0138
    ISBAnimButton *mEasyButton;                         // +0x013c
    ISBAnimButton *mHardButton;                         // +0x0140
    IMBAnim *mGameButtonBoard;                          // +0x0144
    ISBAnimButton *mGameButtons[8];                     // +0x0148
    ISBAnimButton *mLetsGoButton;                       // +0x0168
    IMBAnim *mThoughtBubble;                            // +0x016c
    long mCharacter;                                    // +0x0170
    long mDifficulty;                                   // +0x0174
    long mGameIcon;                                     // +0x0178
    UIPlayerState mMickeyState;                         // +0x017c
    UIPlayerState mMinnieState;                         // +0x0180
    long mEraseGameIndex;                               // +0x0184
    ConfirmationBox mEraseGameDialog;                   // +0x0188
    ConfirmationBox mLeaveGameDialog;                   // +0x01d8
    bool mLoadingGame;                                  // +0x0228
};
