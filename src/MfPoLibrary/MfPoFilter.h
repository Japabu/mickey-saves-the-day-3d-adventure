// MfPoFilter: the Piccolo object of a collision filter entry ("Filter").
#ifndef MFPOLIBRARY_MFPOFILTER_H
#define MFPOLIBRARY_MFPOFILTER_H
#include "MfPoLibrary_api.h"
#include "MfPoBase.h"
#include "MfFilterItem.h"

class MFPO_API MfPoFilter : public MfPoBase {
public:
    MfPoFilter(MfFilterItem *item);
    virtual ~MfPoFilter();
    static void __cdecl Initialize();
    static void __cdecl Shutdown();
    static MfString __cdecl InitConstants();
    static MfScPropTokenTable *__cdecl PropertyTokenTable() { return pttPropTokens; }
    MfFilterItem *Filter() { return mFilter; }
private:
    virtual MfPropertyErrorCode GetProperty(const MfString &, MfScDataHandle **);
    virtual MfPropertyErrorCode SetProperty(MfScDataHandle *, const MfString &, MfScDataHandle *, int);
    MfPoFilter(MfScData::Exemplar);
    virtual MfString GetClassName();
    virtual MfScPropTokenTable *GetPropTokenTable() { return pttPropTokens; }
    virtual void FilloutPropertyNameList(MfStringList *);
    MfScDataHandle *PMFilter(MfScDataHandle *, int, MfScDataHandle **);
    static MfPoFilter *poExemplar;
    static MfScPropTokenTable *pttPropTokens;
    MfFilterItem *mFilter;                                  // 0x14
};

#endif
