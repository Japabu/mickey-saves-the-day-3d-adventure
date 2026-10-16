// PscContentInfo.dll: the "ContentInfoI" interface (MfContent), the cache of
// content information files (*.cif) found under the MFCONTENT folders, and the
// script object "Content" (PcContent). Declarations of the Motion Factory
// runtime (mfutil.dll, PiccoloRT.dll) as far as this plugin uses them; names
// and signatures from the decorated import names, layouts from the code.
#ifndef PSCCONTENTINFO_H
#define PSCCONTENTINFO_H

#include <windows.h>
#include <sys/timeb.h>
#include <fstream.h>

// ---- mfutil.dll -------------------------------------------------------------

class MfNullString;
class MfString;
class MfTime;
class MfCallBack;
class MfListItemInterface;
class MfStringList;

class __declspec(dllimport) MfNullString {
};
extern __declspec(dllimport) MfNullString NULL_STRING;

class __declspec(dllimport) MfStringSize {
public:
    MfStringSize(int n) { m_size = n; }
    int m_size;
};

// Reference-counted string body: vptr, data, length (+8), allocated size,
// reference count. The plugin keeps a local copy of the vtable (??_S) because
// the destructor is inline.
class __declspec(dllimport) MfStringRef {
public:
    MfStringRef(int n) : nRefs(0)
    {
        nAlloc = n + nNumPadChars;
        pData = new char[nAlloc + 1];
        pData[0] = 0;
        nLength = 0;
    }
    MfStringRef(const MfStringRef &);
    virtual ~MfStringRef() { delete pData; }
    MfStringRef *CopyOnWrite() { return new MfStringRef(*this); }
    void NoLeadingWhiteSpace();
    void NoTrailingWhiteSpace();
    void Truncate(int);

    char *pData;
    int nLength;
    int nAlloc;
    int nRefs;
protected:
    static int nNumPadChars;
};

class __declspec(dllimport) MfString {
public:
    MfString();
    MfString(const MfString &);
    MfString(const MfNullString &);
    MfString(const MfStringSize &);
    MfString(const char *);
    MfString(int);
    ~MfString();
    MfString &operator=(const MfString &);
    MfString &operator=(const MfNullString &);
    MfString &operator=(const char *);
    MfString &operator<<(const MfString &);
    MfString &operator<<(const char *);
    MfString &operator<<(int);
    MfString operator+(const MfString &) const;
    int operator==(const MfNullString &) const;
    int operator!=(const MfNullString &) const;
    int operator!=(const char *) const;
    operator const char *() const;
    char GetCharacter(int) const;
    int CaseCompare(const MfString &) const;
    int HaveCharactersInSet(const MfString &, int) const;
    MfString LeftString(int) const;
    MfString RightString(int) const;
    void SplitPath(MfString *, MfString *, MfString *, MfString *) const;
    int GetVariable(const MfString &);
    int FormatTime(char *const, const MfTime &);
    int Length() const { return pRef->nLength; }
    void NoLeadingWhiteSpace()
    {
        MakeUnique();
        pRef->NoLeadingWhiteSpace();
    }
    void NoTrailingWhiteSpace()
    {
        MakeUnique();
        pRef->NoTrailingWhiteSpace();
    }
    void Truncate(int n)
    {
        MakeUnique();
        pRef->Truncate(n);
    }
private:
    void MakeUnique()
    {
        if (pRef == &mfsrNullInitializer) {
            pRef = new MfStringRef(0);
            pRef->nRefs++;
        } else if (pRef->nRefs > 1) {
            pRef->nRefs--;
            pRef = pRef->CopyOnWrite();
            pRef->nRefs++;
        }
    }
    MfStringRef *pRef;
    static MfStringRef mfsrNullInitializer;
};

__declspec(dllimport) istream &operator>>(istream &is, MfString &s);

class MfDblListItem {
public:
    void *GetData() const { return pData; }
    MfDblListItem *GetNext() const { return pliNext; }
    void *pData;
    MfDblListItem *pliPrev;
    MfDblListItem *pliNext;
};

