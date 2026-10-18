#include "CSBLinkingLogsGameBoard.h"
#include "SBCursorItems.h" // every node unit of the original has the cursor item strings
#include <locale> // evidence: the original unit ends with the std::ctype<unsigned short>::id and std::collate<char>::id initializers
#include "SBLinkingLogsNode.h"
#include "CSBLinkingLogsTileFactory.h"
#include "CSBLinkingLogsActor.h"
#include <stdlib.h>
#include <stdio.h>
#include <algorithm>

// The prefs of a level: the board layout and the tile types.
const char PSZ_PROPERTIES_SECTION_NAME[] = "level %d types";
const char PSZ_LAYOUT_SECTION_NAME[] = "level %d board";
const char PSZ_TYPE_VALUE_FORMAT[] = "type%d";
const char PSZ_ROW_COL_FORMAT[] = "row%d col%d";
// how much longer the time limits are on the easy level
const float LL_DIFF_EASY = 0.25f;
const float LL_DIFF_HARD = 0.0f;
const MBString STR_ANIM_FILENAME_PREFIX("LLAN");
const MBString STR_DEFAULT_PROPERTIES("anim = none : walkable = false : type = path");

// TODO 0x10072050 ??0CSBLinkingLogsGameBoard@@QAE@PAVSBLinkingLogsNode@@@Z: first draft (84% similar)
CSBLinkingLogsGameBoard::CSBLinkingLogsGameBoard(SBLinkingLogsNode *pNode)
    : m_nNumRow(0), m_nNumCol(0), m_pNode(pNode), m_pActor(0), m_bComplete(false), m_nLevel(1), m_nIdleTimeout(-1),
      m_nLastMoveTime(0), m_nTotalTimeout(-1), m_nGameStartTime(0)
{
    MBComPtr<ISBGameState> gs(MBIID_CSBGameState);
    if (gs->GetProperty("difficulty", 0) == 0)
        m_fDifficulty = LL_DIFF_EASY;
    else
        m_fDifficulty = LL_DIFF_HARD;
    // the parsers of the tile type attributes
    m_mapParserFunction["anim"] = &CSBLinkingLogsGameBoard::ParseName;
    m_mapParserFunction["walkable"] = &CSBLinkingLogsGameBoard::ParseWalkable;
    m_mapParserFunction["type"] = &CSBLinkingLogsGameBoard::ParseType;
    m_mapParserFunction["time"] = &CSBLinkingLogsGameBoard::ParseTimeout;
    m_mapParserFunction["velocity"] = &CSBLinkingLogsGameBoard::ParseVelocity;
    // the property bits of the type names
    m_mapTileType["default"] = CLinkingLogsTile::kInvalidType;
    m_mapTileType["up"] = CLinkingLogsTile::kUp;
    m_mapTileType["down"] = CLinkingLogsTile::kDown;
    m_mapTileType["left"] = CLinkingLogsTile::kLeft;
    m_mapTileType["right"] = CLinkingLogsTile::kRight;
    m_mapTileType["path"] = CLinkingLogsTile::kPath;
    m_mapTileType["door"] = CLinkingLogsTile::kDoor;
    m_mapTileType["onewaydoor"] = CLinkingLogsTile::kOneWayDoor;
    m_mapTileType["switch"] = CLinkingLogsTile::kSwitch;
    m_mapTileType["trapdoor"] = CLinkingLogsTile::kTrapdoor;
    m_mapTileType["conveyer"] = CLinkingLogsTile::kConveyer;
    m_mapTileType["steam"] = CLinkingLogsTile::kSteam;
    m_mapTileType["start"] = CLinkingLogsTile::kStart;
    m_mapTileType["finish"] = CLinkingLogsTile::kFinish;
    m_iClock.CreateObject(MBIID_IMBGameClock);
}

// 0x10072ef0 ??1CSBLinkingLogsGameBoard@@UAE@XZ
CSBLinkingLogsGameBoard::~CSBLinkingLogsGameBoard()
{
    for (gridmatrix::iterator i = m_matGameGrid.begin(); i != m_matGameGrid.end(); i++)
        delete (*i).second;
    delete m_pActor;
    m_pActor = 0;
}

// 0x100731b0 ?OnTileClicked@CSBLinkingLogsGameBoard@@IAE_NAAVCLinkingLogsTile@@0@Z
bool CSBLinkingLogsGameBoard::OnTileClicked(CLinkingLogsTile &refTopTile, CLinkingLogsTile &refBottomTile)
{
    return true;
}

// 0x100731d0 ?OnStartMove@CSBLinkingLogsGameBoard@@IAE_NABVCGameBoardPos@@0@Z
bool CSBLinkingLogsGameBoard::OnStartMove(const CGameBoardPos &posStart, const CGameBoardPos &posEnd)
{
    return true;
}

// EQUIV 0x100731f0 ?OnEndMove@CSBLinkingLogsGameBoard@@QAEXABVCGameBoardPos@@@Z: reviewed: only the inlined
// destructor of the "CR" string differs (the original walks it in eax, VC6 in ecx; a named local
// gives the same)
void CSBLinkingLogsGameBoard::OnEndMove(const CGameBoardPos &posEnd)
{
    if (GetTileAt(posEnd).IsFinish()) {
        MBComPtr<ISBGameState> gs(MBIID_CSBGameState);
        m_bComplete = true;
        // with only the first level open the game goes on to the credits
        if (gs->GetProperty("LLMaxLevel", 1) == 1)
            m_pNode->SendExitSceneEvent("CR");
        m_pNode->UpdateLevel(GetLevelCompleted());
    }
}

// 0x100733d0 ?OnIdle@CSBLinkingLogsGameBoard@@IAEXXZ
void CSBLinkingLogsGameBoard::OnIdle()
{
    // too long without a move, or the game took too long: the actor is sent home
    long nNow = m_iClock->GetGameTime();
    if ((m_pActor && m_nIdleTimeout + m_nLastMoveTime < nNow) || m_nGameStartTime + m_nTotalTimeout < nNow) {
        m_nGameStartTime = nNow;
        m_nLastMoveTime = nNow;
        m_pActor->GoHome(CSBLinkingLogsActor::Crane);
    }
}

