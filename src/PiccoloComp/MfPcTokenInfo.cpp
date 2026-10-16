// PiccoloComp.dll MfPcTokenInfo.cpp (0x1000f560-0x1000f5b0)
#include "MfPcTokenInfo.h"

// 0x1000f560 ?Merge@MfPcTokenInfo@@QAEXPAV1@0@Z
void MfPcTokenInfo::Merge(MfPcTokenInfo *first, MfPcTokenInfo *last)
{
    if (first->mLine == last->mLine) {
        mLine = first->mLine;
        mStartColumn = first->mStartColumn;
        mEndColumn = last->mEndColumn;
        mText = last->mText;
    } else {
        mLine = last->mLine;
        mStartColumn = 0;
        mEndColumn = last->mEndColumn;
        mText = last->mText;
    }
}
