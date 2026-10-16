#include "CSBLinkingLogsTileFactory.h"
#include "SBCursorItems.h" // every node unit of the original has the cursor item strings
#include <locale> // evidence: the original unit ends with the std::ctype<unsigned short>::id and std::collate<char>::id initializers
#include "CSBLLDoorTile.h"
#include "CSBLLPathTile.h"
#include "CSBLLTrapDoorTile.h"
#include "CSBLLConveyerTile.h"
#include "CSBLLSteamTile.h"
#include "CSBLLOnewayDoorTile.h"

// 0x1007b2b0 ??0CSBLinkingLogsTileFactory@@QAE@XZ
CSBLinkingLogsTileFactory::CSBLinkingLogsTileFactory()
{
}

// 0x1007b310 ??1CSBLinkingLogsTileFactory@@UAE@XZ
CSBLinkingLogsTileFactory::~CSBLinkingLogsTileFactory()
{
}

// The type bits of the conveyer and the one-way door are the other way round from the names the
// PDB gives them (kConveyer makes a one-way door, kOneWayDoor a conveyer).
// EQUIV 0x1007b330 ?CreateTile@CSBLinkingLogsTileFactory@@QAE_NAAPAVCLinkingLogsTile@@ABUtileinfo@2@PAVCSBLinkingLogsGameBoard@@@Z: reviewed: the code matches; the original's unwind table emits the conveyer's cleanup funclet after the one-way door's, ours in state order
bool CSBLinkingLogsTileFactory::CreateTile(CLinkingLogsTile *&pNewTile, const CLinkingLogsTile::tileinfo &refInfo,
                                           CSBLinkingLogsGameBoard *pGameBoard)
{
    switch (refInfo.m_nProperties & CLinkingLogsTile::kTypeMask) {
    case CLinkingLogsTile::kDoor:
    case CLinkingLogsTile::kSwitch:
        pNewTile = new CSBLLDoorTile(refInfo, pGameBoard);
        break;
    case CLinkingLogsTile::kPath:
        pNewTile = new CSBLLPathTile(refInfo, pGameBoard);
        break;
    case CLinkingLogsTile::kTrapdoor:
        pNewTile = new CSBLLTrapDoorTile(refInfo, pGameBoard);
        break;
    case CLinkingLogsTile::kOneWayDoor:
        pNewTile = new CSBLLConveyerTile(refInfo, pGameBoard);
        break;
    case CLinkingLogsTile::kSteam:
        pNewTile = new CSBLLSteamTile(refInfo, pGameBoard);
        break;
    case CLinkingLogsTile::kConveyer:
        pNewTile = new CSBLLOnewayDoorTile(refInfo, pGameBoard);
        break;
    default:
        pNewTile = 0;
        break;
    }
    return pNewTile != 0;
}

// 0x1007b540 ?DestroyTile@CSBLinkingLogsTileFactory@@QAEXAAPAVCLinkingLogsTile@@@Z
void CSBLinkingLogsTileFactory::DestroyTile(CLinkingLogsTile *&pNewTile)
{
    delete pNewTile;
    pNewTile = 0;
}

// Template and inline instances this unit also emits:
// 0x1007b2d0 ??_GCSBLinkingLogsTileFactory@@UAEPAXI@Z

// 0x1007ac70 _$E22
// 0x1007ac90 _$E19
// 0x1007b260 _$E21
// 0x1007b280 _$E20
// 0x1007b570 _$E34
// 0x1007b5b0 _$E30
// 0x1007b5d0 _$E33