// TODO 0x10073420 ?InitializeGameBoard@CSBLinkingLogsGameBoard@@QAEXABVMBRect@@J_NPAVIMBStream@@@Z: first draft (87% similar)
void CSBLinkingLogsGameBoard::InitializeGameBoard(const MBRect &refOriginTile, long nLevel, bool bPlayMickye, IMBStream *iStream)
{
    // read the level's board from the prefs and lay its tiles out row by row
    attributemap mapAttributes;
    CSBLinkingLogsTileFactory factory;
    MBComPtr<IMBPref> pref(MBIID_CMBPref);
    MBComPtr<IMBPref> typePref(MBIID_CMBPref);
    m_nLevel = nLevel;
    LoadBoardSize(pref, iStream);
    CGameBoardPos pos(RowCount() - 1, ColCount() - 1, 1, 0, 0, 0);
    pos.Begin();
    m_rectBoardOrigin = refOriginTile;
    MBRect rectTile = m_rectBoardOrigin;
    while (pos.NextRow()) {
        pos.BeginCol();
        while (pos.NextCol()) {
            long nType;
            long nProperties;
            LoadTile(nType, nProperties, pref, iStream, pos);
            attributemap::iterator it = mapAttributes.find(nType);
            if (it == mapAttributes.end()) {
                tileattributes attrib;
                if (LoadTypeAttributes(attrib, typePref, iStream, nType))
                    it = mapAttributes.insert(attributemap::value_type(nType, attrib)).first;
            }
            if (nProperties == -1)
                nProperties = (*it).second.nProperties;
            CLinkingLogsTile::tileinfo info((*it).second.strAnimName, (*it).second.bWalkable, (*it).second.nTimeout, nType,
                                            pos, rectTile, (CLinkingLogsTile::tiletype)nProperties, (*it).second.fVelocity);
            CLinkingLogsTile *pTile;
            if (factory.CreateTile(pTile, info, this)) {
                pTile->AddToMessageMap(m_mapIUnknownToTile);
                if (pTile->IsStart()) {
                    m_pActor = new CSBLinkingLogsActor(pTile, this);
                    m_pActor->SetActor(bPlayMickye);
                }
                m_matGameGrid[pos] = pTile;
            }
            long nWidth = m_rectBoardOrigin.right - m_rectBoardOrigin.left;
            rectTile.left += nWidth;
            rectTile.right += nWidth;
        }
        long nHeight = m_rectBoardOrigin.bottom - m_rectBoardOrigin.top;
        rectTile.left = m_rectBoardOrigin.left;
        rectTile.right = m_rectBoardOrigin.right;
        rectTile.top += nHeight;
        rectTile.bottom += nHeight;
    }
    m_nGameStartTime = m_iClock->GetGameTime();
    Update();
}

// 0x10073ab0 ??0tileinfo@CLinkingLogsTile@@QAE@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@_NJJABVCGameBoardPos@@ABVMBRect@@W4tiletype@1@M@Z
CLinkingLogsTile::tileinfo::tileinfo(MBString strAnimName, bool bWalkable, long nTimeout, long nBaseType,
                                     const CGameBoardPos &posTile, const MBRect &recTile, tiletype eType, float fVelocity)
    : m_strAnimName(strAnimName), m_nBaseType(nBaseType), m_nProperties(eType), m_bWalkable(bWalkable),
      m_nTimeout(nTimeout), m_fVelocity(fVelocity), m_posBoard(posTile), m_recTile(recTile)
{
}

// EQUIV 0x10073d50 ?LoadTypeAttributes@CSBLinkingLogsGameBoard@@IAE_NAAUtileattributes@1@PAVIMBPref@@PAVIMBStream@@J@Z: reviewed: the original keeps this in ebx and a zero in ebp, and shares one stack byte between the missing flag and the ' ' passed to std::remove
bool CSBLinkingLogsGameBoard::LoadTypeAttributes(tileattributes &refAttrib, IMBPref *iPref, IMBStream *iStream, long nType)
{
    // "name = value : name = value ..." handed to the parser of each name
    MBString strProperties;
    char pszKey[12] = { 0 };
    char pszSection[16] = { 0 };
    bool bLoaded = false;
    stringlist listTokens;
    sprintf(pszSection, PSZ_PROPERTIES_SECTION_NAME, m_nLevel);
    sprintf(pszKey, PSZ_TYPE_VALUE_FORMAT, nType);
    bool bMissing = iPref->GetString(iStream, MBString(pszSection), MBString(pszKey), strProperties, MBString("")) == 0;
    if (bMissing && nType == 0)
        strProperties = STR_DEFAULT_PROPERTIES;
    if (strProperties.size()) {
        strProperties.resize(std::remove(strProperties.begin(), strProperties.end(), ' ') - strProperties.begin());
        GetTokenList(strProperties, ':', listTokens);
        for (stringlist::iterator it = listTokens.begin(); it != listTokens.end(); it++) {
            MBString &strToken = *it;
            unsigned int nPos = strToken.find('=');
            if (nPos != MBString::npos) {
                MBString strValue = strToken.substr(nPos + 1, strToken.size() - nPos - 1);
                MBString strName = strToken.substr(0, nPos);
                (this->*m_mapParserFunction[strName])(strValue, refAttrib);
            }
        }
        bLoaded = true;
    }
    return bLoaded;
}

