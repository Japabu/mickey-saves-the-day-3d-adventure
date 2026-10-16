#pragma once
// CSBGameState.h: the saved state of a game in progress (player, inventory and recycle items, the
// scenes visited, the sandwich level reached and free script properties). Declaration from
// SteamboatApp.pdb.
#include "SBInterfaces.h"
#include "SBItem.h"

class MfAiActor;

IMBUnknown *MakeCSBGameState();

class CSBGameState : public ISBGameState {
public:
    CSBGameState();
protected:
    ~CSBGameState();
    unsigned long _mRefCnt;                             // +0x0004
public:
    virtual long QueryInterface(const MBGUID &iid, void **obj);
    virtual unsigned long AddRef();
    virtual unsigned long Release();
    virtual void ListenToEvent(long channel, long event, void *data);
    virtual void SetName(const MBString &name);
    virtual const MBString &GetName();
    virtual void LoadInventoryItemFromPref(MBComPtr<IMBStream> stream, MBComPtr<IMBPref> pref);
    virtual void LoadRecycleItemsFromPref(MBComPtr<IMBStream> stream, MBComPtr<IMBPref> pref);
    virtual void AddItemToRecycleBin();
    virtual int GetNumberItemsInRecycleBin();
    virtual int GetNumberOfRecycleItems();
    virtual int GetNumberOfPossibleLocationsForRecycleItems();
    virtual void SetHighestSandwichLevel(int level);
    virtual int GetHighestSandwichLevel();
    virtual void VisitScene(MBString &code);
    virtual void UnvisitScene(MBString &code);
    virtual bool HasVisitedScene(MBString &code);
    virtual void ReadFromFile(MBString &prefFile);
    virtual void Load(IMBStream *pPrefStream, IMBPref *pPref, MfAiActor *player);
    virtual void Save(IMBStream *pPrefStream, IMBPref *pPref, bool bExit);
    virtual void ClearMaps();
    virtual void ResetItems();
    virtual void RandomizeRecycleItems(int seed, int nPossibleInstances, int nPossibleLocations);
    virtual SBItem *GetItem(const MBString &itemInstanceName);
    virtual void SetProperty(const char *name, long value);
    virtual void SetProperty(const char *name, const char *value);
    virtual long GetProperty(const char *name, const long defaultValue);
    virtual MBString GetProperty(const char *name, const MBString &defaultValue);
    virtual void SetPrimaryActor(MfAiActor *player);
    virtual void SetPrimaryActor(const MBString &player);
    virtual MfAiActor *GetPrimaryActor();
    virtual void EnableItemHotspots(MBString spaceCode, bool enable);
    enum ItemType {
        cInventory = 0,
        cRecycle = 1
    };
    typedef MBMap<MBString, SBItem *, std::less<MBString> > SBNameToItemMap;
protected:
    bool LoadItemFromPref(MBString section, SBItem *sbitem, MBComPtr<IMBStream> stream, MBComPtr<IMBPref> pref);
    MfAiActor *mPlayer;                                 // +0x0008
    // the name and type of a script property the state saves
    struct propinfo_pair {
        propinfo_pair(const char *name, const char *type) : strName(name), strType(type) {}
        MBString strName;
        MBString strType;
    };
private:
    MBList<propinfo_pair> m_listPropInfo;               // +0x000c
    MBString mName;                                     // +0x0018
    MBString mCharacter;                                // +0x0028
    MBString mSpaceCode;                                // +0x0038
    MBMultimap<int, SBItem *, std::less<int> > mLocationMap; // +0x0048
    SBNameToItemMap mInstanceNameToItemMap;             // +0x0058
    MBVector<MBString> mRecycleNames;                   // +0x0068
    MBVector<SBItem *> m_vecPossibleLocations;          // +0x0078
    int mSeed;                                          // +0x0088
    int mPossibleLocations;                             // +0x008c
    int mNumInstances;                                  // +0x0090
    int mHighestSandwichLevel;                          // +0x0094
    int mItemsInRecycleBin;                             // +0x0098
    MBString mVisitedScenes;                            // +0x009c
    char mAssertString[80];                             // +0x00ac
};
