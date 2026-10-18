// PiccoloRT.dll 0x10016d60-0x1001b8af: the bytecode interpreter.
#include <stdarg.h>
#include <windows.h>
#include <eh.h>
#include "MfScVirtualMachine.h"
#include "MfScVmEnv.h"
#include "MfScThread.h"
#include "MfScDataMethod.h"
#include "MfScBytecodeRoutine.h"
#include "MfScDebugInfo.h"
#include "MfScVmFrame.h"
#include "MfStringList.h"
#include "MfString.h"
#include "MfScOpcodes.h"
#include "MfScBytecodes.h"
#include "MfScObject.h"
#include "MfScDataArray.h"
#include "MfScDataThread.h"
#include "MfScRoutine.h"
#include "MfScDebugger.h"
#include "MfScThreadManager.h"
#include "MfScDiagnostics.h"
#include "MfLogger.h"

extern __declspec(dllimport) MfNullString NULL_STRING;

// 0x10016d60 _$E35 (static initializer of csThreadBreakLock, with the three below)
// 0x10016d70 _$E31
// 0x10016d80 _$E34
// 0x10016d90 _$E32
MfCriticalSection MfScVirtualMachine::csThreadBreakLock;

// a system exception raised while the interpreter runs
class VmSEHandler {
public:
    VmSEHandler(unsigned int code) : mCode(code) {}
    VmSEHandler(const VmSEHandler &other) : mCode(other.mCode) {}
    unsigned int Code() const { return mCode; }
    unsigned int mCode;
};

// 0x10016db0 ?TranslateSystemException@@YAXIPAU_EXCEPTION_POINTERS@@@Z
// turns the system exceptions the interpreter handles into C++ exceptions
// (installed with _set_se_translator)
void TranslateSystemException(unsigned int code, _EXCEPTION_POINTERS *)
{
    if (code == EXCEPTION_FLT_DIVIDE_BY_ZERO || code == EXCEPTION_INT_DIVIDE_BY_ZERO
        || code == STATUS_GUARD_PAGE_VIOLATION)
        throw VmSEHandler(code);
}

// 0x10016de0 ??0VmSEHandler@@QAE@ABV0@@Z

// 0x10016df0 ??0MfScVirtualMachine@@QAE@XZ
MfScVirtualMachine::MfScVirtualMachine()
{
}

// 0x10016e00 ??1MfScVirtualMachine@@UAE@XZ
MfScVirtualMachine::~MfScVirtualMachine()
{
}

// 0x10016e10 ?ExecuteFunctionArrayParams@MfScVirtualMachine@@QAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfScVirtualMachine::ExecuteFunctionArrayParams(MfScDataHandle *function, int numArgs,
                                                               MfScDataHandle **args)
{
    MfScDataMethod *method = (MfScDataMethod *)function->GetData();
    MfScVmEnv *env = ((MfScThread *)MfThread::GetThread())->mVmEnv;
    env->m_sp -= numArgs;
    if (args) {
        for (int i = 1; i <= numArgs; i++)
            env->m_sp[i] = args[i - 1];
    }
    env->PushWord((unsigned long)MfScDataHandle::CreateData(numArgs));
    env->PushData(function);
    // call the function, then return
    unsigned char code[2];
    code[0] = BC_INVOKEFUNC;
    code[1] = BC_EXIT;
    MfScDataHandle *result;
    if (!ExecuteCode(code, method->GetConstantPool()))
        result = (MfScDataHandle *)env->PopWord();
    else
        result = 0;
    env->PopAndReleaseData();
    while (numArgs--)
        env->PopAndReleaseData();
    return result;
}

// 0x10016f30 ?ExecuteFunction@MfScVirtualMachine@@QAAPAVMfScDataHandle@@PAV2@HZZ
MfScDataHandle *MfScVirtualMachine::ExecuteFunction(MfScDataHandle *function, int numArgs, ...)
{
    MfScDataHandle **args = 0;
    if (numArgs > 0) {
        args = new MfScDataHandle *[numArgs];
        va_list ap;
        va_start(ap, numArgs);
        for (int i = 0; i < numArgs; i++)
            args[i] = va_arg(ap, MfScDataHandle *);
        va_end(ap);
    }
    MfScDataHandle *result = ExecuteFunctionArrayParams(function, numArgs, args);
    delete args;
    return result;
}

// the types of MfScDataHandle (MfScDataHandle::GetType)
enum {
    kHandleInt = 0x10,
    kHandleFloat = 0x20,
    kHandleNumber = 0x30,
    kHandleBoxedInt = 0x40,         // MfScObjInteger
    kHandleBoxedFloat = 0x80,       // MfScObjFloat
    kHandleString = 0x100,          // MfScObjString
    kHandleObject = 0x200,
    kHandleArray = 0x400,           // MfScDataArray
    kHandleObjects = 0x7c0,         // the objects that have properties
    kHandleMethod = 0x800,          // MfScDataMethod
    kHandlePointer = 0x1000
};

// The interpreter's helpers. ExecuteCode is too big for the inliner to expand
// all of them; the later handlers call out-of-line copies (0x1001b220-0x1001b2c3),
// with these inline members of the headers:
// 0x1001b230 ?Value@MfScObjFloat@@QBEMXZ (inline, MfScObject.h)

// the signed byte operand that follows an opcode
inline int Fetch8(MfScVmEnv *env)
{
    return *(char *)env->m_ip++;
}

// 0x1001b250 ?Fetch16@@YAHPAVMfScVmEnv@@@Z
// the signed little-endian word operand
inline int Fetch16(MfScVmEnv *env)
{
    unsigned char low = *env->m_ip++;
    int high = *(char *)env->m_ip++;
    return high << 8 | low;
}

// the little-endian dword operand
inline int Fetch32(MfScVmEnv *env)
{
    unsigned char b0 = *env->m_ip++;
    unsigned char b1 = *env->m_ip++;
    unsigned char b2 = *env->m_ip++;
    unsigned char b3 = *env->m_ip++;
    return (((b3 << 8 | b2) << 8) | b1) << 8 | b0;
}

// slot n of the current call frame: arguments above it (count at 6, the
// first argument at 7), locals below
// 0x1001b280 ?Local@@YAPAVMfScDataHandle@@PAVMfScVmEnv@@H@Z
inline MfScDataHandle *Local(MfScVmEnv *env, int n)
{
    return ((MfScDataHandle **)env->m_frame)[n];
}

// 0x1001b290 ?StackItem@@YAPAVMfScDataHandle@@PAVMfScVmEnv@@H@Z
// the value n slots down the data stack (1 is the top)
inline MfScDataHandle *StackItem(MfScVmEnv *env, int n)
{
    return env->m_sp[n];
}

// 0x1001b2a0 ?Constant@@YAPAVMfScDataHandle@@PAVMfScVmEnv@@H@Z
// constant n of the running routine
inline MfScDataHandle *Constant(MfScVmEnv *env, int n)
{
    return env->m_constants[n];
}

// 0x1001b2b0 ?HasType@@YAHPAVMfScDataHandle@@K@Z
inline int HasType(MfScDataHandle *h, unsigned long type)
{
    return h->GetType() & type;
}

// a number operand as a float (comparisons)
#define FLOAT_VALUE(h) (((h)->GetType() & kHandleFloat) ? (h)->GetFloat() : (float)(h)->GetInt())

// 0x1001b240 ?ThreadManager@@YAPAVMfScThreadManager@@XZ
inline MfScThreadManager *ThreadManager()
{
    return gThreadManager;
}