// TODO 0x10074280 ?LoadTile@CSBLinkingLogsGameBoard@@IAE_NAAJ0PAVIMBPref@@PAVIMBStream@@ABVCGameBoardPos@@@Z: (88% similar) first draft (70%); the result flag and the stack slots of the buffers and the token list differ
bool CSBLinkingLogsGameBoard::LoadTile(long &nType, long &nProperties, IMBPref *iPref, IMBStream *iStream, const CGameBoardPos &posTile)
{
    // "row1 col2 = type3 : start": the type number and the extra properties of a square
    bool bLoaded = false;
    MBString strTile;
    char pszSection[16] = { 0 };
    char pszKey[16] = { 0 };
    sprintf(pszSection, PSZ_LAYOUT_SECTION_NAME, m_nLevel);
    sprintf(pszKey, PSZ_ROW_COL_FORMAT, posTile.Row(), posTile.Col());
    nProperties = m_mapTileType[MBString("default")];
    nType = 0;
    bool bFound = iPref->GetString(iStream, MBString(pszSection), MBString(pszKey), strTile, MBString("")) > 0;
    if (bFound) {
        stringlist listTokens;
        strTile.resize(std::remove(strTile.begin(), strTile.end(), ' ') - strTile.begin());
        if (GetTokenList(strTile, ':', listTokens)) {
            stringlist::iterator it = listTokens.begin();
            MBString &strType = *it;
            unsigned int nStart = strType.find_first_of("0123456789");
            unsigned int nEnd = strType.find_first_not_of("0123456789", nStart);
            nType = atoi(strType.substr(nStart, nEnd == MBString::npos ? MBString::npos : nEnd - nStart).c_str());
            if (++it != listTokens.end()) {
                nProperties = m_mapTileType[*it];
                bLoaded = true;
            }
        }
    } else
        bLoaded = true;
    return bLoaded;
}

// TODO 0x100748b0 ?LoadBoardSize@CSBLinkingLogsGameBoard@@IAE_NPAVIMBPref@@PAVIMBStream@@@Z: (89% similar) as ParseTimeout, VC6 folds time * difficulty + time into time * (difficulty + 1)
bool CSBLinkingLogsGameBoard::LoadBoardSize(IMBPref *iPref, IMBStream *iStream)
{
    // the size of the board and its time limits (seconds, scaled up by the difficulty)
    bool bLoaded = false;
    MBComPtr<IMBPref> pref(MBIID_CMBPref);
    MBString strValue;
    char pszSection[16] = { 0 };
    sprintf(pszSection, PSZ_LAYOUT_SECTION_NAME, m_nLevel);
    if (iPref->GetString(iStream, MBString(pszSection), MBString("row count"), strValue, MBString(""))) {
        m_nNumRow = atoi(strValue.c_str());
        if (iPref->GetString(iStream, MBString(pszSection), MBString("col count"), strValue, MBString(""))) {
            m_nNumCol = atoi(strValue.c_str());
            if (iPref->GetString(iStream, MBString(pszSection), MBString("idle time"), strValue, MBString(""))) {
                long nIdleTime = atoi(strValue.c_str()) * 1000;
                m_nIdleTimeout = (long)(nIdleTime * m_fDifficulty + nIdleTime);
                if (iPref->GetString(iStream, MBString(pszSection), MBString("total time"), strValue, MBString(""))) {
                    long nTotalTime = atoi(strValue.c_str()) * 1000;
                    m_nTotalTimeout = (long)(nTotalTime * m_fDifficulty + nTotalTime);
                    bLoaded = true;
                }
            }
        }
    }
    return bLoaded;
}

// 0x10075030 ?GetTileAt@CSBLinkingLogsGameBoard@@QAEAAVCLinkingLogsTile@@ABVCGameBoardPos@@@Z
CLinkingLogsTile &CSBLinkingLogsGameBoard::GetTileAt(const CGameBoardPos &refPos)
{
    return *m_matGameGrid[refPos];
}

// 0x10075060 ?Idle@CSBLinkingLogsGameBoard@@QAEXXZ
void CSBLinkingLogsGameBoard::Idle()
{
    OnIdle();
    Update();
}

// 0x10075090 ?NotifyMoveStarted@CSBLinkingLogsGameBoard@@IAEXAAVCLinkingLogsTile@@@Z
void CSBLinkingLogsGameBoard::NotifyMoveStarted(CLinkingLogsTile &refTopTile)
{
}

// 0x100750b0 ?NotifyMoveEnded@CSBLinkingLogsGameBoard@@IAEXAAVCLinkingLogsTile@@@Z
void CSBLinkingLogsGameBoard::NotifyMoveEnded(CLinkingLogsTile &refTopTile)
{
}

// 0x100750d0 ?HandleAnimEvent@CSBLinkingLogsGameBoard@@IAEXJJPAUMBAnimEventStruct@@@Z
void CSBLinkingLogsGameBoard::HandleAnimEvent(long channel, long event, MBAnimEventStruct *pEventData)
{
    IMBUnknown *iUnknown = 0;
    if (pEventData->anim->QueryInterface(MBIID_IMBUnknown, (void **)&iUnknown) == 0) {
        CLinkingLogsTile::iunknownmessagemap::iterator it = m_mapIUnknownToTile.find(iUnknown);
        if (m_mapIUnknownToTile.end() != it)
            (*it).second->OnAnimEvent(event, *pEventData);
        if (iUnknown) {
            iUnknown->Release();
            iUnknown = 0;
        }
        if (m_pActor)
            m_pActor->OnAnimEvent(event, *pEventData);
    }
}

// 0x100751a0 ?HandleClick@CSBLinkingLogsGameBoard@@IAEXJJPAVIMBUnknown@@@Z
void CSBLinkingLogsGameBoard::HandleClick(long channel, long event, IMBUnknown *iUnknown)
{
    // a click on a tile sends the actor there
    CLinkingLogsTile::iunknownmessagemap::iterator it = m_mapIUnknownToTile.find(iUnknown);
    if (m_pActor && m_mapIUnknownToTile.end() != it) {
        CLinkingLogsTile *pTile = (*it).second;
        m_pActor->MoveTo(pTile->GetBoardPosition());
        m_nLastMoveTime = m_iClock->GetGameTime();
    }
}

// 0x10075280 ?HandleTileEvent@CSBLinkingLogsGameBoard@@IAEXJJPAVCLinkingLogsTile@@@Z
void CSBLinkingLogsGameBoard::HandleTileEvent(long channel, long event, CLinkingLogsTile *pTile)
{
    for (gridmatrix::iterator i = m_matGameGrid.begin(); i != m_matGameGrid.end(); i++)
        (*i).second->OnTileEvent(event, pTile);
}

// 0x100752d0 ?NotifyEvent@CSBLinkingLogsGameBoard@@QAEXJJPAX@Z
void CSBLinkingLogsGameBoard::NotifyEvent(long channel, long event, void *data)
{
    if (data) {
        switch (channel) {
        case -2:
            if (event >= 2 && event <= 7)
                HandleAnimEvent(channel, event, (MBAnimEventStruct *)data);
            break;
        case 204:
            HandleTileEvent(channel, event, (CLinkingLogsTile *)data);
            break;
        case 400:
            if (event == 410)
                HandleClick(channel, event, (IMBUnknown *)data);
            break;
        }
    }
}

