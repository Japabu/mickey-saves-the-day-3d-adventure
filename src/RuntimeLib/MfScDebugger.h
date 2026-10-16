// MfScDebugger (PiccoloRT.dll): RuntimeLib only reads the debugger it holds
// (a private static member; MfWorld is a friend here).
#ifndef RUNTIMELIB_MFSCDEBUGGER_H
#define RUNTIMELIB_MFSCDEBUGGER_H

// the Piccolo debugger interface (only the slot RuntimeLib calls is known)
class MfAiPdbDebugger {
public:
    virtual void v00();
    virtual void v04();
    virtual void v08();
    virtual void v0c();
    virtual void v10();
    virtual void v14();
    virtual void v18();
    virtual void v1c();
    virtual void Suspend(int suspend);                      // +0x20
};

class MfScDebugger {
    friend class MfWorld;
private:
    __declspec(dllimport) static MfAiPdbDebugger *pdbgTheDebugger;
};

#endif