// VC5 lays overloaded virtuals out in reverse order of declaration.
class __declspec(dllimport) MfDblList {
public:
    enum EnumListPosition { LP_BEFORE = 0, LP_AFTER = 1 };

    MfDblList() : pliFirst(0), pliLast(0), nCount(0) {}
    virtual ~MfDblList();                                                    // +0x00
    virtual void Prepend(void *const pData);                                 // +0x04
    virtual void Append(void *const pData);                                  // +0x08
    virtual void Insert(void *pData, EnumListPosition pos, MfDblListItem *pli); // +0x0c
    virtual void Merge(MfDblList &list, EnumListPosition pos, MfDblListItem *pli); // +0x10
    virtual void Add(void *const pData);                                     // +0x14
    virtual void Copy(const MfDblList &list);                                // +0x20
    virtual void Copy(const MfDblList &list, MfCallBack &cb);                // +0x1c
    virtual void Copy(const MfDblList &list, MfListItemInterface &lii);      // +0x18
    virtual void *GetFirst(MfDblListItem *&pli) const                        // +0x24
    {
        pli = pliFirst;
        return pli ? pli->GetData() : 0;
    }
    virtual void *GetNext(MfDblListItem *&pli) const                         // +0x28
    {
        if (pli)
            pli = pli->GetNext();
        return pli ? pli->GetData() : 0;
    }
    virtual void *GetCurrent(MfDblListItem *&pli) const;                     // +0x2c
    virtual void *GetPrev(MfDblListItem *&pli) const;                        // +0x30
    virtual void *GetLast(MfDblListItem *&pli) const;                        // +0x34
    virtual void Iterate(MfCallBack &cb) const;                              // +0x3c
    virtual void Iterate(MfListItemInterface &lii) const;                    // +0x38
    virtual void *DeleteFirst();                                             // +0x40
    virtual void *DeleteLast();                                              // +0x44
    virtual void *Delete(MfDblListItem *&pli);                               // +0x48
    virtual void Clear();                                                    // +0x54
    virtual void Clear(MfCallBack &cb);                                      // +0x50
    virtual void Clear(MfListItemInterface &lii);                            // +0x4c
    virtual MfDblListItem *Find(void *pData) const;                          // +0x6c
    virtual MfDblListItem *Find(void *pData, MfCallBack &cb) const;          // +0x68
    virtual MfDblListItem *Find(void *pData, MfListItemInterface &lii) const; // +0x64
    virtual MfDblListItem *Find(void *pData, MfDblListItem *pli, int n) const; // +0x60
    virtual MfDblListItem *Find(void *pData, MfDblListItem *pli, int n, MfCallBack &cb) const; // +0x5c
    virtual MfDblListItem *Find(void *pData, MfDblListItem *pli, int n, MfListItemInterface &lii) const; // +0x58

protected:
    MfDblListItem *pliFirst; // +4
    MfDblListItem *pliLast;  // +8
    int nCount;              // +0xc
};

class __declspec(dllimport) MfStringTokenizerCallBack {
public:
    MfStringTokenizerCallBack() {}
    virtual ~MfStringTokenizerCallBack() {}
    virtual int CallBack(const MfString &token, MfString &rest);
};

// List of MfString (0x14 bytes); constructor and destructor are inline.
class __declspec(dllimport) MfStringList : public MfDblList, public MfStringTokenizerCallBack {
public:
    MfStringList() {}
    virtual ~MfStringList() { Clear(); }
    virtual MfString *Add(const MfString &s);                     // +0x70
    virtual void Delete(const MfString &s);                       // +0x74
    virtual void *DeleteFirst();                                  // +0x40
    virtual void *DeleteLast();                                   // +0x44
    virtual void *Delete(MfDblListItem *&pli);                    // +0x48
    virtual void Clear();                                         // +0x54
    virtual MfDblListItem *Search(const MfString &s) const;       // +0x78
    void Sort(int bCaseSensitive);
    MfString *First(MfDblListItem *&pos) const
    {
        pos = pliFirst;
        return pos ? (MfString *)pos->pData : 0;
    }
    MfString *Next(MfDblListItem *&pos) const
    {
        if (pos) {
            pos = pos->pliNext;
            return pos ? (MfString *)pos->pData : 0;
        }
        return 0;
    }
private:
    virtual int CallBack(const MfString &token, MfString &rest);
};

