# PscStdlib edits for the port (see port/rewrite.py, EDITS).
EDITS = {}

# plugins/PscStdlib/PscStdlib.h: MfThread, MfScMethodParameterInfo and MfScCiMethod are declared with the member types of the real classes (pointers and MfStrings where this view had padding or int), so sizes and offsets agree on 64-bit.
EDITS['plugins/PscStdlib/PscStdlib.h'] = [
    ('    virtual ~MfStringList() { Clear(); }\n'
     '    virtual MfString *Add(const MfString &s);                     // +0x70\n'
     '    virtual void Clear();                                         // +0x54\n'
     '};\n',
     '    virtual ~MfStringList() { Clear(); }\n'
     '    virtual MfString *Add(const MfString &s);                     // +0x70\n'
     "    virtual void Delete(const MfString &s);                       // PORT: mfutil's slots after Add\n"
     '    virtual MfDblListItem *Search(const MfString &s) const;\n'
     '    virtual int CallBack(const MfString &token, MfString &rest);\n'
     '    virtual void Clear();                                         // +0x54\n'
     '};\n'
     '#line 125\n', 1),
    ('    static MfThread *__cdecl GetThread();\n'
     'private:\n'
     '    char m_pad[0x20];\n'
     '};\n'
     '\n',
     '    static MfThread *__cdecl GetThread();\n'
     'private:\n'
     '    // PORT: the members of the real class (src/mfutil/MfThread.h) in place of\n'
     '    // char m_pad[0x20], so MfScThread::mVmEnv is at the real offset (+64 on 64-bit)\n'
     '    void *m_vtbl;\n'
     '    unsigned int m_threadId;\n'
     '    int m_flags;\n'
     '    MfThread *m_thread;\n'
     '    void *m_handle;\n'
     '    unsigned int m_state;\n'
     '    void *m_workEvent;\n'
     '    void *m_work;\n'
     '};\n'
     '\n'
     '#line 179\n', 1),
    ('    static MfScDataHandle *__cdecl CreateData(MfScDataObject *);\n'
     '    static MfScDataHandle *__cdecl CreateData(MfScMethod, const MfString &, int);\n'
     '    int IsType(unsigned long type) const { return m_flags & type; }\n'
     '    int IsVisiting() const { return m_flags & 4; }\n'
     '    void SetVisiting() { m_flags |= 4; }\n',
     '    static MfScDataHandle *__cdecl CreateData(MfScDataObject *);\n'
     '    static MfScDataHandle *__cdecl CreateData(MfScMethod, const MfString &, int);\n'
     '    int IsType(unsigned int type) const { return m_flags & type; }\n'
     '    int IsVisiting() const { return m_flags & 4; }\n'
     '    void SetVisiting() { m_flags |= 4; }\n', 1),
    ('    union {                     // +4\n'
     '        float m_float;\n'
     '        long m_long;\n'
     '        MfScData *m_data;\n'
     '    };\n',
     '    union {                     // +4\n'
     '        float m_float;\n'
     '        int m_long;\n'
     '        MfScData *m_data;\n'
     '    };\n', 1),
    ('    MfScLinkData() : mCount(0) {}\n'
     '    virtual ~MfScLinkData() {}\n'
     '    static void *__cdecl operator new(unsigned int);\n'
     '    static void __cdecl operator delete(void *);\n'
     '    int mCount;     // +4\n',
     '    MfScLinkData() : mCount(0) {}\n'
     '    virtual ~MfScLinkData() {}\n'
     '    static void *__cdecl operator new(size_t);\n'
     '    static void __cdecl operator delete(void *);\n'
     '    int mCount;     // +4\n', 1),
    ('    MfScClassInfoItem(const MfString &key) : m_c(0), m_key(key) {}\n'
     '    virtual MfString GetKey() const;\n'
     'protected:\n'
     '    int m_c;            // +0x0c\n',
     '    MfScClassInfoItem(const MfString &key) : m_c(0), m_key(key) {}\n'
     '    virtual MfString GetKey() const;\n'
     "    virtual _TREEITEM *DisplayInTree(HWND__ *, _TREEITEM *);    // PORT: PiccoloRT's slots (vtablecensus)\n"
     '    virtual int GetTreeImage() = 0;\n'
     'protected:\n'
     '    int m_c;            // +0x0c\n'
     '#line 491\n', 1),
    ('    virtual ~MfScMethodParameterInfo();\n'
     'private:\n'
     '    int m_8;\n'
     '    int m_c;\n'
     '};\n',
     '    virtual ~MfScMethodParameterInfo();\n'
     'private:\n'
     '    MfString m_8;       // PORT: the real member is an MfString (m_name), not int\n'
     '    int m_c;\n'
     '};\n', 1),
    ('    MfScCiMethod &operator+(MfScMethodParameterInfo *);\n'
     'private:\n'
     '    int m_14;\n'
     '};\n'
     '\n',
     '    MfScCiMethod &operator+(MfScMethodParameterInfo *);\n'
     'private:\n'
     '    void *m_14;         // PORT: the real member is a pointer (m_params)\n'
     '};\n'
     '\n', 1),
    ('    virtual ~MfScClassInfo();\n'
     '    virtual MfString GetKey() const;\n'
     '    MfScClassInfo &operator+(MfScClassInfoItem *);\n'
     'private:\n',
     '    virtual ~MfScClassInfo();\n'
     '    virtual MfString GetKey() const;\n'
     "    virtual int Matches(void *item, void *key);    // PORT: PiccoloRT's primary-vtable slot (vtablecensus)\n"
     '    MfScClassInfo &operator+(MfScClassInfoItem *);\n'
     'private:\n'
     '#line 529\n', 1),
]
