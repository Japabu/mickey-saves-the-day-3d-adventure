// Declarations for the PiccoloComp.dll unit CodeLoader (0x10005af0-0x10006af2):
// MfPcCodeLoader directory scanning and the per-file compile (part of MfPcCodeLoader.cpp).
#ifndef PICCOLOCOMP_CODELOADER_H
#define PICCOLOCOMP_CODELOADER_H
#include <windows.h>
#include "../PiccoloRT/MfAiCompiler.h"
#include "MfPcStringListErrorCB.h"

// ---------------------------------------------------------------- imports (mfutil)   // own copy (all classes up to MfPcCompiler): the mfutil and PiccoloRT headers this unit was built with (imported MfStringRef constructor and CopyOnWrite, inline iteration helpers tuned to its frame)
#include "../mfutil/MfNullString.h"

class MfStringTokenizerCallBack;
class MfString;

// The body of an MfString. In the mfutil header this unit was built with, the
// constructor and CopyOnWrite are imported, not inline (0x10005f60).
class __declspec(dllimport) MfStringRef {
public:
    MfStringRef(int);
    virtual ~MfStringRef();
    MfStringRef *CopyOnWrite();
    int ReadFromFile(const MfString &);
    char *m_str;        // +4
    int m_length;       // +8
    int m_maxLength;    // +0xc
    int m_count;        // +0x10
};

class __declspec(dllimport) MfString {
public:
    MfString();
    MfString(const char *);
    ~MfString();
    MfString &operator=(const MfString &);
    MfString operator+(const MfString &) const;
    int operator!=(const MfNullString &) const;
    operator const char *() const;
    int GetVariable(const MfString &);
    void Tokenize(const MfString &, MfStringTokenizerCallBack &) const;
    void SplitPath(MfString *, MfString *, MfString *, MfString *) const;
    void MakePath(const MfString &, const MfString &, const MfString &, const MfString &);
    int ReadFromFile(const MfString &file)
    {
        MakeUnique();
        return mfsr->ReadFromFile(file);
    }
private:
    void MakeUnique()
    {
        if (mfsr == &mfsrNullInitializer) {
            mfsr = new MfStringRef(0);
            mfsr->m_count++;
        } else if (mfsr->m_count > 1) {
            mfsr->m_count--;
            mfsr = mfsr->CopyOnWrite();
            mfsr->m_count++;
        }
    }
    static MfStringRef mfsrNullInitializer;
    MfStringRef *mfsr;  // +0
};

#include "../mfutil/MfDblListItem.h"

class MfCallBack;
class MfListItemInterface;

// VC5 lays overloaded virtuals out in reverse order of declaration.
class __declspec(dllimport) MfDblList {   // size 0x10
public:
    enum EnumListPosition { LP_BEFORE = 0, LP_AFTER = 1 };

    MfDblList() : m_head(0), m_tail(0), m_count(0) {}
    virtual ~MfDblList();                                                   // +0x00
    virtual void Prepend(void *const);                                      // +0x04
    virtual void Append(void *const);                                       // +0x08
    virtual void Insert(void *, EnumListPosition, MfDblListItem *);         // +0x0c
    virtual void Merge(MfDblList &, EnumListPosition, MfDblListItem *);     // +0x10
    virtual void Add(void *const);                                          // +0x14
    virtual void Copy(const MfDblList &);                                   // +0x20
    virtual void Copy(const MfDblList &, MfCallBack &);                     // +0x1c
    virtual void Copy(const MfDblList &, MfListItemInterface &);            // +0x18
    virtual void *GetFirst(MfDblListItem *&) const;                         // +0x24
    virtual void *GetNext(MfDblListItem *&) const;                          // +0x28
    virtual void *GetCurrent(MfDblListItem *&) const;                       // +0x2c
    virtual void *GetPrev(MfDblListItem *&) const;                          // +0x30
    virtual void *GetLast(MfDblListItem *&) const;                          // +0x34
    virtual void Iterate(MfCallBack &) const;                               // +0x3c
    virtual void Iterate(MfListItemInterface &) const;                      // +0x38
    virtual void *DeleteFirst();                                            // +0x40
    virtual void *DeleteLast();                                             // +0x44
    virtual void *Delete(MfDblListItem *&);                                 // +0x48
    virtual void Clear();                                                   // +0x54
    virtual void Clear(MfCallBack &);                                       // +0x50
    virtual void Clear(MfListItemInterface &);                              // +0x4c
    virtual MfDblListItem *Find(void *) const;                              // +0x6c
    virtual MfDblListItem *Find(void *, MfCallBack &) const;                // +0x68
    virtual MfDblListItem *Find(void *, MfListItemInterface &) const;       // +0x64
    virtual MfDblListItem *Find(void *, MfDblListItem *, int) const;        // +0x60
    virtual MfDblListItem *Find(void *, MfDblListItem *, int, MfCallBack &) const;  // +0x5c
    virtual MfDblListItem *Find(void *, MfDblListItem *, int, MfListItemInterface &) const;  // +0x58
    int GetCount() const { return m_count; }
    void *First(MfDblListItem *&i) const
    {
        i = m_head;
        void *p = i ? i->pData : 0;
        return p;
    }
    void *Next(MfDblListItem *&i) const
    {
        if (i) i = i->pliNext;
        void *p = i ? i->pData : 0;
        return p;
    }
    MfDblListItem *m_head;      // +0x04
    MfDblListItem *m_tail;      // +0x08
    int m_count;                // +0x0c
};

