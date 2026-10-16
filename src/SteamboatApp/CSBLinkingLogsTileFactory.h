#pragma once
// CSBLinkingLogsTileFactory.h: makes the linking-logs tile of a tile description
// (SBLinkingLogsTileFactory.obj, declaration from SteamboatApp.pdb).
#include "CLinkingLogsTile.h"

class CSBLinkingLogsTileFactory {
public:
    CSBLinkingLogsTileFactory();
    virtual ~CSBLinkingLogsTileFactory();
    bool CreateTile(CLinkingLogsTile *&pNewTile, const CLinkingLogsTile::tileinfo &refInfo, CSBLinkingLogsGameBoard *pGameBoard);
    void DestroyTile(CLinkingLogsTile *&pNewTile);
};
