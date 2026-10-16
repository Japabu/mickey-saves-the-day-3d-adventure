// PiccoloConsole.dll 0x10010d60-0x10010dbf: MfPeCommand, a change of the text that can be undone.
#include "PiccoloConsole.h"

// 0x10010d60 ??0MfPeCommand@@QAE@PAVMfPeEditorData@@@Z
MfPeCommand::MfPeCommand(MfPeEditorData *data)
    : mData(data)
{
    data->GetCursor(mPosition);
}

// 0x10010d90 ?Redo@MfPeCommand@@UAEXPAVMfPeDrawingContext@@@Z
void MfPeCommand::Redo(MfPeDrawingContext *context)
{
    mData->SetCursor(mPosition.x, mPosition.y);
    Do(context);
}
