// MfPoMaterial (MfPoLibrary.dll): the Piccolo object of a material. What
// RuntimeLib uses (MfPoSpace's background material); declaration as in
// src/MfPoLibrary/MfPoMaterial.h, whose inline virtuals RuntimeLib's local
// vtable copy reaches through the import thunks, so they are out of line here.
#ifndef RUNTIMELIB_MFPOMATERIAL_H
#define RUNTIMELIB_MFPOMATERIAL_H
#include "MfPoBase.h"
class MfMaterial;

class __declspec(dllimport) MfPoMaterial : public MfPoBase {
public:
    MfPoMaterial(MfMaterial *material, int owner);
    virtual ~MfPoMaterial();
    virtual MfPropertyErrorCode GetProperty(const MfString &, MfScDataHandle **);
    virtual MfPropertyErrorCode SetProperty(MfScDataHandle *, const MfString &, MfScDataHandle *, int);
    MfMaterial *Material() { return mMaterial; }
private:
    virtual MfString GetClassName();
    virtual MfScPropTokenTable *GetPropTokenTable();
    virtual void FilloutPropertyNameList(MfStringList *);
    virtual MfScPropertyExtList *GetPropertyExtensions();
public:
    MfMaterial *mMaterial;                                  // 0x14
    int bOwner;                                             // 0x18 deletes the material
};

#endif
