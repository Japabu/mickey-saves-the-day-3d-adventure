// MfPoMaterial: the Piccolo object of a material ("Material"). It deletes
// the material when it owns it.
#ifndef MFPOLIBRARY_MFPOMATERIAL_H
#define MFPOLIBRARY_MFPOMATERIAL_H
#ifndef MFPO_API
#define MFPO_API __declspec(dllimport)
#endif
#include "MfPoBase.h"
class MfMaterial;

class MFPO_API MfPoMaterial : public MfPoBase {
public:
    MfPoMaterial(MfMaterial *material, int owner);
    virtual ~MfPoMaterial();
    static void __cdecl Initialize();
    static void __cdecl Shutdown();
    static MfString __cdecl InitConstants();
    static MfScPropTokenTable *__cdecl PropertyTokenTable() { return pttPropTokens; }
    virtual MfPropertyErrorCode GetProperty(const MfString &, MfScDataHandle **);
    virtual MfPropertyErrorCode SetProperty(MfScDataHandle *, const MfString &, MfScDataHandle *, int);
    MfMaterial *Material() { return mMaterial; }
private:
    MfPoMaterial(MfScData::Exemplar);
    virtual MfString GetClassName();
    virtual MfScPropTokenTable *GetPropTokenTable() { return pttPropTokens; }
    virtual void FilloutPropertyNameList(MfStringList *);
    virtual MfScPropertyExtList *GetPropertyExtensions() { return pelPropExtensions; }
    MfScDataHandle *PMGetSpecularColor(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMGetColor(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMSetSpecularColor(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMSetColor(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMGetSurface(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMSetSurface(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMCreateTextureCopy(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMSetTexture(MfScDataHandle *, int, MfScDataHandle **);
    static MfPoMaterial *poExemplar;
    static MfScPropertyExtList *pelPropExtensions;
    static MfScPropTokenTable *pttPropTokens;
    MfMaterial *mMaterial;                                  // 0x14
    int bOwner;                                             // 0x18
};

#endif
