// MfPoColor: the Piccolo object of a colour value ("Color"; properties red,
// green, blue, alpha in [0, 1]).
#pragma once
#include "MfPoBase.h"
#include "MfColor.h"

class __declspec(dllexport) MfPoColor : public MfPoBase {
public:
    MfPoColor(MfColor color);
    virtual ~MfPoColor();
    static void __cdecl Initialize();
    static void __cdecl Shutdown();
    static MfScPropTokenTable *__cdecl PropertyTokenTable() { return pttPropTokens; }
    MfColor &Color() { return mColor; }
private:
    MfPoColor(MfScData::Exemplar);
    virtual MfPropertyErrorCode GetProperty(const MfString &, MfScDataHandle **);
    virtual MfPropertyErrorCode SetProperty(MfScDataHandle *, const MfString &, MfScDataHandle *, int);
    virtual MfString GetClassName();
    virtual MfScPropTokenTable *GetPropTokenTable() { return pttPropTokens; }
    virtual void FilloutPropertyNameList(MfStringList *);
    MfScDataHandle *PMColor(MfScDataHandle *, int, MfScDataHandle **);
    static MfPoColor *poExemplar;
    static MfScPropTokenTable *pttPropTokens;
    MfColor mColor;                                         // 0x14
};
