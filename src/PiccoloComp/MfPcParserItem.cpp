// PiccoloComp.dll MfPcParserItem.cpp (0x1000e2e0-0x1000e420)
#include "MfPcParserItem.h"

MfDblList *MfPcParserItem::pdlItems;

// 0x1000e2e0 ??0MfPcParserItem@@QAE@XZ
MfPcParserItem::MfPcParserItem()
    : mRegistered(1), mKeep(0)
{
    pdlItems->Add(this);
}

// 0x1000e310 ??_GMfPcParserItem@@UAEPAXI@Z (compiler-generated)

// 0x1000e330 ??1MfPcParserItem@@UAE@XZ
MfPcParserItem::~MfPcParserItem()
{
    if (mRegistered == 1) {
        MfDblListItem *pli = pdlItems->Find(this);
        pdlItems->Delete(pli);
    }
}

// 0x1000e370 ?Initialize@MfPcParserItem@@SAXXZ
void MfPcParserItem::Initialize()
{
    if (!pdlItems)
        pdlItems = new MfDblList;
}

// 0x1000e3b0 ?FreeItems@MfPcParserItem@@SAXXZ
void MfPcParserItem::FreeItems()
{
    MfDblListItem *pli;
    MfPcParserItem *item = (MfPcParserItem *)pdlItems->GetFirst(pli);
    while (item) {
        if (!item->mKeep) {
            item->mRegistered = 0;
            delete item;
            pdlItems->Delete(pli);
            item = (MfPcParserItem *)pdlItems->GetCurrent(pli);
        } else
            item = (MfPcParserItem *)pdlItems->GetNext(pli);
    }
}
