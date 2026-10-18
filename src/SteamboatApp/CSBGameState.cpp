#include "CSBGameState.h"
#include "SBCursorItems.h" // every node unit of the original has the cursor item strings
#include <locale> // evidence: the original unit ends with the std::ctype<unsigned short>::id and std::collate<char>::id initializers
#include "SBRecycleItem.h"
#include "SBMotivateDriver.h"
#include "MfIntValue.h"
#include "MfStringValue.h"
#include "MfProperty.h"
#include "MBRuntime.h"
#include "SBInventoryItem.h"
#include <stdio.h>
#include <stdlib.h>

// 0x1001e710 ?MakeCSBGameState@@YAPAVIMBUnknown@@XZ
IMBUnknown *MakeCSBGameState()
{
    static IMBUnknown *instance = MBComUtilAddSingleton(new CSBGameState);
    if (instance)
        instance->AddRef();
    return instance;
}

// 0x1001e7d0 ??0CSBGameState@@QAE@XZ
CSBGameState::CSBGameState()
    : mPlayer(0), mSeed(0), mPossibleLocations(0), mNumInstances(0), mHighestSandwichLevel(1),
      mItemsInRecycleBin(0)
{
    _mRefCnt = 0;
    MBComPtr<IMBEventManager> eventManager(MBIID_IMBEventManager);
    if (eventManager)
        eventManager->AddListener(300, this);
}

// 0x1001e9a0 ?QueryInterface@CSBGameState@@UAEJABUMBGUID@@PAPAX@Z
long CSBGameState::QueryInterface(const MBGUID &iid, void **obj)
{
    if (!obj)
        return -2;
    *obj = 0;
    IMBUnknown *p = 0;
    if (iid == MBIID_IMBUnknown)
        p = (IMBUnknown *)this;
    if (iid == MBIID_IMBEventListener)
        p = (IMBEventListener *)this;
    if (iid == MBIID_ISBGameState)
        p = (ISBGameState *)this;
    if (p) {
        p->AddRef();
        *obj = p;
        return 0;
    }
    return -3;
}

// 0x1001eae0 ?AddRef@CSBGameState@@UAEKXZ
unsigned long CSBGameState::AddRef()
{
    return ++_mRefCnt;
}

// 0x1001eb00 ?Release@CSBGameState@@UAEKXZ
unsigned long CSBGameState::Release()
{
    unsigned long r = --_mRefCnt;
    if (!_mRefCnt)
        delete this;
    return r;
}

// 0x1001f070 ??1CSBGameState@@IAE@XZ
CSBGameState::~CSBGameState()
{
    ClearMaps();
    MBComPtr<IMBEventManager> eventManager(MBIID_IMBEventManager);
    if (eventManager)
        eventManager->RemoveListener(300, this);
}

// 0x1001f370 ?ListenToEvent@CSBGameState@@UAEXJJPAX@Z
void CSBGameState::ListenToEvent(long channel, long event, void *data)
{
    // the scene that is starting tells its space code
    if (channel == 300 && event == 0x132)
        mSpaceCode = (const char *)data;
}

// 0x1001eb40 ?SetName@CSBGameState@@UAEXABV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@Z
void CSBGameState::SetName(const MBString &name)
{
    mName = name;
}

// 0x1001ecd0 ?GetName@CSBGameState@@UAEABV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@XZ
const std::basic_string<char,std::char_traits<char>,MBAllocator<char> > &CSBGameState::GetName()
{
    return mName;
}

// 0x1001ecf0 ?AddItemToRecycleBin@CSBGameState@@UAEXXZ
void CSBGameState::AddItemToRecycleBin()
{
    mItemsInRecycleBin++;
}

// 0x1001ed10 ?GetNumberItemsInRecycleBin@CSBGameState@@UAEHXZ
int CSBGameState::GetNumberItemsInRecycleBin()
{
    return mItemsInRecycleBin;
}

// 0x1001ed30 ?GetNumberOfRecycleItems@CSBGameState@@UAEHXZ
int CSBGameState::GetNumberOfRecycleItems()
{
    return mNumInstances;
}

// 0x1001ed50 ?GetNumberOfPossibleLocationsForRecycleItems@CSBGameState@@UAEHXZ
int CSBGameState::GetNumberOfPossibleLocationsForRecycleItems()
{
    return mPossibleLocations;
}

// 0x1001ed70 ?SetHighestSandwichLevel@CSBGameState@@UAEXH@Z
void CSBGameState::SetHighestSandwichLevel(int level)
{
    if (level > mHighestSandwichLevel)
        mHighestSandwichLevel = level;
}

