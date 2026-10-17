# PiccoloComp edits for the port (see port/rewrite.py, EDITS).
#
# Inline members of mfutil's classes: the original imports them from mfutil.dll,
# which emitted and exported every inline member of its dllexport classes. Here
# dllexport is a visibility attribute and unused inlines are not emitted (those that
# are come out hidden), so the views that declare them without a body get mfutil's
# body (src/mfutil/*.h).
MFUTIL = "    // PORT: mfutil's inline"

EDITS = {
    'PiccoloComp/MfCallBack.h': [
        ('    virtual int CallBack(void *);\n'
         '    virtual int CallBack(void *, void *);\n'
         '    virtual int CallBack(void *, void *, void *);\n',
         '    virtual int CallBack(void *) { return 0; }' + MFUTIL + '\n'
         '    virtual int CallBack(void *, void *) { return 0; }' + MFUTIL + '\n'
         '    virtual int CallBack(void *, void *, void *) { return 0; }' + MFUTIL + '\n', 1),
    ],
    'PiccoloComp/MfDblList.h': [
        ('    virtual void *GetFirst(MfDblListItem *&pli) const;  ',
         '    virtual void *GetFirst(MfDblListItem *&pli) const { pli = pliFirst; return pli ? pli->GetData() : 0; }' + MFUTIL + ' ', 1),
        ('    virtual void *GetNext(MfDblListItem *&pli) const;  ',
         '    virtual void *GetNext(MfDblListItem *&pli) const { if (pli) pli = pli->pliNext; return pli ? pli->pData : 0; }' + MFUTIL + ' ', 1),
        ('    virtual void *GetCurrent(MfDblListItem *&pli) const;  ',
         '    virtual void *GetCurrent(MfDblListItem *&pli) const { return pli ? pli->GetData() : 0; }' + MFUTIL + ' ', 1),
    ],
    'PiccoloComp/MfString.h': [
        ('    void CopyOnWrite(int);\n',
         '    void CopyOnWrite(int extra) { if (mfsr == &mfsrNullInitializer) { mfsr = new MfStringRef(extra); mfsr->m_count++; }'
         ' else if (mfsr->m_count > 1) { mfsr->m_count--; mfsr = mfsr->CopyOnWrite(); mfsr->m_count++; } }' + MFUTIL + '\n', 1),
    ],
    # mfutil defines the constructor out of line (src/mfutil/MfStack.cpp) and exports
    # it; the inline one here would need mfutil's vtable, which is not exported
    'PiccoloComp/MfStack.h': [
        ('    MfStack(int max = -1) : nMax(max) {}\n',
         '    MfStack(int max = -1);    // PORT: mfutil\'s out-of-line constructor (src/mfutil/MfStack.cpp)\n', 1),
    ],
}

