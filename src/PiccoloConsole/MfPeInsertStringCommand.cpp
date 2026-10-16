// PiccoloConsole.dll 0x1001a210-0x1001a2af: MfPeInsertStringCommand, typing merged into a string.
#include "PiccoloConsole.h"

// TODO 0x1001a210 ?Do@MfPeInsertStringCommand@@UAEXPAVMfPeDrawingContext@@@Z: 61% - the original converts this to the state after pushing the context and duplicates the call for a null this
void MfPeInsertStringCommand::Do(MfPeDrawingContext *context)
{
    mData->InsertString(&mText, this, context);
}

// 0x1001a250 ?Undo@MfPeInsertStringCommand@@UAEXPAVMfPeDrawingContext@@@Z
void MfPeInsertStringCommand::Undo(MfPeDrawingContext *context)
{
    mData->UndoInsertString(mText.Length(), this, context);
}

// 0x1001a280 ?Redo@MfPeInsertStringCommand@@UAEXPAVMfPeDrawingContext@@@Z
void MfPeInsertStringCommand::Redo(MfPeDrawingContext *context)
{
    mData->SetCursor(mCursor.x, mCursor.y);
    Do(context);
}
