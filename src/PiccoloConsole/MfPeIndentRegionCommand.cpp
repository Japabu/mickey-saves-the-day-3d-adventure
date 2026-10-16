// PiccoloConsole.dll 0x1001a170-0x1001a20f: MfPeIndentRegionCommand.
#include "PiccoloConsole.h"

// 0x1001a170 ?Do@MfPeIndentRegionCommand@@UAEXPAVMfPeDrawingContext@@@Z
void MfPeIndentRegionCommand::Do(MfPeDrawingContext *context)
{
    mRegion = mData->mSelection;
    mData->IndentRegion(this, context);
}

// 0x1001a1b0 ?Undo@MfPeIndentRegionCommand@@UAEXPAVMfPeDrawingContext@@@Z
void MfPeIndentRegionCommand::Undo(MfPeDrawingContext *context)
{
    mData->UndoIndentRegion(this, context);
}

// 0x1001a1e0 ?Redo@MfPeIndentRegionCommand@@UAEXPAVMfPeDrawingContext@@@Z
void MfPeIndentRegionCommand::Redo(MfPeDrawingContext *context)
{
    mData->mSelection = mRegion;
    MfPeCommand::Redo(context);
}
