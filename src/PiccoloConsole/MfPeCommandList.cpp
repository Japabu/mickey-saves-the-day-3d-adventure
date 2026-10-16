// PiccoloConsole.dll 0x10010dc0-0x10010e4f: MfPeCommandList, the commands of an editor.
#include "PiccoloConsole.h"

// 0x10010dc0 ??0MfPeCommandList@@QAE@XZ
MfPeCommandList::MfPeCommandList()
{
}

// 0x10010de0 ??_GMfPeCommandList@@UAEPAXI@Z

// 0x10010e00 ??1MfPeCommandList@@UAE@XZ
MfPeCommandList::~MfPeCommandList()
{
    Clear();
}

// The overrides have the same code as MfPeTextLineList's, which are kept.
void *MfPeCommandList::DeleteFirst()
{
    MfPeCommand *command = (MfPeCommand *)MfDblList::DeleteFirst();
    if (command)
        delete command;
    return 0;
}

void *MfPeCommandList::DeleteLast()
{
    MfPeCommand *command = (MfPeCommand *)MfDblList::DeleteLast();
    if (command)
        delete command;
    return 0;
}

void *MfPeCommandList::Delete(MfDblListItem *&pli)
{
    MfPeCommand *command = (MfPeCommand *)MfDblList::Delete(pli);
    if (command)
        delete command;
    return 0;
}

void MfPeCommandList::Clear()
{
    MfDblListItem *pli;
    for (MfPeCommand *command = (MfPeCommand *)First(pli); command; command = (MfPeCommand *)Next(pli))
        delete command;
    MfDblList::Clear();
}
