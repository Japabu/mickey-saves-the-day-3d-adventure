// MfActorLib.dll 0x10006c70-0x10007c10: MfFile, MfFileType(MfString) and
// MfFileProgress (the object after MfColModel).
#include "MfFile.h"
#include "MfFileProgress.h"

// 0x10006c60 _$E11 (calls $E6 and $E9)
// 0x10006c70 _$E7 (original _$E6:) constructs lRegisteredClasses
// 0x10006c90 _$E10 (original _$E9:) atexit($E7)
// 0x10006ca0 _$E8 (original _$E7:) destroys lRegisteredClasses
MfDblList MfFile::lRegisteredClasses;

// One registered file class (lRegisteredClasses holds them).
struct MfFileClassEntry {
    MfFileClassEntry(MfFileType t, MfFileObjectConstructor c) : type(t), construct(c) {}
    MfFileType type;
    MfFileObjectConstructor construct;
};

// 0x10006cc0 ??0MfFile@@QAE@XZ
MfFile::MfFile()
    : m_handle(0), m_isOpen(0), m_inMemory(0), m_lastError(MF_FILE_OK),
      m_type(0x20202020), m_objectType(0x20202020), m_buffer(0)
{
}

// 0x10006d30 ??1MfFile@@UAE@XZ
MfFile::~MfFile()
{
    if (m_isOpen)
        Close();
    m_isOpen = 0;
}

// EQUIV 0x10006d90 ?Open@MfFile@@UAEHABVMfString@@H@Z: reviewed: order of independent instructions (mov ecx, esi before or after the header field stores)
int MfFile::Open(const MfString &name, int mode)
{
    if (m_isOpen) {
        m_lastError = MF_FILE_ALREADY_OPEN;
        return 0;
    }
    m_mode = mode;
    m_filename = name;
    m_inMemory = 0;

    DWORD access, creation;
    if (mode == MF_FILE_READ) {
        access = GENERIC_READ;
        creation = OPEN_EXISTING;
    } else if (mode == MF_FILE_WRITE) {
        access = GENERIC_WRITE;
        creation = CREATE_ALWAYS;
    } else {
        m_lastError = MF_FILE_BAD_FORMAT;
        return 0;
    }
    m_handle = CreateFileA(name, access, FILE_SHARE_READ, 0, creation,
                           FILE_FLAG_RANDOM_ACCESS | FILE_ATTRIBUTE_NORMAL, 0);
    if (m_handle == 0) {
        m_lastError = MF_FILE_NOT_OPEN;
        return 0;
    }
    m_isOpen = 1;
    m_buffer = new char[MF_FILE_BUFFER_SIZE];
    m_pos = 0;
    m_bufferSize = 0;
    if (mode == MF_FILE_READ && !ReadHeader()) {
        CloseHandle(m_handle);
        m_isOpen = 0;
        return 0;
    }
    if (mode == MF_FILE_WRITE) {
        m_format = 2;
        m_type = MfFileType(0x20202020);
        SetVersion(0);
        if (!WriteHeader()) {
            CloseHandle(m_handle);
            m_isOpen = 0;
            return 0;
        }
    }
    m_lastError = MF_FILE_OK;
    return 1;
}

// 0x10006ec0 ?Open@MfFile@@UAEHHPADH@Z
int MfFile::Open(int mode, char *buffer, int size)
{
    if (m_isOpen) {
        m_lastError = MF_FILE_ALREADY_OPEN;
        return 0;
    }
    m_inMemory = 1;
    m_mode = mode;
    if (mode == MF_FILE_READ) {
        if (!buffer) {
            m_lastError = MF_FILE_NOT_OPEN;
            return 0;
        }
        m_buffer = buffer;
        m_bufferSize = size;
    } else if (mode == MF_FILE_WRITE) {
        m_buffer = new char[MF_FILE_BUFFER_SIZE];
        m_bufferSize = MF_FILE_BUFFER_SIZE;
    } else {
        m_lastError = MF_FILE_BAD_FORMAT;
        return 0;
    }
    m_isOpen = 1;
    m_pos = 0;
    m_lastError = MF_FILE_OK;
    return 1;
}

