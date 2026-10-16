#include "SavedGameDB.h"
#include "SteamboatNode.h"
#include "MBRuntime.h"
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// A failing step throws its source line, so Load and Save sit on the lines they had in the
// original file (the gaps held code that is not reconstructed yet).
#define THROW_IF(cond) if (cond) throw (long)__LINE__

// 0x10037450 ??0CSavedGameDB@@QAE@XZ
CSavedGameDB::CSavedGameDB()
{
}

// 0x10037550 ??1CSavedGameDB@@UAE@XZ
CSavedGameDB::~CSavedGameDB()
{
}

// 0x100375f0 ?SetGameUsed@CSavedGameDB@@QAEXJ_N@Z
void CSavedGameDB::SetGameUsed(long nGame, bool bUsed)
{
    if (bUsed) {
        if (!IsGameUsed(nGame))
            AddDefaultEntry(nGame);
    } else {
        m_mapSaveData.erase(nGame);
    }
}

// 0x10037690 ?IsGameUsed@CSavedGameDB@@QAE_NJ@Z
bool CSavedGameDB::IsGameUsed(long nGame)
{
    sgdbmap::iterator it = m_mapSaveData.find(nGame);
    return m_mapSaveData.end() != it;
}

// 0x10037710 ?SetGameLastSean@CSavedGameDB@@QAEXJABV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@Z
void CSavedGameDB::SetGameLastSean(long nGame, const MBString &strScene)
{
    GetEntry(nGame).strSceneCode = strScene;
}

// 0x100378a0 ?GetGameLastSean@CSavedGameDB@@QAE_NJAAV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@Z
bool CSavedGameDB::GetGameLastSean(long nGame, MBString &strSean)
{
    bool bFound = false;
    if (IsGameUsed(nGame)) {
        strSean = GetEntry(nGame).strSceneCode;
        bFound = true;
    }
    return bFound;
}

// 0x10037a70 ?SetGameIcon@CSavedGameDB@@QAEXJJ@Z
void CSavedGameDB::SetGameIcon(long nGame, long nIcon)
{
    GetEntry(nGame).nIcon = nIcon;
}

// 0x10037aa0 ?GetGameIcon@CSavedGameDB@@QAE_NJAAJ@Z
bool CSavedGameDB::GetGameIcon(long nGame, long &nIcon)
{
    bool bFound = false;
    if (IsGameUsed(nGame)) {
        nIcon = GetEntry(nGame).nIcon;
        bFound = true;
    }
    return bFound;
}

// 0x10037af0 ?GetEntry@CSavedGameDB@@IAEAAUsgdbinfo@1@J@Z
CSavedGameDB::sgdbinfo &CSavedGameDB::GetEntry(long nGame)
{
    if (!IsGameUsed(nGame))
        return AddDefaultEntry(nGame);
    return m_mapSaveData[nGame];
}

// 0x10037b40 ?AddDefaultEntry@CSavedGameDB@@IAEAAUsgdbinfo@1@J@Z
CSavedGameDB::sgdbinfo &CSavedGameDB::AddDefaultEntry(long nGame)
{
    sgdbinfo &info = m_mapSaveData[nGame];
    info.nIcon = -1;
    return info;
}






































































// 0x10037b70 ?Load@CSavedGameDB@@QAE_NXZ
bool CSavedGameDB::Load()
{
    IMBStream *pPrefStream = 0;
    IMBPref *pPref = 0;
    MBString strData;
    char pszSection[32] = { 0 };
    bool bRet = false;
    char *pszToken;
    try {
        m_mapSaveData.clear();
        THROW_IF(!SteamboatNode::CreatePrefAndStream(pPrefStream, pPref, "SGDB.ini", false, false, 0));
        THROW_IF(pPref->GetString(pPrefStream, "used games", "games", strData, "") <= 0);
        // the used games are a list of numbers separated by commas
        strData.resize(std::remove(strData.begin(), strData.end(), ' ') - strData.begin());
        pszToken = strdup(strData.c_str());
        pszToken = strtok(pszToken, ",");
        while (pszToken) {
            sgdbinfo objGameInfo;
            sprintf(pszSection, "game %s data", pszToken);
            // each game has a section with its icon and the scene it was saved in
            //
            THROW_IF(pPref->GetString(pPrefStream, pszSection, "icon", strData, "") <= 0);
            objGameInfo.nIcon = atoi(strData.c_str());
            THROW_IF(pPref->GetString(pPrefStream, pszSection, "scene", strData, "") <= 0);
            objGameInfo.strSceneCode = strData;
            m_mapSaveData[atoi(pszToken)] = objGameInfo;
            pszToken = strtok(0, ",");
        }
        bRet = true;
    } catch (long nLine) {
        MBDebugStr("%s(%d) : ERROR! Failed to load Saved Game Database %s\n", "D:\\tugboat\\code\\steamboat\\SavedGameDB.cpp", nLine, "SGDB.ini");
    }
    if (pPref) {
        pPref->Release();
        pPref = 0;
    }
    if (pPrefStream) {
        pPrefStream->Release();
        pPrefStream = 0;
    }
    return bRet;
}

