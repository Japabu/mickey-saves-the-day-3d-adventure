# MfActorLib edits for the 64-bit port (see port/rewrite.py, EDITS).
#
# Streams and files: the formats hold 32-bit values where the source has C `long`
# (8 bytes on LP64); they are read and written as 32-bit and widened in memory.
#
# Unit views: several units declare another unit's class again, with int or char[]
# filler where the real class (MfStream.h, MfFileStream.h, MfMemoryStream.h, MfFile.h,
# MfTypedValue.h, MfPropertyList.h, mfutil's MfCopyFolder) has `long`, pointer or
# `unsigned long` members. On 32-bit the offsets agreed; here the fillers get the real
# member types so every view has the real LP64 layout (objects allocated from a view,
# and inline members emitted from one, then match the real class).

# MfStream view fillers: int userFormat, long objectType, int objectSize, void *userData
STREAM_VIEW = (
    '    int m_04;\n'
    '    int m_08;\n'
    '    int m_0c;\n'
    '    int m_10;\n'
    '    MfStreamError m_status;',
    '    int m_04;\n'
    '    long m_08;      // PORT: MfStream::objectType is a long\n'
    '    int m_0c;\n'
    '    void *m_10;     // PORT: MfStream::userData is a pointer\n'
    '    MfStreamError m_status;', 1)
# MfFileStream view fillers: char *buffer, unsigned long bufferFill, bufferPos, MfFileMode mode
FILESTREAM_VIEW = (
    '    int m_24;\n'
    '    int m_28;\n'
    '    int m_2c;\n'
    '    int m_30;\n',
    '    char *m_24;             // PORT: MfFileStream::buffer is a pointer\n'
    '    unsigned long m_28;     // PORT: bufferFill\n'
    '    unsigned long m_2c;     // PORT: bufferPos\n'
    '    int m_30;\n', 1)
# MfTypedValue view: long lModCBackId, void (*pModCBack)(...)
TYPEDVALUE_VIEW = (
    'protected:\n'
    '    int m_04;\n'
    '    int m_08;\n'
    '};',
    'protected:\n'
    '    long m_04;      // PORT: MfTypedValue::lModCBackId is a long\n'
    '    void *m_08;     // PORT: MfTypedValue::pModCBack is a pointer\n'
    '};', 1)
# MfFileType holds a 4-byte four-character code (read with ReadBytes(4, &type))
FILETYPE = ('    long m_type;\n', '    port_i32 m_type;    // PORT: a four-character code, 4 bytes in the file\n', 1)

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

EDITS = {
    'MfActorLib/MfStream.cpp': [
        ('MfStream &MfStream::operator>>(long &i)\n'
         '{\n'
         '    ReadBytes(sizeof(i), &i);\n',
         'MfStream &MfStream::operator>>(long &i)\n'
         '{\n'
         '    port_i32 v = 0; ReadBytes(sizeof(v), &v); i = v;    // PORT: a long is 4 bytes in the stream\n', 1),
        ('MfStream &MfStream::operator<<(long i)\n'
         '{\n'
         '    WriteBytes(sizeof(i), &i);\n',
         'MfStream &MfStream::operator<<(long i)\n'
         '{\n'
         '    port_i32 v = (port_i32)i; WriteBytes(sizeof(v), &v);    // PORT: a long is 4 bytes in the stream\n', 1),
        # Status() had no definition but 32-bit-shaped inline ones in unit views (the
        # original was folded into MfMemoryStream::Flush, 0x1001a770)
        ('}\n'
         '\n'
         '// 0x10019b60 ??1MfStream@@QAE@XZ\n',
         '}\n'
         'MfStreamError MfStream::Status() { return status; }    // PORT: the one definition\n'
         '// 0x10019b60 ??1MfStream@@QAE@XZ\n', 1),
    ],
    'MfActorLib/MfFile.h': [FILETYPE],
    'MfActorLib/MfFile_2.cpp': [
        ('    unsigned long pos = GetFilePointer();\n'
         '    long type;\n',
         '    unsigned long pos = GetFilePointer();\n'
         '    port_i32 type;    // PORT: ReadBytes(4, &type) fills it\n', 1),
        ('    long header[4];\n', '    port_i32 header[4];    // PORT: the file header is 4 32-bit words\n', 2),
    ],
    'MfActorLib/MfFileStream_unit.h': [
        STREAM_VIEW, FILESTREAM_VIEW,
        ('    int m_capacity;     // 0x20\n'
         '    int m_pos;          // 0x24\n',
         '    unsigned long m_capacity;   // PORT: MfMemoryStream::size is an unsigned long\n'
         '    unsigned long m_pos;        // PORT: position\n', 1),
    ],
    'MfActorLib/MfActorRecordList_unit.h': [
        ('    char m_pad[0x14];\n',
         '    void *m_vptr; int m_04; long m_08; int m_0c; void *m_10;    // PORT: MfStream\'s LP64 layout\n', 1),
    ],
    'MfActorLib/MfAnimEventList_unit.h': [STREAM_VIEW, FILETYPE],
    'MfActorLib/MfCamera_unit.h': [
        STREAM_VIEW, TYPEDVALUE_VIEW,
        # the real MfPropertyList holds an MfDblList (vptr, first, last, count)
        ('    int m_04, m_08, m_0c, m_10;\n',
         '    void *m_04, *m_08, *m_0c; int m_10;    // PORT: an MfDblList (vptr, first, last, count)\n', 1),
    ],
    'MfActorLib/MfAnimSequence_unit.h': [
        TYPEDVALUE_VIEW, FILETYPE,
        # the MfFile base
        ('    char m_pad00[0x34];                   // base classes / members not yet known\n',
         '    void *m_vptr, *m_handle; int m_isOpen, m_inMemory, m_lastError, m_mode; MfFileType m_type; unsigned short m_format, m_version; MfString m_filename; MfFileType m_objectType; char *m_buffer; unsigned long m_bufferSize, m_pos;    // PORT: MfFile\'s members (MfFile.h)\n', 1),
    ],
    'MfActorLib/MfAnimSequence_2_unit.h': [
        STREAM_VIEW, FILESTREAM_VIEW, TYPEDVALUE_VIEW, FILETYPE,
        ('    char m_data[0xc];\n', '    char *m_data[3];    // PORT: three MfStrings (one pointer each)\n', 1),
        ('    int m_bufferSize;           // 0x2c\n'
         '    int m_pos;                  // 0x30\n',
         '    unsigned long m_bufferSize; // PORT: unsigned long, as in MfFile.h\n'
         '    unsigned long m_pos;        // PORT: unsigned long, as in MfFile.h\n', 1),
    ],
    'MfActorLib/MfCopyFolder.h': [
        ('    char m_data[0xc];\n', '    char *m_data[3];    // PORT: three MfStrings (one pointer each)\n', 1),
    ],
    'MfActorLib/MfFSMSpaceToBhvr_unit.h': [TYPEDVALUE_VIEW],
    'MfActorLib/MfListValue_unit.h': [TYPEDVALUE_VIEW],
    # the base (vptr, long m_flags, unsigned long m_time) is 0x18 bytes, not 0xc; the
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
    # the control point is a raw 0x88-byte record in the stream
    'MfActorLib/MfSpline.h': [
        ('    unsigned long time;                     // +0x00\n',
         '    port_u32 time;                          // +0x00 PORT: 4 bytes in the stream record\n', 1),
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
