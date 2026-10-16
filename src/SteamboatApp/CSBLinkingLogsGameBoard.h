#pragma once
// CSBLinkingLogsGameBoard.h: the board of the linking-logs activity: the tile grid read from the
// level prefs, the actor and the timeouts (SBLinkingLogsGameBoard.obj, declaration from
// SteamboatApp.pdb).
#include "CSBLinkingLogsActor.h"

class SBLinkingLogsNode;

class CSBLinkingLogsGameBoard {
    friend class CSBLinkingLogsActor;                   // MoveTo calls OnStartMove
    struct tileattributes {
        tileattributes() : nTypeID(0), nProperties(0), bWalkable(false), nTimeout(-1), fVelocity(0.0f) {}
        long nTypeID;                                   // +0x0000
        long nProperties;                               // +0x0004
        MBString strAnimName;                           // +0x0008
        bool bWalkable;                                 // +0x0018
        long nTimeout;                                  // +0x001c
        float fVelocity;                                // +0x0020
    };
public:
    CSBLinkingLogsGameBoard(SBLinkingLogsNode *pNode);
    virtual ~CSBLinkingLogsGameBoard();
    typedef MBMap<CGameBoardPos, CLinkingLogsTile *, std::less<CGameBoardPos> > gridmatrix;
    typedef void (CSBLinkingLogsGameBoard::*pParserFunc)(const MBString &, tileattributes &);
    typedef MBMap<MBString, pParserFunc, std::less<MBString> > parserfunctionmap;
    typedef MBMap<MBString, long, std::less<MBString> > tiletypeemap;
    typedef MBMap<long, tileattributes, std::less<long> > attributemap;
    typedef MBList<MBString> stringlist;
    void InitializeGameBoard(const MBRect &refOriginTile, long nLevel, bool bPlayMickye, IMBStream *iStream);
    long GetLevelCompleted();
    void Update();
    int RowCount();
    int ColCount();
    void Idle();
    void NotifyEvent(long channel, long event, void *data);
    void OnEndMove(const CGameBoardPos &posEnd);
    CLinkingLogsTile &GetTileAt(const CGameBoardPos &refPos);
protected:
    bool LoadBoardSize(IMBPref *iPref, IMBStream *iStream);
    bool LoadTile(long &nType, long &nProperties, IMBPref *iPref, IMBStream *iStream, const CGameBoardPos &posTile);
    bool LoadTypeAttributes(tileattributes &refAttrib, IMBPref *iPref, IMBStream *iStream, long nType);
    void NotifyMoveStarted(CLinkingLogsTile &refTopTile);
    void NotifyMoveEnded(CLinkingLogsTile &refTopTile);
    void HandleClick(long channel, long event, IMBUnknown *iUnknown);
    void HandleAnimEvent(long channel, long event, MBAnimEventStruct *pEventData);
    void HandleTileEvent(long channel, long event, CLinkingLogsTile *pTile);
    bool OnTileClicked(CLinkingLogsTile &refTopTile, CLinkingLogsTile &refBottomTile);
    bool OnStartMove(const CGameBoardPos &posStart, const CGameBoardPos &posEnd);
    void OnIdle();
private:
    unsigned int GetTokenList(const MBString &strSource, char cDelim, stringlist &listTokens) const;
    void ParseName(const MBString &strData, tileattributes &refAttrib);
    void ParseWalkable(const MBString &strData, tileattributes &refAttrib);
    void ParseType(const MBString &strData, tileattributes &refAttrib);
    void ParseTimeout(const MBString &strData, tileattributes &refAttrib);
    void ParseVelocity(const MBString &strData, tileattributes &refAttrib);
    MBRect m_rectBoardOrigin;                           // +0x0004
    long m_nNumRow;                                     // +0x0014
    long m_nNumCol;                                     // +0x0018
    gridmatrix m_matGameGrid;                           // +0x001c
    parserfunctionmap m_mapParserFunction;              // +0x002c
    tiletypeemap m_mapTileType;                         // +0x003c
    SBLinkingLogsNode *m_pNode;                         // +0x004c
    CLinkingLogsTile::iunknownmessagemap m_mapIUnknownToTile; // +0x0050
    CSBLinkingLogsActor *m_pActor;                      // +0x0060
    bool m_bComplete;                                   // +0x0064
    long m_nLevel;                                      // +0x0068
    MBComPtr<IMBGameClock> m_iClock;                    // +0x006c
    long m_nIdleTimeout;                                // +0x0070
    long m_nLastMoveTime;                               // +0x0074
    long m_nTotalTimeout;                               // +0x0078
    long m_nGameStartTime;                              // +0x007c
    float m_fDifficulty;                                // +0x0080
};