class __declspec(dllimport) MfStringTokenizerCallBack {
public:
    virtual ~MfStringTokenizerCallBack() {}
};

// MfDblList at +0, MfStringTokenizerCallBack at +0x10; size 0x14.
class __declspec(dllimport) MfStringList : public MfDblList, public MfStringTokenizerCallBack {
public:
    MfStringList() {}
    ~MfStringList() { Clear(); }
    virtual void Add(const MfString &);         // +0x70
    // The local copies in these inline helpers leave no code, but each one takes a
    // temporary slot: together they put the EH funclets' cast temporaries at the
    // original frame offsets ([ebp-0x12c]/[ebp-0x130] in 0x10005bc0's funclets).
    void Append(const MfString &s) { const MfString &t = s; Add(t); }  // inline: virtual call through this
    MfString *FirstString(MfDblListItem *&i) const { MfString *s = (MfString *)First(i); return s; }
    MfString *NextString(MfDblListItem *&i) const { MfString *s = (MfString *)Next(i); return s; }
    virtual void Clear();
};

class __declspec(dllimport) MfRegularExpression {  // size 0x84
public:
    MfRegularExpression(const MfString &);
    virtual ~MfRegularExpression();
    void Optimize();
    int Matches(const MfString &);
    char m_body[0x80];
};

class __declspec(dllimport) MfDirectoryFileInfo {
public:
    MfString GetFileName() const;
    MfString GetFilePath() const;
};

// a list of MfDirectoryFileInfo *, with its own cursor (kept in memory at +0x18 in
// 0x10005bc0, so it is a member, not a local); size 0x1c
class __declspec(dllimport) MfDirectory : public MfDblList {
public:
    MfDirectory();
    virtual ~MfDirectory();
    int Open(const MfString &);
    void Close();
    MfDirectoryFileInfo *GetFirstFile() { MfDirectoryFileInfo *f = (MfDirectoryFileInfo *)First(m_cursor); return f; }
    MfDirectoryFileInfo *GetNextFile() { MfDirectoryFileInfo *f = (MfDirectoryFileInfo *)Next(m_cursor); return f; }
    int m_10;
    int m_14;
    MfDblListItem *m_cursor;    // +0x18
};

class __declspec(dllimport) MfLogger {
public:
    static void Log(int, const MfString &, const MfString &, const MfStringList &);
    static void Log(int, const MfString &, const MfString &, const MfString &);
};

// a FILETIME with comparison operators (8 bytes, no destructor)
class __declspec(dllimport) MfFileTime {
public:
    MfFileTime();
    MfFileTime(const _FILETIME &);
    MfFileTime &operator=(const _FILETIME &);
    operator const _FILETIME *() const { return &m_time; }
    _FILETIME m_time;
};

// ---------------------------------------------------------------- imports (PiccoloRT)
class MfScDataHandle;
class MfPcCodeLoader;

// a list of data handles with a cursor of its own (0x14 bytes)
class __declspec(dllimport) MfScHandleList : public MfDblList {
public:
    virtual ~MfScHandleList();
    MfScDataHandle *First() { MfScDataHandle *h = (MfScDataHandle *)MfDblList::First(mCurrentPos); return h; }
    MfScDataHandle *Next() { MfScDataHandle *h = (MfScDataHandle *)MfDblList::Next(mCurrentPos); return h; }
    MfDblListItem *mCurrentPos;     // +0x10
};

// a compiled module on disk (.pobj); the default constructor is inline (0x38 bytes)
class __declspec(dllimport) MfScPiccoloObjectFile {
public:
    enum MfScPOFErrorCode { kOk = 0 };
    MfScPiccoloObjectFile()
        : mFunctionTable(0), mModuleTable(0), mNumFunctions(0), mNumModules(0), mFlags(0), mVersion(0),
          mLastError(0) {}
    virtual ~MfScPiccoloObjectFile();
    MfScPOFErrorCode Open(const MfString &name, int write, int create, unsigned int flags);
    MfScPOFErrorCode AddFunction(MfScDataHandle *function);
    MfScPOFErrorCode WriteClose();
    MfString ECToString();
private:
    void *mFile;                // +0x04
    void *mMapping;             // +0x08
    void *mView;                // +0x0c
    unsigned int mSize;         // +0x10
    unsigned int mPosition;     // +0x14
    MfString mObjectFileName;   // +0x18
    void *mFunctionTable;       // +0x1c
    void *mModuleTable;         // +0x20
    int mNumFunctions;          // +0x24
    int mNumModules;            // +0x28
    unsigned int mFlags;        // +0x2c
    int mVersion;               // +0x30
    int mLastError;             // +0x34
};

class __declspec(dllimport) MfScPiccolo {
    friend class MfPcCodeLoader;
private:
    static MfString fsDataPath;
};

// ";" (a const char * in .rdata, defined elsewhere)
extern const char *g_pcPathSeparator;

// ---------------------------------------------------------------- MfPcCompiler
// own copy: only what this unit uses (the compiler through its interface).
// MfPcCompiler.h includes the full MfString.h, which conflicts with this unit's
// mfutil view above (MfString with an imported MfStringRef constructor and
// CopyOnWrite, as in the header this unit was built with).
class __declspec(dllexport) MfPcCompiler : public MfAiCompiler {
public:
    static MfPcCompiler *GetMainCompiler();
};

#include "MfPcCodeLoader.h"

#endif
