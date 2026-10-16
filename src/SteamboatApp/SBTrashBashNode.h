#pragma once
// SBTrashBashNode.h: the trash-bash activity, a memory game on stacks of trash tiles
// (SBTrashBashNode.obj, declaration from SteamboatApp.pdb).
#include "SBActivityNode.h"
#include "CActivityLeveler.h"
#include "CSBTrashTile.h"
#include "SBTrashSquare.h"

struct MBMouseEvent;

// game state property holding the highest level the player reached
extern const char *TB_MAX_LEVEL_PROP_NAME;

class SBTrashBashNode : public SBActivityNode {
public:
    SBTrashBashNode(IMBGameEngine *engine, MBString spaceCode);
    SBTrashBashNode(IMBGameEngine *engine, MfAiSpace *space);
    virtual ~SBTrashBashNode();
    virtual void OnLMouseDown(MBMouseEvent &evt);
    virtual void OnLMouseUp(MBMouseEvent &evt);
    virtual void ListenToEvent(long channel, long event, void *data);
    virtual void OnStart();
    virtual void OnEnd();
    virtual void Idle();
protected:
    virtual void OnActivityEvent(long event, void *data);
    void LoadTrash();
    void InitGameboard();
    void ClearBoard();
    bool IsBoardCleared();
    void PlayTile(SBTrashSquare *square);
    void ResetGame();
    MBVector<SBTrashSquare *> mTrashSquares;            // +0x00d0
    MBVector<SBTrashSquare *> mOpenSquares;             // +0x00e0
    unsigned long mEmptyTrashAt;                        // +0x00f0
    CSBTrashTile *mLeftTile;                            // +0x00f4
    CSBTrashTile *mRightTile;                           // +0x00f8
    SBTrashSquare *mLeftTileSquare;                     // +0x00fc
    SBTrashSquare *mRightTileSquare;                    // +0x0100
    long mPairs;                                        // +0x0104
    bool mReadyToPlay;                                  // +0x0108
    long mCansFilled;                                   // +0x010c
    bool mGameStarted;                                  // +0x0110
    bool mFirstMatch;                                   // +0x0111
    unsigned long mGameOverAt;                          // +0x0114
    long mConsecutiveBadMatches;                        // +0x0118
    CActivityLeveler mLeveler;                          // +0x011c
    long mHighestLevelCompleted;                        // +0x017c
};
