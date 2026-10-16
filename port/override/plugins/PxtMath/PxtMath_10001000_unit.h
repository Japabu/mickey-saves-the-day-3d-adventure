// PORT: the runtime classes are declared with the member types of the real ones (pointers and MfStrings where this view had int or padding), so objects allocated here are as big as PiccoloRT's constructors expect on 64-bit and the VM env is read at the real offset.
// PxtMath.dll 0x10001000-0x10002040: the "Math" script extension (MfPxtMath).
// Motion Factory runtime declarations as far as this unit needs them; names and
// signatures from the decorated import names, layouts from the code and RTTI.
#ifndef PXTMATH_10001000_H
#define PXTMATH_10001000_H

class MfStringList;
class MfScLinkData;
class MfScPropTokenTable;
class MfScPropertyExtList;
struct _TREEITEM;
struct HWND__;

class __declspec(dllimport) MfNullString;

class __declspec(dllimport) MfString {
public:
    MfString(const char *);
    ~MfString();
    MfString &operator=(const MfString &);
    MfString &operator=(const MfNullString &);
    MfString operator+(const MfString &) const;
private:
    char *m_p;
};
extern __declspec(dllimport) MfNullString NULL_STRING;

class MfScDataHandle;
class MfScData;
typedef MfScDataHandle *(MfScData::*MfScMethod)(MfScDataHandle *, int, MfScDataHandle **);

// data handle: +0 reference count, +2 type flags, +4 value
class __declspec(dllimport) MfScDataHandle {
public:
    static MfScDataHandle *CreateData(float);
    static MfScDataHandle *CreateData(MfScData *, MfScMethod, const MfString &, int);
    MfScData *GetData() { return (m_flags & 0xfc0) ? m_data : 0; }
    int IsNumber() { return m_type & 0x30; }
    float GetFloat() { return (m_type & 0x10) ? (float)m_int : m_float; }
    void Release() { if (m_refs > 0) m_refs--; }

    unsigned short m_refs;
    union {
        unsigned short m_flags;
        unsigned char m_type;
    };
    union {
        MfScData *m_data;
        int m_int;
        float m_float;
    };
};

class MfScVariableStack {
public:
    enum MfScVariableErrorCode { };
};

class __declspec(dllimport) MfScVmEnv {
public:
    static MfScVariableStack::MfScVariableErrorCode GetGlobalVariable(const MfString &, MfScDataHandle **);
    void ThrowException(int, const MfString &);
};

class __declspec(dllimport) MfThread {
public:
    static MfThread *GetThread();
    // PORT: the members of the real MfThread (src/mfutil/MfThread.h) in place of
    // char m_pad[0x20], so m_env (MfScThread::mVmEnv) is at the real offset (+64 on 64-bit)
    void *m_vtbl;
    unsigned int m_threadId;
    int m_flags;
    MfThread *m_thread;
    void *m_handle;
    unsigned int m_state;
    void *m_workEvent;
    void *m_work;
    MfScVmEnv *m_env;     // +0x20
};

class __declspec(dllimport) MfScData {
public:
    virtual ~MfScData();
protected:
    virtual MfScLinkData *CountCircLinks(MfScDataHandle *);
    virtual void ReleaseCircLinks(MfScLinkData *);
    virtual int GetThroughCount(MfScDataHandle *);
public:
    virtual void AddRef();
    virtual void Release();
    void *m_4;          // PORT: pointer-sized like the real member (MfLock m_lock)
};

class __declspec(dllimport) MfScClassInfo;

class __declspec(dllimport) MfScDataObject : public MfScData {
public:
    enum MfPropertyErrorCode { };
    MfScDataObject(unsigned int);
    virtual ~MfScDataObject();
    virtual MfPropertyErrorCode GetProperty(const MfString &, MfScDataHandle **);
    virtual MfPropertyErrorCode SetProperty(MfScDataHandle *, const MfString &, MfScDataHandle *, int);
    virtual MfString GetClassNameA();
    virtual void GetPropertyNames(MfStringList *, int);
    virtual void GetInstanceProperties(MfStringList *, int);
    virtual MfScPropTokenTable *GetPropTokenTable();
    virtual void FilloutPropertyNameList(MfStringList *);
    virtual MfScPropertyExtList *GetPropertyExtensions();
    static void AddClassInfo(MfScClassInfo *);
    MfString *m_className;  // +8
    void *m_c;              // PORT: a pointer like the real member (the instance properties)
    unsigned int m_10;      // PORT: the flags, after it
};

