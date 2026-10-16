// MfPoSpace: the Piccolo object of a space ("Space", MfPoSpace.cpp,
// RuntimeLib.dll 10075920-10078e30): the actors of the space, ray casts,
// collision checks, fog and the background material for scripts.
#ifndef RUNTIMELIB_MFPOSPACE_CLASS_H
#define RUNTIMELIB_MFPOSPACE_CLASS_H
#include "MfPoBase.h"
#include "MfSpace.h"

class MfPoSpace : public MfPoBase {
public:
    virtual ~MfPoSpace();
    static void Init();                                     // 100771f0
    static void Exit();                                     // 10077280
    static MfString InitConstants();                        // 10078d30

    MfScDataHandle *CastRay(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *CheckCollision(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *CheckSphereContainsGeometry(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *UpdateScene(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *SetFogAttributes(MfScDataHandle *, int, MfScDataHandle **);
    MfSpace *Space() { return (MfSpace *)mSpace; }

    MfAiSpace *mSpace;                                      // 0x14
    MfScDataHandle *mMaterial;                              // 0x18 the MfPoMaterial of the background

    MfPoSpace(MfAiSpace *space);                            // 100759a0
    virtual MfPropertyErrorCode GetProperty(const MfString &, MfScDataHandle **);
    virtual MfPropertyErrorCode SetProperty(MfScDataHandle *, const MfString &, MfScDataHandle *, int);
    MfScDataHandle *GetActorList(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *GetActor(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *AttachCameraView(MfScDataHandle *, int, MfScDataHandle **);
private:
    MfPoSpace(MfScData::Exemplar);                          // 10075aa0 the class object
    virtual MfString GetClassName();
    virtual MfScPropTokenTable *GetPropTokenTable();
    virtual void FilloutPropertyNameList(MfStringList *);
    virtual MfScPropertyExtList *GetPropertyExtensions();

    static MfPoSpace *poExemplar;                           // 100f5a50
    static MfScPropertyExtList *pexlExtensions;             // 100f5a54
    static MfScPropTokenTable *pttPropTokens;               // 100f5a58
};

#endif
