// PiccoloRT.dll (vc5, no PDB): declarations for the MfScConsoleManager_2 unit
// (0x10004500-0x10004f07): the script functions of MfScConsoleManager, then
// MfScLinkData, MfScData, MfScDataArray (+ the private MfScDataArrayImp and
// MfScListLinkData<int>, names from RTTI), MfScDebugger, MfScDiagnosticInfo.
// Layouts from the compiler-generated copy constructors and the code.
#ifndef PICCOLORT_MFSCCONSOLEMANAGER_2_H
#define PICCOLORT_MFSCCONSOLEMANAGER_2_H

#include <windows.h> // GetClassName -> GetClassNameA in the exported names

#define MFIMPORT __declspec(dllimport)
#define PICAPI __declspec(dllexport)

// ---- RuntimeLib / mfutil ---------------------------------------------------

class MfStringSize {
public:
    MfStringSize(int size) : m_size(size) {}
    int m_size;
};

class MFIMPORT MfString {
public:
    MfString();
    MfString(const char *s);
    MfString(const MfString &s);
    MfString(const MfStringSize &size);
    ~MfString();
    MfString &operator=(const MfString &s);
    MfString &operator=(const char *s);
    MfString &operator<<(const MfString &s);
    int operator==(const char *s) const;
private:
    void *m_rep; // +0
};

// Spin lock: the constructor is inline (InterlockedExchange on the counter).
// The destructor is inline and empty: ~MfScData calls nothing for m_lock, but
// the EH unwind funclets call the imported copy (__imp_??1MfLock@@QAE@XZ).
class MFIMPORT MfLock {
public:
    MfLock() : m_count(0) { InterlockedExchange(&m_count, 0); }
    ~MfLock() {}
    int Lock(long timeout = -1);
    void Unlock();
private:
    long m_count; // +0
};

class MfAiPdbDebugger;

// ---- PiccoloRT ---------------------------------------------------------------

class MfScData;
class MfScDataObject;
class MfStringList;

class PICAPI MfScDataHandle {
public:
    int IsType(unsigned long type) const { return m_flags & type; }
    int IsVisiting() const { return m_visiting; }
    void SetVisiting() { m_visiting = 1; }
    void ResetVisiting() { m_visiting = 0; }
    MfScData *GetData() const { return (m_flags & 0xfc0) ? m_data : 0; }
    unsigned short AddRef() { return ++m_refCount; }
    unsigned short Release() { return m_refCount > 0 ? --m_refCount : m_refCount; }
    void InfoShort(MfString *s);
    static MfScDataHandle *CreateData(int value);
private:
    MfScDataHandle(const MfScDataHandle &) {}
    MfScDataHandle &operator=(const MfScDataHandle &) { return *this; }
    unsigned short m_refCount; // +0
    union {                    // +2
        unsigned short m_flags;
        struct {
            unsigned short m_bit0 : 1;
            unsigned short m_bit1 : 1;
            unsigned short m_visiting : 1; // 0x4
            unsigned short m_readOnly : 1; // 0x8
        };
    };
    union {                    // +4
        float m_float;
        long m_long;
        MfScData *m_data;
    };
};

class PICAPI MfScConsoleManager {
public:
    virtual ~MfScConsoleManager();
    static MfScConsoleManager *Manager() { return pcmManager; }
    void PrintString(const MfString &s);
    void PrintStringNl(const MfString &s);
    void ClearConsole();
private:
    static MfScDataHandle *Console_Print(MfScDataHandle *self, int argc, MfScDataHandle **argv);
    static MfScDataHandle *Console_PrintNl(MfScDataHandle *self, int argc, MfScDataHandle **argv);
    static MfScDataHandle *Console_Clear(MfScDataHandle *self, int argc, MfScDataHandle **argv);

    void *m_outputs; // +4
    static MfScConsoleManager *pcmManager;
};

class PICAPI MfScLinkData {
public:
    MfScLinkData() : m_count(0) {}
    virtual ~MfScLinkData() {}
    static void *operator new(size_t size);  // 0x1000bd80
    static void operator delete(void *p);    // 0x1000bd90
    int m_count; // +4
};

template <class T> struct MfScListLinkItem {
    MfScListLinkItem(T value) : m_value(value) {}
    ~MfScListLinkItem() {}
    T m_value;                     // +0
    MfScListLinkItem<T> *m_next;   // +4
};