// 0x10075360 ?Update@CSBLinkingLogsGameBoard@@QAEXXZ
void CSBLinkingLogsGameBoard::Update()
{
    for (gridmatrix::iterator i = m_matGameGrid.begin(); i != m_matGameGrid.end(); i++) {
        (*i).second->Update();
        (*i).second->Place();
    }
    if (m_pActor)
        m_pActor->Update();
}

// 0x100753d0 ?RowCount@CSBLinkingLogsGameBoard@@QAEHXZ
int CSBLinkingLogsGameBoard::RowCount()
{
    return m_nNumRow;
}

// 0x100753f0 ?ColCount@CSBLinkingLogsGameBoard@@QAEHXZ
int CSBLinkingLogsGameBoard::ColCount()
{
    return m_nNumCol;
}

// 0x10075410 ?GetTokenList@CSBLinkingLogsGameBoard@@ABEIABV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@DAAV?$MBList@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@@@Z
unsigned int CSBLinkingLogsGameBoard::GetTokenList(const MBString &strSource, char cDelim, stringlist &listTokens) const
{
    if (strSource.length()) {
        unsigned int nPos = 0;
        unsigned int nStart = 0;
        while (true) {
            nPos = strSource.find(cDelim, nPos);
            if (MBString::npos == nPos) {
                listTokens.push_back(strSource.substr(nStart, strSource.length() - nStart));
                break;
            }
            listTokens.push_back(strSource.substr(nStart, nPos - nStart));
            nStart = ++nPos;
        }
    }
    return listTokens.size();
}

// 0x10075630 ?ParseName@CSBLinkingLogsGameBoard@@AAEXABV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@AAUtileattributes@1@@Z
void CSBLinkingLogsGameBoard::ParseName(const MBString &strData, tileattributes &refAttrib)
{
    if (strData != "none")
        refAttrib.strAnimName = STR_ANIM_FILENAME_PREFIX + strData;
}

// 0x10075810 ?ParseWalkable@CSBLinkingLogsGameBoard@@AAEXABV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@AAUtileattributes@1@@Z
void CSBLinkingLogsGameBoard::ParseWalkable(const MBString &strData, tileattributes &refAttrib)
{
    refAttrib.bWalkable = strData == "true";
}

// 0x100758a0 ?ParseType@CSBLinkingLogsGameBoard@@AAEXABV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@AAUtileattributes@1@@Z
void CSBLinkingLogsGameBoard::ParseType(const MBString &strData, tileattributes &refAttrib)
{
    // "door|start": the property bits of each named type
    stringlist listTokens;
    GetTokenList(strData, '|', listTokens);
    for (stringlist::iterator it = listTokens.begin(); it != listTokens.end(); it++)
        refAttrib.nProperties |= m_mapTileType[*it];
}

// TODO 0x10075a20 ?ParseTimeout@CSBLinkingLogsGameBoard@@AAEXABV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@AAUtileattributes@1@@Z: (82% similar) the original
// multiplies and adds the base time (fld st(0), fmul, fadd); VC6 folds it into base * (1 + diff)
// here (operand orders tried)
void CSBLinkingLogsGameBoard::ParseTimeout(const MBString &strData, tileattributes &refAttrib)
{
    // seconds in the prefs, scaled up by the difficulty
    long nBaseTime = atoi(strData.c_str()) * 1000;
    refAttrib.nTimeout = (long)(nBaseTime * m_fDifficulty + nBaseTime);
}

// 0x10075a90 ?ParseVelocity@CSBLinkingLogsGameBoard@@AAEXABV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@AAUtileattributes@1@@Z
void CSBLinkingLogsGameBoard::ParseVelocity(const MBString &strData, tileattributes &refAttrib)
{
    refAttrib.fVelocity = (float)atof(strData.c_str());
}

// 0x10075ad0 ?GetLevelCompleted@CSBLinkingLogsGameBoard@@QAEJXZ
long CSBLinkingLogsGameBoard::GetLevelCompleted()
{
    if (m_bComplete)
        return m_nLevel;
    return m_nLevel - 1;
}

