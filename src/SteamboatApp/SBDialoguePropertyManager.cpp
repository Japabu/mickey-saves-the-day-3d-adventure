#include "SBDialoguePropertyManager.h"
#include "MBRuntime.h"
#include "MfAi.h"
#include "MfIntValue.h"
#include "MfProperty.h"
#include <stdio.h>
#include <locale> // evidence: the original unit ends with the std::ctype<unsigned short>::id and std::collate<char>::id initializers

SBDialoguePropertyManager *SBDialoguePropertyManager::sInstance;

// 0x1004e040 ?Create@SBDialoguePropertyManager@@SAXXZ
void SBDialoguePropertyManager::Create()
{
    sInstance = new SBDialoguePropertyManager;
}

// 0x1004e0d0 ?Destroy@SBDialoguePropertyManager@@SAXXZ
void SBDialoguePropertyManager::Destroy()
{
    if (sInstance)
        delete sInstance;
}

// 0x1004e100 ?ClearMaps@SBDialoguePropertyManager@@QAEXXZ
void SBDialoguePropertyManager::ClearMaps()
{
    mPropertyHashNameVec.clear();
    mInvPropertyHashNameVec.clear();
    mPropertyNameMap.clear();
    mPropertyMap.clear();
}

// 0x1004e1a0 ?LoadFromPref@SBDialoguePropertyManager@@QAEXV?$MBComPtr@VIMBStream@@@@V?$MBComPtr@VIMBPref@@@@@Z
void SBDialoguePropertyManager::LoadFromPref(MBComPtr<IMBStream> stream, MBComPtr<IMBPref> pref)
{
    ClearMaps();
    int propNum = 0;
    MBString propertyName;
    char property[80];
    sprintf(property, "prop%d", propNum);
    while (pref->GetString(stream, "DialogueProperties", property, propertyName, "")) {
        unsigned long hashVal = MBHashString(propertyName.c_str());
        mPropertyHashNameVec.push_back(hashVal);
        mPropertyNameMap.insert(NameMap::value_type(hashVal, propertyName));
        mPropertyMap.insert(ValueMap::value_type(hashVal, 0));
        char invproperty[80];
        sprintf(invproperty, "!%s", propertyName.c_str());
        unsigned long invHashVal = MBHashString(invproperty);
        mInvPropertyHashNameVec.push_back(invHashVal);
        mPropertyNameMap.insert(NameMap::value_type(invHashVal, MBString(invproperty)));
        mPropertyMap.insert(ValueMap::value_type(invHashVal, 0));
        propNum++;
        sprintf(property, "prop%d", propNum);
    }
    if (pref->GetString(stream, "DialogueProperties", "character code", mCharacterCodePropertyName, ""))
        mIsCharacterCodeValid = false;
}

// 0x1004e8a0 ?UpdateProperties@SBDialoguePropertyManager@@QAEXPAVMfAiActor@@@Z
void SBDialoguePropertyManager::UpdateProperties(MfAiActor *mfPrimaryActor)
{
    for (unsigned int i = 0; i < mPropertyHashNameVec.size(); i++) {
        MBString propertyName = mPropertyNameMap[mPropertyHashNameVec[i]];
        MfProperty property;
        if (mfPrimaryActor->GetProperty(propertyName.c_str(), property)) {
            mPropertyMap[mPropertyHashNameVec[i]] = (int)property;
            mPropertyMap[mInvPropertyHashNameVec[i]] = !(int)property;
        }
    }
    if (!mIsCharacterCodeValid && mfPrimaryActor) {
        MfProperty property;
        if (mfPrimaryActor->GetProperty(mCharacterCodePropertyName.c_str(), property)) {
            mCharacterCode = (const char *)MfString(property);
            mIsCharacterCodeValid = true;
        }
    }
}

// 0x1004ed70 ?GetProperty@SBDialoguePropertyManager@@QAEHK@Z
int SBDialoguePropertyManager::GetProperty(unsigned long propertyID)
{
    return mPropertyMap[propertyID];
}

// 0x1004edc0 ?GetPropertyString@SBDialoguePropertyManager@@QAEABV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@K@Z
const MBString &SBDialoguePropertyManager::GetPropertyString(unsigned long propertyID)
{
    return mPropertyNameMap[propertyID];
}

