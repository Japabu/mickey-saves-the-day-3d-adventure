#include "SBdBlockFirstTimeOnly.h"
#include "SBDialoguePropertyManager.h"
#include <stdlib.h>

// 0x10049fa0 ??0SBdBlockFirstTimeOnly@@QAE@AAV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@Z
SBdBlockFirstTimeOnly::SBdBlockFirstTimeOnly(MBString &blockName) : SBdBlock(blockName), mCurrentLineGroup(0)
{
}

// 0x10049fe0 ??_GSBdBlockFirstTimeOnly@@UAEPAXI@Z

// 0x1004a020 ??1SBdBlockFirstTimeOnly@@UAE@XZ
SBdBlockFirstTimeOnly::~SBdBlockFirstTimeOnly()
{
}

// 0x1004a040 ?LoadFromPref@SBdBlockFirstTimeOnly@@UAEXV?$MBComPtr@VIMBStream@@@@V?$MBComPtr@VIMBPref@@@@@Z
void SBdBlockFirstTimeOnly::LoadFromPref(MBComPtr<IMBStream> stream, MBComPtr<IMBPref> pref)
{
    SBdBlock::LoadFromPref(stream, pref);
    if (pref->GetString(stream, mBlockName, "block type", mBlockType, "")) {
        if (mBlockType.compare(MBString("first time only")) == 0) {
        }
    }
}

// 0x1004a310 ?GetLineGroup@SBdBlockFirstTimeOnly@@UAEPAVSBdLineGroup@@PAVMfAiActor@@@Z
SBdLineGroup *SBdBlockFirstTimeOnly::GetLineGroup(MfAiActor *mfPrimaryActor)
{
    if (SBDialoguePropertyManager::GetInstance()->GetProperty(mSectionID) == 0) {
        mCurrentLineGroup = 0;
        SBDialoguePropertyManager::GetInstance()->SetProperty(mSectionID, 1, mfPrimaryActor);
        return mLineGroups[0];
    }
    return 0;
}

// Not claimed (static initializers and header instances of this object):
// TODO 0x1004a380 $E302: static initializer from the headers (calls ??_B?1???id@?$ctype@G@std@@$D@@9@51, $E295); its $E number depends on the original header set
// 0x1004a3c0 _$E25
// TODO 0x1004a3e0 $E298: static initializer from the headers (registers ?id@?$collate@D@std@@$E with atexit); its $E number depends on the original header set