// 0x1001f2e0 ?ResetItems@CSBGameState@@UAEXXZ
void CSBGameState::ResetItems()
{
    // inventory items start in their scenes, recycle items wait for RandomizeRecycleItems
    for (SBNameToItemMap::iterator it = mInstanceNameToItemMap.begin(); it != mInstanceNameToItemMap.end(); ++it) {
        if (dynamic_cast<SBRecycleItem *>((*it).second))
            (*it).second->SetInScene(false);
        else
            (*it).second->SetInScene(true);
        (*it).second->SetOnCharacter(false);
    }
}

// 0x1001eda0 ?GetHighestSandwichLevel@CSBGameState@@UAEHXZ
int CSBGameState::GetHighestSandwichLevel()
{
    return mHighestSandwichLevel;
}

// 0x10020b00 ?RandomizeRecycleItems@CSBGameState@@UAEXHHH@Z
void CSBGameState::RandomizeRecycleItems(int seed, int nPossibleInstances, int nPossibleLocations)
{
    int nToPlace = nPossibleInstances;
    for (MBVector<SBItem *>::iterator it = m_vecPossibleLocations.begin(); it != m_vecPossibleLocations.end(); it++)
        (*it)->SetInScene(false);
    srand(seed);
    while (nToPlace > 0) {
        int location = rand() % nPossibleLocations;
        if (!m_vecPossibleLocations[location]->GetInScene()) {
            m_vecPossibleLocations[location]->SetInScene(true);
            nToPlace--;
        }
    }
}

// TODO 0x1001f480 ?LoadItemFromPref@CSBGameState@@IAE_NV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVSBItem@@V?$MBComPtr@VIMBStream@@@@V?$MBComPtr@VIMBPref@@@@@Z: (86% similar) register choice: the original keeps the
// constant 0 in esi and nLocations on the stack, VC6 here gives esi to nLocations; it also inlines
// section's destructor on the false path only (67% similar)
bool CSBGameState::LoadItemFromPref(MBString section, SBItem *sbitem, MBComPtr<IMBStream> stream, MBComPtr<IMBPref> pref)
{
    // one item section: its name, instance, the places it may appear and its hotspot polygon
    MBString name;
    MBString spaceCode;
    MBString giveTo;
    if (pref->GetString(stream, MBString(section), MBString("name"), name, MBString(""))) {
        int instance = pref->GetLong(stream, MBString(section), MBString("Instance"), 0);
        int nLocations = pref->GetLong(stream, MBString(section), MBString("numLocations"), 1);
        int location = pref->GetLong(stream, MBString(section), MBString("Location"), 0);
        int inScene = pref->GetLong(stream, MBString(section), MBString("InScene"), 1);
        pref->GetString(stream, MBString(section), MBString("spacecode"), spaceCode, MBString(""));
        pref->GetString(stream, MBString(section), MBString("giveto"), giveTo, MBString(""));

        // the item appears at one of its locations, chosen at random
        location = rand() % nLocations;
        sbitem->SetInstance(instance);
        sbitem->SetLocation(location);
        sbitem->SetInScene(inScene > 0);
        char instanceId[8];
        sprintf(instanceId, "%d", instance);
        sbitem->SetInstanceName(name + instanceId);
        sbitem->SetClassName(name);
        sbitem->SetSpaceCode(spaceCode);
        sbitem->SetGiveTo(giveTo);

        MBComPtr<IMBScreenPoly> poly(MBIID_CMBScreenPoly);
        if (poly->LoadFromStream(stream, pref, section.c_str()) == 0)
            sbitem->SetHotspot(poly);
        mLocationMap.insert(MBMultimap<int, SBItem *, std::less<int> >::value_type(location, sbitem));
        mInstanceNameToItemMap.insert(std::pair<MBString, SBItem *>(name + instanceId, sbitem));
        return true;
    }
    return false;
}

// 0x10020210 ?LoadInventoryItemFromPref@CSBGameState@@UAEXV?$MBComPtr@VIMBStream@@@@V?$MBComPtr@VIMBPref@@@@@Z
void CSBGameState::LoadInventoryItemFromPref(MBComPtr<IMBStream> stream, MBComPtr<IMBPref> pref)
{
    // the sections InventoryItem0, InventoryItem1, ... up to the first one missing
    int i = 0;
    char section[80];
    sprintf(section, "InventoryItem%d", i);
    SBItem *item = new SBInventoryItem;
    while (LoadItemFromPref(section, item, stream, pref)) {
        item = new SBInventoryItem;
        i++;
        sprintf(section, "InventoryItem%d", i);
    }
    delete item;
}

