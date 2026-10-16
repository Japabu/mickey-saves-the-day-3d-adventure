// PiccoloConsole.dll 0x10010e50-0x10010e7f: MfPeCopyRegionCommand.
#include "PiccoloConsole.h"

// 0x10010e50 ?Do@MfPeCopyRegionCommand@@UAEXPAVMfPeDrawingContext@@@Z
void MfPeCopyRegionCommand::Do(MfPeDrawingContext *context)
{
    mData->CopyRegion(this, context);
}
