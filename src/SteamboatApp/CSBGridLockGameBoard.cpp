#include "CSBGridLockGameBoard.h"
#include "SBCursorItems.h" // every node unit of the original has the cursor item strings
#include <locale> // evidence: the original unit ends with the std::ctype<unsigned short>::id and std::collate<char>::id initializers
#include "MBRuntime.h"
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>

// The sound ids combine a sound, the type of the tile stack and one of four variants.
static const CSBGridLockGameBoard::soundid g_arrayType[4] = {
    CSBGridLockGameBoard::kT1, CSBGridLockGameBoard::kT2, CSBGridLockGameBoard::kT3, CSBGridLockGameBoard::kT4
};
static const CSBGridLockGameBoard::soundid g_arrayVariant[4] = {
    CSBGridLockGameBoard::kV1, CSBGridLockGameBoard::kV2, CSBGridLockGameBoard::kV3, CSBGridLockGameBoard::kV4
};

// 0x100573a0 ??0CSBGridLockGameBoard@@QAE@PAVSBGridlockNode@@@Z
CSBGridLockGameBoard::CSBGridLockGameBoard(SBGridlockNode *pNode)
    : m_pCurrentStartTile(0), m_pCurrentDestTile(0), m_nTilesMoveing(0), m_pNode(pNode),
      m_bGoalReached(false), m_pCabinetAnim(0)
{
    // the prize in the cabinet: the first level always shows the cabinet itself
    const char *aszCabinetItems[9] = { "cabinet", "lunch", "plant", "trophy", "picture", "bowl", "hat", "lamp", "table" };
    MBComPtr<ISBGameState> gameState(MBIID_CSBGameState);
    int nItem;
    if (gameState->GetProperty("GLMaxLevel", 1) == 1)
        nItem = 0;
    else
        nItem = MBrandom(1, 8);
    SteamboatNode::AddAnimToWorld(MBString("GL") + aszCabinetItems[nItem], m_pCabinetAnim);
    m_pCabinetAnim->SetCurrentSequence(0);
    m_pCabinetAnim->Place(MBPoint3(245, 40, 100));
    m_pCabinetAnim->Play();
    m_pCabinetAnim->Show();

    m_mapNameToProp["moveable"] = CSBGridlockGameTile::kMoveable;
    m_mapNameToProp["stackable"] = CSBGridlockGameTile::kStackable;
    m_mapNameToProp["selectable"] = CSBGridlockGameTile::kSelectable;
    m_mapNameToProp["none"] = CSBGridlockGameTile::kNone;
    m_mapNameToProp["actor"] = CSBGridlockGameTile::kActor;

    // sound keys: the top byte is the group (0x01 honks, 0x02 the gl03bd lines, 0x04 the moving
    // car, 0x08 Mickey's and 0x10 Minnie's lines, see OnEndMove)
    m_mapIdToSound[0x02010001] = "gl03bd40160";
    m_mapIdToSound[0x02010002] = "gl03bd40150";
    m_mapIdToSound[0x02010004] = "gl03bd40140";
    m_mapIdToSound[0x02010008] = "gl03bd40130";
    m_mapIdToSound[0x02020001] = "gl03bd30120";
    m_mapIdToSound[0x02020002] = "gl03bd30110";
    m_mapIdToSound[0x02020004] = "gl03bd30100";
    m_mapIdToSound[0x02020008] = "gl03bd30090";
    m_mapIdToSound[0x02040001] = "gl03bd20080";
    m_mapIdToSound[0x02040002] = "gl03bd20070";
    m_mapIdToSound[0x02040004] = "gl03bd20060";
    m_mapIdToSound[0x02040008] = "gl03bd20050";
    m_mapIdToSound[0x02080001] = "gl03bd30120";
    m_mapIdToSound[0x02080002] = "gl03bd30110";
    m_mapIdToSound[0x02080004] = "gl03bd30100";
    m_mapIdToSound[0x02080008] = "gl03bd30090";
    m_mapIdToSound[0x01010001] = "glSEhonk1";
    m_mapIdToSound[0x01020001] = "glSEhonk2";
    m_mapIdToSound[0x01040001] = "glSEhonk3";
    m_mapIdToSound[0x01080001] = "glSEhonk2";
    m_mapIdToSound[0x04010001] = "glSEmoveCar";
    m_mapIdToSound[0x10010001] = "gl04bmi0010";
    m_mapIdToSound[0x08010001] = "gl04bmc0010";
    m_mapIdToSound[0x10010002] = "gl08ami0010";
    m_mapIdToSound[0x08010002] = "gl08amc0010";
    m_mapIdToSound[0x10020001] = "gl07xmi0010";
    m_mapIdToSound[0x08020001] = "gl07xmc0010";
    m_mapIdToSound[0x10020002] = "gl07xmi0020";
    m_mapIdToSound[0x08020002] = "gl07xmc0020";
}

// 0x10058e20 ??1CSBGridLockGameBoard@@UAE@XZ
CSBGridLockGameBoard::~CSBGridLockGameBoard()
{
    IMBUnknown *iUnknown;
    if (m_pCabinetAnim) {
        m_pCabinetAnim->QueryInterface(MBIID_IMBUnknown, (void **)&iUnknown);
        SteamboatNode::RemoveFromWorld(iUnknown);
        if (iUnknown) {
            iUnknown->Release();
            iUnknown = 0;
        }
        if (m_pCabinetAnim) {
            m_pCabinetAnim->Release();
            m_pCabinetAnim = 0;
        }
    }
}

// 0x10059110 ?OnTileClicked@CSBGridLockGameBoard@@MAE_NAAVCSBGridlockGameTile@@0@Z
bool CSBGridLockGameBoard::OnTileClicked(CSBGridlockGameTile &refTopTile, CSBGridlockGameTile &refBottomTile)
{
    if (!refTopTile.IsActor() && refTopTile.IsSelectable() && MBrandom(0, 1))
        m_pNode->StartSound(m_mapIdToSound[g_arrayVariant[MBrandom(0, 3) & 3] | g_arrayType[refTopTile.GetStackType()] | kSelect].c_str());
    return true;
}

// 0x100591d0 ?OnStartMove@CSBGridLockGameBoard@@MAE_NAAVCSBGridlockGameTile@@0@Z
bool CSBGridLockGameBoard::OnStartMove(CSBGridlockGameTile &refStartTile, CSBGridlockGameTile &refDestTile)
{
    if (!refStartTile.IsActor())
        m_pNode->StartSound(m_mapIdToSound[g_arrayVariant[0] | kT1 | kMove].c_str());
    return true;
}

// 0x10059240 ?OnInvalidClick@CSBGridLockGameBoard@@MAEXAAVCSBGridlockGameTile@@0@Z
void CSBGridLockGameBoard::OnInvalidClick(CSBGridlockGameTile &refStartTile, CSBGridlockGameTile &refDestTile)
{
    if (!refStartTile.IsActor() && refStartTile.IsSelectable())
        m_pNode->StartSound(m_mapIdToSound[g_arrayVariant[0] | g_arrayType[refStartTile.GetStackType()] | kHonk].c_str());
    else if (refStartTile.IsActor()) {
        // The character says it can't get there.
        if (refDestTile.IsSelectable())
            m_pNode->StartSound(m_mapIdToSound[(m_bPlayAsMickye ? kMickey : kMinney) | kCantGetThere | kV1].c_str());
        else
            m_pNode->StartSound(m_mapIdToSound[(m_bPlayAsMickye ? kMickey : kMinney) | kCantGetThere | kV2].c_str());
    }
}

// 0x10059360 ?OnEndStep@CSBGridLockGameBoard@@MAEXAAVCSBGridlockGameTile@@0@Z
void CSBGridLockGameBoard::OnEndStep(CSBGridlockGameTile &refStartTile, CSBGridlockGameTile &refDestTile)
{
}

// 0x100599a0 ?OnIdle@CSBGridLockGameBoard@@MAEXXZ
void CSBGridLockGameBoard::OnIdle()
{
}

// 0x1005be50 ?GetTileAt@CSBGridLockGameBoard@@IAEAAVCSBGridlockGameTile@@ABVCGameBoardPos@@_N@Z
CSBGridlockGameTile &CSBGridLockGameBoard::GetTileAt(const CGameBoardPos &refPos, bool bTopMost)
{
    if (bTopMost)
        return m_matGameGrid[refPos.Row()][refPos.Col()][m_matGameGrid[refPos.Row()][refPos.Col()].size() - 1];
    return m_matGameGrid[refPos.Row()][refPos.Col()][refPos.Lev()];
}