class __declspec(dllimport) MfScObject : public MfScDataObject {
public:
    MfScObject() : MfScDataObject(0x200) { }
protected:
    virtual MfScLinkData *CountCircLinks(MfScDataHandle *);
    virtual void ReleaseCircLinks(MfScLinkData *);
    virtual int GetThroughCount(MfScDataHandle *);
};

// class info (help/browser tree) classes
class __declspec(dllimport) MfSortedListItem {
public:
    virtual ~MfSortedListItem();
    virtual MfString GetKey() const = 0;
};

// PORT: mfutil's header-only interface (four slots; this view had one guessed slot)
class MfListItemInterface {
public:
    virtual void *Copy(void *item) { return 0; }
    virtual int Iterate(void *item) { return 1; }
    virtual void Destroy(void *item) {}
    virtual int Matches(void *item, void *key) { return 0; }
};

class __declspec(dllimport) MfScClassInfoText {
public:
    virtual ~MfScClassInfoText();
    MfString *m_text;
};

class __declspec(dllimport) MfScClassInfoItem : public MfSortedListItem {
public:
    virtual MfString GetKey() const;
    virtual _TREEITEM *DisplayInTree(HWND__ *, _TREEITEM *);    // PORT: PiccoloRT's slots (vtablecensus)
    virtual int GetTreeImage() = 0;
};

class MfScCiMethod;
class __declspec(dllimport) MfScMethodParameterInfo : public MfScClassInfoText {
public:
    MfScMethodParameterInfo(const MfString &, int);
    virtual ~MfScMethodParameterInfo();
    MfString m_8;       // PORT: the real member is an MfString (the name), not int
    int m_c;
};

class __declspec(dllimport) MfScCiMethod : public MfScClassInfoItem, public MfScClassInfoText {
public:
    MfScCiMethod(const MfString &);
    virtual ~MfScCiMethod();
    virtual _TREEITEM *DisplayInTree(HWND__ *, _TREEITEM *);
    virtual int GetTreeImage();
    MfScCiMethod &operator+(MfScMethodParameterInfo *);
    int m_c;
    MfString m_10;      // PORT: the real member is an MfString (the item name), not int
    void *m_14;         // PORT: a pointer like the real member (the parameter list)
};

class MfScClassInfoItem;
class __declspec(dllimport) MfScClassInfo : public MfSortedListItem, public MfListItemInterface, public MfScClassInfoText {
public:
    MfScClassInfo(const MfString &);
    virtual ~MfScClassInfo();
    virtual MfString GetKey() const;
    virtual int Matches(void *item, void *key);    // PORT: PiccoloRT's primary-vtable slot (vtablecensus)
    MfScClassInfo &operator+(MfScClassInfoItem *);
    MfString m_10;      // PORT: the real member is an MfString (the class name), not int
    void *m_14;         // PORT: a pointer like the real member (the item list)
};

// by-value constructor argument with an empty destructor (its out-of-line copy
// is at 0x100017c0, used by the EH unwind funclet of the constructor)
class MfPxtMathArg {
public:
    MfPxtMathArg() { }
    ~MfPxtMathArg() { }
};

// the plugin's script object, the global "Math"
class MfPxtMath : public MfScObject {
public:
    MfPxtMath(MfPxtMathArg);
    ~MfPxtMath();
    MfScDataHandle *Sinh(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *Cosh(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *Tanh(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *Hypot(MfScDataHandle *, int, MfScDataHandle **);
};

typedef int (*PscCompileFn)(const MfString &, const MfString &, MfString *);

MfString GetMathConstantsSource();
void CreateMathObject();
void DestroyMathObject();

#endif
