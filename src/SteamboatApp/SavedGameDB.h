#pragma once
// SavedGameDB.h: the list of saved games shown by the main menu: for each game slot the scene it
// was saved in and the icon the player picked (SavedGameDB.obj, declaration from SteamboatApp.pdb).
#include "MBContainers.h"

class CSavedGameDB {
public:
    CSavedGameDB();
    virtual ~CSavedGameDB();
    void SetGameUsed(long nGame, bool bUsed);
    bool IsGameUsed(long nGame);
    void SetGameLastSean(long nGame, const MBString &strScene);
    bool GetGameLastSean(long nGame, MBString &strSean);
    void SetGameIcon(long nGame, long nIcon);
    bool GetGameIcon(long nGame, long &nIcon);
    bool Load();
    bool Save();
    bool Delete(long nGame);
    struct sgdbinfo {
        // initializes the members from themselves, not from refToCopy (as the original does); defined
        // inline in SavedGameDB.cpp: the map instances inline it, other callers use its copy
        sgdbinfo(const sgdbinfo &refToCopy);
        sgdbinfo() : nIcon(-1) {}
        MBString strSceneCode;                          // +0x0000
        long nIcon;                                     // +0x0010
    };
protected:
    sgdbinfo &AddDefaultEntry(long nGame);
    sgdbinfo &GetEntry(long nGame);
    typedef MBMap<long, sgdbinfo, std::less<long> > sgdbmap;
private:
    sgdbmap m_mapSaveData;                              // +0x0004
};
