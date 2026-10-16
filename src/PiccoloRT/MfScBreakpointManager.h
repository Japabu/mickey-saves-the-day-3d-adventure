#pragma once

// The debugger's breakpoints (one manager while the runtime runs).
class __declspec(dllexport) MfScBreakpointManager {
public:
    static MfScBreakpointManager *Manager() { return pbrkptmgrMain; }
    static void __cdecl Startup();
    static void __cdecl Shutdown();
private:
    MfScBreakpointManager();
    virtual ~MfScBreakpointManager();
    static MfScBreakpointManager *pbrkptmgrMain;
};
