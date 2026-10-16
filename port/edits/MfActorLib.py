# MfActorLib edits for the 64-bit port (see port/rewrite.py, EDITS).
#
# Unit views: several units declare another unit's class again, with int or char[]
# filler where the real class (MfStream.h, MfFileStream.h, MfFile.h, MfTypedValue.h,
# MfPropertyList.h, mfutil's MfCopyFolder) has pointer members. On 32-bit the offsets
# agreed; here the fillers get the real member types so every view has the real 64-bit
# layout (objects allocated from a view, and inline members emitted from one, then
# match the real class). `long` is 32 bits (rewrite.windows_long), so int fillers for
# long members already agree.

# MfStream view fillers: int userFormat, long objectType, int objectSize, void *userData
STREAM_VIEW = ('    int m_10;\n    MfStreamError m_status;',
               '    void *m_10;     // PORT: MfStream::userData is a pointer\n    MfStreamError m_status;', 1)
# MfFileStream view fillers: char *buffer, unsigned long bufferFill, bufferPos, MfFileMode mode
FILESTREAM_VIEW = ('    int m_24;\n    int m_28;\n',
                   '    char *m_24;             // PORT: MfFileStream::buffer is a pointer\n    int m_28;\n', 1)
# MfTypedValue view: long lModCBackId, void (*pModCBack)(...)
TYPEDVALUE_VIEW = ('protected:\n    int m_04;\n    int m_08;\n};',
                   'protected:\n    int m_04;\n    void *m_08;     // PORT: MfTypedValue::pModCBack is a pointer\n};', 1)

# with long rewritten to int, MfStream's long overloads repeat the int ones
LONG_TWIN = '#if 0 /* PORT: the int overload serves (long is int) */\n{\n'

# mflinalg's inline members: the original imports them from mflinalg.dll, which
# emitted and exported every inline member of its dllexport classes. Here dllexport
# is a visibility attribute and unused inlines are not emitted, so the views that
# declare them without a body get mflinalg's body (src/mflinalg/*.h).
LINALG_INLINES = {
    'MfActorLib/MfFile_unit.h': [
        ('    MfVector3 &operator=(const MfVector3 &);\n',
         '    MfVector3 &operator=(const MfVector3 &v) { x = v.x; y = v.y; z = v.z; return *this; }    // PORT: mflinalg\'s inline\n', 1),
        ('    MfRotation3();\n'
         '    ~MfRotation3() {}\n'
         '    MfRotation3 &operator=(const MfRotation3 &);\n',
         '    MfRotation3() { q[0] = 1.0f; q[1] = 0; q[2] = 0; q[3] = 0; }    // PORT: mflinalg\'s inline\n'
         '    ~MfRotation3() {}\n'
         '    MfRotation3 &operator=(const MfRotation3 &r) { q[0] = r.q[0]; q[1] = r.q[1]; q[2] = r.q[2]; q[3] = r.q[3]; return *this; }    // PORT: mflinalg\'s inline\n', 1),
    ],
    'MfActorLib/MfVector3.h': [
        ('    MfVector3 square();\n',
         '    MfVector3 square() { return MfVector3(_x * _x, _y * _y, _z * _z); }    // PORT: mflinalg\'s inline\n', 1),
    ],
    'MfActorLib/MfTransform3.h': [
        ('    static MfTransform3 identity();\n',
         '    static MfTransform3 identity() { return MfTransform3(MfRotation3::identity(), MfVector3()); }    // PORT: mflinalg\'s inline\n', 1),
    ],
}

# MfListValue's last member: MfPoLibrary's MfDblListItem *pliCursor (see MfListValue.h)
LISTVALUE_CURSOR_VIEW = ('    int m_1c;\n', '    void *m_1c;     // PORT: pointer-sized, MfPoLibrary\'s list cursor\n', 1)

