// PiccoloRT.dll 0x10016310-0x10016400: the out-of-line members of
// MfScPropTokenTable, MfScPropertyExt and MfScPropertyExtList.
#include "MfScPropTokenTable.h"
#include "MfScPropToken.h"
#include "MfScPropertyExt.h"

// 0x10016310 ?CallBack@MfScPropTokenTable@@UAEHPAX@Z
int MfScPropTokenTable::CallBack(void *item)
{
    MfScPropToken *token = (MfScPropToken *)item;
    if (token)
        delete token;
    return 1;
}

// 0x10016340 ?LookupPropertyToken@MfScPropertyExt@@QAEHABVMfString@@@Z
int MfScPropertyExt::LookupPropertyToken(MfString const &property)
{
    int token = -1;
    MfScPropTokenTable *table = GetPropTokenTable();
    if (table) {
        MfScPropToken *t = table->Find(property);
        if (t)
            token = t->Token();
    }
    return token;
}

// 0x10016370 ?Clear@MfScPropertyExtList@@UAEXXZ
void MfScPropertyExtList::Clear()
{
    MfDblListItem *pos;
    for (MfScPropertyExt *ext = First(pos); ext; ext = Next(pos))
        delete (void *)ext;
    MfDblList::Clear();
}

// 0x100163b0 ?Delete@MfScPropertyExtList@@UAEPAXAAPAVMfDblListItem@@@Z
void *MfScPropertyExtList::Delete(MfDblListItem *&pos)
{
    delete MfDblList::Delete(pos);
    return 0;
}

// 0x100163d0 ?DeleteFirst@MfScPropertyExtList@@UAEPAXXZ
void *MfScPropertyExtList::DeleteFirst()
{
    delete MfDblList::DeleteFirst();
    return 0;
}

// 0x100163f0 ?DeleteLast@MfScPropertyExtList@@UAEPAXXZ
void *MfScPropertyExtList::DeleteLast()
{
    delete MfDblList::DeleteLast();
    return 0;
}
