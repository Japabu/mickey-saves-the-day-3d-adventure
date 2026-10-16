// PORT: the placeholder at +0x24 is an int like the real member (MfScBytecodeRoutine.h), so this view has the real layout on 64-bit.
// PiccoloRT.dll (vc5, no PDB): declarations for the MfScBytecodeRoutine unit
// (0x10002060-0x100028b5). Layouts from the compiler-generated copy
// constructors / assignment operators and from the code of this unit.
#ifndef PICCOLORT_MFSCBYTECODEROUTINE_H
#define PICCOLORT_MFSCBYTECODEROUTINE_H

#include <windows.h> // GetClassName -> GetClassNameA in the exported names

#define MFIMPORT __declspec(dllimport)
#define PICAPI __declspec(dllexport)

// RuntimeLib.dll
struct MfStringRep {
    long m_unk0;   // +0
    long m_unk4;   // +4
    int m_length;  // +8
};

class MFIMPORT MfString {
public:
    MfString();
    MfString(const char *s);
    MfString(const MfString &s);
    ~MfString();
    MfString &operator=(const MfString &s);
    int Length() const { return m_rep->m_length; }
private:
    MfStringRep *m_rep; // +0
};

class MFIMPORT MfLock {
public:
    int Lock(long timeout = -1);
    void Unlock();
private:
    void *m_handle;
};

class MfScData;
class MfScLinkData;
class MfStringList;
class MfScPropTokenTable;
class MfScPropertyExtList;
class MfAiClassInfo;

class PICAPI MfScDataHandle {
public:
    int IsType(unsigned long type) const { return m_flags & type; }
    MfScData *GetData() const { return (m_flags & 0xfc0) ? m_data : 0; }
private:
    unsigned short m_refCount; // +0
    unsigned short m_flags;    // +2
    union {                    // +4
        float m_float;
        long m_long;
        MfScData *m_data;
    };
};

class PICAPI MfScData {
public:
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

class PICAPI MfScDataMethod : public MfScData {
public:
    enum MfScMethodType { kNone, kFunction, kMethod, kExtMethod };
    MfScDataMethod(MfScMethodType type, int numArgs);
    virtual ~MfScDataMethod();
    MfScDataHandle **GetConstantPool() const { return m_constants; }
    int Params() const { return m_numArgs; }
protected:
    int m_type;                     // +0x0c
    MfScDataHandle **m_constants;   // +0x10
    int m_numConstants;             // +0x14
    int m_numArgs;                  // +0x18
};

class PICAPI MfScDebugInfo {
public:
    virtual ~MfScDebugInfo();                 // 0x10008bd0
    void SetCodeSize(int size) { m_codeSize = size; }
    int NumCodeLevelInfo() const { return m_numCodeLevelInfo; }
    int NumConstUsageInfo() const { return m_numConstUsageInfo; }
    unsigned int MemoryUsage();               // 0x10009350
private:
    int m_codeSize;            // +0x04
    long m_unk08;              // +0x08
    long m_unk0c;              // +0x0c
    long m_unk10;              // +0x10
    long m_unk14;              // +0x14
    long m_unk18;              // +0x18
    int m_numCodeLevelInfo;    // +0x1c
    long m_unk20;              // +0x20
    int m_numConstUsageInfo;   // +0x24
};

class PICAPI MfScBytecodeRoutine : public MfScDataMethod {
public:
    MfScBytecodeRoutine(int numArgs);
    virtual ~MfScBytecodeRoutine();
    unsigned char *GetBytecodes() const { return m_bytecodes; }
    void Attach(MfScDebugInfo *info);
    MfScDebugInfo *Detach();
    unsigned int MemoryUsage(int what);
private:
    unsigned char *m_bytecodes;   // +0x1c
    int m_codeSize;               // +0x20
    int m_unk24;                  // PORT: int, as the real mFlags (long moved the members after it); +0x24
    MfString *m_str28;            // +0x28
    MfString *m_str2c;            // +0x2c
    MfString *m_str30;            // +0x30
    MfScDebugInfo *m_debugInfo;   // +0x34
    MfString *m_str38;            // +0x38
};

class PICAPI MfScDataObject : public MfScData {
public:
    MfScDataObject(unsigned int flags = 0);   // 0x10007900
    enum MfPropertyErrorCode { MfPropertyErrorCode_unknown };
    virtual ~MfScDataObject();                // 0x100079b0
protected:
    virtual MfScLinkData *CountCircLinks(MfScDataHandle *);
    virtual void ReleaseCircLinks(MfScLinkData *);
    virtual int GetThroughCount(MfScDataHandle *);
public:
    virtual MfPropertyErrorCode GetProperty(const MfString &, MfScDataHandle **);
    virtual MfPropertyErrorCode SetProperty(MfScDataHandle *, const MfString &, MfScDataHandle *, int);
    virtual MfString GetClassName() { return MfString("Object"); }
    virtual void GetPropertyNames(MfStringList *, int);
    virtual void GetInstanceProperties(MfStringList *, int);
    virtual MfScPropTokenTable *GetPropTokenTable() { return 0; }
    virtual void FilloutPropertyNameList(MfStringList *);
    virtual MfScPropertyExtList *GetPropertyExtensions() { return 0; }
    int HasInstanceProperties() const { return m_instanceProps != 0; }
    static void SetClassInfoInterface(MfAiClassInfo *ci) { paiciCiInterface = ci; }
    unsigned int Flags() const { return m_flags; }
protected:
    void *m_instanceProps;      // +0x0c
    unsigned int m_flags;       // +0x10
    static MfAiClassInfo *paiciCiInterface;
};

class PICAPI MfScObject : public MfScDataObject {
public:
    MfScObject(unsigned int flags = 0) : MfScDataObject(flags | 0x200) {}
protected:
    virtual MfScLinkData *CountCircLinks(MfScDataHandle *);
    virtual void ReleaseCircLinks(MfScLinkData *);
    virtual int GetThroughCount(MfScDataHandle *);
    // no user-declared destructor: ??1MfScObject is the implicit one
};

class PICAPI MfScObjString : public MfScObject, public MfString {
public:
    MfScObjString(int readOnly = 0);          // 0x1000c740
    virtual ~MfScObjString();                 // 0x1000c7b0
    virtual MfScPropTokenTable *GetPropTokenTable() { return pttPropTokens; }
    static MfScPropTokenTable *PropertyTokenTable() { return pttPropTokens; }
    int IsReadOnly() const { return m_readOnly; }
    void SetReadOnly() { m_readOnly = 1; }
    void SetReadWrite() { m_readOnly = 0; }
protected:
    virtual MfString GetClassName() { return MfString("String"); }
private:
    int m_readOnly;             // +0x18
    static MfScPropTokenTable *pttPropTokens;
};

class PICAPI MfScBytecodes {
public:
    MfScBytecodes() {}
    static int BytecodeSize(unsigned char op);
    static int IsJump(unsigned char op);
    static int JumpOffset(unsigned char *code);
    static int IsUnconditionalJump(unsigned char op);
    static int IsJumpZero(unsigned char op);
private:
    static int aiBytecodeSize[];
};

#endif