// EQUIV 0x1004ede0 ?SetProperty@SBDialoguePropertyManager@@QAEXKHPAVMfAiActor@@@Z: reviewed: the original inlines the map iterator post-increment (_Lockit, _Inc, ~_Lockit), VC6 calls const_iterator::_Inc; the rest differs only in registers and slots. Was: the original inlines the iterator increment (_Inc), VC6 calls it here
void SBDialoguePropertyManager::SetProperty(unsigned long propertyID, int value, MfAiActor *mfPrimaryActor)
{
    for (NameMap::iterator it = mPropertyNameMap.begin(); it != mPropertyNameMap.end(); it++) {
        if (propertyID == (*it).first) {
            mPropertyMap[propertyID] = value;
            MfProperty intProperty;
            if (mfPrimaryActor->GetProperty((*it).second.c_str(), intProperty)) {
                intProperty.SetValue(new MfIntValue(value));
                mfPrimaryActor->SetProperty(intProperty);
            }
        }
    }
}

// 0x1004f010 ?GetCharacterCode@SBDialoguePropertyManager@@QAEABV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAVMfAiActor@@@Z
const MBString &SBDialoguePropertyManager::GetCharacterCode(MfAiActor *mfPrimaryActor)
{
    return mCharacterCode;
}

// 0x1004f030 ?GetNumberOfProperties@SBDialoguePropertyManager@@QAEHXZ
int SBDialoguePropertyManager::GetNumberOfProperties()
{
    return mPropertyMap.size();
}

// 0x1004f050 ?GetPropertyStringByOrder@SBDialoguePropertyManager@@QAEABV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@H@Z
const MBString &SBDialoguePropertyManager::GetPropertyStringByOrder(int orderNumber)
{
    NameMap::iterator pnIterator = mPropertyNameMap.begin();
    for (int i = 0; i != orderNumber && pnIterator != mPropertyNameMap.end(); i++)
        pnIterator++;
    return (*pnIterator).second;
}

// 0x1004f0a0 ??0SBDialoguePropertyManager@@IAE@XZ
SBDialoguePropertyManager::SBDialoguePropertyManager() : mIsCharacterCodeValid(true)
{
}

// 0x1004f270 ??1SBDialoguePropertyManager@@MAE@XZ
SBDialoguePropertyManager::~SBDialoguePropertyManager()
{
    ClearMaps();
}

