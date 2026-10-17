// MfAiSourceResolver: the "SourceDebugger" interface.
#pragma once
#include "MfAiFunctionRegistry.h"
#include "MfAiSourceType.h"
class MfString;

// "SourceDebugger": the source types registered by module type.
class MfAiSourceResolver : public MfAi {
public:
    virtual int Register(const MfString &type, MfAiSourceType *sourceType) = 0;
    virtual int Unregister(const MfString &type) = 0;
    virtual int Find(const MfString &type, MfAiSourceType **sourceType) = 0;
};