// The constructor and destructor are inline. VC5 lays overloaded virtuals out
// in reverse order: the vtable is ??_E, CallBack(3), CallBack(2), CallBack(1).
class __declspec(dllimport) MfCallBack {
public:
    MfCallBack() {}
    virtual ~MfCallBack() {}
    virtual int CallBack(void *);
    virtual int CallBack(void *, void *);
    virtual int CallBack(void *, void *, void *);
};

class MfListItemInterface {
public:
    virtual void *Copy(void *item) { return 0; }
    virtual int Iterate(void *item) { return 1; }
    virtual void Delete(void *item) {}
    virtual int Matches(void *item, void *key) { return 0; }
};

enum MfHTErrorCode { };

class __declspec(dllimport) MfHashTable {
public:
    class MfChainNode;
    virtual ~MfHashTable();
protected:
    virtual MfHTErrorCode Search(void *, void **, int *, int *, MfChainNode **) = 0;
    virtual void DeleteDataItem(void *, MfCallBack *) = 0;
    virtual void *GetKey(void *) = 0;
    virtual void *GetData(void *) = 0;
    void ClearItems(MfCallBack *);
private:
    void **apBuckets;
    int nNumberOfBuckets;
    int nBucketSize;
    MfChainNode **apmfcnChain;
};

// Hash table keyed by MfString.
class __declspec(dllimport) MfDictionary : public MfHashTable {
public:
    MfDictionary(int nBuckets);
    virtual ~MfDictionary();
    void *Add(const MfString &, void *);
    void *Find(const MfString &);
protected:
    virtual MfHTErrorCode Search(void *, void **, int *, int *, MfHashTable::MfChainNode **);
    virtual void DeleteDataItem(void *, MfCallBack *);
    virtual void *GetKey(void *);
    virtual void *GetData(void *);
};

// Seconds and milliseconds; inline members, the destructor is called through
// the import table only by the unwind funclets.
class __declspec(dllimport) MfTime {
public:
    MfTime(const struct timeb &tb) { m_seconds = tb.time; m_milli = tb.millitm; }
    ~MfTime() {}
    long m_seconds;
    long m_milli;
};

__declspec(dllimport) ostream &operator<<(ostream &os, const MfString &s);

// Inline wrapper of a Win32 critical section.
class __declspec(dllimport) MfCriticalSection {
public:
    MfCriticalSection() { InitializeCriticalSection(&m_cs); }
    ~MfCriticalSection() { DeleteCriticalSection(&m_cs); }
private:
    CRITICAL_SECTION m_cs;
};

class MfDirectory;

// One entry of an MfDirectory (size 0x144).
class __declspec(dllimport) MfDirectoryFileInfo {
public:
    unsigned long QueryAttributes() const { return m_fd.dwFileAttributes; }
    MfString GetFileName() const;
    MfString GetFilePath() const;
private:
    WIN32_FIND_DATAA m_fd;
    MfDirectory *m_dir;
};

// The files of a folder (size 0x1c).
class __declspec(dllimport) MfDirectory : public MfDblList {
public:
    MfDirectory();
    virtual ~MfDirectory();
    int Open(const MfString &path);
    MfDirectoryFileInfo *First()
    {
        m_pliCur = pliFirst;
        MfDirectoryFileInfo *info = m_pliCur ? (MfDirectoryFileInfo *)m_pliCur->pData : 0;
        return info;
    }
    MfDirectoryFileInfo *Next()
    {
        if (m_pliCur)
            m_pliCur = m_pliCur->pliNext;
        return m_pliCur ? (MfDirectoryFileInfo *)m_pliCur->pData : 0;
    }
private:
    MfString m_path;
    int m_unk14;
    MfDblListItem *m_pliCur;
};

