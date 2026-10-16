// PiccoloRT.dll (vc5, no PDB): declarations for the MfScConsoleManager unit
// (0x100033a0-0x100044ff). The range holds the tail of the MfScClassInfo
// object (0x100033a0-0x10003c6f) and the start of the MfScConsoleManager
// object (0x10003c70-). Hierarchy from the RTTI class hierarchy descriptors,
// vtable layouts from the vtables in .rdata.
#ifndef PICCOLORT_MFSCCONSOLEMANAGER_H
#define PICCOLORT_MFSCCONSOLEMANAGER_H

#define STRICT // HWND is a struct pointer in the decorated names (PAUHWND__@@)
#include <windows.h>
#include <commctrl.h> // HTREEITEM, TVINSERTSTRUCT

#define MFIMPORT __declspec(dllimport)
#define PICAPI __declspec(dllexport)

// ---- mfutil.dll ----------------------------------------------------------

class MfStringList;
class MfNullString;

class MFIMPORT MfStringRef {
public:
    virtual ~MfStringRef();
    char *m_buffer;   // +4
    int m_length;     // +8
};

class MFIMPORT MfString {
public:
    MfString();
    MfString(const char *s);
    MfString(const MfString &s);
    ~MfString();
    MfString &operator=(const MfString &s);
    MfString &operator=(const char *s);
    MfString &operator<<(const char *s);
    MfString &operator<<(const MfString &s);
    int operator==(const MfString &s) const;
    int operator==(const MfNullString &s) const;
    int operator!=(const MfNullString &s) const;
    operator char *() const;
    int Length() const { return m_rep->m_length; }
    void ConvertToLines(MfStringList *lines) const;
private:
    MfStringRef *m_rep; // +0
};

class MFIMPORT MfNullString {
};
extern MFIMPORT MfNullString NULL_STRING;

class MFIMPORT MfLock {
public:
    ~MfLock();
    int Lock(long timeout = -1);
    void Unlock();
private:
    void *m_handle;
};

class MfCallBack;
class MfListItemInterface;

class MfDblListItem {
public:
    void *m_data;           // +0
    MfDblListItem *m_prev;  // +4
    MfDblListItem *m_next;  // +8
};

// MSVC places overloaded virtuals in reverse declaration order: the
// overload groups below are declared reversed to give the vtable order.
// Not dllimport at class level: 'new MfDblList' inlines the constructor and
// uses a vtable emitted locally (0x100206b4, RTTI .?AVMfDblList@@), whose
// slot 0 is a local ??_G calling the imported destructor.
class MfDblList {
public:
    enum EnumListPosition { kBefore, kAfter };
    MfDblList() : m_first(0), m_last(0), m_count(0) {}
    MFIMPORT virtual ~MfDblList();
    MFIMPORT virtual void Prepend(void *const item);
    MFIMPORT virtual void Append(void *const item);
    MFIMPORT virtual void Insert(void *item, EnumListPosition pos, MfDblListItem *where);
    MFIMPORT virtual void Merge(MfDblList &list, EnumListPosition pos, MfDblListItem *where);
    MFIMPORT virtual void Add(void *const item);
    MFIMPORT virtual void Copy(const MfDblList &list);
    MFIMPORT virtual void Copy(const MfDblList &list, MfCallBack &cb);
    MFIMPORT virtual void Copy(const MfDblList &list, MfListItemInterface &iface);
    MFIMPORT virtual void *GetFirst(MfDblListItem *&pos) const;
    MFIMPORT virtual void *GetNext(MfDblListItem *&pos) const;
    MFIMPORT virtual void *GetCurrent(MfDblListItem *&pos) const;
    MFIMPORT virtual void *GetPrev(MfDblListItem *&pos) const;
    MFIMPORT virtual void *GetLast(MfDblListItem *&pos) const;
    MFIMPORT virtual void Iterate(MfCallBack &cb) const;
    MFIMPORT virtual void Iterate(MfListItemInterface &iface) const;
    MFIMPORT virtual void *DeleteFirst();
    MFIMPORT virtual void *DeleteLast();
    MFIMPORT virtual void *Delete(MfDblListItem *&pos);
    MFIMPORT virtual void Clear();
    MFIMPORT virtual void Clear(MfCallBack &cb);
    MFIMPORT virtual void Clear(MfListItemInterface &iface);
    MFIMPORT virtual MfDblListItem *Find(void *item) const;
    MFIMPORT virtual MfDblListItem *Find(void *item, MfCallBack &cb) const;
    MFIMPORT virtual MfDblListItem *Find(void *item, MfListItemInterface &iface) const;
    MFIMPORT virtual MfDblListItem *Find(void *item, MfDblListItem *start, int dir) const;
    MFIMPORT virtual MfDblListItem *Find(void *item, MfDblListItem *start, int dir, MfCallBack &cb) const;
    MFIMPORT virtual MfDblListItem *Find(void *item, MfDblListItem *start, int dir, MfListItemInterface &iface) const;
protected:
    MfDblListItem *m_first; // +4
    MfDblListItem *m_last;  // +8
    int m_count;            // +0xc
};

