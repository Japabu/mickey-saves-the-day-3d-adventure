# PxtMath edits for the port (see port/rewrite.py, EDITS).
EDITS = {}

# plugins/PxtMath/PxtMath_10001000_unit.h: the runtime classes are declared with the member types of the real ones (pointers and MfStrings where this view had int or padding), so objects allocated here are as big as PiccoloRT's constructors expect on 64-bit and the VM env is read at the real offset.
EDITS['plugins/PxtMath/PxtMath_10001000_unit.h'] = [
    ('public:\n'
     '    static MfThread *GetThread();\n'
     '    char m_pad[0x20];\n'
     '    MfScVmEnv *m_env;     // +0x20\n'
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
     '    MfScVmEnv *m_env;     // +0x20\n'
     '};\n'
     '#line 71\n', 1),
    ('    virtual void AddRef();\n'
     '    virtual void Release();\n'
     '    int m_4;\n'
     '};\n'
     '\n',
     '    virtual void AddRef();\n'
     '    virtual void Release();\n'
     '    void *m_4;          // PORT: pointer-sized like the real member (MfLock m_lock)\n'
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
    ('};\n'
     '\n'
     'class __declspec(dllimport) MfListItemInterface {\n'
     'public:\n'
     '    virtual int Compare(MfListItemInterface *);\n'
     '};\n'
     '\n',
     '};\n'
     '\n'
     "// PORT: mfutil's header-only interface (four slots; this view had one guessed slot)\n"
     'class MfListItemInterface {\n'
     'public:\n'
     '    virtual void *Copy(void *item) { return 0; }\n'
     '    virtual int Iterate(void *item) { return 1; }\n'
     '    virtual void Destroy(void *item) {}\n'
     '    virtual int Matches(void *item, void *key) { return 0; }\n'
     '};\n'
     '\n'
     '#line 127\n', 1),
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
     '#line 138\n', 1),
    ('    MfScMethodParameterInfo(const MfString &, int);\n'
     '    virtual ~MfScMethodParameterInfo();\n'
     '    int m_8;\n'
     '    int m_c;\n'
     '};\n',
     '    MfScMethodParameterInfo(const MfString &, int);\n'
     '    virtual ~MfScMethodParameterInfo();\n'
     '    MfString m_8;       // PORT: the real member is an MfString (the name), not int\n'
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
     '    MfString m_10;      // PORT: the real member is an MfString (the item name), not int\n'
     '    void *m_14;         // PORT: a pointer like the real member (the parameter list)\n'
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
     '    MfString m_10;      // PORT: the real member is an MfString (the class name), not int\n'
     '    void *m_14;         // PORT: a pointer like the real member (the item list)\n'
     '};\n'
     '\n'
     '#line 170\n', 1),
]