// Template and inline instances this unit also emits:
// 0x10031ac0 ?erase@?$_Tree@PAVIMBUnknown@@U?$pair@QAVIMBUnknown@@PAVCLinkingLogsTile@@@std@@U_Kfn@?$map@PAVIMBUnknown@@PAVCLinkingLogsTile@@U?$less@PAVIMBUnknown@@@std@@V?$MBAllocator@PAVCLinkingLogsTile@@@@@3@U?$less@PAVIMBUnknown@@@3@V?$MBAllocator@PAVCLinkingLogsTile@@@@@std@@QAE?AViterator@12@V312@0@Z
// 0x10075fb0 ??1?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@P8CSBLinkingLogsGameBoard@@AEXABV12@AAUtileattributes@3@@Z@std@@QAE@XZ
// 0x100763a0 ?insert@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@P8CSBLinkingLogsGameBoard@@AEXABV12@AAUtileattributes@3@@ZU?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@P8CSBLinkingLogsGameBoard@@AEXABV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@AAUtileattributes@1@@Z@@@std@@QAE?AU?$pair@Viterator@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@P8CSBLinkingLogsGameBoard@@AEXABV12@AAUtileattributes@3@@Z@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@P8CSBLinkingLogsGameBoard@@AEXABV12@AAUtileattributes@3@@ZU?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@P8CSBLinkingLogsGameBoard@@AEXABV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@AAUtileattributes@1@@Z@@@2@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@P8CSBLinkingLogsGameBoard@@AEXABV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@AAUtileattributes@1@@Z@@@std@@_N@2@ABU?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@P8CSBLinkingLogsGameBoard@@AEXABV12@AAUtileattributes@3@@Z@2@@Z
// 0x10077420 ?insert@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@P8CSBLinkingLogsGameBoard@@AEXABV12@AAUtileattributes@3@@Z@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@P8CSBLinkingLogsGameBoard@@AEXABV12@AAUtileattributes@3@@ZU?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@P8CSBLinkingLogsGameBoard@@AEXABV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@AAUtileattributes@1@@Z@@@2@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@P8CSBLinkingLogsGameBoard@@AEXABV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@AAUtileattributes@1@@Z@@@std@@QAE?AU?$pair@Viterator@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@P8CSBLinkingLogsGameBoard@@AEXABV12@AAUtileattributes@3@@Z@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@P8CSBLinkingLogsGameBoard@@AEXABV12@AAUtileattributes@3@@ZU?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@P8CSBLinkingLogsGameBoard@@AEXABV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@AAUtileattributes@1@@Z@@@2@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@P8CSBLinkingLogsGameBoard@@AEXABV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@AAUtileattributes@1@@Z@@@std@@_N@2@ABU?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@P8CSBLinkingLogsGameBoard@@AEXABV12@AAUtileattributes@3@@Z@2@@Z
// 0x10078860 ??0?$pair@Viterator@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@P8CSBLinkingLogsGameBoard@@AEXABV12@AAUtileattributes@3@@Z@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@P8CSBLinkingLogsGameBoard@@AEXABV12@AAUtileattributes@3@@ZU?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@P8CSBLinkingLogsGameBoard@@AEXABV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@AAUtileattributes@1@@Z@@@2@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@P8CSBLinkingLogsGameBoard@@AEXABV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@AAUtileattributes@1@@Z@@@std@@_N@std@@QAE@ABViterator@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@P8CSBLinkingLogsGameBoard@@AEXABV12@AAUtileattributes@3@@Z@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@P8CSBLinkingLogsGameBoard@@AEXABV12@AAUtileattributes@3@@ZU?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@P8CSBLinkingLogsGameBoard@@AEXABV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@AAUtileattributes@1@@Z@@@2@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@P8CSBLinkingLogsGameBoard@@AEXABV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@AAUtileattributes@1@@Z@@@1@AB_N@Z
// 0x10078c20 ?_Insert@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@P8CSBLinkingLogsGameBoard@@AEXABV12@AAUtileattributes@3@@Z@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@P8CSBLinkingLogsGameBoard@@AEXABV12@AAUtileattributes@3@@ZU?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@P8CSBLinkingLogsGameBoard@@AEXABV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@AAUtileattributes@1@@Z@@@2@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@P8CSBLinkingLogsGameBoard@@AEXABV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@AAUtileattributes@1@@Z@@@std@@IAE?AViterator@12@PAU_Node@12@0ABU?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@P8CSBLinkingLogsGameBoard@@AEXABV12@AAUtileattributes@3@@Z@2@@Z
// 0x10079200 ?_Dec@const_iterator@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@P8CSBLinkingLogsGameBoard@@AEXABV12@AAUtileattributes@3@@Z@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@P8CSBLinkingLogsGameBoard@@AEXABV12@AAUtileattributes@3@@ZU?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@P8CSBLinkingLogsGameBoard@@AEXABV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@AAUtileattributes@1@@Z@@@2@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@P8CSBLinkingLogsGameBoard@@AEXABV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@AAUtileattributes@1@@Z@@@std@@QAEXXZ
// 0x10079700 ?_Construct@std@@YAXPAU?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@P8CSBLinkingLogsGameBoard@@AEXABV12@AAUtileattributes@3@@Z@1@ABU21@@Z

// 0x10032360 ?erase@?$_Tree@PAVIMBUnknown@@U?$pair@QAVIMBUnknown@@PAVCLinkingLogsTile@@@std@@U_Kfn@?$map@PAVIMBUnknown@@PAVCLinkingLogsTile@@U?$less@PAVIMBUnknown@@@std@@V?$MBAllocator@PAVCLinkingLogsTile@@@@@3@U?$less@PAVIMBUnknown@@@3@V?$MBAllocator@PAVCLinkingLogsTile@@@@@std@@QAE?AViterator@12@V312@@Z

// 0x10032a00 ?_Erase@?$_Tree@PAVIMBUnknown@@U?$pair@QAVIMBUnknown@@PAVCLinkingLogsTile@@@std@@U_Kfn@?$map@PAVIMBUnknown@@PAVCLinkingLogsTile@@U?$less@PAVIMBUnknown@@@std@@V?$MBAllocator@PAVCLinkingLogsTile@@@@@3@U?$less@PAVIMBUnknown@@@3@V?$MBAllocator@PAVCLinkingLogsTile@@@@@std@@IAEXPAU_Node@12@@Z

// 0x10032d80 ?_Lbound@?$_Tree@PAVIMBUnknown@@U?$pair@QAVIMBUnknown@@PAVCLinkingLogsTile@@@std@@U_Kfn@?$map@PAVIMBUnknown@@PAVCLinkingLogsTile@@U?$less@PAVIMBUnknown@@@std@@V?$MBAllocator@PAVCLinkingLogsTile@@@@@3@U?$less@PAVIMBUnknown@@@3@V?$MBAllocator@PAVCLinkingLogsTile@@@@@std@@IBEPAU_Node@12@ABQAVIMBUnknown@@@Z

// 0x10032e60 ?_Inc@const_iterator@?$_Tree@PAVIMBUnknown@@U?$pair@QAVIMBUnknown@@PAVCLinkingLogsTile@@@std@@U_Kfn@?$map@PAVIMBUnknown@@PAVCLinkingLogsTile@@U?$less@PAVIMBUnknown@@@std@@V?$MBAllocator@PAVCLinkingLogsTile@@@@@3@U?$less@PAVIMBUnknown@@@3@V?$MBAllocator@PAVCLinkingLogsTile@@@@@std@@QAEXXZ

// 0x10072cd0 ??_GCSBLinkingLogsGameBoard@@UAEPAXI@Z

// 0x10072d10 ??1?$MBMap@VCGameBoardPos@@PAVCLinkingLogsTile@@U?$less@VCGameBoardPos@@@std@@@@QAE@XZ

// 0x10072db0 ??1?$MBMap@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@P8CSBLinkingLogsGameBoard@@AEXABV12@AAUtileattributes@3@@ZU?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@@@QAE@XZ