// Matched by this code (template and inline instances):
// 0x1004e830 ??1?$pair@$$CBKV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@std@@QAE@XZ
// 0x1004f190 ??_GSBDialoguePropertyManager@@MAEPAXI@Z
// 0x1004f1d0 ??1?$MBMap@KV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$less@K@2@@@QAE@XZ
// 0x1004f490 ??1?$MBMap@KKU?$less@K@std@@@@QAE@XZ
// 0x1004f530 ??A?$map@KV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$less@K@2@V?$MBAllocator@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@@@std@@QAEAAV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@1@ABK@Z
// 0x1004f670 ?insert@?$map@KV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$less@K@2@V?$MBAllocator@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@@@std@@QAE?AU?$pair@Viterator@?$_Tree@KU?$pair@$$CBKV?$basic_string@DU?$char_ta59ed54d
// 0x1004f6b0 ??0?$pair@$$CBKV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@std@@QAE@ABKABV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@1@@Z
// 0x1004f840 ?insert@?$_Tree@KU?$pair@$$CBKV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@std@@U_Kfn@?$map@KV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$less@K@2@V?$MBAllocator@V?$basic_string@DU?$char_traits@D@std@@V?$MBA3dcbe421
// 0x1004f9c0 ?erase@?$_Tree@KU?$pair@$$CBKV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@std@@U_Kfn@?$map@KV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$less@K@2@V?$MBAllocator@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@@@2@U?$less@K@2@V?$MBAllocator@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@@@std@@QAE?AViterator@12@V312@0@Z
// 0x1004fb50 ?insert@?$_Tree@KU?$pair@$$CBKK@std@@U_Kfn@?$map@KKU?$less@K@std@@V?$MBAllocator@K@@@2@U?$less@K@2@V?$MBAllocator@K@@@std@@QAE?AU?$pair@Viterator@?$_Tree@KU?$pair@$$CBKK@std@@U_Kfn@?$map@KKU?$less@K@std@@V?$MBAllocator@K@@@2@U?$less@K@2@V?$MBAlloceebb9ac3
// 0x1004fcd0 ?erase@?$_Tree@KU?$pair@$$CBKK@std@@U_Kfn@?$map@KKU?$less@K@std@@V?$MBAllocator@K@@@2@U?$less@K@2@V?$MBAllocator@K@@@std@@QAE?AViterator@12@V312@0@Z
// 0x1004fe50 ?insert@?$vector@KV?$MBAllocator@K@@@std@@QAEPAKPAKABK@Z
// 0x100500a0 ?size@?$vector@KV?$MBAllocator@K@@@std@@QBEIXZ
// 0x100500d0 ??0?$pair@Viterator@?$_Tree@KU?$pair@$$CBKV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@std@@U_Kfn@?$map@KV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$less@K@2@V?$MBAllocator@V?$basic_string@DU?$char_traits@Ddd7adf85
// 0x10050100 ??0?$pair@Viterator@?$_Tree@KU?$pair@$$CBKK@std@@U_Kfn@?$map@KKU?$less@K@std@@V?$MBAllocator@K@@@2@U?$less@K@2@V?$MBAllocator@K@@@std@@_N@std@@QAE@ABViterator@?$_Tree@KU?$pair@$$CBKK@std@@U_Kfn@?$map@KKU?$less@K@std@@V?$MBAllocator@K@@@2@U?$less@Kb7c82ae4
// 0x10050130 ?erase@?$_Tree@KU?$pair@$$CBKV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@std@@U_Kfn@?$map@KV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$less@K@2@V?$MBAllocator@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@@@2@U?$less@K@2@V?$MBAllocator@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@@@std@@QAE?AViterator@12@V312@@Z
// 0x100507b0 ?_Erase@?$_Tree@KU?$pair@$$CBKV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@std@@U_Kfn@?$map@KV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$less@K@2@V?$MBAllocator@V?$basic_string@DU?$char_traits@D@std@@V?$MBA45f2b8bf
// 0x100508a0 ?_Init@?$_Tree@KU?$pair@$$CBKV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@std@@U_Kfn@?$map@KV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$less@K@2@V?$MBAllocator@V?$basic_string@DU?$char_traits@D@std@@V?$MBAla1bcc265
// 0x10050940 ?_Insert@?$_Tree@KU?$pair@$$CBKV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@std@@U_Kfn@?$map@KV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$less@K@2@V?$MBAllocator@V?$basic_string@DU?$char_traits@D@std@@V?$MBd82ced7c
// 0x10050cd0 ?_Inc@const_iterator@?$_Tree@KU?$pair@$$CBKV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@std@@U_Kfn@?$map@KV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$less@K@2@V?$MBAllocator@V?$basic_string@DU?$char_traits@658d5a81
// 0x10050db0 ?erase@?$_Tree@KU?$pair@$$CBKK@std@@U_Kfn@?$map@KKU?$less@K@std@@V?$MBAllocator@K@@@2@U?$less@K@2@V?$MBAllocator@K@@@std@@QAE?AViterator@12@V312@@Z
// 0x100513f0 ?_Erase@?$_Tree@KU?$pair@$$CBKK@std@@U_Kfn@?$map@KKU?$less@K@std@@V?$MBAllocator@K@@@2@U?$less@K@2@V?$MBAllocator@K@@@std@@IAEXPAU_Node@12@@Z
// 0x100514b0 ?_Init@?$_Tree@KU?$pair@$$CBKK@std@@U_Kfn@?$map@KKU?$less@K@std@@V?$MBAllocator@K@@@2@U?$less@K@2@V?$MBAllocator@K@@@std@@IAEXXZ
// 0x10051550 ?_Insert@?$_Tree@KU?$pair@$$CBKK@std@@U_Kfn@?$map@KKU?$less@K@std@@V?$MBAllocator@K@@@2@U?$less@K@2@V?$MBAllocator@K@@@std@@IAE?AViterator@12@PAU_Node@12@0ABU?$pair@$$CBKK@2@@Z
// 0x100518e0 ?_Dec@const_iterator@?$_Tree@KU?$pair@$$CBKV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@std@@U_Kfn@?$map@KV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$less@K@2@V?$MBAllocator@V?$basic_string@DU?$char_traits@18d9bdd9
// 0x100519d0 ?_Dec@const_iterator@?$_Tree@KU?$pair@$$CBKK@std@@U_Kfn@?$map@KKU?$less@K@std@@V?$MBAllocator@K@@@2@U?$less@K@2@V?$MBAllocator@K@@@std@@QAEXXZ
// 0x10051ac0 ?_Ufill@?$vector@KV?$MBAllocator@K@@@std@@IAEXPAKIABK@Z
// 0x10051b00 ?_Inc@const_iterator@?$_Tree@KU?$pair@$$CBKK@std@@U_Kfn@?$map@KKU?$less@K@std@@V?$MBAllocator@K@@@2@U?$less@K@2@V?$MBAllocator@K@@@std@@QAEXXZ
// 0x10051be0 ?_Construct@std@@YAXPAU?$pair@$$CBKV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@1@ABU21@@Z
// 0x10051db0 ?_Construct@std@@YAXPAU?$pair@$$CBKK@1@ABU21@@Z

// Not claimed (static initializers and header instances of this object):
// 0x10051de0 _$E33
// 0x10051e20 _$E29
// 0x10051e40 _$E32