// 0x1005bf30 ?SetTileAt@CSBGridLockGameBoard@@IAEXABVCGameBoardPos@@AAVCSBGridlockGameTile@@_N@Z
void CSBGridLockGameBoard::SetTileAt(const CGameBoardPos &refPos, CSBGridlockGameTile &refNewTile, bool bTopMost)
{
    if (bTopMost) {
        int nTop = m_matGameGrid[refPos.Row()][refPos.Col()].size() - 1;
        refNewTile.SetBoardPosition(CGameBoardPos(refPos.MaxRow(), refPos.MaxCol(), refPos.MaxLev(), refPos.Row(), refPos.Col(), nTop));
        m_matGameGrid[refPos.Row()][refPos.Col()][nTop] = refNewTile;
    } else
        m_matGameGrid[refPos.Row()][refPos.Col()][refPos.Lev()] = refNewTile;
}

// 0x1005c0f0 ?Idle@CSBGridLockGameBoard@@QAEXXZ
void CSBGridLockGameBoard::Idle()
{
    OnIdle();
    Update();
}

// 0x1005c120 ?NotifyMoveStarted@CSBGridLockGameBoard@@IAEXAAVCSBGridlockGameTile@@@Z
void CSBGridLockGameBoard::NotifyMoveStarted(CSBGridlockGameTile &refTopTile)
{
}

// 0x1005c140 ?NotifyMoveEnded@CSBGridLockGameBoard@@IAEXAAVCSBGridlockGameTile@@@Z
void CSBGridLockGameBoard::NotifyMoveEnded(CSBGridlockGameTile &refTopTile)
{
    if (--m_nTilesMoveing == 0) {
        // Both tiles of the step have arrived: store them at their new board positions.
        CSBGridlockGameTile tileStart(*m_pCurrentStartTile);
        CSBGridlockGameTile tileDest(*m_pCurrentDestTile);
        CGameBoardPos posDest = tileDest.GetBoardPosition();
        CGameBoardPos posStart = tileStart.GetBoardPosition();
        SetTileAt(posDest, tileDest, true);
        SetTileAt(posStart, tileStart, true);
        GetTileAt(posDest, true).AddToMessageMap(m_mapIUnknownToTile);
        GetTileAt(posStart, true).AddToMessageMap(m_mapIUnknownToTile);
        m_pCurrentStartTile = &GetTileAt(posStart, true);
        m_pCurrentDestTile = 0;
        MoveTile();
        if (IsMoveInProgress())
            OnEndStep(GetTileAt(posDest, true), *m_pCurrentStartTile);
        else
            OnEndMove(GetTileAt(posDest, true), *m_pCurrentStartTile);
    }
}

// 0x1005c360 ?HandleAnimEvent@CSBGridLockGameBoard@@IAEXJJPAUMBAnimEventStruct@@@Z
void CSBGridLockGameBoard::HandleAnimEvent(long channel, long event, MBAnimEventStruct *pEventData)
{
    IMBUnknown *iUnknown = 0;
    if (pEventData->anim->QueryInterface(MBIID_IMBUnknown, (void **)&iUnknown) == 0) {
        CSBGridlockGameTile::iunknownmessagemap::iterator it = m_mapIUnknownToTile.find(iUnknown);
        if (m_mapIUnknownToTile.end() != it)
            (*it).second->OnAnimEvent(event, *pEventData);
        if (iUnknown)
            iUnknown->Release();
    }
}

// 0x10059380 ?OnEndMove@CSBGridLockGameBoard@@MAEXAAVCSBGridlockGameTile@@0@Z
void CSBGridLockGameBoard::OnEndMove(CSBGridlockGameTile &refStartTile, CSBGridlockGameTile &refDestTile)
{
    if (!m_bGoalReached) {
        // the character reached the top row: next to the cabinet (four columns from the right)
        // or with the cabinet already gone, the level is won
        if (refDestTile.GetBoardPosition().Row() == refDestTile.GetBoardPosition().MinRow()) {
            if (refDestTile.GetBoardPosition().Col() == refDestTile.GetBoardPosition().MaxCol() - 4 || m_pCabinetAnim == 0) {
                MBComPtr<ISBGameState> gameState(MBIID_CSBGameState);
                m_pNode->StartSound("glSEgrabItem");
                m_pCabinetAnim->Hide();
                if (gameState->GetProperty("GLMaxLevel", 1) == 1) {
                    m_pNode->StartSound(m_mapIdToSound[(m_bPlayAsMickye ? 0x08000000 : 0x10000000) | 0x20001].c_str());
                    m_pNode->AddItemToInventory(MBString("PRcabinet0"), false);
                } else {
                    m_pNode->StartSound(m_mapIdToSound[(m_bPlayAsMickye ? 0x08000000 : 0x10000000) | 0x20002].c_str());
                }
                m_bGoalReached = true;
                m_pNode->UpdateLevel(GetLevelCompleted());
            }
        }
    }
    if (m_bGoalReached) {
        if (refDestTile.GetBoardPosition().Row() == refDestTile.GetBoardPosition().MaxRow()) {
            MBComPtr<ISBGameState> gameState(MBIID_CSBGameState);
            m_pNode->StartSound("glSEsuccess");
            if (gameState->GetProperty("GLMaxLevel", 1) == 1)
                m_pNode->SendExitSceneEvent(MBString("DH"));
        }
    }
}

// 0x1005a420 ?LoadTileProperties@CSBGridLockGameBoard@@IAEXPAVIMBStream@@AAV?$MBMap@HW4tileprop@CSBGridlockGameTile@@U?$less@H@std@@@@@Z
void CSBGridLockGameBoard::LoadTileProperties(IMBStream *iStream, tilepropemap &mapTileProp)
{
    // section "game tile properties": type0, type1, ... hold "name=value" settings separated by
    // ':'; "type" lists property names separated by '|', "soundkey" and "stacktype" are numbers
    MBComPtr<IMBPref> pref(MBIID_CMBPref);
    int nType = 0;
    char strKey[16] = { 0 };
    MBString strData;
    stringlist listSettings;
    stringlist::iterator iSettings;
    for (;; nType++) {
        mapTileProp[nType] = CSBGridlockGameTile::kNone;
        sprintf(strKey, "type%d", nType);
        bool bFound = pref->GetString(iStream, MBString("game tile properties"), MBString(strKey), strData, MBString("")) > 0;
        if (!bFound)
            break;
        strData.resize(std::remove(strData.begin(), strData.end(), ' ') - strData.begin());
        listSettings.clear();
        GetTokenList(strData, ':', listSettings);
        for (iSettings = listSettings.begin(); iSettings != listSettings.end(); iSettings++) {
            unsigned int nPos = (*iSettings).find('=');
            if (nPos != MBString::npos) {
                MBString strValues = (*iSettings).substr(nPos + 1, (*iSettings).length() - nPos - 1);
                MBString strSetting = (*iSettings).substr(0, nPos);
                if (strSetting == "type") {
                    stringlist listValues;
                    CSBGridlockGameTile::tileprop &eProp = mapTileProp[nType];
                    GetTokenList(strValues, '|', listValues);
                    for (stringlist::iterator iValues = listValues.begin(); iValues != listValues.end(); iValues++)
                        (int &)eProp |= m_mapNameToProp[(*iValues).c_str()];
                } else if (strSetting.compare("soundkey") == 0) {
                    (int &)mapTileProp[nType] |= atoi(strValues.c_str()) << 24;
                } else if (strSetting.compare("stacktype") == 0) {
                    (int &)mapTileProp[nType] |= atoi(strValues.c_str()) << 16;
                }
            }
        }
    }
}

