// MfDblListItem: one link of an MfDblList (mfutil.dll; not exported, all inline).
// Member names from the type records in SteamboatApp.pdb.
#ifndef MFUTIL_MFDBLLISTITEM_H
#define MFUTIL_MFDBLLISTITEM_H

class MfDblListItem {
public:
    MfDblListItem(void *p) { pliPrev = 0; pData = p; pliNext = 0; }
    void *GetData() const { return pData; }
    MfDblListItem *GetPrev() const { return pliPrev; }
    MfDblListItem *GetNext() const { return pliNext; }
    void SetData(void *p) { pData = p; }
    void SetPrev(MfDblListItem *p) { pliPrev = p; }
    void SetNext(MfDblListItem *p) { pliNext = p; }
    void SetLinks(MfDblListItem *prev, MfDblListItem *next)
    {
        pliPrev = prev;
        pliNext = next;
        if (pliPrev) pliPrev->pliNext = this;
        if (pliNext) pliNext->pliPrev = this;
    }

    void *pData;            // +0
    MfDblListItem *pliPrev; // +4
    MfDblListItem *pliNext; // +8
};

#endif
