// PiccoloConsole.dll 0x100112d0-0x1001133f: MfPeDeleteRegionCommand.
#include "PiccoloConsole.h"

// 0x100112d0 ?Do@MfPeDeleteRegionCommand@@UAEXPAVMfPeDrawingContext@@@Z
void MfPeDeleteRegionCommand::Do(MfPeDrawingContext *context)
{
    mRegion = mData->mSelection;
    mData->DeleteRegion(this, context);
}

// 0x10011310 ?Undo@MfPeDeleteRegionCommand@@UAEXPAVMfPeDrawingContext@@@Z
void MfPeDeleteRegionCommand::Undo(MfPeDrawingContext *context)
{
    mData->UndoDeleteRegion(this, context);
}

// The same code as MfPeDragRegionCommand::Redo, which is kept.
void MfPeDeleteRegionCommand::Redo(MfPeDrawingContext *context)
{
    mData->mSelection = mRegion;
    MfPeCommand::Redo(context);
}