// 0x10072e50 ??1?$MBMap@PAVIMBUnknown@@PAVCLinkingLogsTile@@U?$less@PAVIMBUnknown@@@std@@@@QAE@XZ

// 0x10075bb0 ??A?$map@VCGameBoardPos@@PAVCLinkingLogsTile@@U?$less@VCGameBoardPos@@@std@@V?$MBAllocator@PAVCLinkingLogsTile@@@@@std@@QAEAAPAVCLinkingLogsTile@@ABVCGameBoardPos@@@Z

// 0x10075f90 ??1?$pair@$$CBVCGameBoardPos@@PAVCLinkingLogsTile@@@std@@QAE@XZ

// 0x10076210 ?erase@?$_Tree@VCGameBoardPos@@U?$pair@$$CBVCGameBoardPos@@PAVCLinkingLogsTile@@@std@@U_Kfn@?$map@VCGameBoardPos@@PAVCLinkingLogsTile@@U?$less@VCGameBoardPos@@@std@@V?$MBAllocator@PAVCLinkingLogsTile@@@@@3@U?$less@VCGameBoardPos@@@3@V?$MBAllocator@PAVCLinkingLogsTile@@@@@std@@QAE?AViterator@12@V312@0@Z

// 0x100763e0 ?erase@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@P8CSBLinkingLogsGameBoard@@AEXABV12@AAUtileattributes@3@@Z@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@P8CSBLinkingLogsGameBoard@@AEXABV12@AAUtileattributes@3@@ZU?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@P8CSBLinkingLogsGameBoard@@AEXABV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@AAUtileattributes@1@@Z@@@2@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@P8CSBLinkingLogsGameBoard@@AEXABV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@AAUtileattributes@1@@Z@@@std@@QAE?AViterator@12@V312@0@Z

// TODO 0x100761d0 ?insert@?$map@VCGameBoardPos@@PAVCLinkingLogsTile@@U?$less@VCGameBoardPos@@@std@@V?$MBAllocator@PAVCLinkingLogsTile@@@@@std@@QAE?AU?$pair@Viterator@?$_Tree@VCGameBoardPos@@U?$pair@$$CBVCGameBoardPos@@PAVCLinkingLogsTile@@@std@@U_Kfn@?$map@VCGameBoardPos@@PAVCLinkingLogsTile@@U?$less@VCGameBoardPos@@@std@@V?$MBAllocator@PAVCLinkingLogsTile@@@@@3@U?$less@VCGameBoardPos@@@3@V?$MBAllocator@PAVCLinkingLogsTile@@@@@std@@_N@2@ABU?$pair@$$CBVCGameBoardPos@@PAVCLinkingLogsTile@@@2@@Z: header instance; InitializeGameBoard's m_matGameGrid[pos] calls it out of line in the original, VC6 inlines it there, so this unit emits no copy
// 0x10076a80 ?insert@?$_Tree@VCGameBoardPos@@U?$pair@$$CBVCGameBoardPos@@PAVCLinkingLogsTile@@@std@@U_Kfn@?$map@VCGameBoardPos@@PAVCLinkingLogsTile@@U?$less@VCGameBoardPos@@@std@@V?$MBAllocator@PAVCLinkingLogsTile@@@@@3@U?$less@VCGameBoardPos@@@3@V?$MBAllocator@PAVCLinkingLogsTile@@@@@std@@QAE?AU?$pair@Viterator@?$_Tree@VCGameBoardPos@@U?$pair@$$CBVCGameBoardPos@@PAVCLinkingLogsTile@@@std@@U_Kfn@?$map@VCGameBoardPos@@PAVCLinkingLogsTile@@U?$less@VCGameBoardPos@@@std@@V?$MBAllocator@PAVCLinkingLogsTile@@@@@3@U?$less@VCGameBoardPos@@@3@V?$MBAllocator@PAVCLinkingLogsTile@@@@@std@@_N@2@ABU?$pair@$$CBVCGameBoardPos@@PAVCLinkingLogsTile@@@2@@Z

// 0x10076c30 ?erase@?$_Tree@VCGameBoardPos@@U?$pair@$$CBVCGameBoardPos@@PAVCLinkingLogsTile@@@std@@U_Kfn@?$map@VCGameBoardPos@@PAVCLinkingLogsTile@@U?$less@VCGameBoardPos@@@std@@V?$MBAllocator@PAVCLinkingLogsTile@@@@@3@U?$less@VCGameBoardPos@@@3@V?$MBAllocator@PAVCLinkingLogsTile@@@@@std@@QAE?AViterator@12@V312@@Z

// 0x10077270 ?_Erase@?$_Tree@VCGameBoardPos@@U?$pair@$$CBVCGameBoardPos@@PAVCLinkingLogsTile@@@std@@U_Kfn@?$map@VCGameBoardPos@@PAVCLinkingLogsTile@@U?$less@VCGameBoardPos@@@std@@V?$MBAllocator@PAVCLinkingLogsTile@@@@@3@U?$less@VCGameBoardPos@@@3@V?$MBAllocator@PAVCLinkingLogsTile@@@@@std@@IAEXPAU_Node@12@@Z

// 0x10077340 ?_Inc@const_iterator@?$_Tree@VCGameBoardPos@@U?$pair@$$CBVCGameBoardPos@@PAVCLinkingLogsTile@@@std@@U_Kfn@?$map@VCGameBoardPos@@PAVCLinkingLogsTile@@U?$less@VCGameBoardPos@@@std@@V?$MBAllocator@PAVCLinkingLogsTile@@@@@3@U?$less@VCGameBoardPos@@@3@V?$MBAllocator@PAVCLinkingLogsTile@@@@@std@@QAEXXZ

// 0x100775f0 ?erase@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@P8CSBLinkingLogsGameBoard@@AEXABV12@AAUtileattributes@3@@Z@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@P8CSBLinkingLogsGameBoard@@AEXABV12@AAUtileattributes@3@@ZU?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@P8CSBLinkingLogsGameBoard@@AEXABV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@AAUtileattributes@1@@Z@@@2@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@P8CSBLinkingLogsGameBoard@@AEXABV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@AAUtileattributes@1@@Z@@@std@@QAE?AViterator@12@V312@@Z