// 0x10006f50 ?Close@MfFile@@UAEHXZ
int MfFile::Close()
{
    if (!m_isOpen) {
        m_lastError = MF_FILE_NOT_OPEN;
        return 0;
    }
    if (m_inMemory) {
        if (m_mode == MF_FILE_WRITE)
            delete m_buffer;
        m_buffer = 0;
        m_inMemory = 0;
        m_pos = 0;
        m_isOpen = 0;
        m_lastError = MF_FILE_OK;
        return 1;
    }
    if (m_mode == MF_FILE_WRITE) {
        if (!SetEndOfFile(m_handle)) {
            m_lastError = MF_FILE_IO_ERROR;
            return 0;
        }
        if (!WriteHeader()) {
            m_lastError = MF_FILE_IO_ERROR;
            return 0;
        }
    }
    if (!CloseHandle(m_handle)) {
        m_lastError = MF_FILE_IO_ERROR;
        return 0;
    }
    m_handle = 0;
    if (m_buffer) {
        delete m_buffer;
        m_buffer = 0;
    }
    m_isOpen = 0;
    m_lastError = MF_FILE_OK;
    return 1;
}

// 0x10007010 ?Read@MfFile@@QAEHAAVMfFileObject@@@Z
int MfFile::Read(MfFileObject &obj)
{
    if (!m_isOpen || m_mode != MF_FILE_READ) {
        m_lastError = MF_FILE_NOT_OPEN;
        return 0;
    }
    unsigned long pos = GetFilePointer();
    long type;
    if (!ReadBytes(4, &type))
        return 0;
    if (obj.TypeInFile() != MfFileType(type)) {
        SetFilePointer(pos, 0);
        m_lastError = MF_FILE_WRONG_TYPE;
        return 0;
    }
    return obj.ReadFromFile(*this) != 0;
}

// 0x100070d0 ?Read@MfFile@@QAEHHPAVMfFileObject@@@Z
int MfFile::Read(int n, MfFileObject *objs)
{
    if (!m_isOpen || m_mode != MF_FILE_READ) {
        m_lastError = MF_FILE_NOT_OPEN;
        return 0;
    }
    int i;
    for (i = 0; i < n; i++)
        if (!Read(objs[i]))
            break;
    return i;
}

// 0x10007120 ?ReadObject@MfFile@@QAEPAVMfFileObject@@XZ
MfFileObject *MfFile::ReadObject()
{
    if (!m_isOpen || m_mode != MF_FILE_READ) {
        m_lastError = MF_FILE_NOT_OPEN;
        return 0;
    }
    if (!ReadType(m_objectType))
        return 0;
    MfFileObjectConstructor construct = FindConstructor(m_objectType);
    if (!construct)
        return 0;
    return construct(*this);
}

// 0x10007170 ?Write@MfFile@@QAEHAAVMfFileObject@@@Z
int MfFile::Write(MfFileObject &obj)
{
    if (!m_isOpen || m_mode != MF_FILE_WRITE) {
        m_lastError = MF_FILE_NOT_OPEN;
        return 0;
    }
    long type = obj.TypeInFile();
    if (!WriteBytes(4, &type))
        return 0;
    return obj.WriteToFile(*this);
}

// 0x100071d0 ?Write@MfFile@@QAEHHPAVMfFileObject@@@Z
int MfFile::Write(int n, MfFileObject *objs)
{
    if (!m_isOpen || m_mode != MF_FILE_WRITE) {
        m_lastError = MF_FILE_NOT_OPEN;
        return 0;
    }
    int i;
    for (i = 0; i < n; i++)
        if (!Write(objs[i]))
            break;
    return i;
}

// 0x10007220 ?PeekType@MfFile@@QAEHAAVMfFileType@@@Z
int MfFile::PeekType(MfFileType &type)
{
    unsigned long pos = GetFilePointer();
    int ok = ReadBytes(4, &type) != 0;
    SetFilePointer(pos, 0);
    return ok;
}

// 0x10007260 ?WriteTypeOf@MfFile@@QAEHAAVMfFileObject@@@Z
int MfFile::WriteTypeOf(MfFileObject &obj)
{
    MfFileType type = obj.TypeInFile();
    return WriteBytes(4, &type) != 0;
}

// 0x100072c0 ?Read@MfFile@@QAEHAAF@Z
int MfFile::Read(short &v)
{
    return ReadBytes(2, &v) != 0;
}

// 0x100072e0 ?Read@MfFile@@QAEHAAH@Z (= Read(float &), ReadType: same code)
int MfFile::Read(int &v)
{
    return ReadBytes(4, &v) != 0;
}

int MfFile::Read(float &v)
{
    return ReadBytes(4, &v) != 0;
}

int MfFile::ReadType(MfFileType &type)
{
    return ReadBytes(4, &type) != 0;
}

// 0x10007300 ?Read@MfFile@@QAEHAAVMfString@@@Z
int MfFile::Read(MfString &s)
{
    int len;
    char buf[256];
    if (!ReadBytes(4, &len))
        return 0;
    if (len == 0) {
        s = NULL_STRING;
        return 1;
    }
    if (!ReadBytes(len, buf))
        return 0;
    buf[len] = 0;
    s = buf;
    return 1;
}