// 0x1005acb0 ?LoadAnims@CSBGridLockGameBoard@@IAEX_NPAVIMBStream@@AAV?$MBMap@HV?$MBComPtr@VIMBAnim@@@@U?$less@H@std@@@@@Z
void CSBGridLockGameBoard::LoadAnims(bool bPlayMickye, IMBStream *iStream, animtypemap &mapGameAnims)
{
    // section "game board tiles": type0, type1, ... name the animation of each tile type; "none"
    // leaves a type without one, "actor" stands for the player's character
    MBComPtr<IMBPref> pref(MBIID_CMBPref);
    int nType = 0;
    char szType[16] = { 0 };
    MBString strValue;
    for (;; nType++) {
        sprintf(szType, "type%d", nType);
        bool bFound = pref->GetString(iStream, MBString("game board tiles"), MBString(szType), strValue, MBString("")) > 0 && strValue != "none";
        if (bFound) {
            if (strValue == "actor") {
                if (bPlayMickye)
                    strValue = "GLANMickey";
                else
                    strValue = "GLANMinnie";
            }
            MBComPtr<IMBAnim> anim;
            unsigned long id = MBHashString(strValue.c_str());
            IMBStream *animStream;
            if (SteamboatNode::GetStream(id, 'anm ', animStream) == 0) {
                anim.CreateObject(MBIID_CMBAnim);
                anim->Load(id, animStream, false);
                mapGameAnims.insert(animtypemap::value_type(nType, anim));
                if (animStream) {
                    animStream->Release();
                    animStream = 0;
                }
            }
        } else if (strValue != "none") {
            break;
        }
    }
}

// 0x1005b180 ?LoadLayout@CSBGridLockGameBoard@@IAEXPAVIMBStream@@JAAV?$MBMap@VCGameBoardPos@@HU?$less@VCGameBoardPos@@@std@@@@@Z
void CSBGridLockGameBoard::LoadLayout(IMBStream *iStream, long nLevel, tiletypemap &mapTileType)
{
    // section "level <n> game board", one key "Row<r> Col<c>" per square naming its tile type
    MBComPtr<IMBPref> pref(MBIID_CMBPref);
    MBString strKey("RowX ColX");
    MBString strValue;
    char szSection[30] = { 0 };
    CGameBoardPos pos(RowCount() - 1, ColCount() - 1, 2, 0, 0, 0);
    pos.Begin();
    sprintf(szSection, "level %d game board", nLevel);
    while (pos.NextPos()) {
        strKey[3] = '0' + pos.Row();
        strKey[8] = '0' + pos.Col();
        bool bFound = pref->GetString(iStream, MBString(szSection), strKey, strValue, MBString("")) > 0;
        if (bFound) {
            if (strValue.length()) {
                unsigned int nStart = strValue.find_first_of("0123456789");
                unsigned int nEnd = strValue.find_first_not_of("0123456789", nStart);
                int nType = atoi(strValue.substr(nStart, MBString::npos != nEnd ? nEnd - nStart : MBString::npos).c_str());
                mapTileType.insert(tiletypemap::value_type(pos, nType));
            }
        } else {
            mapTileType.insert(tiletypemap::value_type(pos, 0));
        }
    }
}

// TODO 0x1005b6a0 ?InitGameGrid@CSBGridLockGameBoard@@IAEXPAVIMBStream@@@Z: the original calls vector::size out of line inside the inlined resize and keeps one stack slot less; VC6 inlines the first size() (77% similar)
void CSBGridLockGameBoard::InitGameGrid(IMBStream *iStream)
{
    // section "game board size": the number of rows and columns; every square starts with one
    // tile on the floor level, the squares laid out from the origin tile
    MBComPtr<IMBPref> pref(MBIID_CMBPref);
    MBString strValue;
    pref->GetString(iStream, MBString("game board size"), MBString("Row Count"), strValue, MBString(""));
    int nRows = atoi(strValue.c_str());
    pref->GetString(iStream, MBString("game board size"), MBString("Col Count"), strValue, MBString(""));
    int nCols = atoi(strValue.c_str());

    MBRect recCurentTileOrigin(m_rectBoardOrigin);
    CGameBoardPos objPosition(nRows - 1, nCols - 1, 2, 0, 0, 0);
    objPosition.BeginRow();
    m_matGameGrid.resize(nRows);
    while (objPosition.NextRow()) {
        objPosition.BeginCol();
        m_matGameGrid[objPosition.Row()].resize(nCols);
        while (objPosition.NextCol()) {
            CSBGridlockGameTile objTile;
            objTile.Initialize(this, objPosition, recCurentTileOrigin);
            m_matGameGrid[objPosition.Row()][objPosition.Col()].push_back(objTile);
            recCurentTileOrigin.Offset(m_rectBoardOrigin.Width(), 0);
        }
        recCurentTileOrigin.left = m_rectBoardOrigin.left;
        recCurentTileOrigin.right = m_rectBoardOrigin.right;
        recCurentTileOrigin.Offset(0, m_rectBoardOrigin.Height());
    }
}

// 0x100599c0 ?SetTileProperties@CSBGridLockGameBoard@@IAEXABVCGameBoardPos@@PAVIMBAnim@@W4tileprop@CSBGridlockGameTile@@AAV?$MBMap@HV?$MBComPtr@VIMBAnim@@@@U?$less@H@std@@@@AAV?$MBMap@HW4tileprop@CSBGridlockGameTile@@U?$less@H@std@@@@@Z
void CSBGridLockGameBoard::SetTileProperties(const CGameBoardPos &refPos, IMBAnim *pAnim, CSBGridlockGameTile::tileprop eProp, animtypemap &mapGameAnims, tilepropemap &mapTileProp)
{
    CSBGridlockGameTile &refTile = GetTileAt(refPos, false);
    refTile.SetTileProperties(pAnim, eProp);
    if (refTile.IsStackable() && !refTile.IsMoveable()) {
        // a stackable floor tile gets a second tile on top: the one bits 16-23 name, or an
        // empty one that can be moved and stacked
        CSBGridlockGameTile tileTop;
        CGameBoardPos posTop(refPos);
        IMBAnim *pTopAnim = 0;
        int nTopType = (refTile.m_nProperties >> 16) & 0xff;
        if (nTopType) {
            animtypemap::iterator itAnim = mapGameAnims.find(nTopType);
            if (mapGameAnims.end() != itAnim)
                (*itAnim).second->Clone(&pTopAnim);
        }
        posTop.NextLev();
        tileTop.Initialize(this, posTop, refTile.GetRect());
        if (nTopType) {
            tileTop.SetTileProperties(pTopAnim, mapTileProp[(refTile.m_nProperties >> 16) & 0xff]);
        } else {
            MBString strMoveable("moveable");
            MBString strStackable("stackable");
            tileTop.SetTileProperties(pTopAnim, (CSBGridlockGameTile::tileprop)(m_mapNameToProp[strMoveable] | m_mapNameToProp[strStackable]));
        }
        m_matGameGrid[posTop.Row()][posTop.Col()].push_back(tileTop);
        CSBGridlockGameTile &refTopTile = GetTileAt(posTop, true);
        refTopTile.AddToMessageMap(m_mapIUnknownToTile);
        refTopTile.AddToButtonGroup(m_ButtonGroup);
    }
    CSBGridlockGameTile &refBottomTile = GetTileAt(refPos, false);
    refBottomTile.AddToMessageMap(m_mapIUnknownToTile);
    refBottomTile.AddToButtonGroup(m_ButtonGroup);
}

// 0x10059e60 ?InitializeGameBoard@CSBGridLockGameBoard@@QAEXABVMBRect@@J_NPAVIMBStream@@@Z
void CSBGridLockGameBoard::InitializeGameBoard(const MBRect &refOriginTile, long nLevel, bool bPlayMickye, IMBStream *iStream)
{
    tiletypemap mapTileType;
    tilepropemap mapTileProp;
    animtypemap mapGameAnims;

    m_rectBoardOrigin = refOriginTile;
    m_nLevel = nLevel;
    InitGameGrid(iStream);
    LoadTileProperties(iStream, mapTileProp);
    LoadLayout(iStream, m_nLevel, mapTileType);
    LoadAnims(bPlayMickye, iStream, mapGameAnims);

    CGameBoardPos pos(RowCount() - 1, ColCount() - 1, 2, 0, 0, 0);
    m_bPlayAsMickye = bPlayMickye;
    pos.Begin();
    while (pos.NextPos()) {
        IMBAnim *pAnim = 0;
        tiletypemap::iterator itType = mapTileType.find(pos);
        if (mapTileType.end() != itType) {
            tilepropemap::iterator itProp = mapTileProp.find((*itType).second);
            animtypemap::iterator itAnim = mapGameAnims.find((*itType).second);
            if (mapGameAnims.end() != itAnim)
                (*itAnim).second->Clone(&pAnim);
            SetTileProperties(pos, pAnim, (*itProp).second, mapGameAnims, mapTileProp);
        }
    }
    Update();
}

