// PiccoloComp.dll MfPcLabelGen.cpp (0x1000d5b0-0x1000d670)
#include "MfPcLabelGen.h"

// 0x1000d5b0 ?NewLabel@MfPcLabelGen@@QAE?AVMfString@@XZ
MfString MfPcLabelGen::NewLabel()
{
    MfString label = MfString("@@L") + MfString(mNext++);
    return label;
}