// TODO 0x10016f90 ?ExecuteCode@MfScVirtualMachine@@AAEHPAEPAPAVMfScDataHandle@@@Z: 65% similar; the inliner
// gives up later here (at SETARRAYITEM, the original at CMPGTE) and inlines MfScDataHandle::Release in
// POP_FP, ADDSP, POP_REGn and CLEAR_REGn, where the original calls it; then tail merging and registers differ.
// Review 2026-10-16 (per-case diff by the jump tables): CMPGT..CMPNOT checked instruction by instruction
// against the original, same conditions, constants, calls and order (the gap there is the out-of-line
// GetType/GetInt/GetFloat/SetType/HasType copies the original calls once the inliner gives up); a
// Release defined after MfScVmEnv's inline members changes nothing
// Runs bytecode on the calling thread's MfScVmEnv until BC_EXIT, an exception
// or an interrupt. Returns 0, or the exception code (also in mLastException)
// after reporting it to the debugger, the diagnostics and the log and
// unwinding the call frames.
int MfScVirtualMachine::ExecuteCode(unsigned char *code, MfScDataHandle **constants)
{
    MfScDataHandle *a;
    MfScDataHandle *b;
    MfScDataHandle *h;
    MfScDataHandle *result;
    MfScObjString *string;
    int n;
    int keepObject;
    int test;

    MfScDataHandle::DisableGCForVM();
    MfScDataHandle::WaitForGC(20000);
    MfScVmEnv *env = ((MfScThread *)MfThread::GetThread())->mVmEnv;
    env->m_ip = code;
    env->m_constants = constants;
    unsigned long *stackBase = (unsigned long *)env->m_sp;
    MfAiPdbDebugger *debugger = MfScDebugger::pdbgTheDebugger;
    env->m_frame = 0;
    MfString message;
    _se_translator_function oldTranslator = _set_se_translator(TranslateSystemException);
    unsigned char opcode = 0;
    int done = 0;
    env->ClearInterrupt();
    env->FlagStartExecuting();
    int debuggerStopped = 0;
    try {
        env->StartProcessTimer();
        // the main loop: fetch an opcode and execute it
        while (!done && !env->IsInterrupted()) {
            opcode = *env->m_ip++;
        dispatch:
            switch (opcode) {
            case BC_PUSH_FP:
                env->PushData(Local(env, Fetch8(env)));
                break;
            case BC_PUSH_CPP:
                if (!env->m_constants) {
                    mLastException = kVmeNullConstantPool;
                    goto vmException;
                }
                env->PushData(Constant(env, Fetch16(env)));
                break;
            case BC_POP_FP:
                n = Fetch8(env);
                if (Local(env, n))
                    Local(env, n)->Release();
                ((MfScDataHandle **)env->m_frame)[n] = env->PopDataPassRef();
                break;
            case BC_PUSHIM8:
                env->PushDataPassRef(MfScDataHandle::CreateData(Fetch8(env)));
                break;
            case BC_PUSHIM16:
                env->PushDataPassRef(MfScDataHandle::CreateData(Fetch16(env)));
                break;
            case BC_PUSHIM32:
                env->PushDataPassRef(MfScDataHandle::CreateData(Fetch32(env)));
                break;
            case BC_ADDSP8:
                n = Fetch8(env);
                while (n--)
                    env->PopAndReleaseData();
                break;
            case BC_ADDSP16:
                n = Fetch16(env);
                while (n--)
                    env->PopAndReleaseData();
                break;
            case BC_SUBSP8:
                n = Fetch8(env);
                while (n--)
                    env->PushDataPassRef(0);
                break;
            case BC_SUBSP16:
                n = Fetch16(env);
                while (n--)
                    env->PushDataPassRef(0);
                break;
            case BC_PUSH_REG0:
                env->PushData(env->m_reg0);
                break;
            case BC_PUSH_REG1:
                env->PushData(env->m_reg1);
                break;
            case BC_PUSH_REG2:
                env->PushData(env->m_reg2);
                break;
            case BC_PUSH_REG3:
                env->PushData(env->m_reg3);
                break;
            case BC_POP_REG0:
                env->ClearScratchREG0();
                env->m_reg0 = env->PopDataPassRef();
                break;
            case BC_POP_REG1:
                env->ClearScratchREG1();
                env->m_reg1 = env->PopDataPassRef();
                break;
            case BC_POP_REG2:
                env->ClearScratchREG2();
                env->m_reg2 = env->PopDataPassRef();
                break;
            case BC_POP_REG3:
                env->ClearScratchREG3();
                env->m_reg3 = env->PopDataPassRef();
                break;
            case BC_CLEAR_REG0:
                env->ClearScratchREG0();
                break;
            case BC_CLEAR_REG1:
                env->ClearScratchREG1();
                break;
            case BC_CLEAR_REG2:
                env->ClearScratchREG2();
                break;
            case BC_CLEAR_REG3:
                env->ClearScratchREG3();
                break;
            case BC_PUSH_THIS:
                env->PushData(env->m_object);
                break;
            case BC_SWAP:
                h = env->GetSPHandle(1);
                env->SetSPHandle(1, env->GetSPHandle(2));
                env->SetSPHandle(2, h);
                break;
            case BC_DUP:
                env->PushData(env->GetSPHandle(1));
                break;

            // jumps: the offsets count from the opcode
            case BC_JMP8:
                env->m_ip += *(char *)env->m_ip - 1;
                break;
            case BC_JMP16: {
                unsigned char *start = env->m_ip - 1;
                env->m_ip = start + Fetch16(env);
                break;
            }
            case BC_JMP32: {
                unsigned char *start = env->m_ip - 1;
                env->m_ip = start + Fetch32(env);
                break;
            }
            case BC_JMPZERO8: {
                unsigned char *start = env->m_ip - 1;
                n = Fetch8(env);
                h = env->PopDataPassRef();
                if (!h) {
                    mLastException = kVmeNilOperand;
                    goto vmException;
                }
                if (!HasType(h, kHandleInt)) {
                    h->Release();
                    mLastException = kVmeIntegerRequired;
                    goto vmException;
                }
                if (!h->GetInt())
                    env->m_ip = start + n;
                h->Release();
                break;
            }
            case BC_JMPZERO16: {
                unsigned char *start = env->m_ip - 1;
                n = Fetch16(env);
                h = env->PopDataPassRef();
                if (!h) {
                    mLastException = kVmeNilOperand;
                    goto vmException;
                }
                if (!HasType(h, kHandleInt)) {
                    h->Release();
                    mLastException = kVmeIntegerRequired;
                    goto vmException;
                }
                if (!h->GetInt())
                    env->m_ip = start + n;
                h->Release();
                break;
            }
            case BC_JMPZERO32: {
                unsigned char *start = env->m_ip - 1;
                n = Fetch32(env);
                h = env->PopDataPassRef();
                if (!h) {
                    mLastException = kVmeNilOperand;
                    goto vmException;
                }
                if (!HasType(h, kHandleInt)) {
                    h->Release();
                    mLastException = kVmeIntegerRequired;
                    goto vmException;
                }
                if (!h->GetInt())
                    env->m_ip = start + n;
                h->Release();
                break;
            }
            case BC_JMPNOTZERO8: {
                unsigned char *start = env->m_ip - 1;
                n = Fetch8(env);
                h = env->PopDataPassRef();
                if (!h) {
                    mLastException = kVmeNilOperand;
                    goto vmException;
                }
                if (!HasType(h, kHandleInt)) {
                    h->Release();
                    mLastException = kVmeIntegerRequired;
                    goto vmException;
                }
                if (h->GetInt())
                    env->m_ip = start + n;
                h->Release();
                break;
            }
            case BC_JMPNOTZERO16: {
                unsigned char *start = env->m_ip - 1;
                n = Fetch16(env);
                h = env->PopDataPassRef();
                if (!h) {
                    mLastException = kVmeNilOperand;
                    goto vmException;
                }
                if (!HasType(h, kHandleInt)) {
                    h->Release();
                    mLastException = kVmeIntegerRequired;
                    goto vmException;
                }
                if (h->GetInt())
                    env->m_ip = start + n;
                h->Release();
                break;
            }
            case BC_JMPNOTZERO32: {
                unsigned char *start = env->m_ip - 1;
                n = Fetch32(env);
                h = env->PopDataPassRef();
                if (!h) {
                    mLastException = kVmeNilOperand;
                    goto vmException;
                }
                if (!HasType(h, kHandleInt)) {
                    h->Release();
                    mLastException = kVmeIntegerRequired;
                    goto vmException;
                }
                if (h->GetInt())
                    env->m_ip = start + n;
                h->Release();
                break;
            }

            // arithmetic: [a, b] -> [a op b]
            case BC_ADD: {
                b = env->PopDataPassRef();
                a = env->PopDataPassRef();
                if (!a || !b) {
                    if (a)
                        a->Release();
                    if (b)
                        b->Release();
                    mLastException = kVmeNilOperand;
                    goto vmException;
                }
                unsigned int typeA = a->GetType();
                if (!((typeA & kHandleString) && HasType(b, kHandleNumber | kHandleString))
                    && !((typeA & kHandleNumber) && HasType(b, kHandleNumber))) {
                    a->Release();
                    b->Release();
                    mLastException = kVmeBadMathOperand;
                    goto vmException;
                }
                // a string and anything gives a string, a float and a number a float
                unsigned int type;
                if (typeA & kHandleString)
                    type = kHandleString;
                else if ((typeA & kHandleFloat) || HasType(b, kHandleFloat))
                    type = kHandleFloat;
                else
                    type = kHandleInt;
                if (type & kHandleInt) {
                    result = new (0) MfScDataHandle;
                    result->SetType(type);
                    result->SetInt(a->GetInt() + b->GetInt());
                } else if (type & kHandleFloat) {
                    result = new (0) MfScDataHandle;
                    result->SetType(type);
                    result->SetFloat(a->GetNumber() + b->GetNumber());
                } else {
                    result = new (1) MfScDataHandle;
                    string = new MfScObjString;
                    result->SetType(type);
                    result->SetData(string);
                    MfScObjString *left = (MfScObjString *)a->GetData();
                    unsigned int typeB = b->GetType();
                    if (typeB & kHandleString)
                        *(MfString *)string = *left + *(MfString *)(MfScObjString *)b->GetData();
                    else if (typeB & kHandleInt) {
                        *(MfString *)string = *(MfString *)left;
                        *(MfString *)string << b->GetInt();
                    } else if (typeB & kHandleFloat) {
                        *(MfString *)string = *(MfString *)left;
                        *(MfString *)string << b->GetFloat();
                    }
                }
                env->PushDataPassRef(result);
                a->Release();
                b->Release();
                break;
            }
            case BC_SUB:
                b = env->PopDataPassRef();
                a = env->PopDataPassRef();
                if (!a || !b) {
                    if (a)
                        a->Release();
                    if (b)
                        b->Release();
                    mLastException = kVmeNilOperand;
                    goto vmException;
                }
                if (!HasType(a, kHandleNumber) || !HasType(b, kHandleNumber)) {
                    a->Release();
                    b->Release();
                    mLastException = kVmeBadMathOperand;
                    goto vmException;
                }
                result = new (0) MfScDataHandle;
                result->SetType((HasType(a, kHandleFloat) || HasType(b, kHandleFloat)) ? kHandleFloat : kHandleInt);
                if (result->GetType() & kHandleInt)
                    result->SetInt(a->GetInt() - b->GetInt());
                else if (result->GetType() & kHandleFloat)
                    result->SetFloat(a->GetNumber() - b->GetNumber());
                env->PushDataPassRef(result);
                a->Release();
                b->Release();
                break;
            case BC_MULT:
                b = env->PopDataPassRef();
                a = env->PopDataPassRef();
                if (!a || !b) {
                    if (a)
                        a->Release();
                    if (b)
                        b->Release();
                    mLastException = kVmeNilOperand;
                    goto vmException;
                }
                if (!HasType(a, kHandleNumber) || !HasType(b, kHandleNumber)) {
                    a->Release();
                    b->Release();
                    mLastException = kVmeBadMathOperand;
                    goto vmException;
                }
                result = new (0) MfScDataHandle;
                result->SetType((HasType(a, kHandleFloat) || HasType(b, kHandleFloat)) ? kHandleFloat : kHandleInt);
                if (result->GetType() & kHandleInt)
                    result->SetInt(a->GetInt() * b->GetInt());
                else if (result->GetType() & kHandleFloat)
                    result->SetFloat(a->GetNumber() * b->GetNumber());
                env->PushDataPassRef(result);
                a->Release();
                b->Release();
                break;
            case BC_DIV:
                b = env->PopDataPassRef();
                a = env->PopDataPassRef();
                if (!a || !b) {
                    if (a)
                        a->Release();
                    if (b)
                        b->Release();
                    mLastException = kVmeNilOperand;
                    goto vmException;
                }
                if (!HasType(a, kHandleNumber) || !HasType(b, kHandleNumber)) {
                    a->Release();
                    b->Release();
                    mLastException = kVmeBadMathOperand;
                    goto vmException;
                }
                result = new (0) MfScDataHandle;
                result->SetType((HasType(a, kHandleFloat) || HasType(b, kHandleFloat)) ? kHandleFloat : kHandleInt);
                if (b->GetNumber() == 0.0f) {
                    a->Release();
                    b->Release();
                    mLastException = kVmeDivideByZero;
                    goto vmException;
                }
                if (result->GetType() & kHandleInt)
                    result->SetInt(a->GetInt() / b->GetInt());
                else if (result->GetType() & kHandleFloat)
                    result->SetFloat(a->GetNumber() / b->GetNumber());
                env->PushDataPassRef(result);
                a->Release();
                b->Release();
                break;
            case BC_MOD:
                // integers only; a division by zero raises a system exception
                try {
                    b = env->PopDataPassRef();
                    a = env->PopDataPassRef();
                    if (!a || !b) {
                        if (a)
                            a->Release();
                        if (b)
                            b->Release();
                        mLastException = kVmeNilOperand;
                        goto vmException;
                    }
                    if (!HasType(a, kHandleInt) || !HasType(b, kHandleInt)) {
                        a->Release();
                        b->Release();
                        mLastException = kVmeIntegerRequired;
                        goto vmException;
                    }
                    result = new (0) MfScDataHandle;
                    result->SetType(kHandleInt);
                    result->SetInt(a->GetInt() % b->GetInt());
                    env->PushDataPassRef(result);
                    a->Release();
                    b->Release();
                } catch (VmSEHandler) {
                    a->Release();
                    b->Release();
                    mLastException = kVmeDivideByZero;
                    goto vmException;
                }
                break;
            case BC_NEG:
                a = env->PopDataPassRef();
                if (!a) {
                    mLastException = kVmeNilOperand;
                    goto vmException;
                }
                if (!HasType(a, kHandleNumber)) {
                    a->Release();
                    mLastException = kVmeBadMathOperand;
                    goto vmException;
                }
                result = new (0) MfScDataHandle;
                result->SetType(a->GetType() & kHandleNumber);
                if (result->GetType() & kHandleInt)
                    result->SetInt(-a->GetInt());
                else if (result->GetType() & kHandleFloat)
                    result->SetFloat(-a->GetFloat());
                env->PushDataPassRef(result);
                a->Release();
                break;
            case BC_INCR:
                n = Fetch8(env);
                a = env->PopDataPassRef();
                if (!a) {
                    mLastException = kVmeNilOperand;
                    goto vmException;
                }
                if (!HasType(a, kHandleInt)) {
                    a->Release();
                    mLastException = kVmeBadMathOperand;
                    goto vmException;
                }
                result = new (0) MfScDataHandle;
                result->SetType(kHandleInt);
                result->SetInt(a->GetInt() + n);
                env->PushDataPassRef(result);
                a->Release();
                break;
            case BC_AND:
                b = env->PopDataPassRef();
                a = env->PopDataPassRef();
                if (!a || !b) {
                    if (a)
                        a->Release();
                    if (b)
                        b->Release();
                    mLastException = kVmeNilOperand;
                    goto vmException;
                }
                if (!HasType(a, kHandleInt) || !HasType(b, kHandleInt)) {
                    a->Release();
                    b->Release();
                    mLastException = kVmeIntegerRequired;
                    goto vmException;
                }
                result = new (0) MfScDataHandle;
                result->SetType(kHandleInt);
                result->SetInt(a->GetInt() & b->GetInt());
                env->PushDataPassRef(result);
                a->Release();
                b->Release();
                break;
            case BC_OR:
                b = env->PopDataPassRef();
                a = env->PopDataPassRef();
                if (!a || !b) {
                    if (a)
                        a->Release();
                    if (b)
                        b->Release();
                    mLastException = kVmeNilOperand;
                    goto vmException;
                }
                if (!HasType(a, kHandleInt) || !HasType(b, kHandleInt)) {
                    a->Release();
                    b->Release();
                    mLastException = kVmeIntegerRequired;
                    goto vmException;
                }
                result = new (0) MfScDataHandle;
                result->SetType(kHandleInt);
                result->SetInt(a->GetInt() | b->GetInt());
                env->PushDataPassRef(result);
                a->Release();
                b->Release();
                break;
            case BC_XOR:
                b = env->PopDataPassRef();
                a = env->PopDataPassRef();
                if (!a || !b) {
                    if (a)
                        a->Release();
                    if (b)
                        b->Release();
                    mLastException = kVmeNilOperand;
                    goto vmException;
                }
                if (!HasType(a, kHandleInt) || !HasType(b, kHandleInt)) {
                    a->Release();
                    b->Release();
                    mLastException = kVmeIntegerRequired;
                    goto vmException;
                }
                result = new (0) MfScDataHandle;
                result->SetType(kHandleInt);
                result->SetInt(a->GetInt() ^ b->GetInt());
                env->PushDataPassRef(result);
                a->Release();
                b->Release();
                break;
            case BC_NOT:
                a = env->PopDataPassRef();
                if (!a) {
                    mLastException = kVmeNilOperand;
                    goto vmException;
                }
                if (!HasType(a, kHandleInt)) {
                    a->Release();
                    mLastException = kVmeIntegerRequired;
                    goto vmException;
                }
                result = new (0) MfScDataHandle;
                result->SetType(kHandleInt);
                result->SetInt(~a->GetInt());
                env->PushDataPassRef(result);
                a->Release();
                break;
            case BC_LSHIFT:
                b = env->PopDataPassRef();
                a = env->PopDataPassRef();
                if (!a || !b) {
                    if (a)
                        a->Release();
                    if (b)
                        b->Release();
                    mLastException = kVmeNilOperand;
                    goto vmException;
                }
                if (!HasType(a, kHandleInt) || !HasType(b, kHandleInt)) {
                    a->Release();
                    b->Release();
                    mLastException = kVmeIntegerRequired;
                    goto vmException;
                }
                result = new (0) MfScDataHandle;
                result->SetType(kHandleInt);
                result->SetInt(a->GetInt() << b->GetInt());
                env->PushDataPassRef(result);
                a->Release();
                b->Release();
                break;
            case BC_RSHIFT:
                b = env->PopDataPassRef();
                a = env->PopDataPassRef();
                if (!a || !b) {
                    if (a)
                        a->Release();
                    if (b)
                        b->Release();
                    mLastException = kVmeNilOperand;
                    goto vmException;
                }
                if (!HasType(a, kHandleInt) || !HasType(b, kHandleInt)) {
                    a->Release();
                    b->Release();
                    mLastException = kVmeIntegerRequired;
                    goto vmException;
                }
                result = new (0) MfScDataHandle;
                result->SetType(kHandleInt);
                result->SetInt(a->GetInt() >> b->GetInt());
                env->PushDataPassRef(result);
                a->Release();
                b->Release();
                break;

            // comparisons push the integer 1 or 0
            case BC_CMPLT:
                b = env->PopDataPassRef();
                a = env->PopDataPassRef();
                if (!a || !b) {
                    if (a)
                        a->Release();
                    if (b)
                        b->Release();
                    mLastException = kVmeNilOperand;
                    goto vmException;
                }
                if (!HasType(a, kHandleNumber) || !HasType(b, kHandleNumber)) {
                    a->Release();
                    b->Release();
                    mLastException = kVmeBadMathOperand;
                    goto vmException;
                }
                result = new (0) MfScDataHandle;
                result->SetType(kHandleInt);
                if ((a->GetType() & kHandleInt) && (b->GetType() & kHandleInt))
                    result->SetInt(a->GetInt() < b->GetInt());
                else
                    result->SetInt(FLOAT_VALUE(a) < FLOAT_VALUE(b));
                result->SetType(kHandleInt);
                env->PushDataPassRef(result);
                a->Release();
                b->Release();
                break;
            case BC_CMPLTE:
                b = env->PopDataPassRef();
                a = env->PopDataPassRef();
                if (!a || !b) {
                    if (a)
                        a->Release();
                    if (b)
                        b->Release();
                    mLastException = kVmeNilOperand;
                    goto vmException;
                }
                if (!HasType(a, kHandleNumber) || !HasType(b, kHandleNumber)) {
                    a->Release();
                    b->Release();
                    mLastException = kVmeBadMathOperand;
                    goto vmException;
                }
                result = new (0) MfScDataHandle;
                result->SetType(kHandleInt);
                if ((a->GetType() & kHandleInt) && (b->GetType() & kHandleInt))
                    result->SetInt(a->GetInt() <= b->GetInt());
                else
                    result->SetInt(FLOAT_VALUE(a) <= FLOAT_VALUE(b));
                result->SetType(kHandleInt);
                env->PushDataPassRef(result);
                a->Release();
                b->Release();
                break;
            case BC_CMPGT:
                b = env->PopDataPassRef();
                a = env->PopDataPassRef();
                if (!a || !b) {
                    if (a)
                        a->Release();
                    if (b)
                        b->Release();
                    mLastException = kVmeNilOperand;
                    goto vmException;
                }
                if (!HasType(a, kHandleNumber) || !HasType(b, kHandleNumber)) {
                    a->Release();
                    b->Release();
                    mLastException = kVmeBadMathOperand;
                    goto vmException;
                }
                result = new (0) MfScDataHandle;
                result->SetType(kHandleInt);
                if ((a->GetType() & kHandleInt) && (b->GetType() & kHandleInt))
                    result->SetInt(a->GetInt() > b->GetInt());
                else
                    result->SetInt(FLOAT_VALUE(a) > FLOAT_VALUE(b));
                result->SetType(kHandleInt);
                env->PushDataPassRef(result);
                a->Release();
                b->Release();
                break;
            case BC_CMPGTE:
                b = env->PopDataPassRef();
                a = env->PopDataPassRef();
                if (!a || !b) {
                    if (a)
                        a->Release();
                    if (b)
                        b->Release();
                    mLastException = kVmeNilOperand;
                    goto vmException;
                }
                if (!HasType(a, kHandleNumber) || !HasType(b, kHandleNumber)) {
                    a->Release();
                    b->Release();
                    mLastException = kVmeBadMathOperand;
                    goto vmException;
                }
                result = new (0) MfScDataHandle;
                result->SetType(kHandleInt);
                if ((a->GetType() & kHandleInt) && (b->GetType() & kHandleInt))
                    result->SetInt(a->GetInt() >= b->GetInt());
                else
                    result->SetInt(FLOAT_VALUE(a) >= FLOAT_VALUE(b));
                result->SetType(kHandleInt);
                env->PushDataPassRef(result);
                a->Release();
                b->Release();
                break;
            case BC_CMPEQ:
                // nil equals only nil; values of different types are equal
                // unless both are numbers; objects compare by identity
                b = env->PopDataPassRef();
                a = env->PopDataPassRef();
                result = new (0) MfScDataHandle;
                result->SetType(kHandleInt);
                if (a && b) {
                    if (a->GetType() == b->GetType()) {
                        if (HasType(a, kHandleInt))
                            result->SetInt(a->GetInt() == b->GetInt());
                        else if (HasType(a, kHandleFloat))
                            result->SetInt(a->GetFloat() == b->GetFloat());
                        else if (HasType(a, kHandleString))
                            result->SetInt(*(MfScObjString *)a->GetData() == *(MfString *)(MfScObjString *)b->GetData());
                        else
                            result->SetInt(a == b);
                    } else if (HasType(a, kHandleNumber) && HasType(b, kHandleNumber))
                        result->SetInt(FLOAT_VALUE(a) == FLOAT_VALUE(b));
                    else
                        result->SetInt(0);
                } else
                    result->SetInt(a == b);
                env->PushDataPassRef(result);
                if (a)
                    a->Release();
                if (b)
                    b->Release();
                break;
            case BC_CMPNEQ:
                // nil equals only nil; values of different types are different
                // unless both are numbers; objects compare by identity
                b = env->PopDataPassRef();
                a = env->PopDataPassRef();
                result = new (0) MfScDataHandle;
                result->SetType(kHandleInt);
                if (a && b) {
                    if (a->GetType() == b->GetType()) {
                        if (HasType(a, kHandleInt))
                            result->SetInt(a->GetInt() != b->GetInt());
                        else if (HasType(a, kHandleFloat))
                            result->SetInt(a->GetFloat() != b->GetFloat());
                        else if (HasType(a, kHandleString))
                            result->SetInt(*(MfScObjString *)a->GetData() != *(MfString *)(MfScObjString *)b->GetData());
                        else
                            result->SetInt(a != b);
                    } else if (HasType(a, kHandleNumber) && HasType(b, kHandleNumber))
                        result->SetInt(FLOAT_VALUE(a) != FLOAT_VALUE(b));
                    else
                        result->SetInt(1);
                } else
                    result->SetInt(a != b);
                env->PushDataPassRef(result);
                if (a)
                    a->Release();
                if (b)
                    b->Release();
                break;
            case BC_CMPAND:
                b = env->PopDataPassRef();
                a = env->PopDataPassRef();
                if (!a || !b) {
                    if (a)
                        a->Release();
                    if (b)
                        b->Release();
                    mLastException = kVmeNilOperand;
                    goto vmException;
                }
                if (!HasType(a, kHandleNumber) || !HasType(b, kHandleNumber)) {
                    a->Release();
                    b->Release();
                    mLastException = kVmeBadMathOperand;
                    goto vmException;
                }
                result = new (0) MfScDataHandle;
                result->SetType(kHandleInt);
                if ((a->GetType() & kHandleInt) && (b->GetType() & kHandleInt))
                    result->SetInt(a->GetInt() && b->GetInt());
                else
                    result->SetInt(FLOAT_VALUE(a) != 0.0f && FLOAT_VALUE(b) != 0.0f);
                result->SetType(kHandleInt);
                env->PushDataPassRef(result);
                a->Release();
                b->Release();
                break;
            case BC_CMPOR:
                b = env->PopDataPassRef();
                a = env->PopDataPassRef();
                if (!a || !b) {
                    if (a)
                        a->Release();
                    if (b)
                        b->Release();
                    mLastException = kVmeNilOperand;
                    goto vmException;
                }
                if (!HasType(a, kHandleNumber) || !HasType(b, kHandleNumber)) {
                    a->Release();
                    b->Release();
                    mLastException = kVmeBadMathOperand;
                    goto vmException;
                }
                result = new (0) MfScDataHandle;
                result->SetType(kHandleInt);
                if ((a->GetType() & kHandleInt) && (b->GetType() & kHandleInt))
                    result->SetInt(a->GetInt() || b->GetInt());
                else
                    result->SetInt(FLOAT_VALUE(a) != 0.0f || FLOAT_VALUE(b) != 0.0f);
                result->SetType(kHandleInt);
                env->PushDataPassRef(result);
                a->Release();
                b->Release();
                break;
            case BC_CMPNOT:
                a = env->PopDataPassRef();
                if (!a) {
                    mLastException = kVmeNilOperand;
                    goto vmException;
                }
                if (!HasType(a, kHandleInt)) {
                    a->Release();
                    mLastException = kVmeIntegerRequired;
                    goto vmException;
                }
                result = new (0) MfScDataHandle;
                result->SetType(kHandleInt);
                result->SetInt(!a->GetInt());
                env->PushDataPassRef(result);
                a->Release();
                break;

            // arrays (and strings, as arrays of characters)
            case BC_GETARRAYITEM:
                b = env->PopDataPassRef();      // the index
                a = env->PopDataPassRef();      // the array
                if (!b || !a) {
                    if (b)
                        b->Release();
                    if (a)
                        a->Release();
                    mLastException = kVmeNilOperand;
                    goto vmException;
                }
                if (!HasType(a, kHandleArray) && !HasType(a, kHandleString)) {
                    b->Release();
                    a->Release();
                    mLastException = kVmeArrayRequired;
                    goto vmException;
                }
                if (!HasType(b, kHandleInt)) {
                    b->Release();
                    a->Release();
                    mLastException = kVmeIntegerRequired;
                    goto vmException;
                }
                if (HasType(a, kHandleArray)) {
                    MfScDataArray *array = (MfScDataArray *)a->GetData();
                    MfScDataHandle *item = 0;
                    if (!array->GetAt(b->GetInt(), &item, 1)) {
                        message.Format("Array index %d is out of bounds", b->GetInt());
                        b->Release();
                        a->Release();
                        mLastException = kVmeIndexOutOfBounds;
                        goto vmException;
                    }
                    env->PushDataPassRef(item);
                } else {
                    MfString &text = *(MfScObjString *)a->GetData();
                    n = b->GetInt();
                    if (n < 0 || n >= text.Length()) {
                        message.Format("Array index %d is out of bounds", n);
                        b->Release();
                        a->Release();
                        mLastException = kVmeIndexOutOfBounds;
                        goto vmException;
                    }
                    env->PushDataPassRef(MfScDataHandle::CreateData((int)text.GetCharacter(b->GetInt())));
                }
                b->Release();
                a->Release();
                break;
            case BC_SETARRAYITEM:
                b = env->PopDataPassRef();      // the index
                a = env->PopDataPassRef();      // the array
                h = env->PopDataPassRef();      // the value
                if (!b || !a) {
                    if (h)
                        h->Release();
                    if (b)
                        b->Release();
                    if (a)
                        a->Release();
                    mLastException = kVmeNilOperand;
                    goto vmException;
                }
                if (!HasType(a, kHandleArray) && !HasType(a, kHandleString)) {
                    if (h)
                        h->Release();
                    b->Release();
                    a->Release();
                    mLastException = kVmeArrayRequired;
                    goto vmException;
                }
                if (!HasType(b, kHandleInt)) {
                    if (h)
                        h->Release();
                    b->Release();
                    a->Release();
                    mLastException = kVmeIntegerRequired;
                    goto vmException;
                }
                if (HasType(a, kHandleArray)) {
                    MfScDataArray *array = (MfScDataArray *)a->GetData();
                    array->Lock();
                    if (!array->SetAt(b->GetInt(), h, 0)) {
                        array->Unlock();
                        if (h)
                            h->Release();
                        b->Release();
                        a->Release();
                        mLastException = kVmeIndexOutOfBounds;
                        goto vmException;
                    }
                    array->Unlock();
                } else {
                    // a character of a string
                    if (!h) {
                        b->Release();
                        a->Release();
                        mLastException = kVmeNilOperand;
                        goto vmException;
                    }
                    if (!HasType(h, kHandleInt)) {
                        h->Release();
                        b->Release();
                        a->Release();
                        mLastException = kVmeIntegerRequired;
                        goto vmException;
                    }
                    string = (MfScObjString *)a->GetData();
                    if (string->IsReadOnly()) {
                        h->Release();
                        b->Release();
                        a->Release();
                        mLastException = kVmeAccessDenied;
                        goto vmException;
                    }
                    if (!string->SetCharacter(b->GetInt(), (char)h->GetInt())) {
                        h->Release();
                        b->Release();
                        a->Release();
                        mLastException = kVmeIndexOutOfBounds;
                        goto vmException;
                    }
                }
                if (h)
                    h->Release();
                b->Release();
                a->Release();
                break;

            // conversions and new objects
            case BC_NEW:
                env->PushDataPassRef(MfScDataHandle::CreateData(new MfScDataObject));
                break;
            case BC_NEWSTRING:
                a = env->PopDataPassRef();
                if (!a)
                    result = MfScDataHandle::CreateData(MfString(""), 0);
                else if (HasType(a, kHandleString))
                    result = MfScDataHandle::CreateData(*(MfString *)(MfScObjString *)a->GetData(), 0);
                else if (HasType(a, kHandleInt))
                    result = MfScDataHandle::CreateData(MfString(a->GetInt()), 0);
                else if (HasType(a, kHandleFloat))
                    result = MfScDataHandle::CreateData(MfString(a->GetFloat()), 0);
                else if (HasType(a, kHandleBoxedInt))
                    result = MfScDataHandle::CreateData(MfString(((MfScObjInteger *)a->GetData())->Value()), 0);
                else if (HasType(a, kHandleBoxedFloat))
                    result = MfScDataHandle::CreateData(MfString(((MfScObjFloat *)a->GetData())->Value()), 0);
                else
                    goto badConversion;
                env->PushDataPassRef(result);
                if (a)
                    a->Release();
                break;
            case BC_NEWINTEGER:
                a = env->PopDataPassRef();
                if (!a)
                    result = MfScDataHandle::CreateData(MfScObjInteger(0));
                else if (HasType(a, kHandleString))
                    result = MfScDataHandle::CreateData(MfScObjInteger((int)*(MfString *)(MfScObjString *)a->GetData()));
                else if (HasType(a, kHandleInt))
                    result = MfScDataHandle::CreateData(MfScObjInteger(a->GetInt()));
                else if (HasType(a, kHandleFloat))
                    result = MfScDataHandle::CreateData(MfScObjInteger((int)a->GetFloat()));
                else if (HasType(a, kHandleBoxedInt))
                    result = MfScDataHandle::CreateData(MfScObjInteger(((MfScObjInteger *)a->GetData())->Value()));
                else if (HasType(a, kHandleBoxedFloat))
                    result = MfScDataHandle::CreateData(MfScObjInteger((int)((MfScObjFloat *)a->GetData())->Value()));
                else
                    goto badConversion;
                env->PushDataPassRef(result);
                if (a)
                    a->Release();
                break;
            case BC_NEWFLOAT:
                a = env->PopDataPassRef();
                if (!a)
                    result = MfScDataHandle::CreateData(MfScObjFloat(0));
                else if (HasType(a, kHandleString))
                    result = MfScDataHandle::CreateData(MfScObjFloat((float)*(MfString *)(MfScObjString *)a->GetData()));
                else if (HasType(a, kHandleInt))
                    result = MfScDataHandle::CreateData(MfScObjFloat((float)a->GetInt()));
                else if (HasType(a, kHandleFloat))
                    result = MfScDataHandle::CreateData(MfScObjFloat(a->GetFloat()));
                else if (HasType(a, kHandleBoxedInt))
                    result = MfScDataHandle::CreateData(MfScObjFloat((float)((MfScObjInteger *)a->GetData())->Value()));
                // (sic: tests for a boxed integer again, so a boxed float is
                // a bad operand)
                else if (HasType(a, kHandleBoxedInt))
                    result = MfScDataHandle::CreateData(MfScObjFloat(((MfScObjFloat *)a->GetData())->Value()));
                else {
                badConversion:
                    message = "Expecting a string or number operand.";
                    a->Release();
                    mLastException = kVmeIllegalOperand;
                    goto vmException;
                }
                env->PushDataPassRef(result);
                if (a)
                    a->Release();
                break;
            case BC_NEWARRAY:
                a = Constant(env, Fetch16(env));
                goto newArray;
            case BC_NEWARRAY_DYN:
                a = env->PopDataPassRef();
            newArray:
                if (!a) {
                    mLastException = kVmeNilOperand;
                    goto vmException;
                }
                if (!HasType(a, kHandleInt)) {
                    a->Release();
                    mLastException = kVmeIntegerRequired;
                    goto vmException;
                }
                env->PushDataPassRef(MfScDataHandle::CreateDataArray(a->GetInt()));
                break;
            case BC_NEWTHREAD:
                csThreadBreakLock.Enter();
                a = env->PopDataPassRef();      // the function
                b = env->PopDataPassRef();      // its argument
                if (!a) {
                    if (b)
                        b->Release();
                    csThreadBreakLock.Leave();
                    mLastException = kVmeNilOperand;
                    goto vmException;
                }
                if (!HasType(a, kHandleMethod)) {
                    a->Release();
                    if (b)
                        b->Release();
                    csThreadBreakLock.Leave();
                    mLastException = kVmeIllegalFunction;
                    goto vmException;
                }
                result = MfScDataHandle::CreateData(new MfScDataThread);
                ((MfScDataThread *)result->GetData())->Start(result, a, b);
                env->PushDataPassRef(result);
                a->Release();
                if (b)
                    b->Release();
                csThreadBreakLock.Leave();
                break;

            // properties: GETPROP* take the name from the constant pool,
            // GETPROPSTACK* from the stack; the ...1 forms leave the object on
            // the stack under the value, the ...TEST forms push nil for a
            // property that cannot be read
            case BC_GETPROP:
                a = env->PopDataPassRef();
                b = Constant(env, Fetch16(env));
                b->AddRef();
                keepObject = 0;
                test = 0;
                goto getProperty;
            case BC_GETPROP1:
                a = env->PopDataPassRef();
                b = Constant(env, Fetch16(env));
                b->AddRef();
                keepObject = 1;
                test = 0;
                goto getProperty;
            case BC_GETPROPTEST:
                a = env->PopDataPassRef();
                b = Constant(env, Fetch16(env));
                b->AddRef();
                keepObject = 0;
                test = 1;
                goto getProperty;
            case BC_GETPROPTEST1:
                a = env->PopDataPassRef();
                b = Constant(env, Fetch16(env));
                b->AddRef();
                keepObject = 1;
                test = 1;
                goto getProperty;
            case BC_GETPROPSTACK:
                b = env->PopDataPassRef();
                a = env->PopDataPassRef();
                keepObject = 0;
                test = 0;
                goto getProperty;
            case BC_GETPROPSTACK1:
                b = env->PopDataPassRef();
                a = env->PopDataPassRef();
                keepObject = 1;
                test = 0;
                goto getProperty;
            case BC_GETPROPSTACKTEST:
                b = env->PopDataPassRef();
                a = env->PopDataPassRef();
                keepObject = 0;
                test = 1;
                goto getProperty;
            case BC_GETPROPSTACKTEST1:
                b = env->PopDataPassRef();
                a = env->PopDataPassRef();
                keepObject = 1;
                test = 1;
            getProperty: {
                // a is the object, b the name
                if (!a || !b) {
                    mLastException = kVmeNilOperand;
                    goto vmException;
                }
                if (!(a->GetType() & kHandleObjects)) {
                    a->Release();
                    b->Release();
                    mLastException = kVmeInvalidObject;
                    goto vmException;
                }
                if (~b->GetType() & kHandleString) {
                    a->Release();
                    b->Release();
                    mLastException = kVmeStringRequired;
                    goto vmException;
                }
                MfScDataObject *object = (MfScDataObject *)a->GetData();
                string = (MfScObjString *)b->GetData();
                MfScDataHandle *value;
                int error = object->GetProperty(*(MfString *)string, &value);
                if (error == MfScDataObject::kPropOk) {
                    if (keepObject == 1)
                        env->PushData(a);
                    env->PushDataPassRef(value);
                } else if (test)
                    env->PushDataPassRef(0);
                else {
                    message.Format("Error getting property '%s'", (const char *)*(MfString *)string);
                    a->Release();
                    b->Release();
                    mLastException = kVmePropertyError + error;
                    goto vmException;
                }
                a->Release();
                b->Release();
                break;
            }
            case BC_SETPROP:
                a = env->PopDataPassRef();
                h = env->PopDataPassRef();
                b = Constant(env, Fetch16(env));
                b->AddRef();
                keepObject = 0;
                goto setProperty;
            case BC_SETPROP1:
                a = env->PopDataPassRef();
                h = env->PopDataPassRef();
                b = Constant(env, Fetch16(env));
                b->AddRef();
                keepObject = 1;
                goto setProperty;
            case BC_SETPROPSTACK:
                b = env->PopDataPassRef();
                a = env->PopDataPassRef();
                h = env->PopDataPassRef();
                keepObject = 0;
                goto setProperty;
            case BC_SETPROPSTACK1:
                b = env->PopDataPassRef();
                a = env->PopDataPassRef();
                h = env->PopDataPassRef();
                keepObject = 1;
            setProperty: {
                // a is the object, b the name, h the value
                if (!a || !b) {
                    if (a)
                        a->Release();
                    if (h)
                        h->Release();
                    if (b)
                        b->Release();
                    mLastException = kVmeNilOperand;
                    goto vmException;
                }
                if (!(a->GetType() & kHandleObjects)) {
                    a->Release();
                    b->Release();
                    if (h)
                        h->Release();
                    mLastException = kVmeInvalidObject;
                    goto vmException;
                }
                if (~b->GetType() & kHandleString) {
                    a->Release();
                    b->Release();
                    if (h)
                        h->Release();
                    mLastException = kVmeStringRequired;
                    goto vmException;
                }
                MfScDataObject *object = (MfScDataObject *)a->GetData();
                string = (MfScObjString *)b->GetData();
                object->Lock();
                int error = object->SetProperty(a, *(MfString *)string, h, 0);
                object->Unlock();
                if (error) {
                    message.Format("Error setting property '%s'", (const char *)*(MfString *)string);
                    a->Release();
                    b->Release();
                    if (h)
                        h->Release();
                    mLastException = kVmePropertyError + error;
                    goto vmException;
                }
                if (keepObject)
                    env->PushData(a);
                a->Release();
                b->Release();
                if (h)
                    h->Release();
                break;
            }

            // variables, named by a string constant
            case BC_GETVAR: {
                b = Constant(env, Fetch16(env));
                if (!b) {
                    mLastException = kVmeNilOperand;
                    goto vmException;
                }
                if (~b->GetType() & kHandleString) {
                    mLastException = kVmeStringRequired;
                    goto vmException;
                }
                string = (MfScObjString *)b->GetData();
                MfScDataHandle *value = 0;
                if (env->GetVariable(*(MfString *)string, &value) != MfScVariableStack::kVarOk) {
                    message.Format("'%s': variable does not exist.", (const char *)*(MfString *)string);
                    if (value)
                        value->Release();
                    mLastException = kVmeNoSuchVariable;
                    goto vmException;
                }
                env->PushDataPassRef(value);
                break;
            }
            case BC_SETGLOBALVAR:
                h = env->PopDataPassRef();
                goto setGlobal;
            case BC_CREATEGLOBALVAR:
                h = 0;
            setGlobal: {
                b = Constant(env, Fetch16(env));
                if (~b->GetType() & kHandleString) {
                    if (h)
                        h->Release();
                    mLastException = kVmeStringRequired;
                    goto vmException;
                }
                string = (MfScObjString *)b->GetData();
                int error = MfScVmEnv::SetGlobalVariable(*(MfString *)string, h, 0, 0);
                if (h)
                    h->Release();
                if (error == MfScVariableStack::kVarNotFound) {
                    message.Format("'%s': variable does not exist.", (const char *)*(MfString *)string);
                    mLastException = kVmeNoSuchVariable;
                    goto vmException;
                }
                if (error == MfScVariableStack::kVarReadOnly) {
                    message.Format("'%s': variable is read only.", (const char *)*(MfString *)string);
                    mLastException = kVmeAccessDenied;
                    goto vmException;
                }
                break;
            }
            case BC_SETVAR: {
                b = Constant(env, Fetch16(env));
                h = env->PopDataPassRef();
                if (~b->GetType() & kHandleString) {
                    if (h)
                        h->Release();
                    mLastException = kVmeStringRequired;
                    goto vmException;
                }
                string = (MfScObjString *)b->GetData();
                int error = env->SetVariable(*(MfString *)string, h, 0, 1);
                if (h)
                    h->Release();
                if (error == MfScVariableStack::kVarNotFound) {
                    message.Format("'%s': variable does not exist.", (const char *)*(MfString *)string);
                    mLastException = kVmeNoSuchVariable;
                    goto vmException;
                }
                if (error == MfScVariableStack::kVarReadOnly) {
                    message.Format("'%s': variable is read only.", (const char *)*(MfString *)string);
                    mLastException = kVmeAccessDenied;
                    goto vmException;
                }
                break;
            }

            case BC_SWITCHTABLE: {
                // SWITCHTABLE n, n SWITCHENTRYs, then n + 1 jumps: to the case
                // whose constant equals the value, or to the default
                a = env->PopDataPassRef();
                if (!a) {
                    message = "A switch table cannot be evaluated for a nil operand.";
                    mLastException = kVmeNilOperand;
                    goto vmException;
                }
                if (!HasType(a, kHandleInt)) {
                    message = "The switch table must have an integer operand.";
                    mLastException = kVmeIntegerRequired;
                    goto vmException;
                }
                int count = Fetch16(env);
                unsigned char *jumps = env->m_ip + count * MfScBytecodes::BytecodeSize(BC_SWITCHENTRY);
                int i;
                for (i = 0; i < count; i++) {
                    if (*env->m_ip != BC_SWITCHENTRY) {
                        a->Release();
                        mLastException = kVmeSwitchMissingEntry;
                        goto vmException;
                    }
                    env->m_ip++;
                    h = Constant(env, Fetch16(env));
                    if (!HasType(h, kHandleInt)) {
                        a->Release();
                        message = "SWITCHENTRY instruction must point to an integer value.";
                        mLastException = kVmeIntegerRequired;
                        goto vmException;
                    }
                    if (a->GetInt() == h->GetInt())
                        break;
                }
                a->Release();
                int breakpoint = 0;
                MfAiPdbBreakpointManager *manager;
                env->m_ip = jumps;
                if (MfScBytecodes::IsJump(*env->m_ip)) {
                    if (i > 0)
                        env->m_ip += MfScBytecodes::BytecodeSize(*env->m_ip);
                } else {
                    // the debugger planted a breakpoint on the first jump:
                    // step over the bytecode it replaced
                    if (*env->m_ip != BC_BREAKPOINT) {
                        mLastException = kVmeSwitchMissingJump;
                        goto vmException;
                    }
                    breakpoint = 1;
                    if (debugger) {
                        if (debugger->GetInterface(kPdbBreakpointManager, (MfAi **)&manager) == 0) {
                            MfPdbBreakpointInfo info;
                            manager->GetBreakpointInfo((unsigned long)env->m_ip, (int)env, &info);
                            manager->Release();
                            if (i > 0)
                                env->m_ip += MfScBytecodes::BytecodeSize(info.bytecode);
                        } else {
                            message = MfString("A breakpoint instruction was encountered in the switch table\n")
                                + MfString("and the debugger cannot find a breakpoint manager interface.");
                            mLastException = kVmeInternalError;
                            goto vmException;
                        }
                    } else {
                        message = MfString("A breakpoint instruction was encountered in the switch table\n")
                            + MfString("and the debugger seems to be missing.");
                        mLastException = kVmeInternalError;
                        goto vmException;
                    }
                }
                for (int j = 1; j < i; j++) {
                    env->m_ip += MfScBytecodes::BytecodeSize(*env->m_ip);
                    if (!MfScBytecodes::IsJump(*env->m_ip)) {
                        mLastException = kVmeSwitchMissingJump;
                        goto vmException;
                    }
                }
                if (breakpoint) {
                    // let the debugger plant the breakpoint again
                    if (debugger) {
                        if (debugger->GetInterface(kPdbBreakpointManager, (MfAi **)&manager) == 0) {
                            manager->BreakpointHit((unsigned long)jumps, (int)env->m_ip, env);
                            manager->Release();
                        } else {
                            message = MfString("A breakpoint instruction was encountered in the switch table\n")
                                + MfString("and the debugger cannot find a breakpoint manager interface.");
                            mLastException = kVmeInternalError;
                            goto vmException;
                        }
                    } else {
                        message = MfString("A breakpoint instruction was encountered in the switch table\n")
                            + MfString("and the debugger seems to be missing.");
                        mLastException = kVmeInternalError;
                        goto vmException;
                    }
                }
                break;
            }
            case BC_SWITCHENTRY:
                mLastException = kVmeSwitchEntryWithoutTable;
                goto vmException;

            // calls: [args, count, (object,) function]
            case BC_INVOKEFUNC:
                a = env->PopDataPassRef();      // the function
                b = 0;                          // the object it runs on
                goto invoke;
            case BC_INVOKEMETH:
                a = env->PopDataPassRef();
                b = env->PopDataPassRef();
            invoke: {
                if (!a) {
                    if (b)
                        b->Release();
                    mLastException = kVmeNilOperand;
                    goto vmException;
                }
                if (!a->IsType(kHandleMethod)) {
                    a->Release();
                    if (b)
                        b->Release();
                    mLastException = kVmeIllegalFunction;
                    goto vmException;
                }
                MfScDataMethod *method = (MfScDataMethod *)a->GetData();
                if (method->Params() != -1 && StackItem(env, 1)->GetInt() != method->Params()) {
                    message.Format("Method %s needs %d parameter%s", (const char *)method->Name(), method->Params(),
                                   (!method->Params() || method->Params() > 1) ? "s" : "");
                    a->Release();
                    if (b)
                        b->Release();
                    mLastException = kVmeWrongParameterCount;
                    goto vmException;
                }
                if (!env->PushFunctionFrame()) {
                    a->Release();
                    if (b)
                        b->Release();
                    mLastException = kVmeStackOverflow;
                    goto vmException;
                }
                // the frame takes over the references on the function and the object
                env->m_constants = method->GetConstantPool();
                env->m_function = a;
                env->m_object = b;
                if (method->Type() == MfScDataMethod::kNone) {
                    // bytecode: continue with its first instruction
                    env->m_ip = ((MfScBytecodeRoutine *)method)->GetBytecodes();
                    break;
                }
                // C++ code: call it with the arguments, then return its result
                // as RET does
                if (method->Type() == MfScDataMethod::kFunction) {
                    MfScFunction function = ((MfScFunctionRoutine *)method)->GetFunction();
                    result = function(env->m_object, Local(env, 6)->GetInt(), (MfScDataHandle **)(env->m_frame + 7));
                } else if (method->Type() == MfScDataMethod::kMethod) {
                    MfScData *object = env->m_object->GetData();
                    MfScMethod function = ((MfScMethodRoutine *)method)->GetMethod();
                    result = (object->*function)(env->m_object, Local(env, 6)->GetInt(),
                                                 (MfScDataHandle **)(env->m_frame + 7));
                } else if (method->Type() == MfScDataMethod::kExtMethod) {
                    MfScData *object = ((MfScExtRoutine *)method)->GetObject();
                    MfScMethod function = ((MfScExtRoutine *)method)->GetMethod();
                    result = (object->*function)(env->m_object, Local(env, 6)->GetInt(),
                                                 (MfScDataHandle **)(env->m_frame + 7));
                } else
                    break;
                MfScVmException *exception = env->Exception();
                if (exception) {
                    // the function threw a script exception
                    if (result)
                        result->Release();
                    message = exception->Message();
                    n = exception->Code();
                    env->ClearException();
                    mLastException = n;
                    goto vmException;
                }
                if (env->ArtnIsSet()) {
                    // the function made its caller return an integer as well
                    env->PopFunctionFrame();
                    if (result)
                        result->Release();
                    result = MfScDataHandle::CreateData(env->ArtnValue());
                    env->ArtnClearReturn();
                }
                env->PopFunctionFrame();
                env->PushDataPassRef(result);
                break;
            }
            case BC_RETZERO:
                result = MfScDataHandle::CreateData(0);
                env->PopFunctionFrame();
                env->PushDataPassRef(result);
                break;
            case BC_RET:
                result = env->PopDataPassRef();
                env->PopFunctionFrame();
                env->PushDataPassRef(result);
                break;

            case BC_BREAKPOINT:
                // stop the other script threads and let the debugger have its
                // way; it hands back the bytecode the breakpoint replaced
                env->RefreshProcessTimer();
                if (debugger) {
                    csThreadBreakLock.Enter();
                    ThreadManager()->SuspendAll();
                    MfPdbStepContext context;
                    context.env = env;
                    context.stackBase = stackBase;
                    context.routine = env->m_function
                        ? dynamic_cast<MfScBytecodeRoutine *>(env->m_function->GetData()) : 0;
                    context.vm = this;
                    MfScVmFrame frame;
                    env->StoreFrame(&frame);
                    debugger->SignalBreakpoint(&context, &opcode);
                    env->RestoreFrame(&frame);
                    if (env->IsInterrupted())
                        debuggerStopped = 1;
                    ThreadManager()->ResumeAll();
                    csThreadBreakLock.Leave();
                }
                // (without a debugger the breakpoint is executed again and again)
                env->StartProcessTimer();
                goto dispatch;
            case BC_EXIT:
                done = 1;
                break;
            case BC_NOP:
                break;
            default:
                mLastException = kVmeIllegalBytecode;
                goto vmException;
            }
        }
        env->RefreshProcessTimer();
    } catch (MfScDataHandle::MfScMemoryException) {
        mLastException = kVmeOutOfHandles;
        goto vmException;
    } catch (VmSEHandler exception) {
        if (exception.Code() == STATUS_GUARD_PAGE_VIOLATION) {
            env->ResetStackOverflow();
            mLastException = kVmeStackOverflow;
            goto vmException;
        }
    }
    env->FlagStopExecuting();
    if (env->IsInterrupted()) {
        env->ClearInterrupt();
        mLastException = kVmeInterrupt;
        goto vmException;
    }
    _set_se_translator(oldTranslator);
    MfScDataHandle::EnableGCForVM();
    MfScDataHandle::AddGCHint(1);
    mLastException = kVmeNone;
    return 0;

vmException:
    if (mLastException != kVmeInterrupt)
        env->RefreshProcessTimer();
    env->FlagStopExecuting();
    if (debugger && !debuggerStopped) {
        csThreadBreakLock.Enter();
        ThreadManager()->SuspendAll();
        MfPdbStepContext context;
        context.env = env;
        context.stackBase = stackBase;
        context.routine = env->m_function
            ? dynamic_cast<MfScBytecodeRoutine *>(env->m_function->GetData()) : 0;
        context.vm = this;
        {
            MfScVmFrame frame;
            env->StoreFrame(&frame);
            debugger->SignalException(mLastException, message, &context);
            env->RestoreFrame(&frame);
        }
        ThreadManager()->ResumeAll();
        csThreadBreakLock.Leave();
    }
    MfStringList lines;
    FormatException(mLastException, message, &lines);
    MfDblListItem *pos;
    for (MfString *line = lines.First(pos); line; line = lines.Next(pos))
        MfScDiagnostics::PrintNl(MfScDiagnostics::kAlways, *line);
    MfLogger logger;
    logger.Log(2, MfString("Piccolo"), MfString("VirtualMachine"), lines);
    // unwind the call frames and the data stack
    while (env->m_frame)
        env->PopFunctionFrame();
    env->ReleaseStack(stackBase);
    _set_se_translator(oldTranslator);
    MfScDataHandle::EnableGCForVM();
    MfScDataHandle::AddGCHint(1);
    return mLastException;
}

