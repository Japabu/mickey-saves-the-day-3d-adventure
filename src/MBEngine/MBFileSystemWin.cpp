// MBEngine.dll .\MBEngineRelease\MBFileSystemWin.obj
#include "MBFileSystemWin.h"

// 0x1003f870 _MBFileSpecExists
bool MBFileSpecExists(const IMBFileSpec *spec)
{
    MBString fullPath = spec->GetPath();
    bool exists = GetFileAttributesA(fullPath.c_str()) != 0xffffffff;
    return exists;
}

// 0x1003f8e0 _MBIsFileSpecAFolder
bool MBIsFileSpecAFolder(const IMBFileSpec *spec)
{
    bool isFolder = false;
    MBString fullPath = spec->GetPath();
    unsigned long attributes = GetFileAttributesA(fullPath.c_str());
    if (attributes != 0xffffffff)
        isFolder = (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
    return isFolder;
}

// 0x1003f950 _MBIsFileSpecAFile
bool MBIsFileSpecAFile(const IMBFileSpec *spec)
{
    bool isFile = false;
    MBString fullPath = spec->GetPath();
    unsigned long attributes = GetFileAttributesA(fullPath.c_str());
    if (attributes != 0xffffffff)
        isFile = (attributes & FILE_ATTRIBUTE_DIRECTORY) == 0;
    return isFile;
}

// 0x1003f9c0 _MBIsFileSpecHidden
bool MBIsFileSpecHidden(const IMBFileSpec *spec)
{
    bool isHidden = false;
    MBString fullPath = spec->GetPath();
    unsigned long attributes = GetFileAttributesA(fullPath.c_str());
    if (attributes != 0xffffffff)
        isHidden = (attributes & FILE_ATTRIBUTE_HIDDEN) != 0;
    return isHidden;
}

// 0x1003fa30 _MBCreateFolder
long MBCreateFolder(const IMBFileSpec *folderSpec)
{
    long result = 0;
    if (!MBFileSpecExists(folderSpec)) {
        MBString fullPath = folderSpec->GetPath();
        if (!CreateDirectoryA(fullPath.c_str(), 0))
            result = -2;
    }
    return result;
}

// 0x1003fab0 _MBCreateFile
long MBCreateFile(const IMBFileSpec *fileSpec)
{
    long result = 0;
    if (!MBFileSpecExists(fileSpec)) {
        MBString fullPath = fileSpec->GetPath();
        HANDLE file = CreateFileA(fullPath.c_str(), GENERIC_WRITE, 0, 0, CREATE_NEW,
                                  FILE_ATTRIBUTE_NORMAL, 0);
        if (file == INVALID_HANDLE_VALUE || !CloseHandle(file))
            result = -2;
    }
    return result;
}

// 0x1003fb50 _MBDeleteFile
long MBDeleteFile(const IMBFileSpec *fileSpec)
{
    long result = 0;
    if (MBFileSpecExists(fileSpec)) {
        MBString fullPath = fileSpec->GetPath();
        if (!DeleteFileA(fullPath.c_str()))
            result = -2;
    }
    return result;
}

// 0x1003fbd0 _MBOpenFile
void MBOpenFile(const IMBFileSpec *fileSpec, unsigned long openFlags, IMBStream **stream)
{
    MBOpenFileWithClass(MBIID_CMBWinFile, fileSpec, openFlags, stream);
}

// 0x1003fbf0 _MBOpenFileWithClass
void MBOpenFileWithClass(const MBGUID &classid, const IMBFileSpec *fileSpec, unsigned long openFlags,
                         IMBStream **stream)
{
    *stream = 0;
    MBComPtr<IMBFile> file(classid);
    if (file && file->Open(fileSpec, openFlags))
        file->QueryInterface(MBIID_IMBStream, (void **)stream);
}

// 0x1003fc70 _MBIsFileReadOnly
bool MBIsFileReadOnly(const IMBFileSpec *fileSpec)
{
    bool isReadOnly = false;
    if (MBIsFileSpecAFile(fileSpec)) {
        MBString fullPath = fileSpec->GetPath();
        unsigned long attributes = GetFileAttributesA(fullPath.c_str());
        if (attributes != 0xffffffff)
            isReadOnly = (attributes & FILE_ATTRIBUTE_READONLY) != 0;
    }
    return isReadOnly;
}

// EQUIV 0x1003fcf0 _MBSetFileReadOnly: reviewed: two branches go to duplicated copies of the same return tail
long MBSetFileReadOnly(const IMBFileSpec *fileSpec, bool isReadOnly)
{
    long result = 0;
    if (MBIsFileSpecAFile(fileSpec)) {
        MBString fullPath = fileSpec->GetPath();
        unsigned long attributes = GetFileAttributesA(fullPath.c_str());
        if (attributes != 0xffffffff) {
            if (isReadOnly)
                attributes |= FILE_ATTRIBUTE_READONLY;
            else
                attributes &= ~FILE_ATTRIBUTE_READONLY;
            if (!SetFileAttributesA(fullPath.c_str(), attributes))
                result = -2;
        } else
            result = -2;
        return result;
    }
    return -2;
}

// 0x1003fda0 ?_QueryInterface@?$MBComPtr@VIMBFile@@@@AAEJABQAVIMBUnknown@@@Z (MBComPtr template)


// Matched by this code:
// 0x1003fe20 _$E20
// 0x1003fe40 _$E19
