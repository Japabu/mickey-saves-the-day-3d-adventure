// MfAiApplication: the interface the runtime publishes for the application
// ("ApplicationI"); declaration as in src/plugins/PscDebugger/PscDebugger.h.
#ifndef RUNTIMELIB_MFAIAPPLICATION_H
#define RUNTIMELIB_MFAIAPPLICATION_H
#include "MfAi.h"

class MfAiApplication : public MfAi {
public:
    virtual int GetApplicationType() = 0;
};

#endif
