// PiccoloConsole.dll 0x1001bd00-0x1001bd6f: MfPeTypingCommand, a typed character.
#include "PiccoloConsole.h"

// TODO 0x1001bd00 ?Do@MfPeTypingCommand@@UAEXPAVMfPeDrawingContext@@@Z: 61% - the original converts this to the state after pushing the context and duplicates the call for a null this
void MfPeTypingCommand::Do(MfPeDrawingContext *context)
{
    mData->InsertChar(mChar, this, context);
}

// 0x1001bd40 ?Undo@MfPeTypingCommand@@UAEXPAVMfPeDrawingContext@@@Z
void MfPeTypingCommand::Undo(MfPeDrawingContext *context)
{
    mData->UndoInsertChar(this, context);
}
