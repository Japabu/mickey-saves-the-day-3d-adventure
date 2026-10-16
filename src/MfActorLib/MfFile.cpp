// MfFile unit: MfActorLib.dll 0x10001880-0x10002230 (vc5 /O2 /GX).
// 0x10001880-0x10001b70: inline members of MfFileObject and MfFile (from MfFile.h),
// emitted because both classes are dllexport.
// 0x10001880 ??0MfFileObject@@QAE@XZ
// 0x10001890 ??1MfFileObject@@UAE@XZ
// 0x100018a0 ?ClassTypeInFile@MfFileObject@@SA?AVMfFileType@@XZ
// 0x100018c0 ?TypeInFile@MfFileObject@@UAE?AVMfFileType@@XZ
// 0x100018e0 ?NameInFile@MfFileObject@@UAE?AVMfString@@XZ
// 0x10001910 ?ConstructFromFile@MfFileObject@@SAPAV1@AAVMfFile@@@Z
// 0x10001920 ??0MfFileObject@@QAE@ABV0@@Z (implicit)
// 0x10001930 ??_EMfFileObject@@UAEPAXI@Z (compiler generated)
// 0x10001980 ?GetType@MfFile@@QAE?AVMfFileType@@XZ
// 0x100019a0 ?SetType@MfFile@@QAEXVMfFileType@@@Z
// 0x100019b0 ?GetVersion@MfFile@@QAEGXZ
// 0x100019c0 ?SetVersion@MfFile@@QAEXG@Z
// 0x100019d0 ?Filename@MfFile@@QAE?AVMfString@@XZ
// 0x10001a00 ?GetObjectType@MfFile@@QAE?AVMfFileType@@XZ
// 0x10001a20 ?GetLastError@MfFile@@QAEHXZ
// 0x10001a30 ?GetBuffer@MfFile@@QAEPADAAH@Z
// 0x10001a40 ?SetError@MfFile@@IAEHH@Z
// 0x10001a60 ??0MfFile@@QAE@ABV0@@Z (implicit)
// 0x10001b00 ??4MfFile@@QAEAAV0@ABV0@@Z (implicit)
// 0x10001b70 ??_EMfFile@@UAEPAXI@Z (compiler generated)
#include "MfFile_unit.h"


// 0x10001bc0 ??0MfAnimFile@@QAE@XZ
MfAnimFile::MfAnimFile()
    : m_numFrames(0), m_numNodes(-1), m_headerPos(0), m_dataStart(0), m_dataEnd(0)
{
    m_type = ClassTypeInFile();
    m_version = 1;
}

// 0x10001c40 ?ClassTypeInFile@MfAnimFile@@CA?AVMfFileType@@XZ
MfFileType MfAnimFile::ClassTypeInFile()
{
    return MfFileType(MfString("Aamf"));
}

// 0x10001c70 ??_GMfAnimFile@@UAEPAXI@Z (compiler generated)
// 0x10001c90 ??1MfAnimFile@@UAE@XZ
MfAnimFile::~MfAnimFile()
{
    if (m_isOpen)
        Close();
}

// 0x10001d00 ?Open@MfAnimFile@@UAEHABVMfString@@H@Z
int MfAnimFile::Open(const MfString &path, int mode)
{
    if (m_isOpen)
        Close();
    if (MfFile::Open(path, mode)) {
        path.SplitPath(0, 0, &m_name, 0);
        int ok;
        if (mode == 1)
            ok = _InitWrite();
        else
            ok = _InitRead();
        if (!ok) {
            Close();
            return 0;
        }
        return 1;
    }
    return 0;
}

// 0x10001d80 ?WriteFrame@MfAnimFile@@QAEHPBXH@Z
int MfAnimFile::WriteFrame(const void *data, int numNodes)
{
    if (!m_isOpen || !data || numNodes <= 0)
        return 0;
    if (m_numNodes == -1)
        m_numNodes = numNodes;
    if (!WriteBytes(numNodes * 28, data))
        return 0;
    m_numFrames++;
    return 1;
}

