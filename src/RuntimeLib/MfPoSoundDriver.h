// MfPoSoundDriver: the Piccolo class object "SoundDriver" (RuntimeLib.dll
// 10070e00-10071b20, class name from the RTTI): the volume and speed of sound
// of the sound driver, the listener node, and pausing/resuming all sounds.
#ifndef RUNTIMELIB_MFPOSOUNDDRIVER_H
#define RUNTIMELIB_MFPOSOUNDDRIVER_H
// the script class declarations of the shared headers (see MfScData.h)
#include "MfSoundManager.h"
#include "MfPoBase.h"
class MfScPropertyExtList;

class MfPoSoundDriver : public MfPoBase {
public:
    virtual ~MfPoSoundDriver();
    virtual MfPropertyErrorCode GetProperty(const MfString &, MfScDataHandle **);
    virtual MfPropertyErrorCode SetProperty(MfScDataHandle *, const MfString &, MfScDataHandle *, int);
    static void Initialize();
    static void Shutdown();
private:
    MfPoSoundDriver(MfScData::Exemplar);
    virtual MfString GetClassName();
    virtual MfScPropTokenTable *GetPropTokenTable() { return pttPropTokens; }
    virtual void FilloutPropertyNameList(MfStringList *);
    virtual MfScPropertyExtList *GetPropertyExtensions() { return pexlExtensions; }
    MfScDataHandle *PMStop(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMResume(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMStart(MfScDataHandle *, int, MfScDataHandle **);

    // set while the exemplar constructor registers the methods
    static int bConstructing;                               // 100f5a14
    static MfPoSoundDriver *poExemplar;                     // 100f5a18
    static MfScPropertyExtList *pexlExtensions;             // 100f5a1c
    static MfScPropTokenTable *pttPropTokens;               // 100f5a20
};

#endif
