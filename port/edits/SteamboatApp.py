# SteamboatApp edits for the port (see port/rewrite.py, EDITS).
EDITS = {
    # e[verify(i)]: Visual C++ calls verify (which may reallocate e) before it loads e;
    # C++17 loads e first, so the store went to the old (on the first call null) array.
    'SteamboatApp/Vec.h': [
        ('    void setValueVerify(int i, const T &t) { e[verify(i)] = t; }',
         '    void setValueVerify(int i, const T &t) { int k = verify(i); e[k] = t; }', 1),
    ],
    # msvcirt's flush/setprecision/setw, statically linked into the DLL and written on
    # msvcirt's own ios/ostream layout (SteamboatApp/ios.h, ostream.h). The port's
    # iostreams are the standard ones, whose manipulators serve; the file compiles empty.
    'SteamboatApp/Globals.cpp': [
        ('#include "ios.h"\n', '#if 0 /* port: msvcirt internals, see port/edits */ \n', 1),
        ('    *(int *)((char *)&r + 0x30) = w;\n    return r;\n}\n',
         '    *(int *)((char *)&r + 0x30) = w;\n    return r;\n}\n#endif', 1),
    ],
    # One definition each: SimpleNode.cpp and ScriptHelpNode.cpp compile MBEngine's units
    # into the DLL with their own copies of DatabaseMgr::Database::AddDBRef and
    # ScriptManager::AddRef (VC6 linked the duplicates; the macOS linker refuses them). The
    # copies kept are those of the classes the engine units declare in full (Database's
    # MBComPtr is pointer-sized; DatabaseMgr.h's draft has an int there).
    'SteamboatApp/DatabaseMgr_Database.cpp': [
        ('unsigned long DatabaseMgr::Database::AddDBRef()\n{\n    return ++field_4;\n}\n',
         '#if 0 // port: defined by SimpleNode.cpp (MBEngine/SimpleNode.cpp)\n{\n    return ++field_4;\n#endif\n', 1),
    ],
    'SteamboatApp/ScriptManager.cpp': [
        ('unsigned long ScriptManager::AddRef()\n{\n    return ++_mRefCnt;\n}\n',
         '#if 0 // port: defined by ScriptHelpNode.cpp (MBEngine/ScriptHelpNode.cpp)\n{\n    return ++_mRefCnt;\n#endif\n', 1),
    ],
    # "SBSPbirdHigh" and a letter are 14 bytes with the terminator, in a 10-byte buffer (the
    # original overran its stack too; the macOS fortified sprintf aborts). The song key's
    # count is a size_t (8 bytes) printed with %d.
    'SteamboatApp/SBSongbirdNode.cpp': [
        ('        char name[10];\n        sprintf(name, "SBSPbirdHigh%c", \'C\');\n',
         '        char name[16];\n        sprintf(name, "SBSPbirdHigh%c", \'C\');\n', 1),
        ('sprintf(key, "song%d", mSongs.size() - 1);', 'sprintf(key, "song%d", (int)(mSongs.size() - 1));', 1),
    ],
    # 64-bit: string positions kept in unsigned int are compared with MBString::npos, which
    # is a 64-bit size_t here, so a failed find never matches: GetTokenList loops forever on
    # its last token, and a setting without '=' is cut at a wrapped position. The positions
    # become size_t.
    'SteamboatApp/CSBGridLockGameBoard.cpp': [
        ("unsigned int nPos = (*iSettings).find('=');", "size_t nPos = (*iSettings).find('=');", 1),
        ('unsigned int nStart = strValue.find_first_of("0123456789");', 'size_t nStart = strValue.find_first_of("0123456789");', 1),
        ('unsigned int nEnd = strValue.find_first_not_of("0123456789", nStart);', 'size_t nEnd = strValue.find_first_not_of("0123456789", nStart);', 1),
        ('unsigned int nPos = 0;', 'size_t nPos = 0;', 1),
        ('unsigned int nStart = 0;', 'size_t nStart = 0;', 1),
    ],
    'SteamboatApp/CSBLinkingLogsGameBoard.cpp': [
        ("unsigned int nPos = strToken.find('=');", "size_t nPos = strToken.find('=');", 1),
        ('unsigned int nStart = strType.find_first_of("0123456789");', 'size_t nStart = strType.find_first_of("0123456789");', 1),
        ('unsigned int nEnd = strType.find_first_not_of("0123456789", nStart);', 'size_t nEnd = strType.find_first_not_of("0123456789", nStart);', 1),
        ('unsigned int nPos = 0;', 'size_t nPos = 0;', 1),
        ('unsigned int nStart = 0;', 'size_t nStart = 0;', 1),
    ],
}

