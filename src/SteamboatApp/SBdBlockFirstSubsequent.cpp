#include "SBdBlockFirstSubsequent.h"
#include "SBDialoguePropertyManager.h"

// 0x10049630 ??0SBdBlockFirstSubsequent@@QAE@AAV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@Z
SBdBlockFirstSubsequent::SBdBlockFirstSubsequent(MBString &blockName) : SBdBlock(blockName), mCurrentLineGroup(0)
{
}

// 0x10049670 ??_GSBdBlockFirstSubsequent@@UAEPAXI@Z

// 0x100496b0 ??1SBdBlockFirstSubsequent@@UAE@XZ
SBdBlockFirstSubsequent::~SBdBlockFirstSubsequent()
{
}

// 0x100496d0 ?LoadFromPref@SBdBlockFirstSubsequent@@UAEXV?$MBComPtr@VIMBStream@@@@V?$MBComPtr@VIMBPref@@@@@Z
void SBdBlockFirstSubsequent::LoadFromPref(MBComPtr<IMBStream> stream, MBComPtr<IMBPref> pref)
{
    SBdBlock::LoadFromPref(stream, pref);
    if (pref->GetString(stream, mBlockName, "block type", mBlockType, "")) {
        if (mBlockType.compare(MBString("first time, subsequent times")) == 0) {
        }
    }
}

// EQUIV 0x100499a0 ?GetLineGroup@SBdBlockFirstSubsequent@@UAEPAVSBdLineGroup@@PAVMfAiActor@@@Z: reviewed: the original keeps zero in edx (saving ebx for the size), VC6 tests against zero directly; same compares, branches and stores. Was: the original keeps a zero register for the compares and the else store
SBdLineGroup *SBdBlockFirstSubsequent::GetLineGroup(MfAiActor *mfPrimaryActor)
{
    int count = SBDialoguePropertyManager::GetInstance()->GetProperty(mSectionID);
    if (count && mLineGroups.size() > 1)
        mCurrentLineGroup = 1;
    else
        mCurrentLineGroup = 0;
    SBDialoguePropertyManager::GetInstance()->SetProperty(mSectionID, count + 1, mfPrimaryActor);
    return mLineGroups[mCurrentLineGroup];
}

// Not claimed (static initializers and header instances of this object):
// TODO 0x10049a40 $E302: static initializer from the headers (calls ??_B?1???id@?$ctype@G@std@@$D@@9@51, $E295); its $E number depends on the original header set
// 0x10049a80 _$E25
// TODO 0x10049aa0 $E298: static initializer from the headers (registers ?id@?$collate@D@std@@$E with atexit); its $E number depends on the original header set
