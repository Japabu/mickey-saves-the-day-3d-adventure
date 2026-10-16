// PiccoloConsole.dll 0x1001a770-0x1001a7df: MfPeOverwriteCommand.
#include "PiccoloConsole.h"

// TODO 0x1001a770 ?Do@MfPeOverwriteCommand@@UAEXPAVMfPeDrawingContext@@@Z: 61% - the original converts this to the state after pushing the context and duplicates the call for a null this
void MfPeOverwriteCommand::Do(MfPeDrawingContext *context)
{
    mData->OverwriteChar(mChar, this, context);
}

// 0x1001a7b0 ?Undo@MfPeOverwriteCommand@@UAEXPAVMfPeDrawingContext@@@Z
void MfPeOverwriteCommand::Undo(MfPeDrawingContext *context)
{
    mData->UndoOverwriteChar(this, context);
}