// 0x10020480 ?LoadRecycleItemsFromPref@CSBGameState@@UAEXV?$MBComPtr@VIMBStream@@@@V?$MBComPtr@VIMBPref@@@@@Z
void CSBGameState::LoadRecycleItemsFromPref(MBComPtr<IMBStream> stream, MBComPtr<IMBPref> pref)
{
    // every RecycleItem<n> section is a place where a recycle item may appear
    int i = 0;
    char section[80];
    sprintf(section, "RecycleItem%d", i);
    SBItem *item = new SBRecycleItem;
    while (LoadItemFromPref(section, item, stream, pref)) {
        m_vecPossibleLocations.push_back(item);
        mPossibleLocations++;
        item = new SBRecycleItem;
        i++;
        sprintf(section, "RecycleItem%d", i);
    }
    delete item;

    mNumInstances = pref->GetLong(stream, MBString("RecycleItems"), MBString("numInstances"), 0);
    mSeed = pref->GetLong(stream, MBString("RecycleItems"), MBString("seed"), 0);
    if (mSeed == 0) {
        MBComPtr<IMBGameClock> clock(MBIID_IMBGameClock);
        mSeed = clock->GetRealTime();
    }
    RandomizeRecycleItems(mSeed, mNumInstances, mPossibleLocations);
}

// TODO 0x10020b80 ?ReadFromFile@CSBGameState@@UAEXAAV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@Z: (89% similar) the original calls fileSpec's
// CreateObject out of line and inlines pref's; VC6 inlines both here (39% similar)
void CSBGameState::ReadFromFile(MBString &prefFile)
{
    MBComPtr<IMBFileSpec> fileSpec(MBIID_IMBFileSpec);
    if (fileSpec) {
        fileSpec->SetPath(prefFile.c_str());
        IMBStream *stream;
        MBOpenFile(fileSpec, 5, &stream);
        if (stream) {
            MBComPtr<IMBPref> pref(MBIID_IMBPref);
            LoadInventoryItemFromPref(stream, pref);
            LoadRecycleItemsFromPref(stream, pref);
        }
    }
}

// 0x10020d50 ?GetItem@CSBGameState@@UAEPAVSBItem@@ABV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@Z
SBItem *CSBGameState::GetItem(const MBString &itemInstanceName)
{
    SBNameToItemMap::iterator it = mInstanceNameToItemMap.find(itemInstanceName);
    if (it == mInstanceNameToItemMap.end())
        return 0;
    return (*it).second;
}

// 0x10020dd0 ?ClearMaps@CSBGameState@@UAEXXZ
void CSBGameState::ClearMaps()
{
    for (SBNameToItemMap::iterator it = mInstanceNameToItemMap.begin(); it != mInstanceNameToItemMap.end(); ++it)
        delete mInstanceNameToItemMap[(*it).first];
    mInstanceNameToItemMap.clear();
    mLocationMap.clear();
    m_vecPossibleLocations.clear();
}

// EQUIV 0x10024480 ?VisitScene@CSBGameState@@UAEXAAV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@Z: reviewed: the two lengths of the inlined += are added in the other order, the string-release tails are laid out differently and the original keeps three dead stores into a temporary that is not read again
// _Len + n where VC6 gives n + _Len, and the original keeps the dead stores of the first temporary's
// _Tidy (92% similar)
void CSBGameState::VisitScene(MBString &code)
{
    // mVisitedScenes is a list of space codes, each followed by a comma
    if (!HasVisitedScene(code)) {
        MBString separator(",");
        mVisitedScenes += code + separator;
    }
}

// 0x10024660 ?UnvisitScene@CSBGameState@@UAEXAAV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@Z
void CSBGameState::UnvisitScene(MBString &code)
{
    // a space code and its comma
    int pos = mVisitedScenes.find(code);
    if (pos >= 0 && pos < mVisitedScenes.length())
        mVisitedScenes.erase(pos, 3);
}

// 0x100248d0 ?HasVisitedScene@CSBGameState@@UAE_NAAV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@Z
bool CSBGameState::HasVisitedScene(MBString &code)
{
    int pos = mVisitedScenes.find(code);
    return pos >= 0 && pos < mVisitedScenes.length();
}

// TODO 0x10024ad0 ?EnableItemHotspots@CSBGameState@@UAEXV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@_N@Z: (50% similar) the original calls
// basic_string::compare(pos, n, s, len) out of line and inlines the iterator ++ (_Lockit), VC6 does the
// opposite here (34% similar)
void CSBGameState::EnableItemHotspots(MBString spaceCode, bool enable)
{
    for (MBMultimap<int, SBItem *, std::less<int> >::iterator it = mLocationMap.begin(); it != mLocationMap.end(); ++it) {
        SBItem *item = (*it).second;
        if (item->GetSpaceCode() == spaceCode && item->GetHotspot()) {
            if (enable)
                item->GetHotspot()->Activate();
            else
                item->GetHotspot()->Deactivate();
        }
    }
}

