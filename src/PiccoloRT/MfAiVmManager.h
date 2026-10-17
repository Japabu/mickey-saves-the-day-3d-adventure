// MfAiVmManager: the "VMManagerI" interface.
#pragma once
#include "MfAiFunctionRegistry.h"   // MfAi
class MfScVmEnv;

// "VMManagerI": the script environments (threads) that exist.
class MfAiVmManager : public MfAi {
public:
    virtual int Count() = 0;
    virtual MfScVmEnv *First() = 0;
    virtual MfScVmEnv *Next() = 0;
};