// 0x100073a0 ?Write@MfFile@@QAEHF@Z
int MfFile::Write(short v)
{
    return WriteBytes(2, &v) != 0;
}

// 0x100073c0 ?Write@MfFile@@QAEHH@Z (= Write(float): same code)
int MfFile::Write(int v)
{
    return WriteBytes(4, &v) != 0;
}

int MfFile::Write(float v)
{
    return WriteBytes(4, &v) != 0;
}

// 0x100073e0 ?Write@MfFile@@QAEHVMfString@@@Z
int MfFile::Write(MfString s)
{
    int len = s.Length();
    if (!WriteBytes(4, &len))
        return 0;
    if (len > 0)
        return WriteBytes(len, (const char *)s) != 0;
    return 1;
}

// 0x100074c0 ?ReadHeader@MfFile@@IAEHXZ
int MfFile::ReadHeader()
{
    long header[4];
    if (m_inMemory)
        return 1;
    if (!ReadBytes(sizeof(header), header)) {
        m_lastError = MF_FILE_IO_ERROR;
        return 0;
    }
    if (header[0] != MF_FILE_MAGIC) {
        m_lastError = MF_FILE_BAD_FORMAT;
        return 0;
    }
    m_format = (unsigned short)header[1];
    m_type = header[2];
    m_version = (unsigned short)(header[1] >> 16);
    m_lastError = MF_FILE_OK;
    return 1;
}

// 0x10007530 ?WriteHeader@MfFile@@IAEHXZ
int MfFile::WriteHeader()
{
    long header[4];
    if (m_inMemory)
        return 1;
    SetFilePointer(0, 0);
    header[0] = MF_FILE_MAGIC;
    header[1] = ((long)m_version << 16) + m_format;
    header[2] = m_type;
    header[3] = 0;
    return WriteBytes(sizeof(header), header);
}

// 0x100075a0 ?GetFilePointer@MfFile@@IAEKXZ
unsigned long MfFile::GetFilePointer()
{
    if (m_inMemory)
        return m_pos;
    return ::SetFilePointer(m_handle, 0, 0, FILE_CURRENT);
}

// 0x100075c0 ?SetFilePointer@MfFile@@IAEXKK@Z
void MfFile::SetFilePointer(unsigned long pos, unsigned long method)
{
    if (m_inMemory)
        m_pos = pos;
    else
        ::SetFilePointer(m_handle, (LONG)pos, 0, method);
}

// 0x100075f0 ?ReadBytes@MfFile@@UAEKKPAX@Z
unsigned long MfFile::ReadBytes(unsigned long count, void *data)
{
    unsigned long n;
    int ok;
    if (!m_isOpen || m_mode != MF_FILE_READ) {
        m_lastError = MF_FILE_NOT_OPEN;
        return 0;
    }
    if (m_inMemory) {
        n = m_bufferSize - m_pos;
        if (n > count)
            n = count;
        memcpy(data, m_buffer + m_pos, n);
        m_pos += n;
        ok = n == count;
    } else {
        char *p = (char *)data;
        ok = 1;
        n = 0;
        while (ok && count > 0) {
            if (m_pos == m_bufferSize) {
                ok = ReadFile(m_handle, m_buffer, MF_FILE_BUFFER_SIZE, &m_bufferSize, 0);
                m_pos = 0;
                if (m_bufferSize == 0) {
                    m_lastError = MF_FILE_IO_ERROR;
                    return 0;
                }
            }
            unsigned long chunk = m_bufferSize - m_pos;
            if (chunk > count)
                chunk = count;
            memcpy(p, m_buffer + m_pos, chunk);
            count -= chunk;
            m_pos += chunk;
            p += chunk;
            n += chunk;
        }
    }
    m_lastError = ok ? MF_FILE_OK : MF_FILE_IO_ERROR;
    return n;
}

// 0x10007730 ?WriteBytes@MfFile@@UAEKKPBX@Z
unsigned long MfFile::WriteBytes(unsigned long count, const void *data)
{
    unsigned long n;
    int ok;
    if (!m_isOpen || m_mode == MF_FILE_READ) {
        m_lastError = MF_FILE_NOT_OPEN;
        return 0;
    }
    if (m_inMemory) {
        if (m_pos + count > m_bufferSize) {
            m_bufferSize += m_bufferSize;
            if (m_bufferSize < m_pos + count)
                m_bufferSize = m_pos + count;
            char *buffer = new char[m_bufferSize];
            memcpy(buffer, m_buffer, m_pos);
            delete m_buffer;
            m_buffer = buffer;
        }
        memcpy(m_buffer + m_pos, data, count);
        m_pos += count;
        n = count;
        ok = 1;
    } else {
        ok = WriteFile(m_handle, data, count, &n, 0);
        if (n != count)
            ok = 0;
    }
    m_lastError = ok ? MF_FILE_OK : MF_FILE_IO_ERROR;
    return n;
}

