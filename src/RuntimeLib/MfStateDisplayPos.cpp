// MfStateDisplayPos (RuntimeLib.dll 10085320-10085560).
#include <iostream.h>
#include "MfStateDisplayPos.h"

// 0x10085320 ??0MfStateDisplayPos@@QAE@NNNN@Z
MfStateDisplayPos::MfStateDisplayPos(double x, double y, double width, double height)
    : mX(x), mY(y), mWidth(width), mHeight(height)
{
    mType = TYPE;
}

// 0x100853a0 ??1MfStateDisplayPos@@UAE@XZ
MfStateDisplayPos::~MfStateDisplayPos()
{
}

// 0x100853b0 ?Clone@MfStateDisplayPos@@UAEPAVMfFSMMutable@@XZ
MfFSMMutable *MfStateDisplayPos::Clone()
{
    MfStateDisplayPos *copy = new MfStateDisplayPos(mX, mY, mWidth, mHeight);
    copy->mMachine = mMachine;
    return copy;
}

// 0x10085430 ?Write@MfStateDisplayPos@@UAEXAAVostream@@AAHH@Z
void MfStateDisplayPos::Write(ostream &os, int &indent, int mode)
{
    MfString tabs;
    if (mode == 1)
        return;
    for (int i = 0; i < indent; i++)
        tabs << '\t';
    os << tabs;
    os << "display_pos{" << mX << " " << mY << " " << mWidth << " " << mHeight << "}" << endl;
}
