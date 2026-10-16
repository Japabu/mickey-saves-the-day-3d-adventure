// MfIntList (mfutil.dll, 0x100049a0-0x10004a40)
#include "MfIntList_unit.h"

// 0x100049a0 ?GetPrev@MfDblList@@UBEPAXAAPAVMfDblListItem@@@Z (original ?Prev@MfIntList@@QAEHAAPAVMfDblListItem@@@Z)
// (folded with ?GetPrev@MfDblList and ?Prev@MfStringList)
int MfIntList::Prev(MfDblListItem *&pli)
{
    if (pli)
        pli = pli->GetPrev();
    return pli ? (int)pli->GetData() : 0;
}

// 0x100049c0 ??0MfIntList@@QAE@XZ
MfIntList::MfIntList()
{
}

// 0x100049e0 ??_EMfIntList@@UAEPAXI@Z  (compiler generated)

// 0x10004a30 ??1MfIntList@@UAE@XZ
MfIntList::~MfIntList()
{
}