// 0x10007840 ?Compress@MfFile@@QAEHXZ (= Uncompress: same code)
int MfFile::Compress()
{
    if (m_isOpen && m_inMemory)
        return 1;
    return 0;
}

int MfFile::Uncompress()
{
    if (m_isOpen && m_inMemory)
        return 1;
    return 0;
}

// 0x10007860 ?RegisterClassA@MfFile@@SAXP6APAVMfFileObject@@AAV1@@ZVMfFileType@@@Z
void MfFile::RegisterClass(MfFileObjectConstructor construct, MfFileType type)
{
    lRegisteredClasses.Add(new MfFileClassEntry(type, construct));
}

// 0x100078c0 ?FindConstructor@MfFile@@IAEP6APAVMfFileObject@@AAV1@@ZVMfFileType@@@Z
MfFileObjectConstructor MfFile::FindConstructor(MfFileType type)
{
    MfDblListItem *item;
    for (MfFileClassEntry *entry = (MfFileClassEntry *)lRegisteredClasses.GetFirst(item);
         entry; entry = (MfFileClassEntry *)lRegisteredClasses.GetNext(item))
        if (entry->type == type)
            return entry->construct;
    return 0;
}

// 0x10007900 ??0MfFileType@@QAE@VMfString@@@Z
// The first four characters of the name, padded with blanks.
MfFileType::MfFileType(MfString name)
{
    const char *s = name;
    m_type = 0x20202020;
    for (unsigned int i = 0; i < 4 && s[i]; i++)
        ((char *)&m_type)[i] = s[i];
}

// 0x10007970 _$E17 (calls $E12 and $E15)
// 0x10007980 _$E13 (original _$E12:) constructs MfFileProgress::lock
// 0x10007990 _$E16 (original _$E15:) atexit($E13)
// 0x100079a0 _$E14 (original _$E13:) destroys MfFileProgress::lock
MfCriticalSection MfFileProgress::lock;

// 0x100079c0 _$E22 (calls $E18 and $E20)
// 0x100079d0 _$E19 (original _$E18:) constructs the progress stack
// 0x100079f0 _$E21 (original _$E20:) atexit($E19)
// 0x10007a00 _$E20 (original _$E19:) destroys the progress stack
static MfDblList s_progress;

void (*MfFileProgress::callback)();

// One SetProgress entry.
struct MfFileProgressEntry {
    MfString name;
    int state;
};

// 0x10007a10 ?GetProgress@MfFileProgress@@SAXAAHAAVMfString@@@Z
void MfFileProgress::GetProgress(int &state, MfString &name)
{
    lock.Enter();
    MfDblListItem *item;
    MfFileProgressEntry *entry = (MfFileProgressEntry *)s_progress.GetFirst(item);
    if (entry) {
        state = entry->state;
        name = entry->name;
    } else {
        state = 2;
        name = NULL_STRING;
    }
    lock.Leave();
}

// 0x10007a80 ?SetProgress@MfFileProgress@@SAXHVMfString@@@Z
void MfFileProgress::SetProgress(int state, MfString name)
{
    lock.Enter();
    if (state == 2) {
        MfDblListItem *item;
        MfFileProgressEntry *entry = (MfFileProgressEntry *)s_progress.GetFirst(item);
        if (entry) {
            delete entry;
            s_progress.Delete(item);
        }
    } else if (state == 3) {
        MfDblListItem *item;
        for (MfFileProgressEntry *entry = (MfFileProgressEntry *)s_progress.GetFirst(item);
             entry; entry = (MfFileProgressEntry *)s_progress.GetNext(item))
            delete entry;
        s_progress.Clear();
    } else {
        MfFileProgressEntry *entry = new MfFileProgressEntry;
        entry->state = state;
        entry->name = name;
        s_progress.Prepend(entry);
    }
    lock.Leave();
    UpdateProgress();
}

// 0x10007bc0 ?SetProgressCB@MfFileProgress@@SAXP6AXXZ@Z
void MfFileProgress::SetProgressCB(void (*cb)())
{
    lock.Enter();
    callback = cb;
    lock.Leave();
}

// 0x10007be0 ?UpdateProgress@MfFileProgress@@SAXXZ
void MfFileProgress::UpdateProgress()
{
    lock.Enter();
    if (callback)
        callback();
    lock.Leave();
}