// EQUIV 0x1001b2d0 ?FormatException@MfScVirtualMachine@@QAEXHABVMfString@@PAVMfStringList@@@Z: reviewed: register assignment differs (original: lines in esi, env in edi, the temporaries in the code argument slot, Format and the char* cast called through registers) and it inlines ~MfScVmFrame as ReleaseHandles; same calls and flow
void MfScVirtualMachine::FormatException(int code, const MfString &message, MfStringList *lines)
{
    MfScVmEnv *env = ((MfScThread *)MfThread::GetThread())->mVmEnv;
    lines->Clear();
    MfString line("**** VM Exception encountered: ");
    line << ExceptionName(code);
    lines->Add(line);
    if (message != NULL_STRING)
        lines->Add(message);
    lines->Add(MfString(""));
    lines->Add(MfString("CALL STACK"));
    lines->Add(MfString("============="));

    MfScVmFrame frame;
    env->FirstFrame(&frame);
    while (frame.mFrame) {
        MfScData *function = frame.mFunction ? dynamic_cast<MfScData *>(frame.mFunction->GetData()) : 0;
        MfScBytecodeRoutine *routine =
            frame.mFunction ? dynamic_cast<MfScBytecodeRoutine *>(frame.mFunction->GetData()) : 0;
        if (routine) {
            MfString where;
            where.Format("%s(): ", (const char *)function->Name());
            int offset = frame.mIP - (unsigned long)routine->mBytecodes;
            if (routine->DebugInfo())
                line.Format("Line %d in Module: %s", routine->DebugInfo()->FindLine(offset),
                            (const char *)routine->ModuleName());
            else
                line.Format("Offset %d in Module: %s", offset, (const char *)routine->ModuleName());
            where << line;
            line = where;
        } else
            line.Format("%s()", (const char *)function->Name());
        lines->Add(line);
        env->NextFrame(&frame);
    }

    // keep the innermost frames, the outermost three and an ellipsis
    if (lines->GetCount() > 14) {
        int surplus = lines->GetCount() - 14;
        MfDblListItem *pos;
        lines->MfDblList::GetLast(pos);
        lines->MfDblList::GetPrev(pos);
        lines->MfDblList::GetPrev(pos);
        lines->MfDblList::GetPrev(pos);
        while (surplus--) {
            lines->Delete(pos);
            lines->MfDblList::GetPrev(pos);
        }
        lines->Insert(new MfString("    ...    "), MfDblList::LP_AFTER, pos);
    }
    lines->Add(MfString(NULL_STRING));
}

