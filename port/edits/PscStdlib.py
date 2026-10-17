# PscStdlib edits for the port (see port/rewrite.py, EDITS).
EDITS = {}

# plugins/PscStdlib/PscStdlib.h: the PiccoloRT classes this header still declares itself (until PiccoloRT's headers can be included next to mfutil's) get what the real classes have: 32-bit types for MfScDataHandle's flags and value, the virtual slots of MfScClassInfoItem and MfScClassInfo, and pointers and MfStrings where MfScMethodParameterInfo and MfScCiMethod had int, so sizes and offsets agree on 64-bit.
EDITS['plugins/PscStdlib/PscStdlib.h'] = [
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
     '#line 316\n', 1),
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
     '#line 354\n', 1),
]
