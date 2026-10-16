// PiccoloConsole.dll 0x10011340-0x100113df: MfPeDragRegionCommand, moving the selection with the mouse.
#include "PiccoloConsole.h"

// 0x10011340 ?Do@MfPeDragRegionCommand@@UAEXPAVMfPeDrawingContext@@@Z
void MfPeDragRegionCommand::Do(MfPeDrawingContext *context)
{
    mRegion = mData->mSelection;
    mData->DragRegion(this, context);
}

// 0x10011380 ?Undo@MfPeDragRegionCommand@@UAEXPAVMfPeDrawingContext@@@Z
void MfPeDragRegionCommand::Undo(MfPeDrawingContext *context)
{
    mData->UndoDragRegion(this, context);
}

// 0x100113b0 ?Redo@MfPeDragRegionCommand@@UAEXPAVMfPeDrawingContext@@@Z
void MfPeDragRegionCommand::Redo(MfPeDrawingContext *context)
{
    mData->mSelection = mRegion;
    MfPeCommand::Redo(context);
}
