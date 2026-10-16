// PiccoloConsole.dll 0x10011270-0x100112cf: MfPeDeleteForwardWordCommand.
#include "PiccoloConsole.h"

// 0x10011270 ?Do@MfPeDeleteForwardWordCommand@@UAEXPAVMfPeDrawingContext@@@Z
void MfPeDeleteForwardWordCommand::Do(MfPeDrawingContext *context)
{
    mData->DeleteForwardWord(this, context);
}

// 0x100112a0 ?Undo@MfPeDeleteForwardWordCommand@@UAEXPAVMfPeDrawingContext@@@Z
void MfPeDeleteForwardWordCommand::Undo(MfPeDrawingContext *context)
{
    mData->UndoDeleteForwardWord(this, context);
}
