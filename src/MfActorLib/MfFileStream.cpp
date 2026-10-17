// MfActorLib.dll 0x10019f40-0x1001a5a0: MfFileStream.
#include <windows.h>
#include <string.h>
#include "MfFileStream.h"
#include "MfString.h"
#include "MfDblList.h"
#include "MfCallBack.h"
#include "MfAiHook.h"
#include "MfFileHookData.h"
#include "MfAiHookManager.h"

#define BUFFER_SIZE 0x800

// Stream callback codes: CallBack(code, stream).
enum { FS_OPENED = 0, FS_CLOSED = 1, FS_READ = 2, FS_WRITTEN = 3 };

// 0x10019f40 _$E7 (calls the constructor and registers the destructor)
// 0x10019f50 _$E4
// 0x10019f70 _$E6
// 0x10019f80 _$E5
static MfDblList s_freeBuffers;

// 0x10019f90 ??0MfFileStream@@QAE@ABVMfString@@W4MfFileMode@@_N@Z
MfFileStream::MfFileStream(const MfString &name, MfFileMode m, bool buffered)
{
    mode = m;
    closeFile = true;
    buffer = 0;
    userFormat = 0;
    DWORD access, creation;
    if (m == 0) {
        access = GENERIC_READ;
        creation = OPEN_EXISTING;
    } else {
        access = GENERIC_WRITE;
        creation = CREATE_ALWAYS;
    }
    MfFileHookData data;
    MfAi *ai;
    int hooked = 0;
    if (MfInterfaceRegistry::GetInterface("MfAiHookManager", &ai)) {
        data.fileLocation = name;
        data.mode = m;
        if (((MfAiHookManager *)ai)->CallHooks(MF_HOOK_FILE_HANDLE, &data) > 0)
            hooked = 1;
    }
    if (hooked)
        file = data.fileHandle;
    else
        file = CreateFileA(name, access, FILE_SHARE_READ, 0, creation,
                           FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, 0);
    status = (MfStreamError)(file == INVALID_HANDLE_VALUE);
    if (status == 0 && buffered) {
        buffer = (char *)s_freeBuffers.DeleteFirst();
        if (!buffer)
            buffer = new char[BUFFER_SIZE];
        bufferPos = 0;
        bufferFill = 0;
    }
    if (cb)
        cb->CallBack((void *)FS_OPENED, this);
}

// 0x1001a120 ??1MfFileHookData@@QAE@XZ (compiler generated)

// 0x1001a130 ??0MfFileStream@@QAE@PAXW4MfFileMode@@_N@Z
MfFileStream::MfFileStream(void *handle, MfFileMode m, bool buffered)
{
    file = handle;
    mode = m;
    closeFile = false;
    status = MF_STREAM_OK;
    userFormat = 0;
    if (buffered) {
        buffer = (char *)s_freeBuffers.DeleteFirst();
        if (!buffer)
            buffer = new char[BUFFER_SIZE];
        bufferPos = 0;
        bufferFill = 0;
    } else {
        buffer = 0;
    }
    if (cb)
        cb->CallBack((void *)FS_OPENED, this);
}

// 0x1001a1d0 ??1MfFileStream@@QAE@XZ
MfFileStream::~MfFileStream()
{
    if (file != INVALID_HANDLE_VALUE) {
        Flush();
        if (closeFile)
            CloseHandle(file);
    }
    if (buffer)
        s_freeBuffers.Prepend(buffer);
    if (cb)
        cb->CallBack((void *)FS_CLOSED, this);
}

// 0x1001a270 ?ReadBytes@MfFileStream@@UAE?AW4MfStreamError@@KPAX@Z
MfStreamError MfFileStream::ReadBytes(unsigned long n, void *p)
{
    unsigned long done = 0;
    bool readFile = false;
    if (!buffer) {
        ReadFile(file, p, n, &done, 0);
        readFile = true;
        n -= done;
    } else {
        char *dst = (char *)p;
        while (status == 0 && n > 0) {
            if (bufferPos == bufferFill) {
                ReadFile(file, buffer, BUFFER_SIZE, &bufferFill, 0);
                bufferPos = 0;
                readFile = true;
                if (bufferFill == 0) {
                    status = MF_STREAM_READ_FAILED;
                    break;
                }
            }
            unsigned long count = bufferFill - bufferPos;
            if (count > n)
                count = n;
            n -= count;
            memcpy(dst, buffer + bufferPos, count);
            bufferPos += count;
            dst += count;
            done += count;
        }
    }
    if (status == 0 && n != 0)
        status = MF_STREAM_READ_FAILED;
    if (readFile && cb)
        cb->CallBack((void *)FS_READ, this);
    return status;
}

// 0x1001a3a0 ?WriteBytes@MfFileStream@@UAE?AW4MfStreamError@@KPBX@Z
MfStreamError MfFileStream::WriteBytes(unsigned long n, const void *p)
{
    unsigned long done = 0;
    bool wroteFile = false;
    if (!buffer) {
        WriteFile(file, p, n, &done, 0);
        wroteFile = true;
        n -= done;
    } else {
        const char *src = (const char *)p;
        while (status == 0 && n > 0) {
            if (bufferFill == BUFFER_SIZE) {
                wroteFile = true;
                WriteFile(file, buffer, BUFFER_SIZE, &bufferFill, 0);
                if (bufferFill != BUFFER_SIZE) {
                    status = MF_STREAM_WRITE_FAILED;
                    break;
                }
                bufferFill = 0;
            }
            unsigned long space = BUFFER_SIZE - bufferFill;
            unsigned long count = n;
            if (n > space)
                count = space;
            n -= count;
            memcpy(buffer + bufferFill, src, count);
            bufferFill += count;
            src += count;
            done += count;
        }
    }
    if (status == 0 && n != 0)
        status = MF_STREAM_WRITE_FAILED;
    if (wroteFile && cb)
        cb->CallBack((void *)FS_WRITTEN, this);
    return status;
}

// 0x1001a4f0 ?Flush@MfFileStream@@UAE?AW4MfStreamError@@XZ
// Writes the buffer, or (reading) moves the file back over the bytes not read yet.
MfStreamError MfFileStream::Flush()
{
    unsigned long written = 0;
    int pending = bufferFill - bufferPos;
    if (buffer && pending > 0) {
        if (mode == 0) {
            status = (MfStreamError)(SetFilePointer(file, -pending, 0, FILE_CURRENT) == 0xffffffff ? 3 : 0);
        } else {
            WriteFile(file, buffer, pending, &written, 0);
            status = (MfStreamError)(pending != written ? 4 : 0);
        }
    }
    bufferPos = 0;
    bufferFill = 0;
    return status;
}

// 0x1001a570 ?Seek@MfFileStream@@UAE?AW4MfStreamError@@H@Z
MfStreamError MfFileStream::Seek(int n)
{
    Flush();
    SetFilePointer(file, n, 0, FILE_CURRENT);
    return status;
}
