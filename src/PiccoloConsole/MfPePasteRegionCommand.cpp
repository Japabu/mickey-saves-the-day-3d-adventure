// PiccoloConsole.dll 0x1001a7e0-0x1001a83f: MfPePasteRegionCommand.
#include "PiccoloConsole.h"

// 0x1001a7e0 ?Do@MfPePasteRegionCommand@@UAEXPAVMfPeDrawingContext@@@Z
void MfPePasteRegionCommand::Do(MfPeDrawingContext *context)
{
    mData->PasteRegion(this, context);
}

// 0x1001a810 ?Undo@MfPePasteRegionCommand@@UAEXPAVMfPeDrawingContext@@@Z
void MfPePasteRegionCommand::Undo(MfPeDrawingContext *context)
{
    mData->UndoPasteRegion(this, context);
}
