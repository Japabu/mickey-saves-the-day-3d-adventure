# RuntimeLib edits that make its copies of other modules' classes agree with the
# owner's vtable (see port/tools/vtablecensus.py).
EDITS = {
    # MfListItemInterface is header-only in the original, with default bodies
    # (PiccoloRT/MfListItemInterface.h): MfScClassInfo then declares only Matches, and
    # its primary vtable is ~, ~, GetKey, Matches as PiccoloRT emits it (RuntimeLib's
    # copy declared Copy/Iterate/Destroy too, which added three slots before Matches)
    'RuntimeLib/MfDblList.h': [
        ('    virtual void *Copy(void *pData) = 0;\n'
         '    virtual int Iterate(void *pData) = 0;\n'
         '    virtual void Destroy(void *pData) = 0;\n',
         '    virtual void *Copy(void *pData) { return 0; }\n'
         '    virtual int Iterate(void *pData) { return 1; }\n'
         '    virtual void Destroy(void *pData) {}\n', 1),
    ],
    'RuntimeLib/MfScClassInfo.h': [
        ('    // MfListItemInterface\'s inline defaults (MfDblList.h declares them pure)\n'
         '    virtual void *Copy(void *pData) { return 0; }\n'
         '    virtual int Iterate(void *pData) { return 1; }\n'
         '    virtual void Destroy(void *pData) {}\n',
         '    // (MfListItemInterface\'s Copy, Iterate and Destroy: its defaults, see\n'
         '    // MfDblList.h; declaring them here would add primary-vtable slots)\n\n\n', 1),
    ],
    # MfActorLib's MfProtoNode declares ReadFromStream, WriteToStream, StreamType in that
    # order (they get primary-vtable slots 4..6 in that order)
    'RuntimeLib/MfProtoNode.h': [
        ('    virtual long StreamType() const;\n'
         '    virtual MfStreamError ReadFromStream(MfStream &);\n'
         '    virtual MfStreamError WriteToStream(MfStream &) const;\n',
         '    virtual MfStreamError ReadFromStream(MfStream &);\n'
         '    virtual MfStreamError WriteToStream(MfStream &) const;\n'
         '    virtual long StreamType() const;\n', 1),
    ],
    # MfSkeletonNode_unit.h's view of MfStringList counts MSVC's slots: slot 0 (v00) is
    # the destructor, which takes two slots in the port's vtables
    'RuntimeLib/MfSkeletonNode_unit.h': [
        ('    virtual void v00(); virtual void v04(); virtual void v08(); virtual void v0c();\n',
         '    virtual ~MfStringList(); virtual void v04(); virtual void v08(); virtual void v0c();\n', 1),
    ],
    # mfutil's MfHashTable: +0x0c and +0x10 are the bucket and chain arrays (pointers);
    # with ints MfScPropTokenTable's MfCallBack base came out 8 bytes early and the
    # dictionaries RuntimeLib allocates were too small
    'RuntimeLib/MfHashTable.h': [
        ('    int field_c;\n    int field_10;\n', '    void **field_c;\n    void **field_10;\n', 1),
    ],
    'RuntimeLib/MfLocoPlanCache_unit.h': [
        ('class __declspec(dllimport) MfHashTable {\npublic:\n    virtual ~MfHashTable();\n    int m_04, m_08, m_0c, m_10;\n',
         'class __declspec(dllimport) MfHashTable {\npublic:\n    virtual ~MfHashTable();\n    int m_04, m_08; void *m_0c, *m_10;\n', 1),
        # MfRootObj's first member is its message table pointer (MfRootObj.h)
        ('    virtual ~MfRootObj();\n    int m_04, m_08, m_0c, m_10;\n',
         '    virtual ~MfRootObj();\n    void *m_04; int m_08, m_0c, m_10;\n', 1),
    ],
}
