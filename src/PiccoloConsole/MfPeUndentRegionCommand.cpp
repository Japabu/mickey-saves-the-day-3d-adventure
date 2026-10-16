// PiccoloConsole.dll 0x1001bd70-0x1001be0f: MfPeUndentRegionCommand.
#include "PiccoloConsole.h"

// 0x1001bd70 ?Do@MfPeUndentRegionCommand@@UAEXPAVMfPeDrawingContext@@@Z
void MfPeUndentRegionCommand::Do(MfPeDrawingContext *context)
{
    mRegion = mData->mSelection;
    mData->UndentRegion(this, context);
}

// 0x1001bdb0 ?Undo@MfPeUndentRegionCommand@@UAEXPAVMfPeDrawingContext@@@Z
void MfPeUndentRegionCommand::Undo(MfPeDrawingContext *context)
{
    mData->UndoUndentRegion(this, context);
}

// 0x1001bde0 ?Redo@MfPeUndentRegionCommand@@UAEXPAVMfPeDrawingContext@@@Z
void MfPeUndentRegionCommand::Redo(MfPeDrawingContext *context)
{
    mData->mSelection = mRegion;
    MfPeCommand::Redo(context);
}