// 0x10021050 ?SetProperty@CSBGameState@@UAEXPBDJ@Z
void CSBGameState::SetProperty(const char *name, long value)
{
    if (mPlayer) {
        MfIntValue intValue(value);
        MfProperty prop(name, intValue.Copy());
        mPlayer->SetProperty(prop);
    }
}

// 0x10020ee0 ?SetProperty@CSBGameState@@UAEXPBD0@Z
void CSBGameState::SetProperty(const char *name, const char *value)
{
    if (mPlayer) {
        MfStringValue stringValue(value);
        MfProperty prop(name, stringValue.Copy());
        mPlayer->SetProperty(prop);
    }
}

// 0x10021160 ?GetProperty@CSBGameState@@UAEJPBDJ@Z
long CSBGameState::GetProperty(const char *name, const long defaultValue)
{
    if (mPlayer) {
        MfProperty prop;
        int found;
        {
            MfString propName(name);
            found = mPlayer->GetProperty(propName, prop);
        }
        if (found)
            return (int)prop;
    }
    MBDebugStr("(WARNING) property '%s' not found on player; defaulting to '%d'\n", name, defaultValue);
    return defaultValue;
}

// 0x10021270 ?GetProperty@CSBGameState@@UAE?AV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PBDABV23@@Z
MBString CSBGameState::GetProperty(const char *name, const MBString &defaultValue)
{
    if (mPlayer) {
        MfProperty prop;
        int found;
        {
            MfString propName(name);
            found = mPlayer->GetProperty(propName, prop);
        }
        if (found) {
            MfString value;
            if (prop.ConvertToString(value))
                return (char *)value;
        }
    }
    MBDebugStr("(WARNING) property '%s' not found on player; defaulting to '%s'\n", name, defaultValue.c_str());
    return defaultValue;
}

// TODO 0x10021530 ?Load@CSBGameState@@UAEXPAVIMBStream@@PAVIMBPref@@PAVMfAiActor@@@Z: (93% similar) inline budget and stack slots: the
// original calls assign() for the "location" key and _Eos() in every later inlined one, and calls
// _Tidy() for the last temporaries; it keeps this at esp+0x18 and the iterator at esp+0x1c (78% similar)
void CSBGameState::Load(IMBStream *pPrefStream, IMBPref *pPref, MfAiActor *player)
{
    MBString strPropName;
    MBString strInstanceName;
    MBString strPropType;
    MBString strPropValue;
    char pszKey[32];
    int nProp = 1;

    if (player == 0) {
        MBString playerName;
        pPref->GetString(pPrefStream, "game", "player", playerName, "GDmickey0");
        SetPrimaryActor(playerName);
    } else
        SetPrimaryActor(player);

    // the script properties: sections property1, property2, ... with name, type and value
    m_listPropInfo.clear();
    sprintf(pszKey, "property%d", nProp);
    while (pPref->GetString(pPrefStream, pszKey, "name", strPropName, "")) {
        if (pPref->GetString(pPrefStream, pszKey, "type", strPropType, "")) {
            if (pPref->GetString(pPrefStream, pszKey, "value", strPropValue, "")) {
                m_listPropInfo.push_back(propinfo_pair(strPropName.c_str(), strPropType.c_str()));
                if (strPropType == "string")
                    SetProperty(strPropName.c_str(), strPropValue.c_str());
                else if (strPropType == "integer")
                    SetProperty(strPropName.c_str(), atoi(strPropValue.c_str()));
            }
        }
        nProp++;
        sprintf(pszKey, "property%d", nProp);
    }

    // the items: sections sbitem1, sbitem2, ...
    MBString strValue;
    nProp = 1;
    sprintf(pszKey, "sbitem%d", nProp);
    while (pPref->GetString(pPrefStream, pszKey, "instance name", strInstanceName, "")) {
        SBNameToItemMap::iterator it = mInstanceNameToItemMap.find(strInstanceName);
        if (mInstanceNameToItemMap.end() != it) {
            if (pPref->GetString(pPrefStream, pszKey, "class name", strValue, ""))
                (*it).second->SetClassName(strValue);
            if (pPref->GetString(pPrefStream, pszKey, "give to", strValue, ""))
                (*it).second->SetGiveTo(strValue);
            if (pPref->GetString(pPrefStream, pszKey, "space code", strValue, ""))
                (*it).second->SetSpaceCode(strValue);
            long value = pPref->GetLong(pPrefStream, pszKey, "instance", 0);
            (*it).second->SetInstance(value);
            value = pPref->GetLong(pPrefStream, pszKey, "location", 0);
            (*it).second->SetLocation(value);
            value = pPref->GetLong(pPrefStream, pszKey, "in scene", 0);
            (*it).second->SetInScene(value == 1);
            value = pPref->GetLong(pPrefStream, pszKey, "on character", 0);
            (*it).second->SetOnCharacter(value == 1);
        }
        nProp++;
        sprintf(pszKey, "sbitem%d", nProp);
    }

    long seed = pPref->GetLong(pPrefStream, "RecycleItems", "seed", 0);
    if (seed) {
        mSeed = seed;
        RandomizeRecycleItems(mSeed, mNumInstances, mPossibleLocations);
    }
    pPref->GetString(pPrefStream, "history", "visited", mVisitedScenes, "");
    MBDebugStr("(INFO) Loaded %d game-state properties\n", nProp - 1);

    if (mPlayer && mPlayer->GetInstanceName() == "GDmickey0")
        SetProperty("HCMickeyCaptive", 0L);
    else
        SetProperty("HCMickeyCaptive", 1L);
}

