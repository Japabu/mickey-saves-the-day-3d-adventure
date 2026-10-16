// MfPoPath2: the Piccolo object of a path ("Path2").
#pragma once
#include "MfPoBase.h"
class MfAiPath2;

class __declspec(dllexport) MfPoPath2 : public MfPoBase {
public:
    MfPoPath2(MfAiPath2 *path);
    virtual ~MfPoPath2();
    static void __cdecl Initialize();
    static void __cdecl Shutdown();
    static MfScPropTokenTable *__cdecl PropertyTokenTable() { return pttPropTokens; }
    MfAiPath2 *Path() { return mPath; }
private:
    virtual MfPropertyErrorCode GetProperty(const MfString &, MfScDataHandle **);
    virtual MfPropertyErrorCode SetProperty(MfScDataHandle *, const MfString &, MfScDataHandle *, int);
    MfPoPath2(MfScData::Exemplar);
    virtual MfString GetClassName();
    virtual MfScPropTokenTable *GetPropTokenTable() { return pttPropTokens; }
    virtual void FilloutPropertyNameList(MfStringList *);
    MfScDataHandle *PMGetTransform(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMSetTransform(MfScDataHandle *, int, MfScDataHandle **);
    static MfPoPath2 *poExemplar;
    static MfScPropTokenTable *pttPropTokens;
    MfAiPath2 *mPath;                                  // 0x14
};
