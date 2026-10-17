// MfPoList: the Piccolo object of a list property value ("PropertyList"),
// iterated with First/Next/... through the list's cursor.
#ifndef MFPOLIBRARY_MFPOLIST_H
#define MFPOLIBRARY_MFPOLIST_H
#ifndef MFPO_API
#define MFPO_API __declspec(dllimport)
#endif
#include "MfScObject.h"
#include "MfListValue.h"

class MFPO_API MfPoList : public MfScObject {
public:
    MfPoList(MfListValue *list);
    virtual ~MfPoList();
    static void __cdecl Initialize();
    static void __cdecl Shutdown();
    static MfScPropTokenTable *__cdecl PropertyTokenTable() { return pttPropTokens; }
    virtual MfPropertyErrorCode GetProperty(const MfString &, MfScDataHandle **);
    virtual MfPropertyErrorCode SetProperty(MfScDataHandle *, const MfString &, MfScDataHandle *, int);
    MfListValue *ListValue() const { return mList; }
private:
    MfPoList(MfScData::Exemplar);
    virtual MfString GetClassName();
    virtual MfScPropTokenTable *GetPropTokenTable() { return pttPropTokens; }
    virtual void FilloutPropertyNameList(MfStringList *);
    MfScDataHandle *PMClear(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMFirst(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMNext(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMCurrent(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMLast(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMPrev(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMAppend(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMPrepend(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMDelete(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMGetItem(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMSetItem(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMFind(MfScDataHandle *, int, MfScDataHandle **);
    int IsValidSearchItem(MfScDataHandle *);
    int SearchListForItem(MfTypedValue *, int);
    static MfPoList *poExemplar;
    static MfScPropTokenTable *pttPropTokens;
    MfListValue *mList;                                     // 0x14
};

#endif