// TODO 0x10023030 ?Save@CSBGameState@@UAEXPAVIMBStream@@PAVIMBPref@@_N@Z: (90% similar) stack slots differ (this, the list
// iterator, the MfString), and the original builds strTemp as default construction plus assign
// without our zero register (81% similar)
void CSBGameState::Save(IMBStream *pPrefStream, IMBPref *pPref, bool bExit)
{
    if (mPlayer) {
        MBString playerName = (char *)mPlayer->GetInstanceName();
        pPref->SetString(pPrefStream, "game", "player", playerName);
        MBString name;
        int nProp = 0;
        char pszKey[32];

        // the script properties the player has, numbered from 1
        MBList<propinfo_pair>::iterator i = m_listPropInfo.begin();
        while (i != m_listPropInfo.end()) {
            MfProperty prop;
            int found;
            {
                MfString propName((*i).strName.c_str());
                found = mPlayer->GetProperty(propName, prop);
            }
            if (found) {
                MfString strValue;
                prop.ConvertToString(strValue);
                MBString strPropValue = (const char *)strValue;
                sprintf(pszKey, "property%d", ++nProp);
                pPref->SetString(pPrefStream, pszKey, "name", (*i).strName);
                pPref->SetString(pPrefStream, pszKey, "value", strPropValue);
                pPref->SetString(pPrefStream, pszKey, "type", (*i).strType);
            }
            i++;
        }

        // every item
        nProp = 0;
        SBNameToItemMap::iterator iNameMap = mInstanceNameToItemMap.begin();
        while (iNameMap != mInstanceNameToItemMap.end()) {
            sprintf(pszKey, "sbitem%d", ++nProp);
            MBString strTemp = (*iNameMap).second->GetInstanceName();
            pPref->SetString(pPrefStream, pszKey, "instance name", strTemp);
            strTemp = (*iNameMap).second->GetClassNameA();
            pPref->SetString(pPrefStream, pszKey, "class name", strTemp);
            strTemp = (*iNameMap).second->GetGiveTo();
            pPref->SetString(pPrefStream, pszKey, "give to", strTemp);
            strTemp = (*iNameMap).second->GetSpaceCode();
            pPref->SetString(pPrefStream, pszKey, "space code", strTemp);
            pPref->SetLong(pPrefStream, pszKey, "instance", (*iNameMap).second->GetInstance());
            pPref->SetLong(pPrefStream, pszKey, "location", (*iNameMap).second->GetLocation());
            pPref->SetLong(pPrefStream, pszKey, "in scene", (*iNameMap).second->GetInScene());
            pPref->SetLong(pPrefStream, pszKey, "on character", (*iNameMap).second->GetOnCharacter());
            iNameMap++;
        }

        pPref->SetLong(pPrefStream, "RecycleItems", "seed", mSeed);
        pPref->SetString(pPrefStream, "history", "visited", mVisitedScenes);
        MBDebugStr("(INFO) Saved %d game-state properties\n", nProp);
        if (bExit)
            mPlayer = 0;
    }
}

// 0x10024a10 ?SetPrimaryActor@CSBGameState@@UAEXABV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@Z
void CSBGameState::SetPrimaryActor(const MBString &player)
{
    MfAiSpace *space = SBMotivateDriver::GetMotivateSpace();
    if (space) {
        MfString name(player.c_str());
        mPlayer = space->GetActor(name);
    }
}

// 0x100249f0 ?SetPrimaryActor@CSBGameState@@UAEXPAVMfAiActor@@@Z
void CSBGameState::SetPrimaryActor(MfAiActor *player)
{
    mPlayer = player;
}

// 0x10024ab0 ?GetPrimaryActor@CSBGameState@@UAEPAVMfAiActor@@XZ
MfAiActor *CSBGameState::GetPrimaryActor()
{
    return mPlayer;
}

