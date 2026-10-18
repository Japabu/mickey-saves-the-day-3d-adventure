// PiccoloConsole.dll 0x1001bbc0-0x1001bcff: MfPeTextLineList, the lines of an
// editor; deleting a line from the list deletes the line.
#include "PiccoloConsole.h"

// 0x1001bbc0 ??0MfPeTextLineList@@QAE@XZ
MfPeTextLineList::MfPeTextLineList()
{
}

// 0x1001bbe0 ??_GMfPeTextLineList@@UAEPAXI@Z

// 0x1001bc00 ??1MfPeTextLineList@@UAE@XZ
MfPeTextLineList::~MfPeTextLineList()
{
    Clear();
}

// TODO 0x1001bc50 ?Clear@MfPeTextLineList@@UAEXXZ: 92% - the original copies the next line into a second register and tests it again before the delete
void MfPeTextLineList::Clear()
{
    MfDblListItem *item;
    for (MfPeTextLine *line = (MfPeTextLine *)MfDblList::First(item); line; line = (MfPeTextLine *)MfDblList::Next(item))
        delete line;
    MfDblList::Clear();
}

// 0x1001bca0 ?DeleteFirst@MfPeTextLineList@@UAEPAXXZ
void *MfPeTextLineList::DeleteFirst()
{
    delete (MfPeTextLine *)MfDblList::DeleteFirst();
    return 0;
}

// 0x1001bcc0 ?DeleteLast@MfPeTextLineList@@UAEPAXXZ
void *MfPeTextLineList::DeleteLast()
{
    delete (MfPeTextLine *)MfDblList::DeleteLast();
    return 0;
}

// 0x1001bce0 ?Delete@MfPeTextLineList@@UAEPAXAAPAVMfDblListItem@@@Z
void *MfPeTextLineList::Delete(MfDblListItem *&item)
{
    delete (MfPeTextLine *)MfDblList::Delete(item);
    return 0;
}
