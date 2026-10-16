#pragma once
// SBLinkingLogsNode.h: the linking-logs activity scene (SBLinkingLogsNode.obj, declaration from
// SteamboatApp.pdb).
#include "SBActivityNode.h"
#include "CActivityLeveler.h"

class CSBLinkingLogsGameBoard;

// game state property holding the highest level the player reached
extern const char *MAX_LEVEL_PROP_NAME;
// the board's first tile: top left corner and size
extern MBPoint BOARD_ORIGIN;
extern MBSize BOARD_TILE_SIZE;

class SBLinkingLogsNode : public SBActivityNode {
public:
    SBLinkingLogsNode(IMBGameEngine *engine, MfAiSpace *space);
    SBLinkingLogsNode(IMBGameEngine *engine, MBString spaceCode);
    virtual ~SBLinkingLogsNode();
    virtual void OnStart();
    virtual void OnEnd();
    virtual void ListenToEvent(long channel, long event, void *data);
    virtual void Idle();
    void UpdateLevel(long nLevelCompleted);
protected:
    void InitGame();
    CSBLinkingLogsGameBoard *m_pGameBoard;              // +0x00d0
    IMBAnim *m_pBGAnim;                                 // +0x00d4
    CActivityLeveler m_objLeveler;                      // +0x00d8
    bool m_bPlayAsMickey;                               // +0x0138
};