# SteamboatApp/Graph_NodeContext.h: VC6's std::vector<bool> is a plain vector of bools; libc++ packs it into bits and cannot hand out a bool &, so the values are kept in a struct.
EDITS['SteamboatApp/Graph_NodeContext.h'] = [
    ('class NodeContext {\n'
     'public:\n'
     '    V &GetValue(T *node) { return m_Values.at(node->GetTypeHandle()); }\n'
     'private:\n'
     '    std::vector<V> m_Values;\n'
     '};\n'
     '}\n',
     'class NodeContext {\n'
     'public:\n'
     '    V &GetValue(T *node) { return m_Values.at(node->GetTypeHandle()).value; }\n'
     'private:\n'
     '    struct Slot {\n'
     '        V value;\n'
     '    };\n'
     '    std::vector<Slot> m_Values;\n'
     '};\n'
     '}\n'
     '#line 14\n', 1),
]

# SteamboatApp/MBComPtr.h: adds operator< for maps keyed by MBComPtr (libc++ finds the T *, T & and bool conversions ambiguous); otherwise a copy of src/SteamboatApp/MBComPtr.h with port/rewrite.py's 32-bit long (int), keep in sync.
EDITS['SteamboatApp/MBComPtr.h'] = [
    ('    bool operator!=(MBComPtr<T> &p) { return mInterface != p.mInterface; }\n'
     '    bool operator!=(T *p) { return mInterface != p; }\n'
     '    // The do/while(0) block (a release macro in the original, it seems) compiles to the same code\n'
     "    // as a plain if, but its extra nodes change VC6's inlining budget: with it the callers inline\n",
     '    bool operator!=(MBComPtr<T> &p) { return mInterface != p.mInterface; }\n'
     '    bool operator!=(T *p) { return mInterface != p; }\n'
     '    // PORT: maps keyed by MBComPtr compare the interface pointers (VC6 took the T * conversion)\n'
     '    bool operator<(const MBComPtr<T> &p) const { return mInterface < p.mInterface; }\n'
     '    // The do/while(0) block (a release macro in the original, it seems) compiles to the same code\n'
     "    // as a plain if, but its extra nodes change VC6's inlining budget: with it the callers inline\n"
     '#line 76\n', 1),
    ('    // the instances copy it into a second register). A MBComPtr<T> built from a\n'
     '    // pointer to another MBComPtr comes here (SteamboatNode::ClearHotspots passes its iterators).\n'
     '    long QueryInterface(const MBGUID &iid, void **ppv) const\n'
     '    {\n'
     '        long hr;\n'
     '        if (mInterface)\n'
     '            hr = mInterface->QueryInterface(iid, ppv);\n',
     '    // the instances copy it into a second register). A MBComPtr<T> built from a\n'
     '    // pointer to another MBComPtr comes here (SteamboatNode::ClearHotspots passes its iterators).\n'
     '    int QueryInterface(const MBGUID &iid, void **ppv) const\n'
     '    {\n'
     '        int hr;\n'
     '        if (mInterface)\n'
     '            hr = mInterface->QueryInterface(iid, ppv);\n', 1),
    ('        return hr;\n'
     '    }\n'
     '    long CreateObject(const MBGUID &clsid)\n'
     '    {\n'
     '        IMBUnknown *unk;\n',
     '        return hr;\n'
     '    }\n'
     '    int CreateObject(const MBGUID &clsid)\n'
     '    {\n'
     '        IMBUnknown *unk;\n', 1),
    ('        MBComCreateInstance(0, clsid, &unk);\n'
     '        if (unk) {\n'
     '            long hr = _QueryInterface(unk);\n'
     '            unk->Release();\n'
     '            return hr;\n',
     '        MBComCreateInstance(0, clsid, &unk);\n'
     '        if (unk) {\n'
     '            int hr = _QueryInterface(unk);\n'
     '            unk->Release();\n'
     '            return hr;\n', 1),
    ('    }\n'
     'private:\n'
     '    template <class U> long _QueryInterface(U *const &p)\n'
     '    {\n'
     '        long hr;\n'
     '        if (p) {\n'
     '            T *pInterface;\n',
     '    }\n'
     'private:\n'
     '    template <class U> int _QueryInterface(U *const &p)\n'
     '    {\n'
     '        int hr;\n'
     '        if (p) {\n'
     '            T *pInterface;\n', 1),
    ('    }\n'
     '    // Same shape as the raw-pointer form (the out-of-line copy in CMBCursor.obj).\n'
     '    template <class U> long _QueryInterface(const MBComPtr<U> &p)\n'
     '    {\n'
     '        long hr;\n'
     '        if (p) {\n'
     '            T *pInterface;\n',
     '    }\n'
     '    // Same shape as the raw-pointer form (the out-of-line copy in CMBCursor.obj).\n'
     '    template <class U> int _QueryInterface(const MBComPtr<U> &p)\n'
     '    {\n'
     '        int hr;\n'
     '        if (p) {\n'
     '            T *pInterface;\n', 1),
]

