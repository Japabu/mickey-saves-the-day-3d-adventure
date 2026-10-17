// MfScVmEnv: the execution state of one script thread.
#pragma once
#include "PiccoloRT_api.h"
#include "MfScVariableStack.h"
#include "MfString.h"
#include "MfScDataHandle.h"
#include "MfHPTimer.h"
class MfScDataHandle;
class MfScHandleDict;
class MfScHandleDictIterator;
class MfScVmCallStackList;
class MfScVmFrame;
class MfScVmStack;
class MfString;
struct _RTL_CRITICAL_SECTION;

// The pending exception of an MfScVmEnv.
class MfScVmException {
public:
    MfScVmException(int code, const MfString &message) : mCode(code), mMessage(message) {}
    virtual ~MfScVmException() {}
    int Code() const { return mCode; }
    const MfString &Message() const { return mMessage; }
    int mCode;                  // +0x04
    MfString mMessage;          // +0x08
};

// The execution state of one script thread: data stack, current call frame,
// variable pools and pending exception. Call frames are pushed on the data
// stack as ip, object, function, constant pool and the previous frame pointer
// (MfScVirtualMachine::ExecuteCode reads and writes the state directly).
class PICCOLORT_API MfScVmEnv {
public:
    MfScVmEnv();
    virtual ~MfScVmEnv();

    MfScVariableStack::MfScVariableErrorCode GetVariable(const MfString &name, MfScDataHandle **h) const;
    MfScVariableStack::MfScVariableErrorCode SetVariable(const MfString &name, MfScDataHandle *h, int passRef, int global);
    static MfScVariableStack::MfScVariableErrorCode __cdecl GetGlobalVariable(const MfString &, MfScDataHandle **);
    static MfScVariableStack::MfScVariableErrorCode __cdecl SetGlobalVariable(const MfString &, MfScDataHandle *, int, int);
    static MfScVariableStack::MfScVariableErrorCode __cdecl ClearGlobalVariable(const MfString &, int);
    static void __cdecl EnumGlobalHandles(MfScHandleDictIterator &);
    static void __cdecl Shutdown();

    int PushFunctionFrame();
    void PopFunctionFrame();
    void ReleaseStack(unsigned long *to);
    int FirstFrame(MfScVmFrame *frame) const;
    int NextFrame(MfScVmFrame *frame) const;
    void StoreFrame(MfScVmFrame *frame);
    void RestoreFrame(MfScVmFrame *frame);
    void GetCurrentCallStack(MfScVmCallStackList *);
    void ThrowException(int code, const MfString &message);
    void ClearException();
    void ResetStackOverflow();

    MfScDataHandle *GetSPHandle(int i) const { return m_sp[i]; }
    void SetSPHandle(int i, MfScDataHandle *h) { m_sp[i] = h; }
    void ClearScratchREG0() { if (m_reg0) { m_reg0->Release(); m_reg0 = 0; } }
    void ClearScratchREG1() { if (m_reg1) { m_reg1->Release(); m_reg1 = 0; } }
    void ClearScratchREG2() { if (m_reg2) { m_reg2->Release(); m_reg2 = 0; } }
    void ClearScratchREG3() { if (m_reg3) { m_reg3->Release(); m_reg3 = 0; } }
    void ClearScratchRegisters()
    {
        ClearScratchREG0();
        ClearScratchREG1();
        ClearScratchREG2();
        ClearScratchREG3();
    }
    MfScVariableStack *SetVariableStack(MfScVariableStack *stack)
    {
        MfScVariableStack *old = m_varStack;
        m_varStack = stack;
        return old;
    }
    MfScVmException *Exception() const { return m_exception; }
    void Interrupt() { m_interrupted = 1; }
    void ClearInterrupt() { m_interrupted = 0; }
    int IsInterrupted() const { return m_interrupted; }
    int ArtnValue() const { return m_artnValue; }
    void ArtnSetReturn(int value) { m_artnSet = 1; m_artnValue = value; }
    void ArtnClearReturn() { m_artnSet = 0; m_artnValue = 0; }
    int ArtnIsSet() const { return m_artnSet; }
    int IsMessageThread() const { return m_messageThread; }
    void SetAsMessageThread() { m_messageThread = 1; }
    int IsExecuting() { return m_executing; }
    void FlagStartExecuting() { m_executing = 1; }
    void FlagStopExecuting() { m_executing = 0; }
    int Id() { return m_id; }
    void StartProcessTimer() { m_timer.Start(); }
    void RefreshProcessTimer()
    {
        m_timer.Stop();
        m_accumTime += m_timer.m_stop.QuadPart - m_timer.m_start.QuadPart;
    }
    __int64 GetAccumProcessTime() { return m_accumTime; }

    void PushWord(unsigned long w)
    {
        *(unsigned long *)m_sp = w;
        m_sp--;
    }
    unsigned long PopWord() { return *(unsigned long *)++m_sp; }
    void PushData(MfScDataHandle *h)
    {
        if (h)
            h->AddRef();
        *m_sp = h;
        m_sp--;
    }
    void PushDataPassRef(MfScDataHandle *h)
    {
        *m_sp = h;
        m_sp--;
    }
    MfScDataHandle *PopDataPassRef() { return *++m_sp; }
    void PushDataNoCheck(MfScDataHandle *h)
    {
        h->AddRef();
        *m_sp = h;
        m_sp--;
    }
    void PopAndReleaseData()
    {
        MfScDataHandle *h = *++m_sp;
        if (h)
            h->Release();
    }
    void PopAndReleaseDataNoCheck()
    {
        MfScDataHandle *h = *++m_sp;
        h->Release();
    }

private:
    static _RTL_CRITICAL_SECTION csGuard;
    static int iIdPool;
    static MfScHandleDict *varpool;

public:
    unsigned char *m_ip;            // +0x08 the next bytecode
    MfScDataHandle **m_sp;          // +0x0c the data stack (grows down, points at the free slot)
    unsigned long *m_frame;         // +0x10 the current call frame (FP)
    MfScDataHandle **m_constants;   // +0x14 the running routine's constant pool (CPP)
    MfScDataHandle *m_object;       // +0x18 the object the routine runs on ("this")
    MfScDataHandle *m_function;     // +0x1c the running routine
    MfScDataHandle *m_reg0;         // +0x20
    MfScDataHandle *m_reg1;         // +0x24
    MfScDataHandle *m_reg2;         // +0x28
    MfScDataHandle *m_reg3;         // +0x2c
    MfScVariableStack *m_varStack;  // +0x30
    unsigned long *m_stackBase;     // +0x34
    long m_stackSize;               // +0x38
    MfScVmStack *m_stack;           // +0x3c
    MfScVmException *m_exception;   // +0x40
    int m_artnSet;                  // +0x44
    int m_artnValue;                // +0x48
    int m_messageThread;            // +0x4c
    int m_interrupted;              // +0x50
    int m_executing;                // +0x54
    int m_id;                       // +0x58
    MfHPTimer m_timer;              // +0x60 the time spent executing
    __int64 m_accumTime;            // +0x70
};
