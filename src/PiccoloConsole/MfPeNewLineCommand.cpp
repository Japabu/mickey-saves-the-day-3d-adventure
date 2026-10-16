// PiccoloConsole.dll 0x1001a710-0x1001a76f: MfPeNewLineCommand.
#include "PiccoloConsole.h"

// 0x1001a710 ?Do@MfPeNewLineCommand@@UAEXPAVMfPeDrawingContext@@@Z
void MfPeNewLineCommand::Do(MfPeDrawingContext *context)
{
    mData->NewLine(this, context);
}

// 0x1001a740 ?Undo@MfPeNewLineCommand@@UAEXPAVMfPeDrawingContext@@@Z
void MfPeNewLineCommand::Undo(MfPeDrawingContext *context)
{
    mData->UndoNewLine(this, context);
}
