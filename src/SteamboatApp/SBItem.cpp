#include "SBItem.h"
#include <locale> // evidence: the original unit ends with the std::ctype<unsigned short>::id and std::collate<char>::id initializers

// EQUIV 0x1006dc90 ??0SBItem@@QAE@XZ: reviewed: inline budget: the original still inlines _Grow (max_size, _Xlen, _Tidy, _Copy) in the last assignment ("XX"), VC6 calls it as in the first three
SBItem::SBItem() : mInstance(0), mLocation(0), mInScene(true), mOnCharacter(false), mRolloverCursor(10)
{
    mInstanceName = "nothing0";
    mClassName = "nothing";
    mGiveTo = "noone";
    mSpaceCode = "XX";
}

// 0x1006df50 ?IsItemActiveInSpace@SBItem@@QAE?B_NAAV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@Z
const bool SBItem::IsItemActiveInSpace(MBString &spaceCode)
{
    if ((int)mSpaceCode.find(spaceCode) < 0)
        return false;
    return true;
}

// 0x1006e040 ?SetHotspot@SBItem@@QAEXPAVIMBScreenPoly@@@Z
void SBItem::SetHotspot(IMBScreenPoly *hotspot)
{
    mHotspot = hotspot;
    mHotspot->SetBroadcastChannel(110);
    mHotspot->SetClickMessage(201, this);
    mHotspot->SetEnterMessage(202, this);
    mHotspot->SetLeaveMessage(203);
}

// Instances the linker took from other objects (this unit emits them too):
// 0x100250f0 ??1?$MBComPtr@VIMBScreenPoly@@@@QAE@XZ

// 0x1006e0c0 _$E25
// 0x1006e100 _$E21
// 0x1006e120 _$E24
