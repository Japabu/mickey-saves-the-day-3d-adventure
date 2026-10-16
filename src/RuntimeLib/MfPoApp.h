// MfPoApp: the Piccolo class object "App" (RuntimeLib.dll 1007bcd0-1007c870,
// class name from the RTTI): requests to the application, passed on as hooks
// (open a URL, shut down, a user event).
#ifndef RUNTIMELIB_MFPOAPP_H
#define RUNTIMELIB_MFPOAPP_H
#include "MfPoBase.h"

class MfPoApp : public MfPoBase {
public:
    virtual ~MfPoApp();
    virtual MfPropertyErrorCode GetProperty(const MfString &, MfScDataHandle **);
    virtual MfPropertyErrorCode SetProperty(MfScDataHandle *, const MfString &, MfScDataHandle *, int);
    static void Initialize();
    static void Shutdown();
private:
    MfPoApp(MfScData::Exemplar);
    virtual MfString GetClassName();
    MfScDataHandle *PMOpenURL(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMShutdown(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMUserEvent(MfScDataHandle *, int, MfScDataHandle **);

    static MfPoApp *poExemplar;                             // 100f5a7c
};

#endif
