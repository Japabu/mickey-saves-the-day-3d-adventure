// MfSortedList (mfutil.dll, 0x1000bc90-0x1000bde0)
#include "MfSortedList_unit.h"

// 0x1000bc90 ??0MfSortedList@@QAE@XZ  (inline in header)
// 0x1000bcb0 ??1MfSortedList@@UAE@XZ  (inline in header)
// 0x1000bcc0 ??_EMfSortedList@@UAEPAXI@Z  (compiler generated)

// 0x1000bd10 ?Insert@MfSortedList@@UAEXPAXW4EnumListPosition@MfDblList@@PAVMfDblListItem@@@Z
void MfSortedList::Insert(void *pData, EnumListPosition pos, MfDblListItem *pli)
{
    MfString key = ((MfSortedListItem *)pData)->GetSortKey();
    pli = pliFirst;
    for (void *p = pli ? pli->pData : 0; p;
         p = pli && (pli = pli->pliNext) != 0 ? pli->pData : 0) {
        MfString s = ((MfSortedListItem *)p)->GetSortKey();
        if (key < s)
            break;
    }
    if (!pli)
        MfDblList::Insert(pData, LP_AFTER, 0);
    else
        MfDblList::Insert(pData, LP_BEFORE, pli);
}
