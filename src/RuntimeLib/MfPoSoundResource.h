// MfPoSoundResource: the Piccolo object of a sound of the sound driver
// ("SoundResource"; RuntimeLib.dll 10071b20-100738c0, class name from the
// RTTI). A script creates one for an actor, loads a wave file into it and
// plays it; the properties are the driver's parameters of the sound.
#ifndef RUNTIMELIB_MFPOSOUNDRESOURCE_H
#define RUNTIMELIB_MFPOSOUNDRESOURCE_H
// the script class declarations of the shared headers (see MfScData.h)
#include "MfSoundManager.h"
#include "MfPoBase.h"
class MfScPropertyExtList;

class MfPoSoundResource : public MfPoBase {
public:
    MfPoSoundResource();
    virtual ~MfPoSoundResource();
    virtual MfPropertyErrorCode GetProperty(const MfString &, MfScDataHandle **);
    virtual MfPropertyErrorCode SetProperty(MfScDataHandle *, const MfString &, MfScDataHandle *, int);
    static void Initialize();
    static void Shutdown();
private:
    MfPoSoundResource(MfScData::Exemplar);
    virtual MfString GetClassName();
    virtual MfScPropTokenTable *GetPropTokenTable() { return pttPropTokens; }
    virtual void FilloutPropertyNameList(MfStringList *);
    virtual MfScPropertyExtList *GetPropertyExtensions() { return pexlExtensions; }
    MfScDataHandle *PMSoundResource(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMLoadFromFile(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMPlay(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMStop(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMResume(MfScDataHandle *, int, MfScDataHandle **);

    MfISoundResource *mResource;                            // 0x14
    MfAiActor *mActor;                                      // 0x18 the actor it sounds at

    static MfPoSoundResource *poExemplar;                   // 100f5a28
    static MfScPropertyExtList *pexlExtensions;             // 100f5a2c
    static MfScPropTokenTable *pttPropTokens;               // 100f5a30
};

#endif