# PiccoloComp/CodeLoader_unit.h: MfRegularExpression, MfDirectory and MfScPiccoloObjectFile are declared with the member types of the real classes (pointers and MfStrings where this view had int or char filler), so the stack objects here are as big as the real ones on 64-bit.
EDITS['PiccoloComp/CodeLoader_unit.h'] = [
    ('class __declspec(dllimport) MfStringRef {\n'
     'public:\n'
     '    MfStringRef(int);\n'
     '    virtual ~MfStringRef();\n'
     '    MfStringRef *CopyOnWrite();\n'
     '    int ReadFromFile(const MfString &);\n'
     '    char *m_str;        // +4\n'
     '    int m_length;       // +8\n',
     'class __declspec(dllimport) MfStringRef {\n'
     'public:\n'
     "    MfStringRef(int n) : m_count(0) { m_maxLength = n + nNumPadChars; m_str = new char[m_maxLength + 1]; m_str[0] = 0; m_length = 0; }    // PORT: mfutil's inline\n"
     "    virtual ~MfStringRef() { delete[] m_str; }    // PORT: mfutil's inline\n"
     "    MfStringRef *CopyOnWrite() { return new MfStringRef(*this); }    // PORT: mfutil's inline\n"
     "    int ReadFromFile(const MfString &); MfStringRef(const MfStringRef &o) : m_str(0), m_length(0), m_maxLength(0), m_count(0) { MakeCopy(o); } void MakeCopy(const MfStringRef &); static int nNumPadChars;    // PORT: mfutil's, for the inlines above\n"
     '    char *m_str;        // +4\n'
     '    int m_length;       // +8\n', 1),
    ('public:\n'
     '    virtual ~MfStringTokenizerCallBack() {}\n'
     '};\n'
     '\n',
     'public:\n'
     '    virtual ~MfStringTokenizerCallBack() {}\n'
     "    virtual int CallBack(const MfString &token, MfString &rest);    // PORT: mfutil's slot (this unit emits the vtable)\n"
     '};\n'
     '\n'
     '#line 134\n', 1),
    ('    ~MfStringList() { Clear(); }\n'
     '    virtual void Add(const MfString &);         // +0x70\n'
     '    // The local copies in these inline helpers leave no code, but each one takes a\n'
     "    // temporary slot: together they put the EH funclets' cast temporaries at the\n",
     '    ~MfStringList() { Clear(); }\n'
     '    virtual void Add(const MfString &);         // +0x70\n'
     "    virtual void Delete(const MfString &s);                    // PORT: mfutil's MfStringList slots\n"
     '    virtual MfDblListItem *Search(const MfString &s) const;\n'
     '    virtual int CallBack(const MfString &token, MfString &rest);\n'
     '    // The local copies in these inline helpers leave no code, but each one takes a\n'
     "    // temporary slot: together they put the EH funclets' cast temporaries at the\n"
     '#line 142\n', 1),
    ('    void Optimize();\n'
     '    int Matches(const MfString &);\n'
     '    char m_body[0x80];\n'
     '};\n'
     '\n'
     'class __declspec(dllimport) MfDirectoryFileInfo {\n'
     'public:\n'
     '    MfString GetFileName() const;\n'
     '    MfString GetFilePath() const;\n'
     '};\n'
     '\n',
     '    void Optimize();\n'
     '    int Matches(const MfString &);\n'
     '    // PORT: the members of the real class (src/mfutil/MfRegularExpression_unit.h, with\n'
     "    // regexpr.h's re_pattern_buffer and re_registers): 168 bytes on 64-bit, char[0x80]\n"
     '    // gave 136\n'
     '    MfString m_pattern;\n'
     '    MfString m_error;\n'
     '    int m_match;\n'
     '    int m_registerCount;\n'
     '    int m_valid;\n'
     '    struct {\n'
     '        char *buffer;\n'
     '        int allocated;\n'
     '        int used;\n'
     '        char *fastmap;\n'
     '        char *translate;\n'
     '        char flags[4];\n'
     '    } m_buf;                    // re_pattern_buffer\n'
     '    struct {\n'
     '        int start[10];\n'
     '        int end[10];\n'
     '    } m_regs;                   // re_registers\n'
     '    char *m_fastmap;\n'
     '};\n'
     '\n'
     'class __declspec(dllimport) MfDirectoryFileInfo {\n'
     'public:\n'
     "    MfString GetFileName() const { return MfString(m_fd.cFileName); }    // PORT: mfutil's inline\n"
     "    MfString GetFilePath() const; WIN32_FIND_DATAA m_fd;    // PORT: mfutil's first member (+0), for GetFileName\n"
     '};\n'
     '\n'
     '#line 164\n', 1),
    ('    MfDirectoryFileInfo *GetFirstFile() { MfDirectoryFileInfo *f = (MfDirectoryFileInfo *)First(m_cursor); return f; }\n'
     '    MfDirectoryFileInfo *GetNextFile() { MfDirectoryFileInfo *f = (MfDirectoryFileInfo *)Next(m_cursor); return f; }\n'
     '    int m_10;\n'
     '    int m_14;\n'
     '    MfDblListItem *m_cursor;    // +0x18\n',
     '    MfDirectoryFileInfo *GetFirstFile() { MfDirectoryFileInfo *f = (MfDirectoryFileInfo *)First(m_cursor); return f; }\n'
     '    MfDirectoryFileInfo *GetNextFile() { MfDirectoryFileInfo *f = (MfDirectoryFileInfo *)Next(m_cursor); return f; }\n'
     '    MfString m_10;              // PORT: the real m_path is an MfString (pointer-sized), not int\n'
     '    int m_14;\n'
     '    MfDblListItem *m_cursor;    // +0x18\n', 1),
    ('public:\n'
     '    enum MfScPOFErrorCode { kOk = 0 };\n'
     '    MfScPiccoloObjectFile()\n'
     '        : mFunctionTable(0), mModuleTable(0), mNumFunctions(0), mNumModules(0), mFlags(0), mVersion(0),\n'
     '          mLastError(0) {}\n'
     '    virtual ~MfScPiccoloObjectFile();\n'
     '    MfScPOFErrorCode Open(const MfString &name, int write, int create, unsigned int flags);\n',
     'public:\n'
     '    enum MfScPOFErrorCode { kOk = 0 };\n'
     "    // PORT: the members of the real constructor (PiccoloRT's MfScPiccoloObjectFile.h)\n"
     '    MfScPiccoloObjectFile()\n'
     '        : mOpen(0), mFile(0), mMapping(0), mBase(0), mFunctions(0), mModules(0), mDebugFile(0) {}\n'
     '    virtual ~MfScPiccoloObjectFile();\n'
     '    MfScPOFErrorCode Open(const MfString &name, int write, int create, unsigned int flags);\n', 1),
    ('    MfString ECToString();\n'
     'private:\n'
     '    void *mFile;                // +0x04\n'
     '    void *mMapping;             // +0x08\n'
     '    void *mView;                // +0x0c\n'
     '    unsigned int mSize;         // +0x10\n'
     '    unsigned int mPosition;     // +0x14\n'
     '    MfString mObjectFileName;   // +0x18\n'
     '    void *mFunctionTable;       // +0x1c\n'
     '    void *mModuleTable;         // +0x20\n'
     '    int mNumFunctions;          // +0x24\n'
     '    int mNumModules;            // +0x28\n'
     '    unsigned int mFlags;        // +0x2c\n'
     '    int mVersion;               // +0x30\n'
     '    int mLastError;             // +0x34\n'
     '};\n'
     '\n',
     '    MfString ECToString();\n'
     'private:\n'
     '    // PORT: the members of the real class (src/PiccoloRT/MfScPiccoloObjectFile.h); this\n'
     "    // view's invented list agreed with it on 32-bit only (96 bytes on 64-bit, not 88)\n"
     '    int mError;                 // +0x04\n'
     '    unsigned int mSystemError;  // +0x08\n'
     '    int mWrite;                 // +0x0c\n'
     '    int mType;                  // +0x10\n'
     '    unsigned int mOptions;      // +0x14\n'
     '    MfString mObjectFileName;   // +0x18 mName\n'
     '    int mOpen;                  // +0x1c\n'
     '    void *mFile;                // +0x20\n'
     '    void *mMapping;             // +0x24\n'
     '    char *mBase;                // +0x28\n'
     '    void *mFunctions;           // +0x2c\n'
     '    void *mModules;             // +0x30\n'
     '    MfScPiccoloObjectFile *mDebugFile;  // +0x34\n'
     '};\n'
     '\n'
     '#line 236\n', 1),
]