// 0x10038430 ??0sgdbinfo@CSavedGameDB@@QAE@ABU01@@Z
inline CSavedGameDB::sgdbinfo::sgdbinfo(const sgdbinfo &refToCopy) : strSceneCode(strSceneCode), nIcon(nIcon)
{
}

// EQUIV 0x10038530 ?Save@CSavedGameDB@@QAE_NXZ: reviewed: the iterator sits above the interface pointers
bool CSavedGameDB::Save()
{
    IMBStream *pPrefStream = 0;
    IMBPref *pPref = 0;
    MBString strGames;
    char pszNumber[32] = { 0 };
    char pszSection[32] = { 0 };
    bool bRet = false;
    try {
        sgdbmap::iterator it = m_mapSaveData.begin();
        THROW_IF(!SteamboatNode::CreatePrefAndStream(pPrefStream, pPref, "SGDB.ini", false, false, 8));
        strGames.erase();
        while (it != m_mapSaveData.end()) {
            strGames += itoa((*it).first, pszNumber, 10);
            sprintf(pszSection, "game %d data", (*it).first);
            // a section per game: its icon and the scene it was saved in
            //
            //
            //
            //
            THROW_IF(pPref->SetString(pPrefStream, pszSection, "icon", MBString(itoa((*it).second.nIcon, pszNumber, 10))) <= 0);
            THROW_IF(pPref->SetString(pPrefStream, pszSection, "scene", (*it).second.strSceneCode) <= 0);
            if (++it != m_mapSaveData.end())
                strGames += ",";
        }
        // and the list of the used games
        //
        //
        //
        //
        //
        THROW_IF(pPref->SetString(pPrefStream, "used games", "games", strGames) <= 0);
        bRet = true;
    } catch (long nLine) {
        MBDebugStr("%s(%d) : ERROR! Failed to load Saved Game Database %s\n", "D:\\tugboat\\code\\steamboat\\SavedGameDB.cpp", nLine, "SGDB.ini");
    }
    if (pPref) {
        pPref->Release();
        pPref = 0;
    }
    if (pPrefStream) {
        pPrefStream->Release();
        pPrefStream = 0;
    }
    return bRet;
}

// 0x10038cd0 ?Delete@CSavedGameDB@@QAE_NJ@Z
bool CSavedGameDB::Delete(long nGame)
{
    sgdbmap::iterator it = m_mapSaveData.find(nGame);
    if (it != m_mapSaveData.end()) {
        m_mapSaveData.erase(it);
        return true;
    }
    return false;
}

