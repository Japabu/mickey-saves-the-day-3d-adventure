#pragma once
class MfScDataHandle;
class MfScDebugInfo;
class MfScLinkData;
class MfString;

// RuntimeLib: mutex wrapper.
class __declspec(dllimport) MfLock {
public:
    MfLock();
    ~MfLock();
private:
    void *m_handle;
};

// PiccoloRT.dll: base of everything a data handle can point to.
class __declspec(dllimport) MfScData {
public:
    MfScData();
    virtual ~MfScData();
    virtual MfScLinkData *CountCircLinks(MfScDataHandle *);
    virtual void ReleaseCircLinks(MfScLinkData *);
    virtual int GetThroughCount(MfScDataHandle *);
    virtual void AddRef();
    virtual void Release();
protected:
    MfLock m_lock;      // +4
    MfString *m_name;   // +8
};

// PiccoloRT.dll: base of the callable data, with the constant pool.
class __declspec(dllimport) MfScDataMethod : public MfScData {
public:
    enum MfScMethodType { kNone, kFunction, kMethod, kExtMethod };
    MfScDataMethod(MfScMethodType type, int numArgs);
    virtual ~MfScDataMethod();
    MfScDataHandle **AllocConstantPool(int size);
    void SetConstant(int i, MfScDataHandle *h);
    MfScDataHandle **GetConstantPool() const { return m_constants; }
protected:
    int m_type;                     // +0x0c
    MfScDataHandle **m_constants;   // +0x10
    int m_numConstants;             // +0x14
    int m_numArgs;                  // +0x18
};

// PiccoloRT.dll: a routine compiled to Piccolo bytecode (0x3c bytes).
class __declspec(dllimport) MfScBytecodeRoutine : public MfScDataMethod {
public:
    MfScBytecodeRoutine(int numArgs);
    virtual ~MfScBytecodeRoutine();
    MfScDebugInfo *DebugInfo() const { return mDebugInfo; }
    MfString &FunctionParams() { return *mFunctionParams; }
    MfString &ModuleName() { return *mModuleName; }
    MfString &ModuleType() { return *mModuleType; }
    MfScDebugInfo *Detach();
    unsigned char *AllocBytecodes(int);
    void Attach(MfScDebugInfo *);
    unsigned char *mBytecodes;      // +0x1c
    int mCodeSize;                  // +0x20
    int mFlags;                     // +0x24
    MfString *mModuleName;          // +0x28
    MfString *mModuleType;          // +0x2c
    MfString *mFunctionParams;      // +0x30
    MfScDebugInfo *mDebugInfo;      // +0x34
    MfString *mObjectFileName;      // +0x38
};