# PiccoloComp/MfHashTable.h: declared with the real member types of mfutil's MfHashTable (two of them are pointers), so the size agrees on 64-bit.
EDITS['PiccoloComp/MfHashTable.h'] = [
    ('    static int * aiPrimes;\n'
     'public:\n'
     '    int field_4;\n'
     '    int field_8;\n'
     '    int field_c;\n'
     '    int field_10;\n'
     '};\n',
     '    static int * aiPrimes;\n'
     'public:\n'
     "    // PORT: the member types of mfutil's MfHashTable (src/mfutil/MfHashTable_unit.h), so\n"
     "    // MfDictionary is as big as mfutil's constructor expects (32 bytes on 64-bit, not 24)\n"
     '    int field_4;                    // nNumberOfBuckets\n'
     '    int field_8;                    // nBucketSize\n'
     '    void **field_c;                 // apBuckets\n'
     '    MfChainNode **field_10;         // apmfcnChain\n'
     '};\n'
     '#line 37\n', 1),
]

# PiccoloComp/MfPcConstSetLexer.cpp: hex and octal literals saturate at 32 bits, as strtoul did on Win32 (the host strtoul returns 64 bits).
EDITS['PiccoloComp/MfPcConstSetLexer.cpp'] = [
    ('case 5:\n'
     'YY_USER_ACTION\n'
     '\tyylval.ival = strtoul(yytext, 0, 16);\n'
     '\treturn CSTOK_INTEGER;\n'
     '\tYY_BREAK\n'
     'case 6:\n'
     'YY_USER_ACTION\n'
     '\tyylval.ival = strtoul(yytext, 0, 8);\n'
     '\treturn CSTOK_INTEGER;\n'
     '\tYY_BREAK\n',
     'case 5:\n'
     'YY_USER_ACTION\n'
     '\t{ unsigned long long v = strtoul(yytext, 0, 16); yylval.ival = v > 0xffffffffULL ? -1 : (int)v; } // PORT: saturate at 32 bits\n'
     '\treturn CSTOK_INTEGER;\n'
     '\tYY_BREAK\n'
     'case 6:\n'
     'YY_USER_ACTION\n'
     '\t{ unsigned long long v = strtoul(yytext, 0, 8); yylval.ival = v > 0xffffffffULL ? -1 : (int)v; } // PORT: saturate at 32 bits\n'
     '\treturn CSTOK_INTEGER;\n'
     '\tYY_BREAK\n', 1),
]