// Template and inline instances this unit also emits:
// 0x10037510 ??_GCSavedGameDB@@UAEPAXI@Z
// 0x100384c0 ??1sgdbinfo@CSavedGameDB@@QAE@XZ
// 0x10038ea0 ??1?$pair@$$CBJUsgdbinfo@CSavedGameDB@@@std@@QAE@XZ
// 0x100390e0 ?erase@?$_Tree@JU?$pair@$$CBJUsgdbinfo@CSavedGameDB@@@std@@U_Kfn@?$map@JUsgdbinfo@CSavedGameDB@@U?$less@J@std@@V?$MBAllocator@Usgdbinfo@CSavedGameDB@@@@@2@U?$less@J@2@V?$MBAllocator@Usgdbinfo@CSavedGameDB@@@@@std@@QAE?AViterator@12@V312@@Z
// 0x10039760 ?erase@?$_Tree@JU?$pair@$$CBJUsgdbinfo@CSavedGameDB@@@std@@U_Kfn@?$map@JUsgdbinfo@CSavedGameDB@@U?$less@J@std@@V?$MBAllocator@Usgdbinfo@CSavedGameDB@@@@@2@U?$less@J@2@V?$MBAllocator@Usgdbinfo@CSavedGameDB@@@@@std@@QAE?AViterator@12@V312@0@Z
// 0x10039a10 ?insert@?$_Tree@JU?$pair@$$CBJUsgdbinfo@CSavedGameDB@@@std@@U_Kfn@?$map@JUsgdbinfo@CSavedGameDB@@U?$less@J@std@@V?$MBAllocator@Usgdbinfo@CSavedGameDB@@@@@2@U?$less@J@2@V?$MBAllocator@Usgdbinfo@CSavedGameDB@@@@@std@@QAE?AU?$pair@Viterator@?$_Tree@JU?$pair@$$CBJUsgdbinfo@CSavedGameDB@@@std@@U_Kfn@?$map@JUsgdbinfo@CSavedGameDB@@U?$less@J@std@@V?$MBAllocator@Usgdbinfo@CSavedGameDB@@@@@2@U?$less@J@2@V?$MBAllocator@Usgdbinfo@CSavedGameDB@@@@@std@@_N@2@ABU?$pair@$$CBJUsgdbinfo@CSavedGameDB@@@2@@Z
// 0x10039b90 ?_Erase@?$_Tree@JU?$pair@$$CBJUsgdbinfo@CSavedGameDB@@@std@@U_Kfn@?$map@JUsgdbinfo@CSavedGameDB@@U?$less@J@std@@V?$MBAllocator@Usgdbinfo@CSavedGameDB@@@@@2@U?$less@J@2@V?$MBAllocator@Usgdbinfo@CSavedGameDB@@@@@std@@IAEXPAU_Node@12@@Z
// 0x10039c80 ?_Inc@const_iterator@?$_Tree@JU?$pair@$$CBJUsgdbinfo@CSavedGameDB@@@std@@U_Kfn@?$map@JUsgdbinfo@CSavedGameDB@@U?$less@J@std@@V?$MBAllocator@Usgdbinfo@CSavedGameDB@@@@@2@U?$less@J@2@V?$MBAllocator@Usgdbinfo@CSavedGameDB@@@@@std@@QAEXXZ
// 0x10039d60 ?_Insert@?$_Tree@JU?$pair@$$CBJUsgdbinfo@CSavedGameDB@@@std@@U_Kfn@?$map@JUsgdbinfo@CSavedGameDB@@U?$less@J@std@@V?$MBAllocator@Usgdbinfo@CSavedGameDB@@@@@2@U?$less@J@2@V?$MBAllocator@Usgdbinfo@CSavedGameDB@@@@@std@@IAE?AViterator@12@PAU_Node@12@0ABU?$pair@$$CBJUsgdbinfo@CSavedGameDB@@@2@@Z
// 0x1003a0f0 ?_Lbound@?$_Tree@JU?$pair@$$CBJUsgdbinfo@CSavedGameDB@@@std@@U_Kfn@?$map@JUsgdbinfo@CSavedGameDB@@U?$less@J@std@@V?$MBAllocator@Usgdbinfo@CSavedGameDB@@@@@2@U?$less@J@2@V?$MBAllocator@Usgdbinfo@CSavedGameDB@@@@@std@@IBEPAU_Node@12@ABJ@Z
// 0x1003a160 ?_Ubound@?$_Tree@JU?$pair@$$CBJUsgdbinfo@CSavedGameDB@@@std@@U_Kfn@?$map@JUsgdbinfo@CSavedGameDB@@U?$less@J@std@@V?$MBAllocator@Usgdbinfo@CSavedGameDB@@@@@2@U?$less@J@2@V?$MBAllocator@Usgdbinfo@CSavedGameDB@@@@@std@@IBEPAU_Node@12@ABJ@Z
// 0x1003a1d0 ?_Dec@const_iterator@?$_Tree@JU?$pair@$$CBJUsgdbinfo@CSavedGameDB@@@std@@U_Kfn@?$map@JUsgdbinfo@CSavedGameDB@@U?$less@J@std@@V?$MBAllocator@Usgdbinfo@CSavedGameDB@@@@@2@U?$less@J@2@V?$MBAllocator@Usgdbinfo@CSavedGameDB@@@@@std@@QAEXXZ

// Template and inline instances this unit also emits:
// 0x10038f10 ?append@?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@QAEAAV12@ID@Z
// 0x100390a0 ?insert@?$map@JUsgdbinfo@CSavedGameDB@@U?$less@J@std@@V?$MBAllocator@Usgdbinfo@CSavedGameDB@@@@@std@@QAE?AU?$pair@Viterator@?$_Tree@JU?$pair@$$CBJUsgdbinfo@CSavedGameDB@@@std@@U_Kfn@?$map@JUsgdbinfo@CSavedGameDB@@U?$less@J@std@@V?$MBAllocator@Usgdbinfo@CSavedGameDB@@@@@2@U?$less@J@2@V?$MBAllocator@Usgdbinfo@CSavedGameDB@@@@@std@@_N@2@ABU?$pair@$$CBJUsgdbinfo@CSavedGameDB@@@2@@Z
// 0x100398f0 ?clear@?$_Tree@JU?$pair@$$CBJUsgdbinfo@CSavedGameDB@@@std@@U_Kfn@?$map@JUsgdbinfo@CSavedGameDB@@U?$less@J@std@@V?$MBAllocator@Usgdbinfo@CSavedGameDB@@@@@2@U?$less@J@2@V?$MBAllocator@Usgdbinfo@CSavedGameDB@@@@@std@@QAEXXZ
// 0x1003a3a0 ?remove_copy@std@@YAPADPAD00ABD@Z
// 0x10038d60 ??A?$map@JUsgdbinfo@CSavedGameDB@@U?$less@J@std@@V?$MBAllocator@Usgdbinfo@CSavedGameDB@@@@@std@@QAEAAUsgdbinfo@CSavedGameDB@@ABJ@Z
// 0x1003a2c0 ?_Construct@std@@YAXPAU?$pair@$$CBJUsgdbinfo@CSavedGameDB@@@1@ABU21@@Z

// the unit includes these after its code (the line numbers above are the original ones)
#include "SBCursorItems.h" // every node unit of the original has the cursor item strings
#include <locale> // evidence: the original unit ends with the std::ctype<unsigned short>::id and std::collate<char>::id initializers

// 0x10036e10 _$E52
// 0x10036e30 _$E49
// 0x10037400 _$E51
// 0x10037420 _$E50
// 0x1003a3f0 _$E64
// 0x1003a430 _$E60
// 0x1003a450 _$E63