// size 0x84
class __declspec(dllimport) MfRegularExpression {
public:
    MfRegularExpression(const MfString &pattern);
    virtual ~MfRegularExpression();
    void Optimize();
    int Matches(const MfString &s);
private:
    char m_data[0x80];
};

class __declspec(dllimport) MfLogger {
public:
    static void __cdecl Log(int, const MfString &, const MfString &, const MfString &);
    static void __cdecl Log(int, const MfString &, const MfString &, int, ...);
};

class __declspec(dllimport) MfCopyFolder {
public:
    static int Exists(const MfString &);
    static int CreateFolder(const MfString &);
};

// Registry access. Constructor and destructor are empty inlines: the unwind
// funclets call the imported destructor, the normal path nothing.
class __declspec(dllimport) UtilityApp {
public:
    UtilityApp() {}
    ~UtilityApp() {}
    MfString GetGlobalString(const MfString &, const MfString &, const MfString &);
    int WriteProfileStringA(const MfString &, const MfString &, const MfString &);
};

// ---- PiccoloRT.dll ----------------------------------------------------------

class __declspec(dllimport) MfScPiccolo {
public:
    static void LoadPiccoloLibFunctions(int);
    static void LoadPiccoloDataPathFunctions(int);
    static void FlushPiccoloDataPathFunctions();
private:
    static MfString fsDataPath;
    friend class MfContent;
};

// ---- PiccoloRT.dll: the script runtime -------------------------------------------

class MfScLinkData;
class MfScPropTokenTable;
class MfScPropertyExtList;
struct _TREEITEM;
class MfScDataHandle;
class MfScData;
class MfScDataObject;
typedef MfScDataHandle *(MfScData::*MfScMethod)(MfScDataHandle *, int, MfScDataHandle **);

// +0 reference count, +2 type flags, +4 value
class __declspec(dllimport) MfScDataHandle {
public:
    static MfScDataHandle *CreateData(int);
    static MfScDataHandle *CreateData(const MfString &, int);
    static MfScDataHandle *CreateData(MfScDataObject *);
    static MfScDataHandle *CreateData(MfScMethod, const MfString &, int);
    int IsType(unsigned long type) const { return m_flags & type; }
    MfScData *GetData() const { return (m_flags & 0xfc0) ? m_data : 0; }
    void SetInt(int i) { m_long = i; }
    unsigned short Release() { return m_refCount > 0 ? --m_refCount : m_refCount; }

    unsigned short m_refCount;
    unsigned short m_flags;
    union {
        MfScData *m_data;
        long m_long;
    };
};

class MfScVariableStack {
public:
    enum MfScVariableErrorCode { };
};

class __declspec(dllimport) MfScVmEnv {
public:
    static MfScVariableStack::MfScVariableErrorCode GetGlobalVariable(const MfString &, MfScDataHandle **);
    static MfScVariableStack::MfScVariableErrorCode SetGlobalVariable(const MfString &, MfScDataHandle *, int, int);
    void ThrowException(int, const MfString &);
};

class __declspec(dllimport) MfThread {
public:
    static MfThread *GetThread();
    char m_pad[0x20];
    MfScVmEnv *m_env;       // +0x20
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
    int m_4;
};

class MfScClassInfo;

class __declspec(dllimport) MfScDataObject : public MfScData {
public:
    enum MfPropertyErrorCode { kPropertyOk = 0 };
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
    int m_c;
    int m_10;
};

class __declspec(dllimport) MfScObject : public MfScDataObject {
public:
    MfScObject() : MfScDataObject(0x200) { }
protected:
    virtual MfScLinkData *CountCircLinks(MfScDataHandle *);
    virtual void ReleaseCircLinks(MfScLinkData *);
    virtual int GetThroughCount(MfScDataHandle *);
};

class __declspec(dllimport) MfScObjString : public MfScObject, public MfString {
private:
    int m_readOnly;
};

