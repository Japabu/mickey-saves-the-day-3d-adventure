// MfAiPdbDebugger: the "PiccoloDebugger" interface of the debugger plugin.
#pragma once
#include "MfAiFunctionRegistry.h"   // MfAi
#include "MfPdbStepContext.h"
class MfPdbListener;
class MfString;

// "PiccoloDebugger": the debugger plugin's interface.
class MfAiPdbDebugger : public MfAi {
public:
    virtual int SignalBreakpoint(MfPdbStepContext *ctx, unsigned char *bytecode) = 0;
    virtual int SignalException(int code, const MfString &message, MfPdbStepContext *ctx) = 0;
    virtual void Initialize() = 0;
    virtual void Shutdown() = 0;
    virtual int IsActive() = 0;
    virtual void ProgramState(int state) = 0;
    virtual int GetInterface(int id, MfAi **ai) = 0;
    virtual void AddListener(MfPdbListener *listener) = 0;
    virtual void RemoveListener(MfPdbListener *listener) = 0;
};

// The interface ids MfAiPdbDebugger::GetInterface knows.
enum { kPdbBreakpointManager = 1 };
