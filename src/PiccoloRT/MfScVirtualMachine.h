// MfScVirtualMachine: executes bytecode functions on the calling thread's MfScVmEnv.
#pragma once
#ifndef PICCOLORT_API
#define PICCOLORT_API __declspec(dllimport)
#endif
#include "../mfutil/MfCriticalSection.h"
class MfScDataHandle;
class MfString;
class MfStringList;

// Executes bytecode functions on the calling thread's MfScVmEnv.
class PICCOLORT_API MfScVirtualMachine {
public:
    // the exceptions ExecuteCode ends with (LastException, ExceptionName)
    enum MfScVmExceptionCode {
        kVmeNone = 0,
        kVmeIllegalBytecode = 1,
        kVmeNullConstantPool = 2,
        kVmeOutOfHandles = 3,
        kVmeInternalError = 4,
        kVmeStackOverflow = 5,
        kVmeNilOperand = 6,
        kVmeAccessDenied = 7,
        kVmeBadMathOperand = 8,
        kVmeDivideByZero = 9,
        kVmeIntegerRequired = 10,
        kVmeStringRequired = 11,
        kVmeIllegalOperand = 12,
        kVmeArrayRequired = 13,
        kVmeIndexOutOfBounds = 14,
        kVmeInvalidObject = 15,
        kVmeNoSuchVariable = 16,
        kVmeIllegalFunction = 17,
        kVmeWrongParameterCount = 18,
        kVmeInterrupt = 19,
        kVmeSwitchMissingEntry = 20,
        kVmeSwitchMissingJump = 21,
        kVmeSwitchEntryWithoutTable = 22,
        kVmeUserException = 23,
        // 24 + MfScDataObject::MfPropertyErrorCode
        kVmePropertyError = 24,
        kVmePropertyReadOnly = 25,
        kVmeNoSuchProperty = 26,
        kVmeNoPropertyTable = 27,
        kVmeInvalidPropertyType = 28,
        kVmeInvalidPropertyHandle = 29
    };

    MfScVirtualMachine();
    virtual ~MfScVirtualMachine();
    MfScDataHandle *ExecuteFunctionArrayParams(MfScDataHandle *function, int numArgs, MfScDataHandle **args);
    MfScDataHandle *__cdecl ExecuteFunction(MfScDataHandle *function, int numArgs, ...);
    int LastException() const { return mLastException; }
    static MfString __cdecl ExceptionName(int code);
    void FormatException(int code, const MfString &message, MfStringList *lines);
private:
    int ExecuteCode(unsigned char *code, MfScDataHandle **result);
    static MfCriticalSection csThreadBreakLock;

    int mLastException;             // +0x04
};
