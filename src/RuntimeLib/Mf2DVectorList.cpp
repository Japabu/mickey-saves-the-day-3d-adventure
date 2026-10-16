// Mf2DVectorList (RuntimeLib.dll 1007dc50-1007dd30).
#include "Mf2DVectorList.h"

// 0x1007dc50 ??0Mf2DVectorList@@QAE@H@Z
Mf2DVectorList::Mf2DVectorList(int ownsItems)
    : mOwnsItems(ownsItems)
{
}

// 0x1007dc90 ??1Mf2DVectorList@@UAE@XZ

// 0x1007dce0 ?Clear@Mf2DVectorList@@UAEXXZ
void Mf2DVectorList::Clear()
{
    if (mOwnsItems == 1) {
        MfDblListItem *pli;
        for (Mf2DVector *p = (Mf2DVector *)GetFirst(pli); p; p = (Mf2DVector *)GetNext(pli))
            delete p;
    }
    MfDblList::Clear();
}

// (folded with the other empty functions at 1007dd30)
Mf2DVector::~Mf2DVector()
{
}
