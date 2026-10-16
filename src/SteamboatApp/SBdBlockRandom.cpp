#include "SBdBlockRandom.h"
#include "SBDialoguePropertyManager.h"
#include <stdlib.h>

// 0x1004aca0 ??0SBdBlockRandom@@QAE@AAV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@Z
SBdBlockRandom::SBdBlockRandom(MBString &blockName) : SBdBlock(blockName), mCurrentLineGroup(0)
{
}

// 0x1004ace0 ??_GSBdBlockRandom@@UAEPAXI@Z

// 0x1004ad20 ??1SBdBlockRandom@@UAE@XZ
SBdBlockRandom::~SBdBlockRandom()
{
}

// 0x1004ad40 ?LoadFromPref@SBdBlockRandom@@UAEXV?$MBComPtr@VIMBStream@@@@V?$MBComPtr@VIMBPref@@@@@Z
void SBdBlockRandom::LoadFromPref(MBComPtr<IMBStream> stream, MBComPtr<IMBPref> pref)
{
    SBdBlock::LoadFromPref(stream, pref);
    if (pref->GetString(stream, mBlockName, "block type", mBlockType, "")) {
        if (mBlockType.compare(MBString("random")) == 0) {
        }
    }
}

// 0x1004b010 ?GetLineGroup@SBdBlockRandom@@UAEPAVSBdLineGroup@@PAVMfAiActor@@@Z
SBdLineGroup *SBdBlockRandom::GetLineGroup(MfAiActor *mfPrimaryActor)
{
    int count = SBDialoguePropertyManager::GetInstance()->GetProperty(mSectionID);
    if (count == 0) {
        if (mLineGroups.size() == 1)
            mCurrentLineGroup = 0;
        else
            mCurrentLineGroup = rand() % (mLineGroups.size() - 1);
    } else {
        int pick;
        do {
            if (mLineGroups.size() == 1) {
                mCurrentLineGroup = -1;
                pick = 0;
            } else
                pick = rand() % (mLineGroups.size() - 1);
        } while (pick == mCurrentLineGroup);
        mCurrentLineGroup = pick;
    }
    SBDialoguePropertyManager::GetInstance()->SetProperty(mSectionID, count + 1, mfPrimaryActor);
    return mLineGroups[mCurrentLineGroup];
}

// Not claimed (static initializers and header instances of this object):
// TODO 0x1004b140 $E302: static initializer from the headers (calls ??_B?1???id@?$ctype@G@std@@$D@@9@51, $E295); its $E number depends on the original header set
// 0x1004b180 _$E25
// TODO 0x1004b1a0 $E298: static initializer from the headers (registers ?id@?$collate@D@std@@$E with atexit); its $E number depends on the original header set