// TODO 0x1005c420 ?HandleClick@CSBGridLockGameBoard@@IAEXJJPAVIMBUnknown@@@Z: the original inlines map::find (lower_bound and the key test) and keeps iClickable in the parameter slot; VC6 calls find here (70% similar)
void CSBGridLockGameBoard::HandleClick(long channel, long event, IMBUnknown *iUnknown)
{
    CSBGridlockGameTile::iunknownmessagemap::iterator it = m_mapIUnknownToTile.find(iUnknown);
    if (m_mapIUnknownToTile.end() != it) {
        CGameBoardPos posClicked = (*it).second->GetBoardPosition();
        CSBGridlockGameTile &refTopTile = GetTileAt(posClicked, true);
        IMBUnknown *iClickable = refTopTile.GetClickableInterface();
        if (iClickable) {
            ISBAnimButton *iButton;
            if (iClickable->QueryInterface(MBIID_ISBAnimButton, (void **)&iButton) == 0) {
                m_ButtonGroup.OnEvent(channel, event, iButton);
                if (iButton) {
                    iButton->Release();
                    iButton = 0;
                }
            }
            if (iClickable) {
                iClickable->Release();
                iClickable = 0;
            }
        }
        posClicked.SetLev(posClicked.MinLev());
        CSBGridlockGameTile &refBottomTile = GetTileAt(posClicked, false);
        if (!IsMoveInProgress() && OnTileClicked(refTopTile, refBottomTile)) {
            if (refTopTile.IsActor())
                m_ButtonGroup.ClearSelected();
            if (refTopTile.IsSelectable())
                m_pCurrentStartTile = &refTopTile;
            else if (m_pCurrentStartTile) {
                if (IsStraightLine(m_pCurrentStartTile->GetBoardPosition(), refTopTile.GetBoardPosition())) {
                    if (m_pCurrentStartTile->IsSwappable(refBottomTile)) {
                        CGameBoardPos posStart = m_pCurrentStartTile->GetBoardPosition();
                        CGameBoardPos posEnd = refTopTile.GetBoardPosition();
                        if (BuildPath(posStart, posEnd)) {
                            if (OnStartMove(*m_pCurrentStartTile, refTopTile))
                                MoveTile();
                        } else
                            OnInvalidClick(*m_pCurrentStartTile, refTopTile);
                    } else
                        OnInvalidClick(*m_pCurrentStartTile, refTopTile);
                } else
                    OnInvalidClick(*m_pCurrentStartTile, refTopTile);
            }
        }
    }
}

// 0x1005c730 ?IsMoveInProgress@CSBGridLockGameBoard@@IAE_NXZ
bool CSBGridLockGameBoard::IsMoveInProgress()
{
    return m_pCurrentDestTile != 0 || m_vecMovePath.size() != 0;
}

// 0x1005c770 ?IsStraightLine@CSBGridLockGameBoard@@IAE_NABVCGameBoardPos@@0@Z
bool CSBGridLockGameBoard::IsStraightLine(const CGameBoardPos &posStart, const CGameBoardPos &posEnd)
{
    return posStart.Row() == posEnd.Row() || posStart.Col() == posEnd.Col();
}

// 0x1005c7e0 ?BuildPath@CSBGridLockGameBoard@@IAE_NABVCGameBoardPos@@0@Z
bool CSBGridLockGameBoard::BuildPath(const CGameBoardPos &posStart, const CGameBoardPos &posEnd)
{
    // Walk from the start to the end in a straight line; a selectable tile on the way blocks it.
    bool bPathFound = true;
    CGameBoardPos::direction eDirection = posStart.GetDirectionTo(posEnd);
    CGameBoardPos posCurrent(posStart);
    CGameBoardPos posNext(posStart);
    m_vecMovePath.clear();
    for (posNext = posCurrent; bPathFound && posCurrent != posEnd; posNext.Move(eDirection)) {
        posCurrent = posNext;
        if (posStart != posCurrent && GetTileAt(posCurrent, true).IsSelectable()) {
            m_vecMovePath.clear();
            bPathFound = false;
        } else
            m_vecMovePath.push_back(posCurrent);
    }
    return bPathFound;
}

// 0x1005c9f0 ?MoveTile@CSBGridLockGameBoard@@IAEXXZ
void CSBGridLockGameBoard::MoveTile()
{
    if (IsMoveInProgress()) {
        // The next step of the path: the moving tile and the one in its way swap places.
        bool bLastMove = false;
        CGameBoardPos posStart(m_vecMovePath.front());
        m_vecMovePath.erase(m_vecMovePath.begin());
        CGameBoardPos posDest(m_vecMovePath.front());
        if (m_vecMovePath.size() == 1) {
            m_vecMovePath.erase(m_vecMovePath.begin());
            bLastMove = true;
        }
        CSBGridlockGameTile &refStartTile = GetTileAt(posStart, true);
        CSBGridlockGameTile &refDestTile = GetTileAt(posDest, true);
        refStartTile.MoveTo(posDest, bLastMove);
        refDestTile.MoveTo(posStart, true);
        m_pCurrentStartTile = &refStartTile;
        m_pCurrentDestTile = &refDestTile;
        m_nTilesMoveing = 2;
    }
    m_ButtonGroup.Enable(!IsMoveInProgress());
}

// 0x1005cc00 ?NotifyEvent@CSBGridLockGameBoard@@QAEXJJPAX@Z
void CSBGridLockGameBoard::NotifyEvent(long channel, long event, void *data)
{
    if (data) {
        switch (channel) {
        case 400:
            if (event == 410)
                HandleClick(channel, event, (IMBUnknown *)data);
            break;
        case -2:
            if (event >= 2 && event <= 7)
                HandleAnimEvent(channel, event, (MBAnimEventStruct *)data);
            break;
        }
    }
}

// 0x1005cc70 ?Update@CSBGridLockGameBoard@@QAEXXZ
void CSBGridLockGameBoard::Update()
{
    for (gridmatrix::iterator itRow = m_matGameGrid.begin(); itRow != m_matGameGrid.end(); ++itRow)
        for (gridcolumn::iterator itCol = (*itRow).begin(); itCol != (*itRow).end(); ++itCol)
            for (gridsquare::iterator itTile = (*itCol).begin(); itTile != (*itCol).end(); ++itTile)
                (*itTile).Update();
}

// 0x1005ccf0 ?RowCount@CSBGridLockGameBoard@@QAEHXZ
int CSBGridLockGameBoard::RowCount()
{
    return m_matGameGrid.size();
}

// 0x1005cd20 ?ColCount@CSBGridLockGameBoard@@QAEHXZ
int CSBGridLockGameBoard::ColCount()
{
    if (m_matGameGrid.size())
        return m_matGameGrid[0].size();
    return 0;
}

// 0x1005cd60 ?GetLevelCompleted@CSBGridLockGameBoard@@QAEJXZ
long CSBGridLockGameBoard::GetLevelCompleted()
{
    return m_bGoalReached ? m_nLevel : m_nLevel - 1;
}

// 0x1005cd90 ?GetTokenList@CSBGridLockGameBoard@@IBEIABV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@DAAV?$MBList@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@@@Z
unsigned int CSBGridLockGameBoard::GetTokenList(const MBString &strSource, char cDelim, stringlist &listTokens) const
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

