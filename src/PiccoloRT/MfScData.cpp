#include "MfScData.h"
#include "MfScDataHandle.h"
#include "MfScLinkData.h"
#include "MfScObject.h"

// 0x10008330 ?CountCircLinks@MfScData@@UAEPAVMfScLinkData@@PAVMfScDataHandle@@@Z
MfScLinkData *MfScData::CountCircLinks(MfScDataHandle *)
{
    return new MfScLinkData;
}

// 0x10008360 ?ReleaseCircLinks@MfScData@@UAEXPAVMfScLinkData@@@Z
void MfScData::ReleaseCircLinks(MfScLinkData *)
{
}

// 0x10008370 ?GetThroughCount@MfScData@@UAEHPAVMfScDataHandle@@@Z
int MfScData::GetThroughCount(MfScDataHandle *)
{
    return 0;
}

// MfScObject's three are the same functions (the linker folded them with MfScData's).

// 0x10008330 ?CountCircLinks@MfScObject@@MAEPAVMfScLinkData@@PAVMfScDataHandle@@@Z
MfScLinkData *MfScObject::CountCircLinks(MfScDataHandle *)
{
    return new MfScLinkData;
}

// 0x10008360 ?ReleaseCircLinks@MfScObject@@MAEXPAVMfScLinkData@@@Z
void MfScObject::ReleaseCircLinks(MfScLinkData *)
{
}

// 0x10008370 ?GetThroughCount@MfScObject@@MAEHPAVMfScDataHandle@@@Z
int MfScObject::GetThroughCount(MfScDataHandle *)
{
    return 0;
}
