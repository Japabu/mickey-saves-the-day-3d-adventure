#pragma once
// SBDialoguePropertyManager.h: the dialogue properties ("prop0", "prop1", ... and their "!"
// negations) and their values for the current actor (declaration from SteamboatApp.pdb).
#include "MBInterfaces.h"

class MfAiActor;

class SBDialoguePropertyManager {
public:
    static void __cdecl Create();
    static void __cdecl Destroy();
    static SBDialoguePropertyManager *__cdecl GetInstance() { return sInstance; }
    int GetProperty(unsigned long propertyID);
    void SetProperty(unsigned long propertyID, int value, MfAiActor *mfPrimaryActor);
    int GetNumberOfProperties();
    const MBString &GetPropertyStringByOrder(int orderNumber);
    const MBString &GetPropertyString(unsigned long propertyID);
    const MBString &GetCharacterCode(MfAiActor *mfPrimaryActor);
    void LoadFromPref(MBComPtr<IMBStream> stream, MBComPtr<IMBPref> pref);
    void UpdateProperties(MfAiActor *mfPrimaryActor);
    void ClearMaps();
protected:
    SBDialoguePropertyManager(const SBDialoguePropertyManager &);
    SBDialoguePropertyManager();
    virtual ~SBDialoguePropertyManager();
private:
    typedef MBMap<unsigned long, MBString, std::less<unsigned long> > NameMap;
    typedef MBMap<unsigned long, unsigned long, std::less<unsigned long> > ValueMap;
    NameMap mPropertyNameMap;                           // +0x0004 size 0x10
    ValueMap mPropertyMap;                              // +0x0014 size 0x10
    MBVector<unsigned long> mPropertyHashNameVec;       // +0x0024 size 0x10
    MBVector<unsigned long> mInvPropertyHashNameVec;    // +0x0034 size 0x10
    MBString mCharacterCodePropertyName;                // +0x0044 size 0x10
    MBString mCharacterCode;                            // +0x0054 size 0x10
    bool mIsCharacterCodeValid;                         // +0x0064 size 0x1
    static SBDialoguePropertyManager *sInstance;
};