template <class T> class MfScListLinkData : public MfScLinkData {
public:
    typedef MfScListLinkItem<T> Item;
    MfScListLinkData() : m_head(0) {}
    virtual ~MfScListLinkData() {
        Item *item = m_head;
        while (item) {
            Item *next = item->m_next;
            delete item;
            item = next;
        }
    }
    void Add(T value) {
        Item *item = new Item(value);
        item->m_next = m_head;
        m_head = item;
    }
    Item *m_head; // +8
};

class PICAPI MfScData {
public:
    MfScData();
    virtual ~MfScData();
    virtual MfScLinkData *CountCircLinks(MfScDataHandle *h);
    virtual void ReleaseCircLinks(MfScLinkData *l);
    virtual int GetThroughCount(MfScDataHandle *h);
    virtual void AddRef();
    virtual void Release();
protected:
    MfLock m_lock;      // +4
    MfString *m_name;   // +8
};

class MfScPropTokenTable;
class MfScPropertyExtList;

class PICAPI MfScDataObject : public MfScData {
public:
    enum MfPropertyErrorCode { kPropOk = 0, kPropReadOnly = 1, kPropNotFound = 2 };
    MfScDataObject(unsigned int flags = 0);
    virtual ~MfScDataObject();
protected:
    virtual MfScLinkData *CountCircLinks(MfScDataHandle *);
    virtual void ReleaseCircLinks(MfScLinkData *);
    virtual int GetThroughCount(MfScDataHandle *);
public:
    virtual MfPropertyErrorCode GetProperty(const MfString &name, MfScDataHandle **value);
    virtual MfPropertyErrorCode SetProperty(MfScDataHandle *self, const MfString &name,
                                            MfScDataHandle *value, int create);
    virtual MfString GetClassName();
    virtual void GetPropertyNames(MfStringList *list, int all);
    virtual void GetInstanceProperties(MfStringList *list, int all);
    virtual MfScPropTokenTable *GetPropTokenTable();
    virtual void FilloutPropertyNameList(MfStringList *list);
    virtual MfScPropertyExtList *GetPropertyExtensions();
protected:
    void *m_instanceProps;      // +0x0c
    unsigned int m_flags;       // +0x10
};

class PICAPI MfScDataArray : public MfScDataObject {
public:
    virtual ~MfScDataArray();
    virtual MfPropertyErrorCode GetProperty(const MfString &name, MfScDataHandle **value);
    virtual MfPropertyErrorCode SetProperty(MfScDataHandle *self, const MfString &name,
                                            MfScDataHandle *value, int create);
    // Pure virtuals (names unknown; not exported).
    virtual int SetAt(int index, MfScDataHandle *value, int noAddRef) = 0;
    virtual int GetAt(int index, MfScDataHandle **value, int addRef) = 0;
    virtual int IsValid() = 0;
protected:
    MfScDataArray(int size);
    virtual MfString GetClassName();
    int m_size; // +0x14
};

// Private implementation (vtable 0x1002079c, RTTI .?AVMfScDataArrayImp@@).
// own copy: this unit declares its own views of MfString, MfScData and
// MfScDataObject, which conflict with the headers MfScDataArrayImp.h includes.
class MfScDataArrayImp : public MfScDataArray {
public:
    MfScDataArrayImp(int size);
    virtual ~MfScDataArrayImp();
    virtual MfScLinkData *CountCircLinks(MfScDataHandle *h);
    virtual void ReleaseCircLinks(MfScLinkData *l);
    virtual int GetThroughCount(MfScDataHandle *h);
    virtual int SetAt(int index, MfScDataHandle *value, int noAddRef);
    virtual int GetAt(int index, MfScDataHandle **value, int addRef);
    virtual int IsValid();
private:
    MfScDataHandle **m_items; // +0x18
};

class PICAPI MfScDebugger {
public:
    virtual ~MfScDebugger() {}
    static MfAiPdbDebugger *Debugger();
    static void InitDebuggerPointer(MfAiPdbDebugger *dbg);
    static void ClearDebuggerPointer();
private:
    static MfAiPdbDebugger *pdbgTheDebugger;
};

class PICAPI MfScDiagnosticInfo {
public:
    MfScDiagnosticInfo() : m_type(0) {}
    MfString m_text; // +0
    int m_type;      // +4
};

class PICAPI MfScDiagnosticCallBack {
public:
    virtual ~MfScDiagnosticCallBack() {}
    virtual void CallBack(const MfScDiagnosticInfo &info);
};

#endif