// Template instances emitted by the code above:
// 0x1005d2b0 ??A?$map@JV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$less@J@2@V?$MBAllocator@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@@@std@@QAEAAV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@1@ABJ@Z
// 0x1005d820 ??1?$pair@$$CBJV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@std@@QAE@XZ
// 0x1005f7b0 ?insert@?$vector@VCGameBoardPos@@V?$MBAllocator@VCGameBoardPos@@@@@std@@QAEXPAVCGameBoardPos@@IABV3@@Z
// 0x10060270 ?insert@?$_Tree@JU?$pair@$$CBJV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@std@@U_Kfn@?$map@JV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$less@J@2@V?$MBAllocator@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@@@2@U?$less@J@2@V?$MBAllocator@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@@@std@@QAE?AU?$pair@Viterator@?$_Tree@JU?$pair@$$CBJV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@std@@U_Kfn@?$map@JV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$less@J@2@V?$MBAllocator@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@@@2@U?$less@J@2@V?$MBAllocator@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@@@std@@_N@2@ABU?$pair@$$CBJV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@@Z
// 0x10063650 ?_Lbound@?$_Tree@PAVIMBUnknown@@U?$pair@QAVIMBUnknown@@PAVCSBGridlockGameTile@@@std@@U_Kfn@?$map@PAVIMBUnknown@@PAVCSBGridlockGameTile@@U?$less@PAVIMBUnknown@@@std@@V?$MBAllocator@PAVCSBGridlockGameTile@@@@@3@U?$less@PAVIMBUnknown@@@3@V?$MBAllocator@PAVCSBGridlockGameTile@@@@@std@@IBEPAU_Node@12@ABQAVIMBUnknown@@@Z
// 0x100636c0 ?_Insert@?$_Tree@JU?$pair@$$CBJV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@std@@U_Kfn@?$map@JV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$less@J@2@V?$MBAllocator@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@@@2@U?$less@J@2@V?$MBAllocator@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@@@std@@IAE?AViterator@12@PAU_Node@12@0ABU?$pair@$$CBJV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@@Z
// 0x10064660 ?_Dec@const_iterator@?$_Tree@JU?$pair@$$CBJV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@std@@U_Kfn@?$map@JV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$less@J@2@V?$MBAllocator@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@@@2@U?$less@J@2@V?$MBAllocator@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@@@std@@QAEXXZ
// 0x100651a0 ?_Construct@std@@YAXPAVCGameBoardPos@@ABV2@@Z
// 0x10065210 ?_Construct@std@@YAXPAU?$pair@$$CBJV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@1@ABU21@@Z

// Instances the linker took from other objects (this unit emits them too):
// 0x1003dd10 ?find@?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@QBEIPBDII@Z
// 0x10048d00 ?substr@?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@QBE?AV12@II@Z
// 0x1004cc50 ?_Construct@std@@YAXPAV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@1@ABV21@@Z
// 0x10058b10 ??_GCSBGridLockGameBoard@@UAEPAXI@Z
// 0x10058b50 ??1?$MBVector@V?$MBVector@V?$MBVector@VCSBGridlockGameTile@@@@@@@@QAE@XZ
// 0x10058bf0 ??1?$MBVector@VCGameBoardPos@@@@QAE@XZ
// 0x10058c40 ??1?$MBMap@PAVIMBUnknown@@PAVCSBGridlockGameTile@@U?$less@PAVIMBUnknown@@@std@@@@QAE@XZ
// 0x10058ce0 ??1?$MBMap@JV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$less@J@2@@@QAE@XZ
// 0x10058d80 ??1?$MBMap@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@JU?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@@@QAE@XZ
// 0x1005be00 ??1?$MBVector@VCSBGridlockGameTile@@@@QAE@XZ
// 0x1005d660 ??1?$vector@V?$MBVector@VCSBGridlockGameTile@@@@V?$MBAllocator@V?$MBVector@VCSBGridlockGameTile@@@@@@@std@@QAE@XZ
// 0x1005e090 ?erase@?$_Tree@JU?$pair@$$CBJV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@std@@U_Kfn@?$map@JV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$less@J@2@V?$MBAllocator@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@@@2@U?$less@J@2@V?$MBAllocator@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@@@std@@QAE?AViterator@12@V312@0@Z
// 0x1005e3f0 ?erase@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@J@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@JU?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@J@@@2@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@J@@@std@@QAE?AViterator@12@V312@0@Z
// 0x100603f0 ?erase@?$_Tree@JU?$pair@$$CBJV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@std@@U_Kfn@?$map@JV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$less@J@2@V?$MBAllocator@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@@@2@U?$less@J@2@V?$MBAllocator@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@@@std@@QAE?AViterator@12@V312@@Z
// 0x10060d30 ?erase@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@J@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@JU?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@J@@@2@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@J@@@std@@QAE?AViterator@12@V312@@Z
// 0x100627e0 ?destroy@?$allocator@V?$MBVector@VCSBGridlockGameTile@@@@@std@@QAEXPAV?$MBVector@VCSBGridlockGameTile@@@@@Z
// 0x10064750 ?_Inc@const_iterator@?$_Tree@JU?$pair@$$CBJV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@std@@U_Kfn@?$map@JV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$less@J@2@V?$MBAllocator@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@@@2@U?$less@J@2@V?$MBAllocator@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@@@std@@QAEXXZ
// 0x10064920 ?_Inc@const_iterator@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@J@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@JU?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@J@@@2@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@J@@@std@@QAEXXZ
// 0x10065630 ??_GCGameBoardPos@@QAEPAXI@Z