EDITS = {
    'MfActorLib/MfStream.cpp': [
        ('MfStream &MfStream::operator>>(long &i)\n{\n    ReadBytes(sizeof(i), &i);\n    return *this;\n}\n',
         LONG_TWIN + '    ReadBytes(sizeof(i), &i);\n    return *this;\n#endif\n', 1),
        ('MfStream &MfStream::operator<<(long i)\n{\n    WriteBytes(sizeof(i), &i);\n    return *this;\n}\n',
         LONG_TWIN + '    WriteBytes(sizeof(i), &i);\n    return *this;\n#endif\n', 1),
        ('int MfStream::SizeOf(long)\n{\n    return 4;\n}\n',
         LONG_TWIN + '    return 4;\n#endif\n', 1),
        # Status() had no definition but 32-bit-shaped inline ones in unit views (the
        # original was folded into MfMemoryStream::Flush, 0x1001a770)
        ('}\n'
         '\n'
         '// 0x10019b60 ??1MfStream@@QAE@XZ\n',
         '}\n'
         'MfStreamError MfStream::Status() { return status; }    // PORT: the one definition\n'
         '// 0x10019b60 ??1MfStream@@QAE@XZ\n', 1),
    ],
    'MfActorLib/MfFileStream_unit.h': [STREAM_VIEW, FILESTREAM_VIEW],
    'MfActorLib/MfActorRecordList_unit.h': [
        ('    char m_pad[0x14];\n',
         '    void *m_vptr; int m_04; int m_08; int m_0c; void *m_10;    // PORT: MfStream\'s 64-bit layout\n', 1),
    ],
    'MfActorLib/MfAnimEventList_unit.h': [STREAM_VIEW],
    'MfActorLib/MfCamera_unit.h': [
        STREAM_VIEW, TYPEDVALUE_VIEW,
        # the real MfPropertyList holds an MfDblList (vptr, first, last, count)
        ('    int m_04, m_08, m_0c, m_10;\n',
         '    void *m_04, *m_08, *m_0c; int m_10;    // PORT: an MfDblList (vptr, first, last, count)\n', 1),
    ],
    'MfActorLib/MfAnimSequence_unit.h': [
        TYPEDVALUE_VIEW,
        # the MfFile base
        ('    char m_pad00[0x34];                   // base classes / members not yet known\n',
         '    void *m_vptr, *m_handle; int m_isOpen, m_inMemory, m_lastError, m_mode; MfFileType m_type; unsigned short m_format, m_version; MfString m_filename; MfFileType m_objectType; char *m_buffer; unsigned long m_bufferSize, m_pos;    // PORT: MfFile\'s members (MfFile.h)\n', 1),
    ],
    'MfActorLib/MfAnimSequence_2_unit.h': [
        STREAM_VIEW, FILESTREAM_VIEW, TYPEDVALUE_VIEW,
        ('    char m_data[0xc];\n', '    char *m_data[3];    // PORT: three MfStrings (one pointer each)\n', 1),
        LISTVALUE_CURSOR_VIEW,
    ],
    'MfActorLib/MfCopyFolder.h': [
        ('    char m_data[0xc];\n', '    char *m_data[3];    // PORT: three MfStrings (one pointer each)\n', 1),
    ],
    'MfActorLib/MfFSMSpaceToBhvr_unit.h': [TYPEDVALUE_VIEW],
    'MfActorLib/MfListValue_unit.h': [TYPEDVALUE_VIEW, LISTVALUE_CURSOR_VIEW],
    # MfPoLibrary reads this member as its MfDblListItem *pliCursor: it must be
    # pointer-sized so every MfActorLib constructor clears all 8 bytes
    'MfActorLib/MfListValue.h': [
        ('    int flags;  // +0x1c (cleared by the constructors, otherwise unused)\n',
         '    void *flags;  // PORT: pointer-sized (MfPoLibrary\'s list cursor); cleared by the constructors\n', 1),
    ],
    # the base (vptr, long m_flags, unsigned long m_time) is 0x10 bytes, not 0xc; the
    # record is the 0xc8 bytes from mFlags on
    'MfActorLib/MfJointKeyframe.cpp': [
        ('    s.ReadBytes(sizeof(MfJointKeyframe) - 0xc, &mFlags);\n',
         '    s.ReadBytes(sizeof(MfJointKeyframe) - ((const char *)&mFlags - (const char *)this), &mFlags);    // PORT: base size\n', 1),
        ('    s.WriteBytes(sizeof(MfJointKeyframe) - 0xc, &mFlags);\n',
         '    s.WriteBytes(sizeof(MfJointKeyframe) - ((const char *)&mFlags - (const char *)this), &mFlags);    // PORT: base size\n', 1),
    ],
    # node pointer arrays: 8-byte pointers
    'MfActorLib/MfTreeTraversal.cpp': [
        ('operator new(numNodes << 2);', 'operator new(numNodes * sizeof(*mNodes));    // PORT: pointer size', 1),
        ('operator new(mNumNodes << 2);', 'operator new(mNumNodes * sizeof(*mNodes));    // PORT: pointer size', 1),
    ],
    'MfActorLib/MfProtoTraversal.cpp': [
        ('operator new(root->treeSize(0) << 2);', 'operator new(root->treeSize(0) * sizeof(*mNodes));    // PORT: pointer size', 1),
    ],
    # the geometry node's mTriangles pointer is a 32-bit "has triangles" flag in the stream
    'MfActorLib/MfProtoNode.cpp': [
        ('            s >> (int &)geom->mTriangles;\n',
         '            { int v = 0; s >> v; geom->mTriangles = (MfTriangleSet *)(intptr_t)v; }    // PORT: a 32-bit flag\n', 1),
    ],
    'MfActorLib/MfProtoNode_2.cpp': [
        ('        s >> (int &)((MfProtoGeomInfo *)mInfo)->mTriangles;\n',
         '        { int v = 0; s >> v; ((MfProtoGeomInfo *)mInfo)->mTriangles = (MfTriangleSet *)(intptr_t)v; }    // PORT: a 32-bit flag\n', 1),
        ('        s << (int)((MfProtoGeomInfo *)mInfo)->mTriangles;\n',
         '        s << (int)(((MfProtoGeomInfo *)mInfo)->mTriangles != 0);    // PORT: a 32-bit flag\n', 1),
    ],
}
assert not set(EDITS) & set(LINALG_INLINES)
EDITS.update(LINALG_INLINES)

