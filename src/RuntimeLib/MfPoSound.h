// MfPoSound: the Piccolo class object "Sound" (RuntimeLib.dll 1006f770-
// 10070e00, class name from the RTTI). The script plays wave files through
// the sound manager; most of the methods it registers are stubs.
#ifndef RUNTIMELIB_MFPOSOUND_H
#define RUNTIMELIB_MFPOSOUND_H
// the script class declarations of the shared headers (see MfScData.h)
#include "MfSoundManager.h"
#include "MfPoBase.h"

class MfPoSound : public MfPoBase {
public:
    virtual ~MfPoSound();
    virtual MfPropertyErrorCode GetProperty(const MfString &, MfScDataHandle **);
    virtual MfPropertyErrorCode SetProperty(MfScDataHandle *, const MfString &, MfScDataHandle *, int);
    static void Initialize();
    static void Shutdown();
    static MfString InitConstants();
private:
    MfPoSound(MfScData::Exemplar);
    virtual MfString GetClassName();
    MfScDataHandle *PMPlaySound(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMStopSound(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMBeep(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMGetSpeedOfSound(MfScDataHandle *, int, MfScDataHandle **);
    // the methods that do nothing: they answer 1 (LoadSound/UnloadSound -1)
    MfScDataHandle *PMNotImplemented(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMNotLoaded(MfScDataHandle *, int, MfScDataHandle **);

    // the sound played last
    MfISoundResource *mResource;                            // 0x14

    static MfPoSound *poExemplar;                           // 100f5a04
};

#endif
