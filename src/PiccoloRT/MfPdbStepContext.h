// MfPdbStepContext: where a script thread stopped, handed to the debugger.
#pragma once
class MfScBytecodeRoutine;
class MfScVirtualMachine;
class MfScVmEnv;

// Where a script thread stopped, handed to the debugger.
struct MfPdbStepContext {
    MfScVmEnv *env;
    unsigned long *stackBase;       // the data stack when the VM was entered
    MfScBytecodeRoutine *routine;   // the routine that runs, if it is bytecode
    MfScVirtualMachine *vm;
};
