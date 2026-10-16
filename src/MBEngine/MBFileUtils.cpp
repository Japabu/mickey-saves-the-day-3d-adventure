// MBEngine.dll .\MBEngineRelease\MBFileUtils.obj
#include "MBFileUtils.h"

char kMBDirectorySeparator = '\\';
char kMBExtensionSeparator = '.';

// 0x1003fe50 _MBGetCurrentDirectory
bool MBGetCurrentDirectory(IMBFileSpec **path)
{
    char *fn;
    char buf[260];

    MBComCreateInstance(0, MBIID_CMBWinFileSpec, (IMBUnknown **)path);
    if (*path) {
        GetFullPathNameA(".", sizeof(buf), buf, &fn);
        (*path)->SetPath(buf);
        return true;
    }
    return false;
}

// 0x1003feb0 _MBGetDesktopDirectory
bool MBGetDesktopDirectory(IMBFileSpec **out)
{
    HKEY key;
    unsigned long dataSize;
    unsigned long dataType;
    char path[260];

    *out = 0;
    dataSize = sizeof(path);
    if (RegOpenKeyExA(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders", 0,
                      KEY_ALL_ACCESS, &key) == ERROR_SUCCESS) {
        if (RegQueryValueExA(key, "Desktop", 0, &dataType, (unsigned char *)path, &dataSize) == ERROR_SUCCESS) {
            // tests the terminator, not the last character, so the separator is always appended
            if (path[MBstrlen(path)] != '\\')
                MBstrcat(path, "\\");
            MBComCreateInstance(0, MBIID_CMBWinFileSpec, (IMBUnknown **)out);
            if (*out)
                (*out)->SetPath(path);
        }
        RegCloseKey(key);
    }
    return *out ? true : false;
}

// 0x1003ff80 ?MBSetCurrentDirectory@@YA_NV?$MBComPtr@VIMBFileSpec@@@@@Z
bool MBSetCurrentDirectory(MBComPtr<IMBFileSpec> spec)
{
    return SetCurrentDirectoryA(spec->GetPath().c_str()) != 0;
}

// 0x10040010 ?MBMakeDirectory@@YA_NV?$MBComPtr@VIMBFileSpec@@@@@Z
bool MBMakeDirectory(MBComPtr<IMBFileSpec> spec)
{
    bool ret = false;
    char buf[260];

    MBstrcpy(buf, spec->GetPath().c_str());
    char *sep = MBstrchr(buf, kMBDirectorySeparator);
    while (sep) {
        char c = *sep;
        *sep = 0;
        ret |= CreateDirectoryA(buf, 0) == TRUE;
        *sep = c;
        sep = MBstrchr(sep + 1, kMBDirectorySeparator);
    }
    ret |= CreateDirectoryA(buf, 0) == TRUE;
    spec->SetPath(spec->GetPath().c_str());
    return ret;
}

// 0x10040160 ?MBRemoveDirectory@@YA_NV?$MBComPtr@VIMBFileSpec@@@@_N@Z
bool MBRemoveDirectory(MBComPtr<IMBFileSpec> spec, bool remove_files)
{
    if (!MBIsFileSpecAFolder(spec))
        return false;
    MBVector<MBComPtr<IMBFileSpec> > files;
    if (remove_files) {
        MBGetFilesInDirectory(spec, files, 0xe, 0);
        for (MBVector<MBComPtr<IMBFileSpec> >::iterator it = files.begin(); it != files.end(); it++) {
            if (MBIsFileSpecAFolder(*it))
                MBRemoveDirectory(*it, true);
            else
                MBDeleteFile(*it);
        }
    }
    return RemoveDirectoryA(spec->GetPath().c_str()) == TRUE;
}