# PiccoloComp/MfPcLexer.cpp: hex and octal literals saturate at 32 bits, as strtoul did on Win32 (the host strtoul returns 64 bits).
EDITS['PiccoloComp/MfPcLexer.cpp'] = [
    ('case 5:\n'
     'YY_USER_ACTION\n'
     '\tyylval.ival = strtoul(yytext, 0, 16);\n'
     '\treturn TOK_INTEGER;\n'
     '\tYY_BREAK\n'
     'case 6:\n'
     'YY_USER_ACTION\n'
     '\tyylval.ival = strtoul(yytext, 0, 8);\n'
     '\treturn TOK_INTEGER;\n'
     '\tYY_BREAK\n',
     'case 5:\n'
     'YY_USER_ACTION\n'
     '\t{ unsigned long long v = strtoul(yytext, 0, 16); yylval.ival = v > 0xffffffffULL ? -1 : (int)v; } // PORT: saturate at 32 bits\n'
     '\treturn TOK_INTEGER;\n'
     '\tYY_BREAK\n'
     'case 6:\n'
     'YY_USER_ACTION\n'
     '\t{ unsigned long long v = strtoul(yytext, 0, 8); yylval.ival = v > 0xffffffffULL ? -1 : (int)v; } // PORT: saturate at 32 bits\n'
     '\treturn TOK_INTEGER;\n'
     '\tYY_BREAK\n', 1),
]

# PiccoloComp/MfPcTokenInfo.h: mToken is pointer-sized so this class keeps the layout of YYSTYPE, which the parser and scanner alias with it.
EDITS['PiccoloComp/MfPcTokenInfo.h'] = [
    ('#pragma once\n'
     '\n'
     '// Source position of a token or of a parsed construct (0x14 bytes, no\n',
     '#pragma once\n'
     '#include <stdint.h> // PORT: intptr_t\n'
     '\n'
     '// Source position of a token or of a parsed construct (0x14 bytes, no\n'
     '#line 4\n', 1),
    ('    void Merge(MfPcTokenInfo *first, MfPcTokenInfo *last);\n'
     '\n'
     '    int mToken;                 // +0x00\n'
     '    int mLine;                  // +0x04\n'
     '    int mStartColumn;           // +0x08\n',
     '    void Merge(MfPcTokenInfo *first, MfPcTokenInfo *last);\n'
     '\n'
     '    // PORT: pointer-sized, as the value union of YYSTYPE (MfPcParser.h) it overlays: the\n'
     '    // parser reads token positions through (MfPcTokenInfo *)&yylval, so line, columns\n'
     '    // and text must sit where YYSTYPE has them (+8, +12, +16, +24 on 64-bit)\n'
     '    intptr_t mToken;            // +0x00\n'
     '    int mLine;                  // +0x04\n'
     '    int mStartColumn;           // +0x08\n'
     '#line 20\n', 1),
]

