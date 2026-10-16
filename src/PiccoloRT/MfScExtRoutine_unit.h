// PiccoloRT.dll (vc5, no PDB): declarations for the MfScExtRoutine unit
// (0x10001000-0x100016ba). Layouts from the compiler-generated copy
// constructors / assignment operators.
#ifndef PICCOLORT_MFSCEXTROUTINE_H
#define PICCOLORT_MFSCEXTROUTINE_H

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

class MfScDataHandle;

class PICAPI MfScDataHandle {
public:
    unsigned short GetRefCount() const { return m_refCount; }
    unsigned int GetType() const { return m_flags & 0x1ff0; }
private:
    unsigned short m_refCount;  // +0
    unsigned short m_flags;     // +2
};

class PICAPI MfScBreakpointManager {
public:
    static MfScBreakpointManager *Manager() { return pbrkptmgrMain; }
    static void Startup();
    static void Shutdown();
private:
    MfScBreakpointManager();
    virtual ~MfScBreakpointManager();
    static MfScBreakpointManager *pbrkptmgrMain;
};

class PICAPI MfScData {
public:
    virtual ~MfScData();
    MfString &Name() { return *m_name; }
    void Lock() { m_lock.Lock(); }
    void Unlock() { m_lock.Unlock(); }
protected:
    MfLock m_lock;      // +4
    MfString *m_name;   // +8
};

typedef MfScDataHandle *(*MfScFunction)(MfScDataHandle *, int, MfScDataHandle **);
typedef MfScDataHandle *(MfScData::*MfScMethod)(MfScDataHandle *, int, MfScDataHandle **);

class PICAPI MfScDataMethod : public MfScData {
public:
    enum MfScMethodType { kNone, kFunction, kMethod, kExtMethod };
    MfScDataMethod(MfScMethodType type, int numArgs);
    virtual ~MfScDataMethod();
    MfScDataHandle *GetConstant(int i) { return m_constants[i]; }
    int ConstantPoolSize() const { return m_numConstants; }
protected:
    int m_type;                     // +0x0c
    MfScDataHandle **m_constants;   // +0x10
    int m_numConstants;             // +0x14
    int m_numArgs;                  // +0x18
};

class PICAPI MfScFunctionRoutine : public MfScDataMethod {
public:
    MfScFunctionRoutine(MfScFunction fn = 0, int numArgs = -1)
        : MfScDataMethod(kFunction, numArgs), m_function(fn) {}
    void SetFunction(MfScFunction fn) { m_function = fn; }
private:
    MfScFunction m_function;        // +0x1c
};

class PICAPI MfScMethodRoutine : public MfScDataMethod {
public:
    MfScMethodRoutine(MfScMethod method = 0, int numArgs = -1)
        : MfScDataMethod(kMethod, numArgs), m_ordinal(0), m_method(method) {}
    MfScMethodRoutine(int ordinal, MfScMethod method, int numArgs = -1)
        : MfScDataMethod(kMethod, numArgs), m_method(method), m_ordinal(ordinal) {}
    void SetMethod(MfScMethod method) { m_method = method; }
    void SetOrdinal(int ordinal) { m_ordinal = ordinal; }
private:
    MfScMethod m_method;            // +0x1c
    int m_ordinal;                  // +0x20
};

class PICAPI MfScExtRoutine : public MfScDataMethod {
public:
    MfScExtRoutine(MfScData *object, MfScMethod method, int numArgs = -1);
    MfScExtRoutine(MfScData *object, int ordinal, MfScMethod method, int numArgs = -1);
    void Bind(MfScData *object, MfScMethod method);
    void SetOrdinal(int ordinal);
private:
    MfScMethod m_method;            // +0x1c
    MfScData *m_object;             // +0x20
    int m_ordinal;                  // +0x24
};

#endif