// The list interface the standard library plugin registers.
class __declspec(dllimport) MfScStdLibListInterface {
public:
    virtual int CreateList(MfDblList **list, MfScDataHandle **handle);
    virtual int GetList(MfDblList **list, MfScDataHandle *handle);
};

class __declspec(dllimport) MfScStdLib {
private:
    static MfScStdLibListInterface *pslliListInterface;
    friend class PcContent;
};

// class info (help/browser tree) classes
class __declspec(dllimport) MfSortedListItem {
public:
    virtual ~MfSortedListItem();
    virtual MfString GetKey() const = 0;
};

class __declspec(dllimport) MfScClassInfoText {
public:
    virtual ~MfScClassInfoText();
    MfString *m_text;
};

class __declspec(dllimport) MfScClassInfoItem : public MfSortedListItem {
public:
    virtual MfString GetKey() const;
};

class __declspec(dllimport) MfScMethodParameterInfo : public MfScClassInfoText {
public:
    MfScMethodParameterInfo(const MfString &, int);
    virtual ~MfScMethodParameterInfo();
    int m_8;
    int m_c;
};

class __declspec(dllimport) MfScCiMethod : public MfScClassInfoItem, public MfScClassInfoText {
public:
    MfScCiMethod(const MfString &);
    virtual ~MfScCiMethod();
    virtual _TREEITEM *DisplayInTree(HWND, _TREEITEM *);
    virtual int GetTreeImage();
    MfScCiMethod &operator+(MfScMethodParameterInfo *);
    int m_c;
    int m_10;
    int m_14;
};

class __declspec(dllimport) MfScClassInfo : public MfSortedListItem, public MfListItemInterface, public MfScClassInfoText {
public:
    MfScClassInfo(const MfString &);
    virtual ~MfScClassInfo();
    virtual MfString GetKey() const;
    MfScClassInfo &operator+(MfScClassInfoItem *);
    int m_10;
    int m_14;
};

class MfAi;

class __declspec(dllimport) MfInterfaceRegistry {
public:
    static int AddInterface(MfAi *);
    static int RemoveInterface(const MfString &);
};

// ---- interfaces ---------------------------------------------------------------

class MfAi {
public:
    virtual void GetInterfaceName(char *name, int size, int *major, int *minor) = 0;
    virtual int AddRef() = 0;
    virtual int Release() = 0;
};

// One content: what a *.cif file describes.
struct MfContentDescription {
    int FromObject(MfScDataHandle *h);
    static MfScDataHandle *ToObject(const MfContentDescription &desc);

    MfString name;          // +0x00
    MfString vendor;        // +0x04
    MfString description;   // +0x08
    MfString version;       // +0x0c
    MfString defaultSpace;  // +0x10
    MfString path;          // +0x14
};

typedef void (*MfContentNotify)(int stage);

// "ContentInfoI": the content (data folder) the application works with.
class MfAiContent : public MfAi {
public:
    virtual int SelectContent(const MfString &name) = 0;
    virtual int GetCurrentContent(MfContentDescription *desc) = 0;
    virtual int GetContent(const MfString &name, MfContentDescription *desc) = 0;
    virtual MfString GetDataPath() = 0;
    virtual int ChooseContent(HWND hParent, MfString *name) = 0;
    virtual int SetContentPath(const MfString &path) = 0;
    virtual int SaveContentInfoToPath(const MfString &path, const MfString &fileName, MfContentDescription *desc) = 0;
    virtual int CreateDataFolders() = 0;
    virtual MfContentNotify SetNotify(MfContentNotify notify) = 0;
    virtual int GetContentNames(MfStringList *names) = 0;
    virtual void Refresh() = 0;
};

// ---- this plugin ------------------------------------------------------------

// What the cache keeps per content: the description and the *.cif file.
struct MfContentInfo : public MfContentDescription {
    MfString file;          // +0x18

    MfContentInfo &operator=(const MfContentInfo &other);
    void Clear();
    void GetDescription(MfContentDescription &desc);
    MfString GetFolder();
};