// Template and inline instances this unit also emits:
// 0x1001b5a0 ?compare@?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@QBEHPBD@Z
// 0x10039020 ?_Freeze@?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@AAEXXZ
// 0x1003f190 ?CreateObject@?$MBComPtr@VISBGameState@@@@QAEJABUMBGUID@@@Z
// 0x10048f50 ?find_first_of@?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@QBEIPBDII@Z
// 0x1005a240 ??1?$MBMap@VCGameBoardPos@@HU?$less@VCGameBoardPos@@@std@@@@QAE@XZ
// 0x1005a2e0 ??1?$MBMap@HW4tileprop@CSBGridlockGameTile@@U?$less@H@std@@@@QAE@XZ
// 0x1005a380 ??1?$MBMap@HV?$MBComPtr@VIMBAnim@@@@U?$less@H@std@@@@QAE@XZ
// 0x1005b150 ??1?$pair@$$CBHV?$MBComPtr@VIMBAnim@@@@@std@@QAE@XZ
// 0x1005b680 ??1?$pair@$$CBVCGameBoardPos@@H@std@@QAE@XZ
// 0x1005bd60 ??1?$MBVector@V?$MBVector@VCSBGridlockGameTile@@@@@@QAE@XZ
// 0x1005d0d0 ?size@?$vector@V?$MBVector@V?$MBVector@VCSBGridlockGameTile@@@@@@V?$MBAllocator@V?$MBVector@V?$MBVector@VCSBGridlockGameTile@@@@@@@@@std@@QBEIXZ
// 0x1005d4b0 ??0?$map@HV?$MBComPtr@VIMBAnim@@@@U?$less@H@std@@V?$MBAllocator@V?$MBComPtr@VIMBAnim@@@@@@@std@@QAE@ABU?$less@H@1@ABV?$MBAllocator@V?$MBComPtr@VIMBAnim@@@@@@@Z
// 0x1005d570 ??0?$map@HW4tileprop@CSBGridlockGameTile@@U?$less@H@std@@V?$MBAllocator@W4tileprop@CSBGridlockGameTile@@@@@std@@QAE@ABU?$less@H@1@ABV?$MBAllocator@W4tileprop@CSBGridlockGameTile@@@@@Z
// 0x1005d630 ??0?$vector@V?$MBVector@VCSBGridlockGameTile@@@@V?$MBAllocator@V?$MBVector@VCSBGridlockGameTile@@@@@@@std@@QAE@ABV?$MBAllocator@V?$MBVector@VCSBGridlockGameTile@@@@@@@Z
// 0x1005d6b0 ?size@?$vector@V?$MBVector@VCSBGridlockGameTile@@@@V?$MBAllocator@V?$MBVector@VCSBGridlockGameTile@@@@@@@std@@QBEIXZ
// 0x1005d6e0 ??0?$vector@VCSBGridlockGameTile@@V?$MBAllocator@VCSBGridlockGameTile@@@@@std@@QAE@ABV?$MBAllocator@VCSBGridlockGameTile@@@@@Z
// 0x1005d710 ??1?$vector@VCSBGridlockGameTile@@V?$MBAllocator@VCSBGridlockGameTile@@@@@std@@QAE@XZ
// 0x1005d760 ??0?$map@VCGameBoardPos@@HU?$less@VCGameBoardPos@@@std@@V?$MBAllocator@H@@@std@@QAE@ABU?$less@VCGameBoardPos@@@1@ABV?$MBAllocator@H@@@Z
// 0x1005d890 ??1?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@J@std@@QAE@XZ
// 0x1005d900 ?find_first_not_of@?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@QBEIPBDII@Z
// 0x1005d980 ?CreateObject@?$MBComPtr@VIMBPref@@@@QAEJABUMBGUID@@@Z
// 0x1005db20 ?insert@?$vector@V?$MBVector@V?$MBVector@VCSBGridlockGameTile@@@@@@V?$MBAllocator@V?$MBVector@V?$MBVector@VCSBGridlockGameTile@@@@@@@@@std@@QAEXPAV?$MBVector@V?$MBVector@VCSBGridlockGameTile@@@@@@IABV3@@Z
// 0x1005de60 ?erase@?$vector@V?$MBVector@V?$MBVector@VCSBGridlockGameTile@@@@@@V?$MBAllocator@V?$MBVector@V?$MBVector@VCSBGridlockGameTile@@@@@@@@@std@@QAEPAV?$MBVector@V?$MBVector@VCSBGridlockGameTile@@@@@@PAV3@0@Z
// 0x1005e050 ?insert@?$map@JV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$less@J@2@V?$MBAllocator@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@@@std@@QAE?AU?$pair@Viterator@?$_Tree@JU?$pair@$$CBJV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@std@@U_Kfn@?$map@JV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$less@J@2@V?$MBAllocator@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@@@2@U?$less@J@2@V?$MBAllocator@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@@@std@@_N@2@ABU?$pair@$$CBJV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@@Z
// 0x1005e220 ??0?$pair@$$CBJV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@std@@QAE@ABJABV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@1@@Z
// 0x1005e3b0 ?insert@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@JU?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@J@@@std@@QAE?AU?$pair@Viterator@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@J@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@JU?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@J@@@2@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@J@@@std@@_N@2@ABU?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@J@2@@Z
// 0x1005e710 ?Release@?$MBComPtr@VISBGameState@@@@QAEXXZ
// 0x1005e740 ?insert@?$_Tree@HU?$pair@$$CBHV?$MBComPtr@VIMBAnim@@@@@std@@U_Kfn@?$map@HV?$MBComPtr@VIMBAnim@@@@U?$less@H@std@@V?$MBAllocator@V?$MBComPtr@VIMBAnim@@@@@@@2@U?$less@H@2@V?$MBAllocator@V?$MBComPtr@VIMBAnim@@@@@@@std@@QAE?AU?$pair@Viterator@?$_Tree@HU?$pair@$$CBHV?$MBComPtr@VIMBAnim@@@@@std@@U_Kfn@?$map@HV?$MBComPtr@VIMBAnim@@@@U?$less@H@std@@V?$MBAllocator@V?$MBComPtr@VIMBAnim@@@@@@@2@U?$less@H@2@V?$MBAllocator@V?$MBComPtr@VIMBAnim@@@@@@@std@@_N@2@ABU?$pair@$$CBHV?$MBComPtr@VIMBAnim@@@@@2@@Z
// 0x1005e8c0 ?erase@?$_Tree@HU?$pair@$$CBHV?$MBComPtr@VIMBAnim@@@@@std@@U_Kfn@?$map@HV?$MBComPtr@VIMBAnim@@@@U?$less@H@std@@V?$MBAllocator@V?$MBComPtr@VIMBAnim@@@@@@@2@U?$less@H@2@V?$MBAllocator@V?$MBComPtr@VIMBAnim@@@@@@@std@@QAE?AViterator@12@V312@0@Z
// 0x1005ea80 ?find@?$_Tree@HU?$pair@$$CBHV?$MBComPtr@VIMBAnim@@@@@std@@U_Kfn@?$map@HV?$MBComPtr@VIMBAnim@@@@U?$less@H@std@@V?$MBAllocator@V?$MBComPtr@VIMBAnim@@@@@@@2@U?$less@H@2@V?$MBAllocator@V?$MBComPtr@VIMBAnim@@@@@@@std@@QAE?AViterator@12@ABH@Z
// 0x1005eaf0 ?insert@?$map@HW4tileprop@CSBGridlockGameTile@@U?$less@H@std@@V?$MBAllocator@W4tileprop@CSBGridlockGameTile@@@@@std@@QAE?AU?$pair@Viterator@?$_Tree@HU?$pair@$$CBHW4tileprop@CSBGridlockGameTile@@@std@@U_Kfn@?$map@HW4tileprop@CSBGridlockGameTile@@U?$less@H@std@@V?$MBAllocator@W4tileprop@CSBGridlockGameTile@@@@@2@U?$less@H@2@V?$MBAllocator@W4tileprop@CSBGridlockGameTile@@@@@std@@_N@2@ABU?$pair@$$CBHW4tileprop@CSBGridlockGameTile@@@2@@Z
// 0x1005eb30 ?erase@?$_Tree@HU?$pair@$$CBHW4tileprop@CSBGridlockGameTile@@@std@@U_Kfn@?$map@HW4tileprop@CSBGridlockGameTile@@U?$less@H@std@@V?$MBAllocator@W4tileprop@CSBGridlockGameTile@@@@@2@U?$less@H@2@V?$MBAllocator@W4tileprop@CSBGridlockGameTile@@@@@std@@QAE?AViterator@12@V312@0@Z
// 0x1005ecb0 ?find@?$_Tree@HU?$pair@$$CBHW4tileprop@CSBGridlockGameTile@@@std@@U_Kfn@?$map@HW4tileprop@CSBGridlockGameTile@@U?$less@H@std@@V?$MBAllocator@W4tileprop@CSBGridlockGameTile@@@@@2@U?$less@H@2@V?$MBAllocator@W4tileprop@CSBGridlockGameTile@@@@@std@@QAE?AViterator@12@ABH@Z
// 0x1005ed50 ?insert@?$vector@V?$MBVector@VCSBGridlockGameTile@@@@V?$MBAllocator@V?$MBVector@VCSBGridlockGameTile@@@@@@@std@@QAEXPAV?$MBVector@VCSBGridlockGameTile@@@@IABV3@@Z
// 0x1005f090 ?erase@?$vector@V?$MBVector@VCSBGridlockGameTile@@@@V?$MBAllocator@V?$MBVector@VCSBGridlockGameTile@@@@@@@std@@QAEPAV?$MBVector@VCSBGridlockGameTile@@@@PAV3@0@Z
// 0x1005f100 ?insert@?$vector@VCSBGridlockGameTile@@V?$MBAllocator@VCSBGridlockGameTile@@@@@std@@QAEPAVCSBGridlockGameTile@@PAV3@ABV3@@Z
// 0x1005f370 ?size@?$vector@VCSBGridlockGameTile@@V?$MBAllocator@VCSBGridlockGameTile@@@@@std@@QBEIXZ
// 0x1005f3b0 ?_Destroy@?$vector@VCSBGridlockGameTile@@V?$MBAllocator@VCSBGridlockGameTile@@@@@std@@IAEXPAVCSBGridlockGameTile@@0@Z
// 0x1005f3f0 ?insert@?$_Tree@VCGameBoardPos@@U?$pair@$$CBVCGameBoardPos@@H@std@@U_Kfn@?$map@VCGameBoardPos@@HU?$less@VCGameBoardPos@@@std@@V?$MBAllocator@H@@@3@U?$less@VCGameBoardPos@@@3@V?$MBAllocator@H@@@std@@QAE?AU?$pair@Viterator@?$_Tree@VCGameBoardPos@@U?$pair@$$CBVCGameBoardPos@@H@std@@U_Kfn@?$map@VCGameBoardPos@@HU?$less@VCGameBoardPos@@@std@@V?$MBAllocator@H@@@3@U?$less@VCGameBoardPos@@@3@V?$MBAllocator@H@@@std@@_N@2@ABU?$pair@$$CBVCGameBoardPos@@H@2@@Z
// 0x1005f5a0 ?erase@?$_Tree@VCGameBoardPos@@U?$pair@$$CBVCGameBoardPos@@H@std@@U_Kfn@?$map@VCGameBoardPos@@HU?$less@VCGameBoardPos@@@std@@V?$MBAllocator@H@@@3@U?$less@VCGameBoardPos@@@3@V?$MBAllocator@H@@@std@@QAE?AViterator@12@V312@0@Z
// 0x1005f730 ?find@?$_Tree@VCGameBoardPos@@U?$pair@$$CBVCGameBoardPos@@H@std@@U_Kfn@?$map@VCGameBoardPos@@HU?$less@VCGameBoardPos@@@std@@V?$MBAllocator@H@@@3@U?$less@VCGameBoardPos@@@3@V?$MBAllocator@H@@@std@@QAE?AViterator@12@ABVCGameBoardPos@@@Z
// 0x100601b0 ?_Erase@?$_Tree@PAVIMBUnknown@@U?$pair@QAVIMBUnknown@@PAVCSBGridlockGameTile@@@std@@U_Kfn@?$map@PAVIMBUnknown@@PAVCSBGridlockGameTile@@U?$less@PAVIMBUnknown@@@std@@V?$MBAllocator@PAVCSBGridlockGameTile@@@@@3@U?$less@PAVIMBUnknown@@@3@V?$MBAllocator@PAVCSBGridlockGameTile@@@@@std@@IAEXPAU_Node@12@@Z
// 0x10060a70 ?_Erase@?$_Tree@JU?$pair@$$CBJV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@std@@U_Kfn@?$map@JV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$less@J@2@V?$MBAllocator@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@@@2@U?$less@J@2@V?$MBAllocator@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@@@std@@IAEXPAU_Node@12@@Z
// 0x10060b60 ?insert@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@J@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@JU?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@J@@@2@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@J@@@std@@QAE?AU?$pair@Viterator@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@J@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@JU?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@J@@@2@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@J@@@std@@_N@2@ABU?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@J@2@@Z
// 0x100613b0 ?_Erase@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@J@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@JU?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@J@@@2@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@J@@@std@@IAEXPAU_Node@12@@Z
// 0x100614a0 ?erase@?$_Tree@HU?$pair@$$CBHV?$MBComPtr@VIMBAnim@@@@@std@@U_Kfn@?$map@HV?$MBComPtr@VIMBAnim@@@@U?$less@H@std@@V?$MBAllocator@V?$MBComPtr@VIMBAnim@@@@@@@2@U?$less@H@2@V?$MBAllocator@V?$MBComPtr@VIMBAnim@@@@@@@std@@QAE?AViterator@12@V312@@Z
// 0x10061b00 ?_Erase@?$_Tree@HU?$pair@$$CBHV?$MBComPtr@VIMBAnim@@@@@std@@U_Kfn@?$map@HV?$MBComPtr@VIMBAnim@@@@U?$less@H@std@@V?$MBAllocator@V?$MBComPtr@VIMBAnim@@@@@@@2@U?$less@H@2@V?$MBAllocator@V?$MBComPtr@VIMBAnim@@@@@@@std@@IAEXPAU_Node@12@@Z
// 0x10061bd0 ?_Insert@?$_Tree@HU?$pair@$$CBHV?$MBComPtr@VIMBAnim@@@@@std@@U_Kfn@?$map@HV?$MBComPtr@VIMBAnim@@@@U?$less@H@std@@V?$MBAllocator@V?$MBComPtr@VIMBAnim@@@@@@@2@U?$less@H@2@V?$MBAllocator@V?$MBComPtr@VIMBAnim@@@@@@@std@@IAE?AViterator@12@PAU_Node@12@0ABU?$pair@$$CBHV?$MBComPtr@VIMBAnim@@@@@2@@Z
// 0x10061f60 ?insert@?$_Tree@HU?$pair@$$CBHW4tileprop@CSBGridlockGameTile@@@std@@U_Kfn@?$map@HW4tileprop@CSBGridlockGameTile@@U?$less@H@std@@V?$MBAllocator@W4tileprop@CSBGridlockGameTile@@@@@2@U?$less@H@2@V?$MBAllocator@W4tileprop@CSBGridlockGameTile@@@@@std@@QAE?AU?$pair@Viterator@?$_Tree@HU?$pair@$$CBHW4tileprop@CSBGridlockGameTile@@@std@@U_Kfn@?$map@HW4tileprop@CSBGridlockGameTile@@U?$less@H@std@@V?$MBAllocator@W4tileprop@CSBGridlockGameTile@@@@@2@U?$less@H@2@V?$MBAllocator@W4tileprop@CSBGridlockGameTile@@@@@std@@_N@2@ABU?$pair@$$CBHW4tileprop@CSBGridlockGameTile@@@2@@Z
// 0x100620e0 ?erase@?$_Tree@HU?$pair@$$CBHW4tileprop@CSBGridlockGameTile@@@std@@U_Kfn@?$map@HW4tileprop@CSBGridlockGameTile@@U?$less@H@std@@V?$MBAllocator@W4tileprop@CSBGridlockGameTile@@@@@2@U?$less@H@2@V?$MBAllocator@W4tileprop@CSBGridlockGameTile@@@@@std@@QAE?AViterator@12@V312@@Z
// 0x10062720 ?_Erase@?$_Tree@HU?$pair@$$CBHW4tileprop@CSBGridlockGameTile@@@std@@U_Kfn@?$map@HW4tileprop@CSBGridlockGameTile@@U?$less@H@std@@V?$MBAllocator@W4tileprop@CSBGridlockGameTile@@@@@2@U?$less@H@2@V?$MBAllocator@W4tileprop@CSBGridlockGameTile@@@@@std@@IAEXPAU_Node@12@@Z
// 0x10062800 ?insert@?$vector@VCSBGridlockGameTile@@V?$MBAllocator@VCSBGridlockGameTile@@@@@std@@QAEXPAVCSBGridlockGameTile@@IABV3@@Z
// 0x10062b60 ?destroy@?$allocator@VCSBGridlockGameTile@@@std@@QAEXPAVCSBGridlockGameTile@@@Z
// 0x10062b80 ?erase@?$_Tree@VCGameBoardPos@@U?$pair@$$CBVCGameBoardPos@@H@std@@U_Kfn@?$map@VCGameBoardPos@@HU?$less@VCGameBoardPos@@@std@@V?$MBAllocator@H@@@3@U?$less@VCGameBoardPos@@@3@V?$MBAllocator@H@@@std@@QAE?AViterator@12@V312@@Z
// 0x100631c0 ?_Erase@?$_Tree@VCGameBoardPos@@U?$pair@$$CBVCGameBoardPos@@H@std@@U_Kfn@?$map@VCGameBoardPos@@HU?$less@VCGameBoardPos@@@std@@V?$MBAllocator@H@@@3@U?$less@VCGameBoardPos@@@3@V?$MBAllocator@H@@@std@@IAEXPAU_Node@12@@Z
// 0x10063290 ?_Insert@?$_Tree@VCGameBoardPos@@U?$pair@$$CBVCGameBoardPos@@H@std@@U_Kfn@?$map@VCGameBoardPos@@HU?$less@VCGameBoardPos@@@std@@V?$MBAllocator@H@@@3@U?$less@VCGameBoardPos@@@3@V?$MBAllocator@H@@@std@@IAE?AViterator@12@PAU_Node@12@0ABU?$pair@$$CBVCGameBoardPos@@H@2@@Z
// 0x10063620 ??0?$pair@Viterator@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@J@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@JU?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@J@@@2@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@J@@@std@@_N@std@@QAE@ABViterator@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@J@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@JU?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@J@@@2@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@J@@@1@AB_N@Z
// 0x10063a50 ?_Insert@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@J@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@JU?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@J@@@2@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@J@@@std@@IAE?AViterator@12@PAU_Node@12@0ABU?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@J@2@@Z
// 0x10063de0 ?_Lbound@?$_Tree@HU?$pair@$$CBHV?$MBComPtr@VIMBAnim@@@@@std@@U_Kfn@?$map@HV?$MBComPtr@VIMBAnim@@@@U?$less@H@std@@V?$MBAllocator@V?$MBComPtr@VIMBAnim@@@@@@@2@U?$less@H@2@V?$MBAllocator@V?$MBComPtr@VIMBAnim@@@@@@@std@@IBEPAU_Node@12@ABH@Z
// 0x10063e50 ?_Dec@const_iterator@?$_Tree@HU?$pair@$$CBHV?$MBComPtr@VIMBAnim@@@@@std@@U_Kfn@?$map@HV?$MBComPtr@VIMBAnim@@@@U?$less@H@std@@V?$MBAllocator@V?$MBComPtr@VIMBAnim@@@@@@@2@U?$less@H@2@V?$MBAllocator@V?$MBComPtr@VIMBAnim@@@@@@@std@@QAEXXZ
// 0x10063f40 ?_Insert@?$_Tree@HU?$pair@$$CBHW4tileprop@CSBGridlockGameTile@@@std@@U_Kfn@?$map@HW4tileprop@CSBGridlockGameTile@@U?$less@H@std@@V?$MBAllocator@W4tileprop@CSBGridlockGameTile@@@@@2@U?$less@H@2@V?$MBAllocator@W4tileprop@CSBGridlockGameTile@@@@@std@@IAE?AViterator@12@PAU_Node@12@0ABU?$pair@$$CBHW4tileprop@CSBGridlockGameTile@@@2@@Z
// 0x100642d0 ?_Lbound@?$_Tree@HU?$pair@$$CBHW4tileprop@CSBGridlockGameTile@@@std@@U_Kfn@?$map@HW4tileprop@CSBGridlockGameTile@@U?$less@H@std@@V?$MBAllocator@W4tileprop@CSBGridlockGameTile@@@@@2@U?$less@H@2@V?$MBAllocator@W4tileprop@CSBGridlockGameTile@@@@@std@@IBEPAU_Node@12@ABH@Z
// 0x10064340 ?_Ucopy@?$vector@VCSBGridlockGameTile@@V?$MBAllocator@VCSBGridlockGameTile@@@@@std@@IAEPAVCSBGridlockGameTile@@PBV3@0PAV3@@Z
// 0x10064390 ?_Ufill@?$vector@VCSBGridlockGameTile@@V?$MBAllocator@VCSBGridlockGameTile@@@@@std@@IAEXPAVCSBGridlockGameTile@@IABV3@@Z
// 0x100643d0 ?_Lbound@?$_Tree@VCGameBoardPos@@U?$pair@$$CBVCGameBoardPos@@H@std@@U_Kfn@?$map@VCGameBoardPos@@HU?$less@VCGameBoardPos@@@std@@V?$MBAllocator@H@@@3@U?$less@VCGameBoardPos@@@3@V?$MBAllocator@H@@@std@@IBEPAU_Node@12@ABVCGameBoardPos@@@Z
// 0x10064490 ?_Dec@const_iterator@?$_Tree@VCGameBoardPos@@U?$pair@$$CBVCGameBoardPos@@H@std@@U_Kfn@?$map@VCGameBoardPos@@HU?$less@VCGameBoardPos@@@std@@V?$MBAllocator@H@@@3@U?$less@VCGameBoardPos@@@3@V?$MBAllocator@H@@@std@@QAEXXZ
// 0x10064830 ?_Dec@const_iterator@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@J@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@JU?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@J@@@2@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@J@@@std@@QAEXXZ
// 0x10064a00 ?_Inc@const_iterator@?$_Tree@HU?$pair@$$CBHV?$MBComPtr@VIMBAnim@@@@@std@@U_Kfn@?$map@HV?$MBComPtr@VIMBAnim@@@@U?$less@H@std@@V?$MBAllocator@V?$MBComPtr@VIMBAnim@@@@@@@2@U?$less@H@2@V?$MBAllocator@V?$MBComPtr@VIMBAnim@@@@@@@std@@QAEXXZ
// 0x10064ae0 ?_Dec@const_iterator@?$_Tree@HU?$pair@$$CBHW4tileprop@CSBGridlockGameTile@@@std@@U_Kfn@?$map@HW4tileprop@CSBGridlockGameTile@@U?$less@H@std@@V?$MBAllocator@W4tileprop@CSBGridlockGameTile@@@@@2@U?$less@H@2@V?$MBAllocator@W4tileprop@CSBGridlockGameTile@@@@@std@@QAEXXZ
// 0x10064bd0 ?_Inc@const_iterator@?$_Tree@HU?$pair@$$CBHW4tileprop@CSBGridlockGameTile@@@std@@U_Kfn@?$map@HW4tileprop@CSBGridlockGameTile@@U?$less@H@std@@V?$MBAllocator@W4tileprop@CSBGridlockGameTile@@@@@2@U?$less@H@2@V?$MBAllocator@W4tileprop@CSBGridlockGameTile@@@@@std@@QAEXXZ
// 0x10064cb0 ?_Inc@const_iterator@?$_Tree@VCGameBoardPos@@U?$pair@$$CBVCGameBoardPos@@H@std@@U_Kfn@?$map@VCGameBoardPos@@HU?$less@VCGameBoardPos@@@std@@V?$MBAllocator@H@@@3@U?$less@VCGameBoardPos@@@3@V?$MBAllocator@H@@@std@@QAEXXZ
// 0x10064d90 ?fill@std@@YAXPAVCSBGridlockGameTile@@0ABV2@@Z
// 0x10064dd0 ?copy_backward@std@@YAPAVCSBGridlockGameTile@@PAV2@00@Z
// 0x10064e20 ?_Construct@std@@YAXPAV?$MBVector@V?$MBVector@VCSBGridlockGameTile@@@@@@ABV2@@Z
// 0x10064fa0 ?construct@?$allocator@VCSBGridlockGameTile@@@std@@QAEXPAVCSBGridlockGameTile@@ABV3@@Z
// 0x10064fd0 ?_Construct@std@@YAXPAU?$pair@$$CBHV?$MBComPtr@VIMBAnim@@@@@1@ABU21@@Z
// 0x10065040 ?_Construct@std@@YAXPAV?$MBVector@VCSBGridlockGameTile@@@@ABV2@@Z
// 0x10065130 ?_Construct@std@@YAXPAU?$pair@$$CBVCGameBoardPos@@H@1@ABU21@@Z
// 0x100653e0 ?_Construct@std@@YAXPAU?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@J@1@ABU21@@Z
// 0x10065590 ?_Construct@std@@YAXPAU?$pair@$$CBHW4tileprop@CSBGridlockGameTile@@@1@ABU21@@Z
// 0x100655c0 ?_Construct@std@@YAXPAVCSBGridlockGameTile@@ABV2@@Z
// 0x10065670 ??4?$vector@V?$MBVector@VCSBGridlockGameTile@@@@V?$MBAllocator@V?$MBVector@VCSBGridlockGameTile@@@@@@@std@@QAEAAV01@ABV01@@Z
// 0x100658d0 ??_G?$MBVector@VCSBGridlockGameTile@@@@QAEPAXI@Z
// 0x10065910 ??4?$vector@VCSBGridlockGameTile@@V?$MBAllocator@VCSBGridlockGameTile@@@@@std@@QAEAAV01@ABV01@@Z

