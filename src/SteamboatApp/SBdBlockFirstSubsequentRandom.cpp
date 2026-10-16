#include "SBdBlockFirstSubsequentRandom.h"
#include "SBDialoguePropertyManager.h"
#include <stdlib.h>

// 0x10049ac0 ??0SBdBlockFirstSubsequentRandom@@QAE@AAV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@Z
SBdBlockFirstSubsequentRandom::SBdBlockFirstSubsequentRandom(MBString &blockName) : SBdBlock(blockName), mCurrentLineGroup(0)
{
}

// 0x10049b00 ??_GSBdBlockFirstSubsequentRandom@@UAEPAXI@Z

// 0x10049b40 ??1SBdBlockFirstSubsequentRandom@@UAE@XZ
SBdBlockFirstSubsequentRandom::~SBdBlockFirstSubsequentRandom()
{
}

// 0x10049b60 ?LoadFromPref@SBdBlockFirstSubsequentRandom@@UAEXV?$MBComPtr@VIMBStream@@@@V?$MBComPtr@VIMBPref@@@@@Z
void SBdBlockFirstSubsequentRandom::LoadFromPref(MBComPtr<IMBStream> stream, MBComPtr<IMBPref> pref)
{
    SBdBlock::LoadFromPref(stream, pref);
    if (pref->GetString(stream, mBlockName, "block type", mBlockType, "")) {
        if (mBlockType.compare(MBString("first time, subsequent times")) == 0) {
        }
    }
}

// 0x10049e30 ?GetLineGroup@SBdBlockFirstSubsequentRandom@@UAEPAVSBdLineGroup@@PAVMfAiActor@@@Z
SBdLineGroup *SBdBlockFirstSubsequentRandom::GetLineGroup(MfAiActor *mfPrimaryActor)
{
    int count = SBDialoguePropertyManager::GetInstance()->GetProperty(mSectionID);
    if (count == 0)
        mCurrentLineGroup = 0;
    else {
        int tries = 0;
        int maxTries = mLineGroups.size() * 2;
        int pick;
        do {
            pick = rand() % (mLineGroups.size() - 1);
            tries++;
        } while (pick == mCurrentLineGroup && tries <= maxTries);
        mCurrentLineGroup = pick;
    }
    SBDialoguePropertyManager::GetInstance()->SetProperty(mSectionID, count + 1, mfPrimaryActor);
    return mLineGroups[mCurrentLineGroup];
}

// Not claimed (static initializers and header instances of this object):
// TODO 0x10049f20 $E302: static initializer from the headers (calls ??_B?1???id@?$ctype@G@std@@$D@@9@51, $E295); its $E number depends on the original header set
// 0x10049f60 _$E25
// TODO 0x10049f80 $E298: static initializer from the headers (registers ?id@?$collate@D@std@@$E with atexit); its $E number depends on the original header set
