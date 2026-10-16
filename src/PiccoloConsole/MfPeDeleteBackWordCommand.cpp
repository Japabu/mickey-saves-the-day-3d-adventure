// PiccoloConsole.dll 0x100111b0-0x1001120f: MfPeDeleteBackWordCommand.
#include "PiccoloConsole.h"

// 0x100111b0 ?Do@MfPeDeleteBackWordCommand@@UAEXPAVMfPeDrawingContext@@@Z
void MfPeDeleteBackWordCommand::Do(MfPeDrawingContext *context)
{
    mData->DeleteBackWord(this, context);
}

// 0x100111e0 ?Undo@MfPeDeleteBackWordCommand@@UAEXPAVMfPeDrawingContext@@@Z
void MfPeDeleteBackWordCommand::Undo(MfPeDrawingContext *context)
{
    mData->UndoDeleteBackWord(this, context);
}
