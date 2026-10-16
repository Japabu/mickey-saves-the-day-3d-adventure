#include "MfScLineInfo.h"

// 0x10009140 ??0MfScLineInfo@@QAE@XZ
MfScLineInfo::MfScLineInfo()
{
    mOffset = -1;
    mLine = -1;
}

// 0x1001b220 ?Offset@MfScLineInfo@@QBEHXZ
int MfScLineInfo::Offset() const
{
    return mOffset;
}
