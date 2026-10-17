# PscContentInfo edits for the port (see port/rewrite.py, EDITS).
EDITS = {}

# plugins/PscContentInfo/MfContent.cpp: the dialog keeps its MfContent pointer with SetWindowLongPtr/GetWindowLongPtr (SetWindowLong's LONG is 32 bits and would truncate it).
EDITS['plugins/PscContentInfo/MfContent.cpp'] = [
    ('    if (msg == WM_INITDIALOG) {\n'
     '        MfContent *content = (MfContent *)lParam;\n'
     '        SetWindowLong(hDlg, GWL_USERDATA, lParam);\n'
     '        MfStringList names;\n'
     '        content->m_cache->GetNames(names);\n',
     '    if (msg == WM_INITDIALOG) {\n'
     '        MfContent *content = (MfContent *)lParam;\n'
     '        SetWindowLongPtr(hDlg, GWLP_USERDATA, lParam); // PORT: pointer-sized\n'
     '        MfStringList names;\n'
     '        content->m_cache->GetNames(names);\n', 1),
    ('        result = TRUE;\n'
     '    } else {\n'
     '        MfContent *content = (MfContent *)GetWindowLong(hDlg, GWL_USERDATA);\n'
     '        HWND hList = GetDlgItem(hDlg, 1000);\n'
     '        if (msg == WM_COMMAND) {\n',
     '        result = TRUE;\n'
     '    } else {\n'
     '        MfContent *content = (MfContent *)GetWindowLongPtr(hDlg, GWLP_USERDATA); // PORT: pointer-sized\n'
     '        HWND hList = GetDlgItem(hDlg, 1000);\n'
     '        if (msg == WM_COMMAND) {\n', 1),
]

