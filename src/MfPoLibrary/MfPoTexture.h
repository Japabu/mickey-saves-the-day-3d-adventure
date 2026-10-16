// MfPoTexture: the Piccolo object of a texture reference of a material
// ("Texture"; properties name, type, flags).
#pragma once
#include "MfPoBase.h"
#include "MfMaterial.h"

class __declspec(dllexport) MfPoTexture : public MfPoBase {
public:
    MfPoTexture(MfTexture texture);
    virtual ~MfPoTexture();
    static void __cdecl Initialize();
    static void __cdecl Shutdown();
    static MfString __cdecl InitConstants();
    static MfScPropTokenTable *__cdecl PropertyTokenTable() { return pttPropTokens; }
    MfTexture &Texture() { return mTexture; }
private:
    MfPoTexture(MfScData::Exemplar);
    virtual MfPropertyErrorCode GetProperty(const MfString &, MfScDataHandle **);
    virtual MfPropertyErrorCode SetProperty(MfScDataHandle *, const MfString &, MfScDataHandle *, int);
    virtual MfString GetClassName();
    virtual MfScPropTokenTable *GetPropTokenTable() { return pttPropTokens; }
    virtual void FilloutPropertyNameList(MfStringList *);
    MfScDataHandle *PMTexture(MfScDataHandle *, int, MfScDataHandle **);
    static MfPoTexture *poExemplar;
    static MfScPropTokenTable *pttPropTokens;
    MfTexture mTexture;                                     // 0x14
};
