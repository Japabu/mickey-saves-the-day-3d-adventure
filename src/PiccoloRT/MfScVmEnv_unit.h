// PiccoloRT.dll (vc5, no PDB): declarations for the MfScVmEnv unit
// (0x10001af0-0x10002058). Inline members of MfScVmEnv / MfScBytecodeRoutine
// and their compiler-generated members. Layouts from the generated copy
// constructors / assignment operators.
#ifndef PICCOLORT_MFSCVMENV_H
#define PICCOLORT_MFSCVMENV_H

#include <windows.h>

#define MFIMPORT __declspec(dllimport)
#define PICAPI __declspec(dllexport)

// RuntimeLib.dll
class MFIMPORT MfString;

class MFIMPORT MfLock {
public:
    int Lock(long timeout = -1);
    void Unlock();
private:
    void *m_handle;
};

class MfScVmException;
class MfScDebugInfo;
class MfScVariableStack;

class PICAPI MfScDataHandle {
public:
    unsigned short AddRef() { return ++m_refCount; }
    unsigned short Release() { return m_refCount > 0 ? --m_refCount : m_refCount; }
private:
    unsigned short m_refCount;  // +0
    unsigned short m_flags;     // +2
};

// sizeof == 0x78; 8-byte aligned (__int64 members), so the vfptr is padded
// to 8 and +4 / +0x5c are padding (not copied by the generated copy ctor).
class PICAPI MfScVmEnv {
public:
    virtual ~MfScVmEnv();

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
    void StartProcessTimer() { QueryPerformanceCounter(&m_timer.start); }
    void RefreshProcessTimer() {
        QueryPerformanceCounter(&m_timer.now);
        m_accumTime += m_timer.now.QuadPart - m_timer.start.QuadPart;
    }
    __int64 GetAccumProcessTime() { return m_accumTime; }

    void PushData(MfScDataHandle *h) {
        if (h)
            h->AddRef();
        *m_sp = h;
        m_sp--;
    }
    void PushDataNoCheck(MfScDataHandle *h) {
        h->AddRef();
        *m_sp = h;
        m_sp--;
    }
    void PushDataPassRef(MfScDataHandle *h) {
        *m_sp = h;
        m_sp--;
    }
    void PushWord(unsigned long w) {
        *(unsigned long *)m_sp = w;
        m_sp--;
    }
    MfScDataHandle *PopDataPassRef() { return *++m_sp; }
    unsigned long PopWord() { return *(unsigned long *)++m_sp; }
    void PopAndReleaseData() {
        MfScDataHandle *h = *++m_sp;
        if (h)
            h->Release();
    }
    void PopAndReleaseDataNoCheck() {
        MfScDataHandle *h = *++m_sp;
        h->Release();
    }

private:
    long m_unk8;                 // +0x08
    MfScDataHandle **m_sp;       // +0x0c
    long m_unk10;                // +0x10
    long m_unk14;                // +0x14
    long m_unk18;                // +0x18
    long m_unk1c;                // +0x1c
    MfScDataHandle *m_reg0;      // +0x20
    MfScDataHandle *m_reg1;      // +0x24
    MfScDataHandle *m_reg2;      // +0x28
    MfScDataHandle *m_reg3;      // +0x2c
    MfScVariableStack *m_varStack; // +0x30
    long m_unk34;                // +0x34
    long m_unk38;                // +0x38
    long m_unk3c;                // +0x3c
    MfScVmException *m_exception; // +0x40
    int m_artnSet;               // +0x44
    int m_artnValue;             // +0x48
    int m_messageThread;         // +0x4c
    int m_interrupted;           // +0x50
    int m_executing;             // +0x54
    int m_id;                    // +0x58
    struct Timer {
        LARGE_INTEGER start;     // +0x60
        LARGE_INTEGER now;       // +0x68
    };
    Timer m_timer;               // +0x60
    __int64 m_accumTime;         // +0x70
};

class PICAPI MfScData {
public:
    virtual ~MfScData();
protected:
    MfLock m_lock;      // +4
    MfString *m_name;   // +8
};

class PICAPI MfScDataMethod : public MfScData {
public:
    virtual ~MfScDataMethod();
protected:
    int m_type;                     // +0x0c
    MfScDataHandle **m_constants;   // +0x10
    int m_numConstants;             // +0x14
    int m_numArgs;                  // +0x18
};

class PICAPI MfScBytecodeRoutine : public MfScDataMethod {
public:
    MfScBytecodeRoutine(int numArgs = -1);
    virtual ~MfScBytecodeRoutine();

    unsigned char *AllocBytecodes(int size) {
        m_size = size;
        m_bytecodes = new unsigned char[size];
        return m_bytecodes;
    }
    void DeleteBytecodes() {
        if (m_bytecodes)
            delete[] m_bytecodes;
        m_bytecodes = 0;
    }
    unsigned char *GetBytecodeLocAtOffset(int offset) { return m_bytecodes + offset; }
    int CalcOffsetOfBytecode(unsigned char *p) { return p - m_bytecodes; }
    int HasDebugInfo() const { return m_debugInfo != 0; }
    MfScDebugInfo *DebugInfo() const { return m_debugInfo; }
    MfString &ModuleName() { return *m_moduleName; }
    MfString &ModuleType() { return *m_moduleType; }
    MfString &FunctionParams() { return *m_functionParams; }
    MfString &ObjectFileName() { return *m_objectFileName; }
    int &Flags() { return m_flags; }

private:
    unsigned char *m_bytecodes;     // +0x1c
    int m_size;                     // +0x20
    int m_flags;                    // +0x24
    MfString *m_moduleName;         // +0x28
    MfString *m_moduleType;         // +0x2c
    MfString *m_functionParams;     // +0x30
    MfScDebugInfo *m_debugInfo;     // +0x34
    MfString *m_objectFileName;     // +0x38
};

#endif