# plugins/PscContentInfo/PscContentInfo.h: the runtime classes are declared with the member types of the real ones (pointers and MfStrings where this view had int or char filler), so objects allocated here are as big as the real constructors expect on 64-bit, PcContent::m_content no longer overlays MfScDataObject's flags, and the VM env is read at the real offset.
EDITS['plugins/PscContentInfo/PscContentInfo.h'] = [
    ('        nLength = 0;\n'
     '    }\n'
     '    MfStringRef(const MfStringRef &);\n'
     '    virtual ~MfStringRef() { delete pData; }\n'
     '    MfStringRef *CopyOnWrite() { return new MfStringRef(*this); }\n',
     '        nLength = 0;\n'
     '    }\n'
     "    MfStringRef(const MfStringRef &o) : pData(0), nLength(0), nAlloc(0), nRefs(0) { MakeCopy(o); } void MakeCopy(const MfStringRef &);    // PORT: mfutil's inline\n"
     '    virtual ~MfStringRef() { delete pData; }\n'
     '    MfStringRef *CopyOnWrite() { return new MfStringRef(*this); }\n', 1),
    ('    MfCallBack() {}\n'
     '    virtual ~MfCallBack() {}\n'
     '    virtual int CallBack(void *);\n'
     '    virtual int CallBack(void *, void *);\n'
     '    virtual int CallBack(void *, void *, void *);\n'
     '};\n'
     '\n',
     '    MfCallBack() {}\n'
     '    virtual ~MfCallBack() {}\n'
     "    virtual int CallBack(void *) { return 0; }    // PORT: mfutil's inline\n"
     "    virtual int CallBack(void *, void *) { return 0; }    // PORT: mfutil's inline\n"
     "    virtual int CallBack(void *, void *, void *) { return 0; }    // PORT: mfutil's inline\n"
     '};\n'
     '\n', 1),
    ('    virtual void *Copy(void *item) { return 0; }\n'
     '    virtual int Iterate(void *item) { return 1; }\n'
     '    virtual void Delete(void *item) {}\n'
     '    virtual int Matches(void *item, void *key) { return 0; }\n'
     '};\n',
     '    virtual void *Copy(void *item) { return 0; }\n'
     '    virtual int Iterate(void *item) { return 1; }\n'
     "    virtual void Destroy(void *item) {}    // PORT: mfutil's name for the slot\n"
     '    virtual int Matches(void *item, void *key) { return 0; }\n'
     '};\n', 1),
    ('    MfTime(const struct timeb &tb) { m_seconds = tb.time; m_milli = tb.millitm; }\n'
     '    ~MfTime() {}\n'
     '    long m_seconds;\n'
     '    long m_milli;\n'
     '};\n'
     '\n',
     '    MfTime(const struct timeb &tb) { m_seconds = tb.time; m_milli = tb.millitm; }\n'
     '    ~MfTime() {}\n'
     '    int m_seconds;\n'
     '    int m_milli;\n'
     '};\n'
     '\n', 1),
    ('class __declspec(dllimport) MfDirectoryFileInfo {\n'
     'public:\n'
     '    unsigned long QueryAttributes() const { return m_fd.dwFileAttributes; }\n'
     '    MfString GetFileName() const;\n'
     '    MfString GetFilePath() const;\n'
     'private:\n',
     'class __declspec(dllimport) MfDirectoryFileInfo {\n'
     'public:\n'
     '    unsigned int QueryAttributes() const { return m_fd.dwFileAttributes; }\n'
     "    MfString GetFileName() const { return MfString(m_fd.cFileName); }    // PORT: mfutil's inline\n"
     '    MfString GetFilePath() const;\n'
     'private:\n', 1),
    ('    int Matches(const MfString &s);\n'
     'private:\n'
     '    char m_data[0x80];\n'
     '};\n'
     '\n',
     '    int Matches(const MfString &s);\n'
     'private:\n'
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
     '#line 338\n', 1),
    ('    static MfScDataHandle *CreateData(MfScDataObject *);\n'
     '    static MfScDataHandle *CreateData(MfScMethod, const MfString &, int);\n'
     '    int IsType(unsigned long type) const { return m_flags & type; }\n'
     '    MfScData *GetData() const { return (m_flags & 0xfc0) ? m_data : 0; }\n'
     '    void SetInt(int i) { m_long = i; }\n',
     '    static MfScDataHandle *CreateData(MfScDataObject *);\n'
     '    static MfScDataHandle *CreateData(MfScMethod, const MfString &, int);\n'
     '    int IsType(unsigned int type) const { return m_flags & type; }\n'
     '    MfScData *GetData() const { return (m_flags & 0xfc0) ? m_data : 0; }\n'
     '    void SetInt(int i) { m_long = i; }\n', 1),
    ('    union {\n'
     '        MfScData *m_data;\n'
     '        long m_long;\n'
     '    };\n'
     '};\n',
     '    union {\n'
     '        MfScData *m_data;\n'
     '        int m_long;\n'
     '    };\n'
     '};\n', 1),
    ('public:\n'
     '    static MfThread *GetThread();\n'
     '    char m_pad[0x20];\n'
     '    MfScVmEnv *m_env;       // +0x20\n'
     '};\n',
     'public:\n'
     '    static MfThread *GetThread();\n'
     '    // PORT: the members of the real MfThread (src/mfutil/MfThread.h) in place of\n'
     '    // char m_pad[0x20], so m_env (MfScThread::mVmEnv) is at the real offset (+64 on 64-bit)\n'
     '    void *m_vtbl;\n'
     '    unsigned int m_threadId;\n'
     '    int m_flags;\n'
     '    MfThread *m_thread;\n'
     '    void *m_handle;\n'
     '    unsigned int m_state;\n'
     '    void *m_workEvent;\n'
     '    void *m_work;\n'
     '    MfScVmEnv *m_env;       // +0x20\n'
     '};\n'
     '#line 421\n', 1),
    ('    virtual void AddRef();\n'
     '    virtual void Release();\n'
     '    int m_4;\n'
     '};\n'
     '\n',
     '    virtual void AddRef();\n'
     '    virtual void Release();\n'
     '    void *m_4;              // PORT: pointer-sized like the real member (MfLock m_lock)\n'
     '};\n'
     '\n', 1),
    ('    static void AddClassInfo(MfScClassInfo *);\n'
     '    MfString *m_className;  // +8\n'
     '    int m_c;\n'
     '    int m_10;\n'
     '};\n'
     '\n',
     '    static void AddClassInfo(MfScClassInfo *);\n'
     '    MfString *m_className;  // +8\n'
     '    void *m_c;              // PORT: a pointer like the real member (the instance properties)\n'
     '    unsigned int m_10;      // PORT: the flags, after it\n'
     '};\n'
     '\n', 1),
    ('public:\n'
     '    virtual MfString GetKey() const;\n'
     '};\n'
     '\n',
     'public:\n'
     '    virtual MfString GetKey() const;\n'
     "    virtual _TREEITEM *DisplayInTree(HWND__ *, _TREEITEM *);    // PORT: PiccoloRT's slots (vtablecensus)\n"
     '    virtual int GetTreeImage() = 0;\n'
     '};\n'
     '\n'
     '#line 501\n', 1),
    ('    MfScMethodParameterInfo(const MfString &, int);\n'
     '    virtual ~MfScMethodParameterInfo();\n'
     '    int m_8;\n'
     '    int m_c;\n'
     '};\n',
     '    MfScMethodParameterInfo(const MfString &, int);\n'
     '    virtual ~MfScMethodParameterInfo();\n'
     '    MfString m_8;           // PORT: the real member is an MfString (the name), not int\n'
     '    int m_c;\n'
     '};\n', 1),
    ('    MfScCiMethod &operator+(MfScMethodParameterInfo *);\n'
     '    int m_c;\n'
     '    int m_10;\n'
     '    int m_14;\n'
     '};\n'
     '\n',
     '    MfScCiMethod &operator+(MfScMethodParameterInfo *);\n'
     '    int m_c;\n'
     '    MfString m_10;          // PORT: the real member is an MfString (the item name), not int\n'
     '    void *m_14;             // PORT: a pointer like the real member (the parameter list)\n'
     '};\n'
     '\n', 1),
    ('    virtual ~MfScClassInfo();\n'
     '    virtual MfString GetKey() const;\n'
     '    MfScClassInfo &operator+(MfScClassInfoItem *);\n'
     '    int m_10;\n'
     '    int m_14;\n'
     '};\n'
     '\n',
     '    virtual ~MfScClassInfo();\n'
     '    virtual MfString GetKey() const;\n'
     "    virtual int Matches(void *item, void *key);    // PORT: PiccoloRT's primary-vtable slot (vtablecensus)\n"
     '    MfScClassInfo &operator+(MfScClassInfoItem *);\n'
     '    MfString m_10;          // PORT: the real member is an MfString (the class name), not int\n'
     '    void *m_14;             // PORT: a pointer like the real member (the item list)\n'
     '};\n'
     '\n'
     '#line 531\n', 1),
]
