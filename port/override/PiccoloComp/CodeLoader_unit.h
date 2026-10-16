// PORT: MfRegularExpression, MfDirectory and MfScPiccoloObjectFile are declared with the member types of the real classes (pointers and MfStrings where this view had int or char filler), so the stack objects here are as big as the real ones on 64-bit.
// Declarations for the PiccoloComp.dll unit CodeLoader (0x10005af0-0x10006af2):
// MfPcCodeLoader directory scanning and the per-file compile (part of MfPcCodeLoader.cpp).
#ifndef PICCOLOCOMP_CODELOADER_H
#define PICCOLOCOMP_CODELOADER_H
#include <windows.h>
#include "MfAiCompiler.h"
#include "MfPcStringListErrorCB.h"

// ---------------------------------------------------------------- imports (mfutil)
class __declspec(dllimport) MfNullString {
public:
    int m_dummy;
};
extern __declspec(dllimport) MfNullString NULL_STRING;

class MfStringTokenizerCallBack;
class MfString;

// The body of an MfString. In the mfutil header this unit was built with, the
// constructor and CopyOnWrite are imported, not inline (0x10005f60).
class __declspec(dllimport) MfStringRef {
public:
    MfStringRef(int n) : m_count(0) { m_maxLength = n + nNumPadChars; m_str = new char[m_maxLength + 1]; m_str[0] = 0; m_length = 0; }    // PORT: mfutil's inline
    virtual ~MfStringRef() { delete[] m_str; }    // PORT: mfutil's inline
    MfStringRef *CopyOnWrite() { return new MfStringRef(*this); }    // PORT: mfutil's inline
    int ReadFromFile(const MfString &); MfStringRef(const MfStringRef &o) : m_str(0), m_length(0), m_maxLength(0), m_count(0) { MakeCopy(o); } void MakeCopy(const MfStringRef &); static int nNumPadChars;    // PORT: mfutil's, for the inlines above
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

class MfDblListItem {
public:
    void *m_data;               // +0
    MfDblListItem *m_prev;      // +4
    MfDblListItem *m_next;      // +8
};

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
        void *p = i ? i->m_data : 0;
        return p;
    }
    void *Next(MfDblListItem *&i) const
    {
        if (i) i = i->m_next;
        void *p = i ? i->m_data : 0;
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
    // PORT: the members of the real class (src/mfutil/MfRegularExpression_unit.h, with
    // regexpr.h's re_pattern_buffer and re_registers): 168 bytes on 64-bit, char[0x80]
    // gave 136
    MfString m_pattern;
    MfString m_error;
    int m_match;
    int m_registerCount;
    int m_valid;
    struct {
        char *buffer;
        int allocated;
        int used;
        char *fastmap;
        char *translate;
        char flags[4];
    } m_buf;                    // re_pattern_buffer
    struct {
        int start[10];
        int end[10];
    } m_regs;                   // re_registers
    char *m_fastmap;
};

class __declspec(dllimport) MfDirectoryFileInfo {
public:
    MfString GetFileName() const { return MfString(m_fd.cFileName); }    // PORT: mfutil's inline
    MfString GetFilePath() const; WIN32_FIND_DATAA m_fd;    // PORT: mfutil's first member (+0), for GetFileName
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
    MfString m_10;              // PORT: the real m_path is an MfString (pointer-sized), not int
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
    // PORT: the members of the real constructor (PiccoloRT's MfScPiccoloObjectFile.h)
    MfScPiccoloObjectFile()
        : mOpen(0), mFile(0), mMapping(0), mBase(0), mFunctions(0), mModules(0), mDebugFile(0) {}
    virtual ~MfScPiccoloObjectFile();
    MfScPOFErrorCode Open(const MfString &name, int write, int create, unsigned int flags);
    MfScPOFErrorCode AddFunction(MfScDataHandle *function);
    MfScPOFErrorCode WriteClose();
    MfString ECToString();
private:
    // PORT: the members of the real class (src/PiccoloRT/MfScPiccoloObjectFile.h); this
    // view's invented list agreed with it on 32-bit only (96 bytes on 64-bit, not 88)
    int mError;                 // +0x04
    unsigned int mSystemError;  // +0x08
    int mWrite;                 // +0x0c
    int mType;                  // +0x10
    unsigned int mOptions;      // +0x14
    MfString mObjectFileName;   // +0x18 mName
    int mOpen;                  // +0x1c
    void *mFile;                // +0x20
    void *mMapping;             // +0x24
    char *mBase;                // +0x28
    void *mFunctions;           // +0x2c
    void *mModules;             // +0x30
    MfScPiccoloObjectFile *mDebugFile;  // +0x34
};

class __declspec(dllimport) MfScPiccolo {
    friend class MfPcCodeLoader;
private:
    static MfString fsDataPath;
};

// ";" (a const char * in .rdata, defined elsewhere)
extern const char *g_pcPathSeparator;

// ---------------------------------------------------------------- MfPcCompiler
// Only what this unit uses: the compiler through its interface (MfPcCompiler.h
// needs the full MfString.h).
class __declspec(dllexport) MfPcCompiler : public MfAiCompiler {
public:
    static MfPcCompiler *GetMainCompiler();
};

// ---------------------------------------------------------------- MfPcCodeLoader
class __declspec(dllexport) MfPcCodeLoader {    // no data members, all static
public:
    MfPcCodeLoader() {}
    static void UpdatePiccoloDataPathFiles(int);
    static void UpdatePiccoloLibFiles(int);
    static int UpdateSingleFile(const MfString &, int, MfScHandleList *);
private:
    static void UpdateFilesInDirectoryPath(const MfString &, int);
};

#endif
