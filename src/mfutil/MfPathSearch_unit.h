// MfPathSearch (mfutil.dll 0x1000acf0-0x1000b100): looks for a file name in the
// directories of a ';'-separated path list. Reconstructed declarations (no PDB):
// layouts from the code, signatures from the export names.
#ifndef MFUTIL_MFPATHSEARCH_H
#define MFUTIL_MFPATHSEARCH_H

#define MFUTIL_API __declspec(dllexport)

class MfStringTokenizerCallBack;

// size 4 (pointer to the shared MfStringRef); only what this unit uses
class MFUTIL_API MfString {
public:
    MfString();
    MfString(const MfString &s);
    MfString(const char *s);
    ~MfString();
    MfString &operator=(const MfString &s);
    operator char *() const;
    MfString &operator<<(const MfString &s);
    MfString &operator<<(const char *s);
    void SplitPath(MfString *drive, MfString *dir, MfString *fname, MfString *ext) const;
    void Tokenize(const MfString &separators, MfStringTokenizerCallBack &cb) const;
private:
    void *mfsr;
};

// vtable 0x100172f4: ??_E, CallBack (inline members emitted by MfCopyFolder)
class MFUTIL_API MfStringTokenizerCallBack {
public:
    MfStringTokenizerCallBack() {}
    virtual ~MfStringTokenizerCallBack() {}
    virtual int CallBack(const MfString &token, MfString &rest) { return 1; }
};

// size 0xc, vtable 0x10017674 (only the destructor)
class MFUTIL_API MfPathSearch {
public:
    MfPathSearch(const MfString &file, const MfString &path);
    MfPathSearch(const MfPathSearch &o);
    virtual ~MfPathSearch();
    MfPathSearch &operator=(const MfPathSearch &o)
    {
        m_file = o.m_file;
        m_path = o.m_path;
        return *this;
    }
    static int TestAccess(const MfString &file);
    MfString Search();

    MfString m_file;    // +4
    MfString m_path;    // +8
};

#endif