// Template and inline instances this unit also emits:
// 0x10008630 ?assign@?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@QAEAAV12@PBDI@Z
// 0x10010220 ?compare@?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@QBEHIIPBDI@Z
// 0x10014670 ?CreateObject@?$MBComPtr@VIMBEventManager@@@@QAEJABUMBGUID@@@Z
// 0x1001edc0 ??1?$MBList@Upropinfo_pair@CSBGameState@@@@QAE@XZ
// 0x1001ee50 ??1?$MBMultimap@HPAVSBItem@@U?$less@H@std@@@@QAE@XZ
// 0x1001eef0 ??1?$MBMap@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVSBItem@@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@@@QAE@XZ
// 0x1001ef90 ??1?$MBVector@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@@QAE@XZ
// 0x1001f020 ??1?$MBVector@PAVSBItem@@@@QAE@XZ
// 0x100201a0 ??1?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVSBItem@@@std@@QAE@XZ
// 0x10020980 ??1SBItem@@QAE@XZ
// 0x10022f90 ??1propinfo_pair@CSBGameState@@QAE@XZ
// 0x10025190 ??Eiterator@?$list@Upropinfo_pair@CSBGameState@@V?$MBAllocator@Upropinfo_pair@CSBGameState@@@@@std@@QAE?AV012@H@Z
// 0x100251b0 ??1?$_Tree@HU?$pair@$$CBHPAVSBItem@@@std@@U_Kfn@?$multimap@HPAVSBItem@@U?$less@H@std@@V?$MBAllocator@PAVSBItem@@@@@2@U?$less@H@2@V?$MBAllocator@PAVSBItem@@@@@std@@QAE@XZ
// 0x10025250 ??A?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVSBItem@@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAVSBItem@@@@@std@@QAEAAPAVSBItem@@ABV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@1@@Z
// 0x10025330 ??1?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVSBItem@@@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVSBItem@@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAVSBItem@@@@@2@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAVSBItem@@@@@std@@QAE@XZ
// 0x10025500 ?_Destroy@?$vector@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@V?$MBAllocator@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@@@std@@IAEXPAV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@2@0@Z
// 0x10025680 ?_Buynode@?$list@Upropinfo_pair@CSBGameState@@V?$MBAllocator@Upropinfo_pair@CSBGameState@@@@@std@@IAEPAU_Node@12@PAU312@0@Z
// 0x100256c0 ??0?$_Tree@HU?$pair@$$CBHPAVSBItem@@@std@@U_Kfn@?$multimap@HPAVSBItem@@U?$less@H@std@@V?$MBAllocator@PAVSBItem@@@@@2@U?$less@H@2@V?$MBAllocator@PAVSBItem@@@@@std@@QAE@ABU?$less@H@1@_NABV?$MBAllocator@PAVSBItem@@@@@Z
// 0x10025880 ?erase@?$_Tree@HU?$pair@$$CBHPAVSBItem@@@std@@U_Kfn@?$multimap@HPAVSBItem@@U?$less@H@std@@V?$MBAllocator@PAVSBItem@@@@@2@U?$less@H@2@V?$MBAllocator@PAVSBItem@@@@@std@@QAE?AViterator@12@V312@0@Z
// 0x10025a00 ??0?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVSBItem@@@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVSBItem@@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAVSBItem@@@@@2@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAVSBItem@@@@@std@@QAE@ABU?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@1@_NABV?$MBAllocator@PAVSBItem@@@@@Z
// 0x10025a60 ?insert@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVSBItem@@@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVSBItem@@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAVSBItem@@@@@2@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAVSBItem@@@@@std@@QAE?AU?$pair@Viterator@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVSBItem@@@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVSBItem@@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAVSBItem@@@@@2@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAVSBItem@@@@@std@@_N@2@ABU?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVSBItem@@@2@@Z
// 0x10025c30 ?erase@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVSBItem@@@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVSBItem@@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAVSBItem@@@@@2@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAVSBItem@@@@@std@@QAE?AViterator@12@V312@0@Z
// 0x10025e80 ?_Inc@const_iterator@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVSBItem@@@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVSBItem@@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAVSBItem@@@@@2@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAVSBItem@@@@@std@@QAEXXZ
// 0x100261b0 ?_Destroy@?$vector@PAVSBItem@@V?$MBAllocator@PAVSBItem@@@@@std@@IAEXPAPAVSBItem@@0@Z
// 0x100261d0 ??0?$pair@Viterator@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVSBItem@@@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVSBItem@@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAVSBItem@@@@@2@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAVSBItem@@@@@std@@_N@std@@QAE@ABViterator@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVSBItem@@@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVSBItem@@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAVSBItem@@@@@2@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAVSBItem@@@@@1@AB_N@Z
// 0x100262a0 ?erase@?$list@Upropinfo_pair@CSBGameState@@V?$MBAllocator@Upropinfo_pair@CSBGameState@@@@@std@@QAE?AViterator@12@V312@@Z
// 0x10026310 ?erase@?$_Tree@HU?$pair@$$CBHPAVSBItem@@@std@@U_Kfn@?$multimap@HPAVSBItem@@U?$less@H@std@@V?$MBAllocator@PAVSBItem@@@@@2@U?$less@H@2@V?$MBAllocator@PAVSBItem@@@@@std@@QAE?AViterator@12@V312@@Z
// 0x10026a10 ?_Init@?$_Tree@HU?$pair@$$CBHPAVSBItem@@@std@@U_Kfn@?$multimap@HPAVSBItem@@U?$less@H@std@@V?$MBAllocator@PAVSBItem@@@@@2@U?$less@H@2@V?$MBAllocator@PAVSBItem@@@@@std@@IAEXXZ
// 0x10026e40 ?_Inc@const_iterator@?$_Tree@HU?$pair@$$CBHPAVSBItem@@@std@@U_Kfn@?$multimap@HPAVSBItem@@U?$less@H@std@@V?$MBAllocator@PAVSBItem@@@@@2@U?$less@H@2@V?$MBAllocator@PAVSBItem@@@@@std@@QAEXXZ
// 0x10026f20 ?erase@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVSBItem@@@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVSBItem@@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAVSBItem@@@@@2@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAVSBItem@@@@@std@@QAE?AViterator@12@V312@@Z
// 0x10027690 ?_Init@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVSBItem@@@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVSBItem@@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAVSBItem@@@@@2@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAVSBItem@@@@@std@@IAEXXZ
// 0x10027730 ?_Insert@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVSBItem@@@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVSBItem@@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAVSBItem@@@@@2@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAVSBItem@@@@@std@@IAE?AViterator@12@PAU_Node@12@0ABU?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVSBItem@@@2@@Z
// 0x10027bb0 ?_Lbound@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVSBItem@@@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVSBItem@@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAVSBItem@@@@@2@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAVSBItem@@@@@std@@IBEPAU_Node@12@ABV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@2@@Z
// 0x10027ca0 ?_Dec@const_iterator@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVSBItem@@@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVSBItem@@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAVSBItem@@@@@2@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAVSBItem@@@@@std@@QAEXXZ
// 0x10027fc0 ??Hstd@@YA?AV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@0@ABV10@0@Z
// 0x100283d0 ?_Construct@std@@YAXPAU?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVSBItem@@@1@ABU21@@Z
// 0x100285b0 ?compare@?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@QBEHABV12@@Z
// Inline members of the Motivate value classes:
// 0x10024c60 ??1MfTypedValue@@UAE@XZ
// 0x10024c80 ?Compare@MfTypedValue@@UAEHABV1@@Z
// 0x10024ca0 ?ConvertTo@MfTypedValue@@UBEPAV1@J@Z
// 0x10024cc0 ??_GMfTypedValue@@UAEPAXI@Z
// 0x10024d00 ?Copy@MfIntValue@@UBEPAVMfTypedValue@@XZ
// 0x10024d40 ?Destroy@MfIntValue@@UAEXXZ
// 0x10024d60 ??_GMfIntValue@@UAEPAXI@Z
// 0x10024da0 ??1MfIntValue@@UAE@XZ
// 0x10024dc0 ?Copy@MfStringValue@@UBEPAVMfTypedValue@@XZ
// 0x10024e50 ?Destroy@MfStringValue@@UAEXXZ
// 0x10024e70 ??_GMfStringValue@@UAEPAXI@Z
// 0x10024eb0 ??1MfStringValue@@UAE@XZ