// 0x10077c70 ?_Erase@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@P8CSBLinkingLogsGameBoard@@AEXABV12@AAUtileattributes@3@@Z@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@P8CSBLinkingLogsGameBoard@@AEXABV12@AAUtileattributes@3@@ZU?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@P8CSBLinkingLogsGameBoard@@AEXABV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@AAUtileattributes@1@@Z@@@2@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@P8CSBLinkingLogsGameBoard@@AEXABV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@AAUtileattributes@1@@Z@@@std@@IAEXPAU_Node@12@@Z

// 0x10078890 ?_Insert@?$_Tree@VCGameBoardPos@@U?$pair@$$CBVCGameBoardPos@@PAVCLinkingLogsTile@@@std@@U_Kfn@?$map@VCGameBoardPos@@PAVCLinkingLogsTile@@U?$less@VCGameBoardPos@@@std@@V?$MBAllocator@PAVCLinkingLogsTile@@@@@3@U?$less@VCGameBoardPos@@@3@V?$MBAllocator@PAVCLinkingLogsTile@@@@@std@@IAE?AViterator@12@PAU_Node@12@0ABU?$pair@$$CBVCGameBoardPos@@PAVCLinkingLogsTile@@@2@@Z

// 0x10079110 ?_Dec@const_iterator@?$_Tree@VCGameBoardPos@@U?$pair@$$CBVCGameBoardPos@@PAVCLinkingLogsTile@@@std@@U_Kfn@?$map@VCGameBoardPos@@PAVCLinkingLogsTile@@U?$less@VCGameBoardPos@@@std@@V?$MBAllocator@PAVCLinkingLogsTile@@@@@3@U?$less@VCGameBoardPos@@@3@V?$MBAllocator@PAVCLinkingLogsTile@@@@@std@@QAEXXZ

// 0x100792f0 ?_Inc@const_iterator@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@P8CSBLinkingLogsGameBoard@@AEXABV12@AAUtileattributes@3@@Z@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@P8CSBLinkingLogsGameBoard@@AEXABV12@AAUtileattributes@3@@ZU?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@P8CSBLinkingLogsGameBoard@@AEXABV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@AAUtileattributes@1@@Z@@@2@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@P8CSBLinkingLogsGameBoard@@AEXABV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@AAUtileattributes@1@@Z@@@std@@QAEXXZ

// 0x10079690 ?_Construct@std@@YAXPAU?$pair@$$CBVCGameBoardPos@@PAVCLinkingLogsTile@@@1@ABU21@@Z