# mfutil's inline MfDblList::GetCurrent (src/mfutil/MfDblList.h), for the vtables of
# the lists derived here; in the unit views MfDblListItem is incomplete, so the body
# reads its first member (pData, +0) directly.
GETCURRENT = {
    'MfActorLib/MfDblList.h': ('    virtual void *GetCurrent(MfDblListItem *&pli) const;\n',
                               '    virtual void *GetCurrent(MfDblListItem *&pli) const { return pli ? pli->pData : 0; }'
                               '    // PORT: mfutil\'s inline\n', 1),
    'MfActorLib/MfActorRecordList_unit.h': ('    virtual void *GetCurrent(MfDblListItem *&) const;\n',
                                            '    virtual void *GetCurrent(MfDblListItem *&pli) const { return pli ? *(void **)pli : 0; }'
                                            '    // PORT: mfutil\'s inline (pData)\n', 1),
    'MfActorLib/MfAnimEventList_unit.h': ('    virtual void *GetCurrent(MfDblListItem *&) const;\n',
                                          '    virtual void *GetCurrent(MfDblListItem *&pli) const { return pli ? *(void **)pli : 0; }'
                                          '    // PORT: mfutil\'s inline (pData)\n', 1),
}
for _path, _edit in GETCURRENT.items():
    EDITS.setdefault(_path, []).append(_edit)