// 0x100402c0 ?GetChildrenHelper@@YAXABV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@AAV?$MBVector@V?$MBComPtr@VIMBFileSpec@@@@@@KJ@Z
void GetChildrenHelper(const MBString &root, MBVector<MBComPtr<IMBFileSpec> > &kids, unsigned long flags,
                       long recursionDepth)
{
    MBString path;
    path.reserve(root.size() + 16);
    path = root;
    path += "\\";
    unsigned long rootLength = path.size();
    path += "*.*";

    WIN32_FIND_DATAA findData;
    memset(&findData, 0, sizeof(findData));
    HANDLE hFind = FindFirstFileA(path.c_str(), &findData);
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if ((flags & 8) && !(findData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)) {
                if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                    if (findData.cFileName[0] != '.' && stricmp(findData.cFileName, "_hide")) {
                        path.resize(rootLength);
                        path += findData.cFileName;
                        if (flags & 2) {
                            MBComPtr<IMBFileSpec> newSpec(MBIID_CMBWinFileSpec);
                            newSpec->SetPath(path.c_str());
                            kids.push_back(newSpec);
                        }
                        if ((flags & 1) && recursionDepth)
                            GetChildrenHelper(path, kids, flags, recursionDepth - 1);
                    }
                } else if (flags & 4) {
                    path.resize(rootLength);
                    path += findData.cFileName;
                    MBComPtr<IMBFileSpec> newSpec(MBIID_CMBWinFileSpec);
                    newSpec->SetPath(path.c_str());
                    kids.push_back(newSpec);
                }
            }
        } while (FindNextFileA(hFind, &findData));
        FindClose(hFind);
    }
}

// 0x100405e0 _MBGetFilesInDirectory
void MBGetFilesInDirectory(const IMBFileSpec *directory, MBVector<MBComPtr<IMBFileSpec> > &kids,
                           unsigned long flags, long recursionDepth)
{
    if (MBFileSpecExists(directory) && MBIsFileSpecAFolder(directory))
        GetChildrenHelper(directory->GetPath(), kids, flags, recursionDepth);
}

// 0x10040660 ?MBCopyFile@@YA_NV?$MBComPtr@VIMBFileSpec@@@@0@Z
bool MBCopyFile(MBComPtr<IMBFileSpec> source, MBComPtr<IMBFileSpec> dest)
{
    MBComPtr<IMBFileSpec> real_dest(MBIID_IMBFileSpec);
    if (MBIsFileSpecAFolder(source))
        return false;
    if (MBIsFileSpecAFolder(dest))
        real_dest->SetPath(dest, source->GetName().c_str());
    else
        real_dest = dest;
    if (CopyFileA(source->GetPath().c_str(), real_dest->GetPath().c_str(), FALSE)) {
        MBSetFileReadOnly(real_dest, false);
        return true;
    }
    return false;
}

// 0x100407e0 _MBMakeCDPath
bool MBMakeCDPath(const char *partialPath, IMBFileSpec **spec)
{
    char drive[4];
    char buf[260];

    *spec = 0;
    MBstrcpy(&buf[2], partialPath);
    buf[1] = ':';
    for (char letter = 'a'; letter <= 'z'; letter++) {
        drive[0] = letter;
        drive[1] = ':';
        drive[2] = '\\';
        drive[3] = 0;
        if (GetDriveTypeA(drive) == DRIVE_CDROM) {
            buf[0] = letter;
            MBComCreateInstance(0, MBIID_CMBWinFileSpec, (IMBUnknown **)spec);
            if (*spec) {
                (*spec)->SetPath(buf);
                if (MBFileSpecExists(*spec))
                    return true;
                (*spec)->Release();
                *spec = 0;
            }
        }
    }
    return false;
}

// Matched by this code:
// 0x100408a0 ?append@?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@QAEAAV12@ID@Z
// 0x10040970 ?insert@?$vector@V?$MBComPtr@VIMBFileSpec@@@@V?$MBAllocator@V?$MBComPtr@VIMBFileSpec@@@@@@@std@@QAEXPAV?$MBComPtr@VIMBFileSpec@@@@IABV3@@Z
// 0x10040c00 ?_Construct@std@@YAXPAV?$MBComPtr@VIMBFileSpec@@@@ABV2@@Z
// 0x10040c20 $E20
// 0x10040c40 $E19
// (inline MBComPtr<IMBFileSpec> members; the linker kept SimpleNode.obj's identical copies)