# PiccoloComp/MfScPiccoloObjectFile.h: PiccoloRT's MfScPiccoloObjectFile is declared with the real members (pointers where this view had ints), so stack objects are as big as the real class on 64-bit.
EDITS['PiccoloComp/MfScPiccoloObjectFile.h'] = [
    ('public:\n'
     '    enum MfScPOFErrorCode { kOk = 0 };\n'
     '    MfScPiccoloObjectFile()\n'
     '        : mFunctionTable(0), mModuleTable(0), mNumFunctions(0), mNumModules(0), mFlags(0), mVersion(0),\n'
     '          mLastError(0) {}\n'
     '    virtual ~MfScPiccoloObjectFile();\n'
     '    MfScPOFErrorCode Open(MfString const &name, int write, int create, unsigned int flags);\n',
     'public:\n'
     '    enum MfScPOFErrorCode { kOk = 0 };\n'
     "    // PORT: the members of the real constructor (PiccoloRT's MfScPiccoloObjectFile.h)\n"
     '    MfScPiccoloObjectFile()\n'
     '        : mOpen(0), mFile(0), mMapping(0), mBase(0), mFunctions(0), mModules(0), mDebugFile(0) {}\n'
     '    virtual ~MfScPiccoloObjectFile();\n'
     '    MfScPOFErrorCode Open(MfString const &name, int write, int create, unsigned int flags);\n', 1),
    ('    MfString const &ObjectFileName() const { return mObjectFileName; }\n'
     'private:\n'
     '    void *mFile;                // +0x04\n'
     '    void *mMapping;             // +0x08\n'
     '    void *mView;                // +0x0c\n'
     '    unsigned int mSize;         // +0x10\n'
     '    unsigned int mPosition;     // +0x14\n'
     '    MfString mObjectFileName;   // +0x18\n'
     '    void *mFunctionTable;       // +0x1c\n'
     '    void *mModuleTable;         // +0x20\n'
     '    int mNumFunctions;          // +0x24\n'
     '    int mNumModules;            // +0x28\n'
     '    unsigned int mFlags;        // +0x2c\n'
     '    int mVersion;               // +0x30\n'
     '    int mLastError;             // +0x34\n'
     '};\n',
     '    MfString const &ObjectFileName() const { return mObjectFileName; }\n'
     'private:\n'
     '    // PORT: the members of the real class (src/PiccoloRT/MfScPiccoloObjectFile.h); this\n'
     "    // view's invented list agreed with it on 32-bit only (96 bytes on 64-bit, not 88)\n"
     '    int mError;                 // +0x04\n'
     '    unsigned int mSystemError;  // +0x08\n'
     '    int mWrite;                 // +0x0c\n'
     '    int mType;                  // +0x10\n'
     '    unsigned int mOptions;      // +0x14\n'
     '    MfString mObjectFileName;   // +0x18 mName\n'
     '    int mOpen;                  // +0x1c\n'
     '    void *mFile;                // +0x20\n'
     '    void *mMapping;             // +0x24\n'
     '    char *mBase;                // +0x28\n'
     '    void *mFunctions;           // +0x2c\n'
     '    void *mModules;             // +0x30\n'
     '    MfScPiccoloObjectFile *mDebugFile;  // +0x34\n'
     '};\n'
     '#line 34\n', 1),
]
