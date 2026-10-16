// PiccoloConsole.dll 0x1001a2b0-0x1001a30f: MfPeJoinLineCommand.
#include "PiccoloConsole.h"

// 0x1001a2b0 ?Do@MfPeJoinLineCommand@@UAEXPAVMfPeDrawingContext@@@Z
void MfPeJoinLineCommand::Do(MfPeDrawingContext *context)
{
    mData->JoinLine(mForward == 1, this, context);
}

// 0x1001a2e0 ?Undo@MfPeJoinLineCommand@@UAEXPAVMfPeDrawingContext@@@Z
void MfPeJoinLineCommand::Undo(MfPeDrawingContext *context)
{
    mData->UndoJoinLine(this, context);
}
