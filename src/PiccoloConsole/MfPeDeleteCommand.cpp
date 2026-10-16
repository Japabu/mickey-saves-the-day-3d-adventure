// PiccoloConsole.dll 0x10011210-0x1001126f: MfPeDeleteCommand, deleting a character.
#include "PiccoloConsole.h"

// 0x10011210 ?Do@MfPeDeleteCommand@@UAEXPAVMfPeDrawingContext@@@Z
void MfPeDeleteCommand::Do(MfPeDrawingContext *context)
{
    mData->DeleteChar(mBackspace == 0, this, context);
}

// 0x10011240 ?Undo@MfPeDeleteCommand@@UAEXPAVMfPeDrawingContext@@@Z
void MfPeDeleteCommand::Undo(MfPeDrawingContext *context)
{
    mData->UndoDeleteChar(this, context);
}
