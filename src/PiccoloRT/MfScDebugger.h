// MfScDebugger: the debugger the runtime reports to.
#pragma once
#ifndef PICCOLORT_API
#define PICCOLORT_API __declspec(dllimport)
#endif
#include "MfAiFunctionRegistry.h"
class MfPdbListener;
class MfScBytecodeRoutine;
class MfScVirtualMachine;
class MfScVmEnv;
class MfString;
#include "MfPdbStepContext.h"
#include "MfPdbBreakpointInfo.h"
#include "MfAiPdbDebugger.h"
#include "MfAiPdbBreakpointManager.h"

// The debugger the runtime reports to, while one is installed.
class PICCOLORT_API MfScDebugger {
public:
    virtual ~MfScDebugger() {}
    static MfAiPdbDebugger *Debugger();
    static void InitDebuggerPointer(MfAiPdbDebugger *dbg);
    static void ClearDebuggerPointer();
private:
    friend class MfScPiccolo;
    friend class MfScVirtualMachine;
    static MfAiPdbDebugger *pdbgTheDebugger;
};
