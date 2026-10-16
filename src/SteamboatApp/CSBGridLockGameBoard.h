#pragma once
// CSBGridLockGameBoard.h: the board of the Gridlock puzzle: a grid of tile stacks, the moves
// and the sounds (SBGridLockGameBoard.obj, declaration from SteamboatApp.pdb).
#include "CSBGridlockGameTile.h"
#include "CMBButtonGroup.h"
#include "SBGridlockNode.h"


class CSBGridLockGameBoard {
    friend class CSBGridlockGameTile;
    friend class CSBGridlockActor;
    typedef MBVector<CSBGridlockGameTile> gridsquare;
    typedef MBVector<MBVector<CSBGridlockGameTile> > gridcolumn;
    typedef MBVector<MBVector<MBVector<CSBGridlockGameTile> > > gridmatrix;
    typedef MBMap<CGameBoardPos, int, std::less<CGameBoardPos> > tiletypemap;
    typedef MBMap<int, MBComPtr<IMBAnim>, std::less<int> > animtypemap;
    typedef MBMap<int, CSBGridlockGameTile::tileprop, std::less<int> > tilepropemap;
    typedef MBVector<CGameBoardPos> movepath;
    typedef MBList<MBString> stringlist;
public:
    enum soundid {
        kHonk = 0x01000000,
        kSelect = 0x02000000,
        kMove = 0x04000000,
        kMickey = 0x08000000,
        kMinney = 0x10000000,
        kCantGetThere = 0x00010000,
        kT1 = 0x00010000,
        kGotItem = 0x00020000,
        kT2 = 0x00020000,
        kT3 = 0x00040000,
        kT4 = 0x00080000,
        kV1 = 0x00000001,
        kV2 = 0x00000002,
        kV3 = 0x00000004,
        kV4 = 0x00000008
    };
    CSBGridLockGameBoard(const CSBGridLockGameBoard &);
    CSBGridLockGameBoard(SBGridlockNode *pNode);
    virtual ~CSBGridLockGameBoard();                    // deleting destructor at vftable+0x00
    void InitializeGameBoard(const MBRect &refOriginTile, long nLevel, bool bPlayMickye, IMBStream *iStream);
    long GetLevelCompleted();
    void Update();
    int RowCount();
    int ColCount();
    void Idle();
    void NotifyEvent(long channel, long event, void *data);
protected:
    void InitGameGrid(IMBStream *iStream);
    void LoadTileProperties(IMBStream *iStream, tilepropemap &mapTileProp);
    void LoadAnims(bool bPlayMickye, IMBStream *iStream, animtypemap &mapGameAnims);
    void LoadLayout(IMBStream *iStream, long nLevel, tiletypemap &mapTileType);
    bool BuildPath(const CGameBoardPos &posStart, const CGameBoardPos &posEnd);
    void MoveTile();
    bool IsMoveInProgress();
    bool IsStraightLine(const CGameBoardPos &posStart, const CGameBoardPos &posEnd);
    CSBGridlockGameTile &GetTileAt(const CGameBoardPos &refPos, bool bTopMost);
    void SetTileAt(const CGameBoardPos &refPos, CSBGridlockGameTile &refNewTile, bool bTopMost);
    void SetTileProperties(const CGameBoardPos &refPos, IMBAnim *pAnim, CSBGridlockGameTile::tileprop eProp, animtypemap &mapGameAnims, tilepropemap &mapTileProp);
    void NotifyMoveStarted(CSBGridlockGameTile &refTopTile);
    void NotifyMoveEnded(CSBGridlockGameTile &refTopTile);
    void HandleClick(long channel, long event, IMBUnknown *iUnknown);
    void HandleAnimEvent(long channel, long event, MBAnimEventStruct *pEventData);
    virtual bool OnTileClicked(CSBGridlockGameTile &refTopTile, CSBGridlockGameTile &refBottomTile); // vftable+0x04
    virtual void OnInvalidClick(CSBGridlockGameTile &refStartTile, CSBGridlockGameTile &refDestTile); // vftable+0x08
    virtual bool OnStartMove(CSBGridlockGameTile &refStartTile, CSBGridlockGameTile &refDestTile); // vftable+0x0c
    virtual void OnEndStep(CSBGridlockGameTile &refStartTile, CSBGridlockGameTile &refDestTile); // vftable+0x10
    virtual void OnEndMove(CSBGridlockGameTile &refStartTile, CSBGridlockGameTile &refDestTile); // vftable+0x14
    virtual void OnIdle();                              // vftable+0x18
    unsigned int GetTokenList(const MBString &strSource, char cDelim, stringlist &listTokens) const;
    gridmatrix m_matGameGrid;                           // +0x0004 size 0x10
    movepath m_vecMovePath;                             // +0x0014 size 0x10
    CSBGridlockGameTile::iunknownmessagemap m_mapIUnknownToTile; // +0x0024 size 0x10
    MBRect m_rectBoardOrigin;                           // +0x0034 size 0x10
    CMBButtonGroup m_ButtonGroup;                       // +0x0044 size 0x30
    CSBGridlockGameTile *m_pCurrentStartTile;           // +0x0074 size 0x4
    CSBGridlockGameTile *m_pCurrentDestTile;            // +0x0078 size 0x4
    bool m_bMoveInProgress;                             // +0x007c size 0x1
    long m_nTilesMoveing;                               // +0x0080 size 0x4
    SBGridlockNode *m_pNode;                            // +0x0084 size 0x4
    bool m_bGoalReached;                                // +0x0088 size 0x1
    IMBAnim *m_pCabinetAnim;                            // +0x008c size 0x4
    long m_nLevel;                                      // +0x0090 size 0x4
    bool m_bComplete;                                   // +0x0094 size 0x1
    MBMap<long, MBString, std::less<long> > m_mapIdToSound; // +0x0098 size 0x10
    MBMap<MBString, long, std::less<MBString> > m_mapNameToProp; // +0x00a8 size 0x10
    bool m_bPlayAsMickye;                               // +0x00b8 size 0x1
};