// Instances the linker took from other objects (this unit emits them too):
// 0x1005cfb0 ??0?$list@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@V?$MBAllocator@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@@@std@@QAE@ABV?$MBAllocator@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@@@Z
// 0x1005da20 ?erase@?$list@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@V?$MBAllocator@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@@@std@@QAE?AViterator@12@V312@0@Z
// 0x1005dae0 ?_Buynode@?$list@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@V?$MBAllocator@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@@@std@@IAEPAU_Node@12@PAU312@0@Z
// 0x10065c30 ?find@std@@YAPADPAD0ABD@Z
// 0x10073bd0 ??1?$MBMap@JUtileattributes@CSBLinkingLogsGameBoard@@U?$less@J@std@@@@QAE@XZ
// 0x10073c70 ??1tileattributes@CSBLinkingLogsGameBoard@@QAE@XZ
// 0x10073ce0 ??1?$pair@$$CBJUtileattributes@CSBLinkingLogsGameBoard@@@std@@QAE@XZ
// 0x10075af0 ??0?$map@VCGameBoardPos@@PAVCLinkingLogsTile@@U?$less@VCGameBoardPos@@@std@@V?$MBAllocator@PAVCLinkingLogsTile@@@@@std@@QAE@ABU?$less@VCGameBoardPos@@@1@ABV?$MBAllocator@PAVCLinkingLogsTile@@@@@Z
// 0x10075c50 ??0?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@P8CSBLinkingLogsGameBoard@@AEXABV12@AAUtileattributes@3@@ZU?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@P8CSBLinkingLogsGameBoard@@AEXABV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@AAUtileattributes@1@@Z@@@std@@QAE@ABU?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@1@ABV?$MBAllocator@P8CSBLinkingLogsGameBoard@@AEXABV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@AAUtileattributes@1@@Z@@@Z
// 0x10075d10 ??A?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@JU?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@J@@@std@@QAEAAJABV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@1@@Z
// 0x10075df0 ??0?$map@PAVIMBUnknown@@PAVCLinkingLogsTile@@U?$less@PAVIMBUnknown@@@std@@V?$MBAllocator@PAVCLinkingLogsTile@@@@@std@@QAE@ABU?$less@PAVIMBUnknown@@@1@ABV?$MBAllocator@PAVCLinkingLogsTile@@@@@Z
// 0x10075eb0 ??0?$map@JUtileattributes@CSBLinkingLogsGameBoard@@U?$less@J@std@@V?$MBAllocator@Utileattributes@CSBLinkingLogsGameBoard@@@@@std@@QAE@ABU?$less@J@1@ABV?$MBAllocator@Utileattributes@CSBLinkingLogsGameBoard@@@@@Z
// 0x10075f70 ??Diterator@?$_Tree@JU?$pair@$$CBJUtileattributes@CSBLinkingLogsGameBoard@@@std@@U_Kfn@?$map@JUtileattributes@CSBLinkingLogsGameBoard@@U?$less@J@std@@V?$MBAllocator@Utileattributes@CSBLinkingLogsGameBoard@@@@@2@U?$less@J@2@V?$MBAllocator@Utileattributes@CSBLinkingLogsGameBoard@@@@@std@@QBEAAU?$pair@$$CBJUtileattributes@CSBLinkingLogsGameBoard@@@2@XZ
// 0x10076570 ??0?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@P8CSBLinkingLogsGameBoard@@AEXABV12@AAUtileattributes@3@@Z@std@@QAE@ABV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@1@ABQ8CSBLinkingLogsGameBoard@@AEX0AAUtileattributes@3@@Z@Z
// 0x10076700 ?insert@?$_Tree@JU?$pair@$$CBJUtileattributes@CSBLinkingLogsGameBoard@@@std@@U_Kfn@?$map@JUtileattributes@CSBLinkingLogsGameBoard@@U?$less@J@std@@V?$MBAllocator@Utileattributes@CSBLinkingLogsGameBoard@@@@@2@U?$less@J@2@V?$MBAllocator@Utileattributes@CSBLinkingLogsGameBoard@@@@@std@@QAE?AU?$pair@Viterator@?$_Tree@JU?$pair@$$CBJUtileattributes@CSBLinkingLogsGameBoard@@@std@@U_Kfn@?$map@JUtileattributes@CSBLinkingLogsGameBoard@@U?$less@J@std@@V?$MBAllocator@Utileattributes@CSBLinkingLogsGameBoard@@@@@2@U?$less@J@2@V?$MBAllocator@Utileattributes@CSBLinkingLogsGameBoard@@@@@std@@_N@2@ABU?$pair@$$CBJUtileattributes@CSBLinkingLogsGameBoard@@@2@@Z
// 0x10076a10 ?find@?$_Tree@JU?$pair@$$CBJUtileattributes@CSBLinkingLogsGameBoard@@@std@@U_Kfn@?$map@JUtileattributes@CSBLinkingLogsGameBoard@@U?$less@J@std@@V?$MBAllocator@Utileattributes@CSBLinkingLogsGameBoard@@@@@2@U?$less@J@2@V?$MBAllocator@Utileattributes@CSBLinkingLogsGameBoard@@@@@std@@QAE?AViterator@12@ABJ@Z
// 0x100783e0 ?_Erase@?$_Tree@JU?$pair@$$CBJUtileattributes@CSBLinkingLogsGameBoard@@@std@@U_Kfn@?$map@JUtileattributes@CSBLinkingLogsGameBoard@@U?$less@J@std@@V?$MBAllocator@Utileattributes@CSBLinkingLogsGameBoard@@@@@2@U?$less@J@2@V?$MBAllocator@Utileattributes@CSBLinkingLogsGameBoard@@@@@std@@IAEXPAU_Node@12@@Z
// 0x100784d0 ?_Insert@?$_Tree@JU?$pair@$$CBJUtileattributes@CSBLinkingLogsGameBoard@@@std@@U_Kfn@?$map@JUtileattributes@CSBLinkingLogsGameBoard@@U?$less@J@std@@V?$MBAllocator@Utileattributes@CSBLinkingLogsGameBoard@@@@@2@U?$less@J@2@V?$MBAllocator@Utileattributes@CSBLinkingLogsGameBoard@@@@@std@@IAE?AViterator@12@PAU_Node@12@0ABU?$pair@$$CBJUtileattributes@CSBLinkingLogsGameBoard@@@2@@Z
// 0x10078fb0 ?_Lbound@?$_Tree@JU?$pair@$$CBJUtileattributes@CSBLinkingLogsGameBoard@@@std@@U_Kfn@?$map@JUtileattributes@CSBLinkingLogsGameBoard@@U?$less@J@std@@V?$MBAllocator@Utileattributes@CSBLinkingLogsGameBoard@@@@@2@U?$less@J@2@V?$MBAllocator@Utileattributes@CSBLinkingLogsGameBoard@@@@@std@@IBEPAU_Node@12@ABJ@Z
// 0x10079020 ?_Dec@const_iterator@?$_Tree@JU?$pair@$$CBJUtileattributes@CSBLinkingLogsGameBoard@@@std@@U_Kfn@?$map@JUtileattributes@CSBLinkingLogsGameBoard@@U?$less@J@std@@V?$MBAllocator@Utileattributes@CSBLinkingLogsGameBoard@@@@@2@U?$less@J@2@V?$MBAllocator@Utileattributes@CSBLinkingLogsGameBoard@@@@@std@@QAEXXZ
// 0x100793d0 ?_Inc@const_iterator@?$_Tree@JU?$pair@$$CBJUtileattributes@CSBLinkingLogsGameBoard@@@std@@U_Kfn@?$map@JUtileattributes@CSBLinkingLogsGameBoard@@U?$less@J@std@@V?$MBAllocator@Utileattributes@CSBLinkingLogsGameBoard@@@@@2@U?$less@J@2@V?$MBAllocator@Utileattributes@CSBLinkingLogsGameBoard@@@@@std@@QAEXXZ
// 0x10076020 ??0tileattributes@CSBLinkingLogsGameBoard@@QAE@ABU01@@Z (compiler-generated)
// 0x100794b0 ?_Construct@std@@YAXPAU?$pair@$$CBJUtileattributes@CSBLinkingLogsGameBoard@@@1@ABU21@@Z
// 0x10076880 ?erase@?$_Tree@JU?$pair@$$CBJUtileattributes@CSBLinkingLogsGameBoard@@@std@@U_Kfn@?$map@JUtileattributes@CSBLinkingLogsGameBoard@@U?$less@J@std@@V?$MBAllocator@Utileattributes@CSBLinkingLogsGameBoard@@@@@2@U?$less@J@2@V?$MBAllocator@Utileattributes@CSBLinkingLogsGameBoard@@@@@std@@QAE?AViterator@12@V312@0@Z
// 0x10077d60 ?erase@?$_Tree@JU?$pair@$$CBJUtileattributes@CSBLinkingLogsGameBoard@@@std@@U_Kfn@?$map@JUtileattributes@CSBLinkingLogsGameBoard@@U?$less@J@std@@V?$MBAllocator@Utileattributes@CSBLinkingLogsGameBoard@@@@@2@U?$less@J@2@V?$MBAllocator@Utileattributes@CSBLinkingLogsGameBoard@@@@@std@@QAE?AViterator@12@V312@@Z

// 0x10071670 _$E32
// 0x10071690 _$E29
// 0x10071c60 _$E31
// 0x10071c80 _$E30
// 0x10071cb0 _$E69
// 0x10071cd0 _$E66
// 0x10071df0 _$E68
// 0x10071e10 _$E67
// 0x10071e80 _$E74
// 0x10071ea0 _$E71
// 0x10071fc0 _$E73
// 0x10071fe0 _$E72
// 0x100798b0 _$E88
// 0x100798f0 _$E84
// 0x10079910 _$E87