// Template and inline instances this unit also emits:
// 0x1001b400 ??0?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@QAE@ABV?$MBAllocator@D@@@Z
// 0x10027e40 ??Hstd@@YA?AV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@0@ABV10@PBD@Z
// 0x1005d0a0 ??0?$vector@V?$MBVector@V?$MBVector@VCSBGridlockGameTile@@@@@@V?$MBAllocator@V?$MBVector@V?$MBVector@VCSBGridlockGameTile@@@@@@@@@std@@QAE@ABV?$MBAllocator@V?$MBVector@V?$MBVector@VCSBGridlockGameTile@@@@@@@@@Z
// 0x1005d100 ??0?$vector@VCGameBoardPos@@V?$MBAllocator@VCGameBoardPos@@@@@std@@QAE@ABV?$MBAllocator@VCGameBoardPos@@@@@Z
// 0x1005d130 ??0?$map@PAVIMBUnknown@@PAVCSBGridlockGameTile@@U?$less@PAVIMBUnknown@@@std@@V?$MBAllocator@PAVCSBGridlockGameTile@@@@@std@@QAE@ABU?$less@PAVIMBUnknown@@@1@ABV?$MBAllocator@PAVCSBGridlockGameTile@@@@@Z
// 0x1005d1f0 ??0?$map@JV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$less@J@2@V?$MBAllocator@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@@@std@@QAE@ABU?$less@J@1@ABV?$MBAllocator@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@@@Z
// 0x1005d3f0 ??0?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@JU?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@J@@@std@@QAE@ABU?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@1@ABV?$MBAllocator@J@@@Z
// 0x1005e580 ??0?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@J@std@@QAE@ABV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@1@ABJ@Z
// 0x1005cff0 ??1?$list@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@V?$MBAllocator@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@@@std@@QAE@XZ
// 0x1005d080 ?begin@?$list@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@V?$MBAllocator@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@@@std@@QAE?AViterator@12@XZ
// 0x1005ed20 ??0?$pair@$$CBHW4tileprop@CSBGridlockGameTile@@@std@@QAE@ABHABW4tileprop@CSBGridlockGameTile@@@Z

// Static initializers and their atexit stubs: the PDB numbers them by the original header set, which
// ours does not reproduce yet, so they cannot be claimed by name.
// 0x10056d60 _$E57
// 0x10056d80 _$E54
// 0x10057350 _$E56
// 0x10057370 _$E55
// 0x10065c60 _$E76
// 0x10065ca0 _$E72
// 0x10065cc0 _$E75