class MfSortedListItem;

// Same as MfDblList: local vtable 0x1002063c (.?AVMfSortedList@@).
class MfSortedList : public MfDblList {
public:
    MfSortedList() {}
    MFIMPORT virtual ~MfSortedList();
    MFIMPORT virtual void Insert(void *item, EnumListPosition pos, MfDblListItem *where);
    void Add(MfSortedListItem *item) { Insert(item, kAfter, m_last); }
};

class MFIMPORT MfStringTokenizerCallBack {
public:
    MfStringTokenizerCallBack() {}
    virtual ~MfStringTokenizerCallBack() {}
    virtual int CallBack(const MfString &token, MfString &rest);
};

class MFIMPORT MfStringList : public MfDblList, public MfStringTokenizerCallBack {
public:
    MfStringList() {}
    virtual ~MfStringList() { Clear(); }
    virtual void Clear();
    MfString *First(MfDblListItem *&pos) const {
        pos = m_first;
        return pos ? (MfString *)pos->m_data : 0;
    }
    MfString *Next(MfDblListItem *&pos) const {
        if (pos)
            pos = pos->m_next;
        return pos ? (MfString *)pos->m_data : 0;
    }
};

class MFIMPORT MfHashTable {
public:
    virtual ~MfHashTable();
private:
    void *m_table;      // +4
    int m_size;         // +8
    int m_count;        // +0xc
    void *m_unk10;      // +0x10
};

class MFIMPORT MfDictionary : public MfHashTable {
public:
    MfDictionary(int size);
    virtual ~MfDictionary();
};

// Header-only interfaces of the list classes (vtables emitted locally).
class MfListItemInterface {
public:
    virtual void *Copy(void *item) { return 0; }
    virtual int Iterate(void *item) { return 1; }
    virtual void Delete(void *item) {}
    virtual int Matches(void *item, void *key) { return 0; }
};

class MfSortedListItem {
public:
    virtual ~MfSortedListItem() {}
    virtual MfString GetKey() const = 0;
};

// Console output interface (RuntimeLib); only called through the vtable.
class MfAiConsoleOutput {
public:
    virtual ~MfAiConsoleOutput() {}
    virtual void AddRef() = 0;
    virtual void Release() = 0;
    virtual void Print(const MfString &s) = 0;
    virtual void PrintNl(const MfString &s) = 0;
    virtual void Clear() = 0;
};

// ---- PiccoloRT -----------------------------------------------------------

class MfScLinkData;
class MfScDataHandle;
class MfScClassInfo;

class PICAPI MfScData {
public:
    virtual ~MfScData();
    virtual MfScLinkData *CountCircLinks(MfScDataHandle *h);
    virtual void ReleaseCircLinks(MfScLinkData *l);
    virtual int GetThroughCount(MfScDataHandle *h);
    virtual void AddRef();
    virtual void Release();
    void SetName(const char *name) { *m_name = name; }
protected:
    MfLock m_lock;      // +4
    MfString *m_name;   // +8
};

class PICAPI MfScDataObject : public MfScData {
public:
    enum MfPropertyErrorCode { kPropOk };
    MfScDataObject(unsigned int flags = 0);
    virtual ~MfScDataObject();
    virtual MfPropertyErrorCode GetProperty(const MfString &name, MfScDataHandle **value);
    virtual MfPropertyErrorCode SetProperty(MfScDataHandle *self, const MfString &name,
                                            MfScDataHandle *value, int create);
    static void AddClassInfo(MfScClassInfo *ci);
protected:
    void *m_instanceProps;      // +0x0c
    unsigned int m_flags;       // +0x10
};

typedef MfScDataHandle *(*MfScFunction)(MfScDataHandle *self, int argc, MfScDataHandle **argv);

class PICAPI MfScDataHandle {
public:
    MfScData *GetData() const { return (m_flags & 0xfc0) ? m_data : 0; }
    void SetReadOnly() { m_readOnly = 1; }
    static MfScDataHandle *CreateData(MfScDataObject *obj);
    static MfScDataHandle *CreateData(MfScFunction fn, const MfString &name, int numArgs);
private:
    unsigned short m_refCount; // +0
    union {                    // +2
        unsigned short m_flags;
        struct {
            unsigned short m_bit0 : 1;
            unsigned short m_bit1 : 1;
            unsigned short m_visiting : 1;
            unsigned short m_readOnly : 1;
        };
    };
    union {                    // +4
        float m_float;
        long m_long;
        MfScData *m_data;
    };
};

