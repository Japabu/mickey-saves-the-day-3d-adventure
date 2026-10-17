// MfAiPdbBreakpointManager: the "PiccoloBreakpointManager" interface of the debugger plugin.
#pragma once
#include "MfAiFunctionRegistry.h"   // MfAi
#include "MfPdbBreakpointInfo.h"
class MfScVmEnv;
class MfString;

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
    virtual int BreakpointHit(unsigned long address, int, MfScVmEnv *env) = 0;
    virtual int GetBreakpointInfo(unsigned long address, int, MfPdbBreakpointInfo *info) = 0;
    virtual int PlantModule(const MfString &module) = 0;
    virtual int Save() = 0;
};