# SteamboatApp/libpng/pngconf.h: TARGET_OS_MAC (set by the macOS SDK) means Classic Mac OS to libpng 1.0 (fp.h, no math.h); the port is a Unix build.
EDITS['SteamboatApp/libpng/pngconf.h'] = [
    ('#ifndef MACOS\n'
     '#if (defined(__MWERKS__) && defined(macintosh)) || defined(applec) || \\\n'
     '    defined(THINK_C) || defined(__SC__) || defined(TARGET_OS_MAC)\n'
     '#define MACOS\n'
     '#endif\n',
     '#ifndef MACOS\n'
     '#if (defined(__MWERKS__) && defined(macintosh)) || defined(applec) || \\\n'
     '    defined(THINK_C) || defined(__SC__)\n'
     '#define MACOS\n'
     '#endif\n', 1),
    (' */\n'
     '\n'
     'typedef unsigned long png_uint_32;\n'
     'typedef long png_int_32;\n'
     'typedef unsigned short png_uint_16;\n'
     'typedef short png_int_16;\n',
     ' */\n'
     '\n'
     'typedef unsigned int png_uint_32;\n'
     'typedef int png_int_32;\n'
     'typedef unsigned short png_uint_16;\n'
     'typedef short png_int_16;\n', 1),
]

# SteamboatApp/MBContainers.h: MBVector with pointer iterators (VC6 vector iterators are T *)
EDITS['SteamboatApp/MBContainers.h'] = [
    ('template <class T> class MBVector : public std::vector<T, MBAllocator<T> > {\n',
     "// The game walks MBVectors with plain pointers (VC6's vector iterator is T *): the port's\n"
     '// MBVector hands out pointers too and takes them back in insert and erase.\n'
     'template <class T> class MBVector : public std::vector<T, MBAllocator<T> > {\n'
     '    typedef std::vector<T, MBAllocator<T> > Base;\n'
     '    typename Base::iterator at(T *p) { return Base::begin() + (p - begin()); }\n'
     'public:\n'
     '    typedef T *iterator;\n'
     '    typedef const T *const_iterator;\n'
     '    typedef typename Base::size_type size_type;\n'
     '    T *begin() { return Base::empty() ? 0 : Base::data(); }\n'
     '    T *end() { return Base::empty() ? 0 : Base::data() + Base::size(); }\n'
     '    const T *begin() const { return Base::empty() ? 0 : Base::data(); }\n'
     '    const T *end() const { return Base::empty() ? 0 : Base::data() + Base::size(); }\n'
     '    T *erase(T *p) { size_type i = p - begin(); Base::erase(at(p)); return begin() + i; }\n'
     '    T *erase(T *first, T *last)\n'
     '    {\n'
     '        size_type i = first - begin();\n'
     '        Base::erase(at(first), at(last));\n'
     '        return begin() + i;\n'
     '    }\n'
     '    T *insert(T *p, const T &v) { size_type i = p - begin(); Base::insert(at(p), v); return begin() + i; }\n'
     '    void insert(T *p, size_type n, const T &v) { Base::insert(at(p), n, v); }\n'
     '    template <class I> void insert(T *p, I first, I last) { Base::insert(at(p), first, last); }\n'
     '#line 27\n', 1),
]