class PICAPI MfScVariableStack {
public:
    enum MfScVariableErrorCode { kOk = 0 };
};

class PICAPI MfScVmEnv {
public:
    static MfScVariableStack::MfScVariableErrorCode SetGlobalVariable(
        const MfString &name, MfScDataHandle *value, int create, int readOnly);
};

class PICAPI MfScClassInfoText {
public:
    MfScClassInfoText() {}
    MfScClassInfoText(const MfString &text) : m_text(text) {}
    virtual ~MfScClassInfoText() {}
    void AppendText(const MfString &text) { m_text << " " << text; }
    HTREEITEM DisplayTextInfoInTree(HWND hwnd, HTREEITEM parent);
protected:
    MfString m_text; // +4
};

class MfScClassInfoItem;

class LiiExemplar {
};

class PICAPI MfScClassInfo : public MfSortedListItem, public MfListItemInterface,
                             public MfScClassInfoText {
public:
    MfScClassInfo(LiiExemplar);
    MfScClassInfo(const MfString &name);
    virtual ~MfScClassInfo();
    MfScClassInfo &operator+(MfScClassInfoItem *item);
    void AddItems(MfScClassInfo *other);
    void InsertIntoTree(HWND hwnd, HTREEITEM parent);
    virtual MfString GetKey() const;
    virtual int Matches(void *item, void *key);
private:
    MfString m_name;        // +0x10
    MfSortedList *m_items;  // +0x14
};

class PICAPI MfScClassInfoItem : public MfSortedListItem, public MfScClassInfoText {
public:
    MfScClassInfoItem(const MfString &name, int type) : m_type(type), m_name(name) {}
    virtual ~MfScClassInfoItem() {}
    virtual MfString GetKey() const;
    virtual HTREEITEM DisplayInTree(HWND hwnd, HTREEITEM parent);
    virtual int GetTreeImage() = 0;
protected:
    int m_type;         // +0xc
    MfString m_name;    // +0x10
};

class PICAPI MfScMethodParameterInfo : public MfScClassInfoText {
public:
    MfScMethodParameterInfo(const MfString &name, int optional);
    virtual ~MfScMethodParameterInfo();
    HTREEITEM DisplayInTree(HWND hwnd, HTREEITEM parent, int index);
private:
    MfString m_name;    // +8
    int m_optional;     // +0xc
};

class PICAPI MfScCiMethod : public MfScClassInfoItem {
public:
    MfScCiMethod(const MfString &name);
    virtual ~MfScCiMethod();
    MfScCiMethod &operator+(MfScMethodParameterInfo *param);
    virtual HTREEITEM DisplayInTree(HWND hwnd, HTREEITEM parent);
    virtual int GetTreeImage();
private:
    MfDblList *m_params; // +0x14
};

class PICAPI MfScCiProperty : public MfScClassInfoItem {
public:
    MfScCiProperty(const MfString &name) : MfScClassInfoItem(name, 0) {}
    virtual ~MfScCiProperty() {}
    virtual HTREEITEM DisplayInTree(HWND hwnd, HTREEITEM parent);
    virtual int GetTreeImage();
};

class PICAPI MfScHandleDict : public MfDictionary {
public:
    MfScHandleDict() : MfDictionary(0x35) {}
    virtual ~MfScHandleDict() { Clear(); }
    void Clear();
};

class PICAPI MfScConsoleManager {
public:
    class Exemplar {
    public:
        ~Exemplar() {}  // by-value parameter: EH state 0 from the start of the ctor
    };
    MfScConsoleManager() {}
    MfScConsoleManager(Exemplar);
    virtual ~MfScConsoleManager();
    static MfScConsoleManager *Manager() { return pcmManager; }
    void AddOutput(MfAiConsoleOutput *output);
    void RemoveOutput(MfAiConsoleOutput *output);
    void RemoveAllOutputs();
    void PrintString(const MfString &s);
    void PrintStringNl(const MfString &s);
    void ClearConsole();
private:
    static MfScDataHandle *Console_Print(MfScDataHandle *self, int argc, MfScDataHandle **argv);
    static MfScDataHandle *Console_PrintNl(MfScDataHandle *self, int argc, MfScDataHandle **argv);
    static MfScDataHandle *Console_Clear(MfScDataHandle *self, int argc, MfScDataHandle **argv);

    MfDblList *m_outputs;   // +4
    static MfScConsoleManager *pcmManager;
};

#endif