// Template and inline instances this unit also emits:
// 0x10008400 ??0?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@QAE@PBDABV?$MBAllocator@D@@@Z
// 0x10025090 ??0?$MBComPtr@VIMBStream@@@@QAE@ABV0@@Z
// 0x100250c0 ??0?$MBComPtr@VIMBStream@@@@QAE@PAVIMBStream@@@Z
// 0x10025140 ??1?$MBComPtr@VIMBPref@@@@QAE@XZ
// 0x10025400 ?Release@?$MBComPtr@VIMBScreenPoly@@@@QAEXXZ
// 0x10025f60 ?insert@?$vector@PAVSBItem@@V?$MBAllocator@PAVSBItem@@@@@std@@QAEPAPAVSBItem@@PAPAV3@ABQAV3@@Z
// 0x10026950 ?_Erase@?$_Tree@HU?$pair@$$CBHPAVSBItem@@@std@@U_Kfn@?$multimap@HPAVSBItem@@U?$less@H@std@@V?$MBAllocator@PAVSBItem@@@@@2@U?$less@H@2@V?$MBAllocator@PAVSBItem@@@@@std@@IAEXPAU_Node@12@@Z
// 0x100275a0 ?_Erase@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVSBItem@@@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVSBItem@@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAVSBItem@@@@@2@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAVSBItem@@@@@std@@IAEXPAU_Node@12@@Z
// 0x10027d90 ?size@?$vector@PAVSBItem@@V?$MBAllocator@PAVSBItem@@@@@std@@QBEIXZ
// 0x10027dc0 ?_Ucopy@?$vector@PAVSBItem@@V?$MBAllocator@PAVSBItem@@@@@std@@IAEPAPAVSBItem@@PBQAV3@0PAPAV3@@Z
// 0x10027e00 ?_Ufill@?$vector@PAVSBItem@@V?$MBAllocator@PAVSBItem@@@@@std@@IAEXPAPAVSBItem@@IABQAV3@@Z
// 0x10028150 ?_QueryInterface@?$MBComPtr@VIMBPref@@@@AAEJABQAVIMBUnknown@@@Z
// 0x10028580 ?_Construct@std@@YAXPAPAVSBItem@@ABQAV2@@Z
// 0x10069290 ?Release@?$MBComPtr@VIMBFileSpec@@@@QAEXXZ
// 0x100692c0 ?_QueryInterface@?$MBComPtr@VIMBFileSpec@@@@AAEJABQAVIMBUnknown@@@Z
// 0x10020130 ??1?$pair@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVSBItem@@@std@@QAE@XZ
// 0x10025120 ??C?$MBComPtr@VIMBScreenPoly@@@@QBEPAVIMBScreenPoly@@XZ
// 0x10025170 ?begin@?$list@Upropinfo_pair@CSBGameState@@V?$MBAllocator@Upropinfo_pair@CSBGameState@@@@@std@@QAE?AViterator@12@XZ
// 0x10025430 ?CreateObject@?$MBComPtr@VIMBScreenPoly@@@@QAEJABUMBGUID@@@Z
// 0x10025580 ?insert@?$list@Upropinfo_pair@CSBGameState@@V?$MBAllocator@Upropinfo_pair@CSBGameState@@@@@std@@QAE?AViterator@12@V312@ABUpropinfo_pair@CSBGameState@@@Z
// 0x10025600 ?erase@?$list@Upropinfo_pair@CSBGameState@@V?$MBAllocator@Upropinfo_pair@CSBGameState@@@@@std@@QAE?AViterator@12@V312@0@Z
// 0x10025700 ?insert@?$_Tree@HU?$pair@$$CBHPAVSBItem@@@std@@U_Kfn@?$multimap@HPAVSBItem@@U?$less@H@std@@V?$MBAllocator@PAVSBItem@@@@@2@U?$less@H@2@V?$MBAllocator@PAVSBItem@@@@@std@@QAE?AU?$pair@Viterator@?$_Tree@HU?$pair@$$CBHPAVSBItem@@@std@@U_Kfn@?$multimap@2456e25c
// 0x10025a40 ?begin@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVSBItem@@@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVSBb82f14c1
// 0x10025dc0 ?find@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVSBItem@@@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVSBIaaef8d38
// 0x10026ab0 ?_Insert@?$_Tree@HU?$pair@$$CBHPAVSBItem@@@std@@U_Kfn@?$multimap@HPAVSBItem@@U?$less@H@std@@V?$MBAllocator@PAVSBItem@@@@@2@U?$less@H@2@V?$MBAllocator@PAVSBItem@@@@@std@@IAE?AViterator@12@PAU_Node@12@0ABU?$pair@$$CBHPAVSBItem@@@2@@Z
// 0x10027ac0 ?_Dec@const_iterator@?$_Tree@HU?$pair@$$CBHPAVSBItem@@@std@@U_Kfn@?$multimap@HPAVSBItem@@U?$less@H@std@@V?$MBAllocator@PAVSBItem@@@@@2@U?$less@H@2@V?$MBAllocator@PAVSBItem@@@@@std@@QAEXXZ
// 0x100281f0 ?_Construct@std@@YAXPAUpropinfo_pair@CSBGameState@@ABU23@@Z
// 0x100283a0 ?_Construct@std@@YAXPAU?$pair@$$CBHPAVSBItem@@@1@ABU21@@Z

// Static initializers and their atexit stubs: the PDB numbers them by the original header set, which
// ours does not reproduce yet, so they cannot be claimed by name.
// 0x1001e0d0 _$E22
// 0x1001e0f0 _$E19
// 0x1001e6c0 _$E21
// 0x1001e6e0 _$E20
// 0x10028620 _$E47
// 0x10028660 _$E43
// 0x10028680 _$E46
