// MfPoTransform: the Piccolo object of a transform ("Transform"; properties
// rotation, translation). It owns the transform unless it wraps one given by
// pointer.
#ifndef MFPOLIBRARY_MFPOTRANSFORM_H
#define MFPOLIBRARY_MFPOTRANSFORM_H
#include "MfPoLibrary_api.h"
#include "MfPoBase.h"
#include "MfTransform3.h"
class MfPoRotation3;
class MfPoVector3;

class MFPO_API MfPoTransform : public MfPoBase {
public:
    MfPoTransform();
    MfPoTransform(const MfTransform3 &);
    MfPoTransform(const MfPoTransform &);
    MfPoTransform(MfTransform3 *);
    virtual ~MfPoTransform();
    static void __cdecl Initialize();
    static void __cdecl Shutdown();
    static MfScPropTokenTable *__cdecl PropertyTokenTable() { return pttPropTokens; }
    virtual MfPropertyErrorCode GetProperty(const MfString &, MfScDataHandle **);
    virtual MfPropertyErrorCode SetProperty(MfScDataHandle *, const MfString &, MfScDataHandle *, int);
    MfTransform3 Transform() { return *_t3; }
private:
    MfPoTransform(MfPoRotation3 *rotation, MfPoVector3 *translation);
    MfPoTransform(MfScData::Exemplar);
    virtual MfString GetClassName();
    virtual MfScPropTokenTable *GetPropTokenTable() { return pttPropTokens; }
    virtual void FilloutPropertyNameList(MfStringList *);
    MfScDataHandle *PMTransform(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMCopy(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMInverse(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMMultiply(MfScDataHandle *, int, MfScDataHandle **);
    static MfPoTransform *poExemplar;
    static MfScPropTokenTable *pttPropTokens;
    MfTransform3 *_t3;                                       // 0x14
    int bOwner;                                             // 0x18
};

#endif
