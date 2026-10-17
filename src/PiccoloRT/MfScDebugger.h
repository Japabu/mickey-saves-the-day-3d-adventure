// MfScDebugger: the debugger the runtime reports to.
#pragma once
#include "PiccoloRT_api.h"
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
