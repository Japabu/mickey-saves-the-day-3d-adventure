#include "SBdBlockOrdered.h"
#include "SBDialoguePropertyManager.h"
#include <stdlib.h>

// 0x1004a400 ??0SBdBlockOrdered@@QAE@AAV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@Z
SBdBlockOrdered::SBdBlockOrdered(MBString &blockName)
    : SBdBlock(blockName), mFirstLineGroup(-1), mLastLineGroup(-1), mRandomLow(-1), mRandomHigh(-1),
      mCurrentLineGroup(-1), mPreviousLineGroup(-1), mInOrder(false), mRandomly(false)
{
}

// 0x1004a470 ??_GSBdBlockOrdered@@UAEPAXI@Z

// 0x1004a4b0 ??1SBdBlockOrdered@@UAE@XZ
SBdBlockOrdered::~SBdBlockOrdered()
{
}

// 0x1004a4d0 ?LoadFromPref@SBdBlockOrdered@@UAEXV?$MBComPtr@VIMBStream@@@@V?$MBComPtr@VIMBPref@@@@@Z
void SBdBlockOrdered::LoadFromPref(MBComPtr<IMBStream> stream, MBComPtr<IMBPref> pref)
{
    SBdBlock::LoadFromPref(stream, pref);
    if (pref->GetString(stream, mBlockName, "block type", mBlockType, "")) {
        if (mBlockType == MBString("in order then randomly")) {
            mFirstLineGroup = pref->GetLong(stream, mBlockName, "first time", 0);
            int numLineGroups = mLineGroups.size();
            mLastLineGroup = pref->GetLong(stream, mBlockName, "last time", numLineGroups - 1);
            mRandomLow = pref->GetLong(stream, mBlockName, "random low", 0);
            numLineGroups = mLineGroups.size();
            mRandomHigh = pref->GetLong(stream, mBlockName, "random high", numLineGroups - 1);
            mInOrder = true;
            mRandomly = true;
            mCurrentLineGroup = mFirstLineGroup;
        } else if (mBlockType == MBString("in order")) {
            mFirstLineGroup = 0;
            mLastLineGroup = mLineGroups.size() - 1;
            mInOrder = true;
            mRandomly = false;
            mCurrentLineGroup = 0;
        }
    }
}

// 0x1004aab0 ?GetLineGroup@SBdBlockOrdered@@UAEPAVSBdLineGroup@@PAVMfAiActor@@@Z
SBdLineGroup *SBdBlockOrdered::GetLineGroup(MfAiActor *mfPrimaryActor)
{
    if (mInOrder) {
        int count = SBDialoguePropertyManager::GetInstance()->GetProperty(mSectionID);
        if (count < mLineGroups.size() && count <= mLastLineGroup) {
            mCurrentLineGroup = count;
            mPreviousLineGroup = count;
        } else if (mRandomly) {
            int tries = 0;
            int maxTries = mLineGroups.size() * 2;
            int pick;
            do {
                pick = rand() % (mRandomHigh - mRandomLow + 1) + mRandomLow;
                tries++;
            } while (pick == mPreviousLineGroup && tries <= maxTries);
            mPreviousLineGroup = mCurrentLineGroup;
            mCurrentLineGroup = pick;
        }
        SBDialoguePropertyManager::GetInstance()->SetProperty(mSectionID, count + 1, mfPrimaryActor);
        return mLineGroups[mCurrentLineGroup];
    }
    return 0;
}

// Not claimed (static initializers and header instances of this object):
// TODO 0x1004ac20 $E302: static initializer from the headers (calls ??_B?1???id@?$ctype@G@std@@$D@@9@51, $E295); its $E number depends on the original header set
// 0x1004ac60 _$E25
// TODO 0x1004ac80 $E298: static initializer from the headers (registers ?id@?$collate@D@std@@$E with atexit); its $E number depends on the original header set
