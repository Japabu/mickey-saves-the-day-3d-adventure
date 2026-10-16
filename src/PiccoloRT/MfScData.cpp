#include "MfScData.h"
#include "MfScDataHandle.h"
#include "MfScLinkData.h"

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