// Reads a content information file: lines of "<directive> <value>", '#'
// starts a comment, a trailing '\\' continues the line.
class MfContentInfoFile {
public:
    MfContentInfoFile(const MfString &fileName);
    virtual ~MfContentInfoFile();
    int Read(MfContentInfo &info);

    enum Directive {
        kUnknown = 0, kEmpty = 1, kVendor = 2, kName = 3, kDescription = 4,
        kVersion = 5, kDefaultSpace = 6
    };

private:
    int NextLine();
    int ParseDirective(MfString &keyword);

    MfString m_fileName;        // +0x04
    int m_line;                 // +0x08
    MfString m_text;            // +0x0c
    ifstream m_in;              // +0x10
    int m_open;                 // +0x6c
};

// All content information files below the content folders, by content name.
class MfContentInfoCache : public MfDictionary {
public:
    MfContentInfoCache();
    virtual ~MfContentInfoCache();

    void AddPath(const MfString &path);
    int Add(const MfContentInfo &info);
    int Find(const MfString &name, MfContentInfo &info);
    void Refresh();
    void GetNames(MfStringList &names);

    class MfContentInfoCacheClearCB : public MfCallBack {
    public:
        virtual int CallBack(void *data);
    };
    class MfCopyNameListCB : public MfListItemInterface {
    public:
        virtual void *Copy(void *item);
    };

private:
    void ScanFolder(const MfString &folder);
    int IsStandardFolder(const MfString &name);

    MfStringList m_paths;               // +0x14
    MfStringList m_names;               // +0x28
    MfCriticalSection m_lock;           // +0x3c
};

class MfContent : public MfAiContent {
public:
    MfContent();

    virtual void GetInterfaceName(char *name, int size, int *major, int *minor);
    virtual int AddRef() { return ++m_nRefs; }
    virtual int Release()
    {
        int n = --m_nRefs;
        if (n == 0)
            delete this;
        return n;
    }
    virtual int SelectContent(const MfString &name);
    virtual int GetCurrentContent(MfContentDescription *desc);
    virtual int GetContent(const MfString &name, MfContentDescription *desc);
    virtual MfString GetDataPath();
    virtual int ChooseContent(HWND hParent, MfString *name);
    virtual int SetContentPath(const MfString &path);
    virtual int SaveContentInfoToPath(const MfString &path, const MfString &fileName, MfContentDescription *desc);
    virtual int CreateDataFolders();
    virtual MfContentNotify SetNotify(MfContentNotify notify);
    virtual int GetContentNames(MfStringList *names);
    virtual void Refresh();
    virtual ~MfContent();

    void SetDataPath(const MfString &path);
    MfString MakeContentPath(const MfString &name);
    int CreateDataFolder(const MfString &folder);
    static BOOL CALLBACK ChooseContentProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);

    int m_nRefs;                        // +0x04
    MfString m_dataPath;                // +0x08
    MfString m_contentName;             // +0x0c
    MfString m_chosen;                  // +0x10
    MfContentInfoCache *m_cache;        // +0x14
    MfContentNotify m_notify;           // +0x18
};

// by-value constructor argument with an empty destructor (its out-of-line copy
// is used by the EH unwind funclet of the constructor)
class PcContentArg {
public:
    PcContentArg() { }
    ~PcContentArg() { }
};

// The script object "Content".
class PcContent : public MfScObject {
public:
    PcContent(PcContentArg);
    virtual ~PcContent();
    virtual MfString GetClassNameA();

    MfScDataHandle *ChangeContentFolder(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *GetCurrentInfo(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *GetInfo(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *CurrentContentFolder(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PickContent(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *ChangeContentFolderToPath(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *SaveContentInfoToPath(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *CreateContentSubFolders(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *GetContentNames(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *Refresh(MfScDataHandle *, int, MfScDataHandle **);

    MfAiContent *m_content;     // +0x14
};

extern HINSTANCE g_hInstance;

#endif
