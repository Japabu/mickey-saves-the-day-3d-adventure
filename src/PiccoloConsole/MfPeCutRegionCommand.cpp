// PiccoloConsole.dll 0x10011170-0x100111af: MfPeCutRegionCommand.
#include "PiccoloConsole.h"

// 0x10011170 ?Do@MfPeCutRegionCommand@@UAEXPAVMfPeDrawingContext@@@Z
void MfPeCutRegionCommand::Do(MfPeDrawingContext *context)
{
    mRegion = mData->mSelection;
    mData->CutRegion(this, context);
}

// The same code as MfPeDeleteRegionCommand::Undo, which is kept.
void MfPeCutRegionCommand::Undo(MfPeDrawingContext *context)
{
    mData->UndoDeleteRegion(this, context);
}

// The same code as MfPeDragRegionCommand::Redo, which is kept.
void MfPeCutRegionCommand::Redo(MfPeDrawingContext *context)
{
    mData->mSelection = mRegion;
    MfPeCommand::Redo(context);
}
