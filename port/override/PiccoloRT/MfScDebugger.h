// PORT: stack words are uintptr_t (pointer-sized): the 32-bit source used unsigned long, which was the size of a pointer there.
#pragma once
#include <stdint.h> // PORT: uintptr_t
#include "MfAiFunctionRegistry.h"
class MfPdbListener;
class MfScBytecodeRoutine;
class MfScVirtualMachine;
class MfScVmEnv;
class MfString;

// Where a script thread stopped, handed to the debugger.
struct MfPdbStepContext {
    MfScVmEnv *env;
    uintptr_t *stackBase;           // PORT: uintptr_t; the data stack when the VM was entered
    MfScBytecodeRoutine *routine;   // the routine that runs, if it is bytecode
    MfScVirtualMachine *vm;
};

// What MfAiPdbBreakpointManager::GetBreakpointInfo reports about a planted
// breakpoint.
struct MfPdbBreakpointInfo {
    unsigned long data;
    unsigned char bytecode;         // the bytecode the breakpoint replaced
    void *entry;
};

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

// "PiccoloBreakpointManager": the debugger's breakpoints (planted as
// BC_BREAKPOINT bytecodes).
class MfAiPdbBreakpointManager : public MfAi {
public:
    virtual int AddBreakpoint(const MfString &module, int line) = 0;
    virtual int RemoveBreakpoint(const MfString &module, int line) = 0;
    virtual int ToggleBreakpoint(const MfString &module, int line) = 0;
    virtual int EnableBreakpoint(const MfString &module, int line) = 0;
    virtual int DisableBreakpoint(const MfString &module, int line) = 0;
    virtual int ToggleEnabled(const MfString &module, int line) = 0;
    virtual void RemoveAll() = 0;
    virtual void RemoveModule(const MfString &module) = 0;
    virtual void DisableAll() = 0;
    virtual void DisableModule(const MfString &module) = 0;
    virtual int PlantAll() = 0;
    virtual int UnplantAll() = 0;
    // a breakpoint at address was passed: plants it again
    // PORT: the bytecode address is uintptr_t (pointer-sized)
    virtual int BreakpointHit(uintptr_t address, int, MfScVmEnv *env) = 0;
    virtual int GetBreakpointInfo(uintptr_t address, int, MfPdbBreakpointInfo *info) = 0;
    virtual int PlantModule(const MfString &module) = 0;
    virtual int Save() = 0;
};

// The debugger the runtime reports to, while one is installed.
class __declspec(dllexport) MfScDebugger {
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