// 0x1001b670 ?ExceptionName@MfScVirtualMachine@@SA?AVMfString@@H@Z
MfString __cdecl MfScVirtualMachine::ExceptionName(int code)
{
    MfString name((MfStringSize(128)));
    name.Format("VME%08x: ", code);
    switch (code) {
    case kVmeNone:
        name << "No Exception";
        break;
    case kVmeIllegalBytecode:
        name << "Illegal bytecode";
        break;
    case kVmeOutOfHandles:
        name << "Out of handles";
        break;
    case kVmeStackOverflow:
        name << "Stack Overflow";
        break;
    case kVmeNullConstantPool:
        name << "Null constant pool pointer";
        break;
    case kVmeNilOperand:
        name << "Operation on 'nil' operand";
        break;
    case kVmeAccessDenied:
        name << "Access Denied";
        break;
    case kVmeBadMathOperand:
        name << "Bad math operand";
        break;
    case kVmeDivideByZero:
        name << "Divide by zero";
        break;
    case kVmeIntegerRequired:
        name << "Integer operand required";
        break;
    case kVmeStringRequired:
        name << "String operand required";
        break;
    case kVmeIllegalOperand:
        name << "Illegal operand";
        break;
    case kVmeArrayRequired:
        name << "Array operand required";
        break;
    case kVmeIndexOutOfBounds:
        name << "Array index out of bounds";
        break;
    case kVmeInvalidObject:
        name << "Invalid object";
        break;
    case kVmePropertyError:
        name << "There was no property error";
        break;
    case kVmePropertyReadOnly:
        name << "Permission denied on property";
        break;
    case kVmeNoSuchProperty:
        name << "No such property";
        break;
    case kVmeNoPropertyTable:
        name << "No property table";
        break;
    case kVmeInvalidPropertyType:
        name << "Invalid property type";
        break;
    case kVmeInvalidPropertyHandle:
        name << "Invalid property handle";
        break;
    case kVmeNoSuchVariable:
        name << "No such variable";
        break;
    case kVmeIllegalFunction:
        name << "Illegal function";
        break;
    case kVmeWrongParameterCount:
        name << "Invalid number of function parameters";
        break;
    case kVmeInterrupt:
        name << "Interrupt";
        break;
    case kVmeUserException:
        name << "User defined exception";
        break;
    case kVmeSwitchMissingEntry:
        name << "Switch table missing an entry";
        break;
    case kVmeSwitchMissingJump:
        name << "Switch table missing a jump instruction";
        break;
    case kVmeSwitchEntryWithoutTable:
        name << "Switch entry encountered without a SWITCHTABLE";
        break;
    case kVmeInternalError:
        name << "Internal VM error";
        break;
    default:
        name << "Unknown exception";
        break;
    }
    return name;
}