// 0x10001dd0 ?ReadFrame@MfAnimFile@@QAEHHPAXH@Z
int MfAnimFile::ReadFrame(int frame, void *data, int numNodes)
{
    if (!m_isOpen || numNodes != m_numNodes)
        return 0;
    unsigned long pos = m_dataStart + frame * m_numNodes * 28;
    if (pos < m_dataStart || pos >= m_dataEnd)
        return 0;
    _Seek(pos);
    return ReadBytes(numNodes * 28, data) != 0;
}

// EQUIV 0x10001e40 ?ReadNodeFrames@MfAnimFile@@QAEHHPAVMfTransform3@@HAAH1@Z: reviewed: register choice (node in ebp vs reloaded from the argument slot); same imports and calls
int MfAnimFile::ReadNodeFrames(int node, MfTransform3 *xf, int numFrames,
                               int &rotChanged, int &posChanged)
{
    if (!m_isOpen || node < 0 || node >= m_numNodes)
        return 0;
    MfTransform3 t;
    rotChanged = 0;
    posChanged = 0;
    for (int i = 0; i < numFrames; i++) {
        unsigned long p = m_dataStart + (node + i * m_numNodes) * 28;
        if (p < m_dataEnd) {
            _Seek(p);
            if (!ReadBytes(28, &t))
                return 0;
        }
        xf[i].m_rotation = t.m_rotation;
        xf[i].m_translation = t.m_translation;
        if (i > 0) {
            if (!rotChanged && !(xf[i - 1].m_rotation == t.m_rotation))
                rotChanged = 1;
            if (!posChanged && !(xf[i - 1].m_translation == t.m_translation))
                posChanged = 1;
        }
    }
    return 1;
}

// 0x10001fb0 ?Close@MfAnimFile@@UAEHXZ
int MfAnimFile::Close()
{
    if (m_numFrames > 0 && m_mode == 1) {
        _Seek(m_headerPos);
        if (!Write(m_name))
            return 0;
        if (!Write(m_numNodes))
            return 0;
        if (!Write(m_numFrames))
            return 0;
        SetFilePointer(0, 2);
    }
    m_numFrames = 0;
    m_numNodes = 0;
    return MfFile::Close() != 0;
}

// 0x10002040 ?_Seek@MfAnimFile@@AAEXK@Z
void MfAnimFile::_Seek(unsigned long pos)
{
    m_pos = pos;
    SetFilePointer(pos, 0);
}

// 0x10002060 ?ReadBytes@MfAnimFile@@UAEKKPAX@Z
unsigned long MfAnimFile::ReadBytes(unsigned long n, void *buf)
{
    if (!m_isOpen || m_mode != 0) {
        m_lastError = 1;
        return 0;
    }
    unsigned long nRead = 0;
    int ok = ReadFile(m_handle, buf, n, &nRead, 0);
    if (!nRead) {
        m_lastError = 4;
        return 0;
    }
    m_pos += nRead;
    m_lastError = ok ? 0 : 4;
    return nRead;
}

// 0x100020e0 ?_InitWrite@MfAnimFile@@AAEHXZ
int MfAnimFile::_InitWrite()
{
    if (!m_isOpen)
        return 0;
    m_headerPos = GetFilePointer();
    if (!Write(m_name))
        return 0;
    if (!Write(m_numNodes))
        return 0;
    if (!Write(m_numFrames))
        return 0;
    m_dataEnd = m_dataStart = GetFilePointer();
    return 1;
}

// 0x10002160 ?_InitRead@MfAnimFile@@AAEHXZ
int MfAnimFile::_InitRead()
{
    if (!m_isOpen)
        return 0;
    m_numNodes = 0;
    m_name = NULL_STRING;
    m_headerPos = m_pos;
    if (!Read(m_name))
        return 0;
    if (!Read(m_numNodes))
        return 0;
    if (!Read(m_numFrames))
        return 0;
    m_dataStart = m_pos;
    SetFilePointer(0, 2);
    m_dataEnd = GetFilePointer();
    _Seek(m_dataStart);
    if (m_numNodes)
        m_numFrames = (m_dataEnd - m_dataStart) / (m_numNodes * 28);
    return 1;
}

// 0x10002220 ?IsOpen@MfAnimSequence@@QAEHXZ (inline, from MfFile.h)
