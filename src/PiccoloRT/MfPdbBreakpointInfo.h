// MfPdbBreakpointInfo: what the debugger's breakpoint manager reports about a breakpoint.
#pragma once

// What MfAiPdbBreakpointManager::GetBreakpointInfo reports about a planted
// breakpoint.
struct MfPdbBreakpointInfo {
    unsigned long data;
    unsigned char bytecode;         // the bytecode the breakpoint replaced
    void *entry;
};
