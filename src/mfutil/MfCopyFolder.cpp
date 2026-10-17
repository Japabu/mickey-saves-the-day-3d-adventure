// mfutil.dll 0x10001000-0x10002100: MfCopyFolder, preceded by the out-of-line
// copies of the inline members of the exported classes in MfString.h
// (0x10001000-0x10001820; this is the first object of the DLL).
#include "MfCopyFolder.h"

// 0x10001000 ??0MfCallBack@@QAE@XZ                      (inline, header)
// 0x10001010 ?CallBack@MfCallBack@@UAEHPAX@Z            (inline, header)
// 0x10001020 ?CallBack@MfCallBack@@UAEHPAX0@Z           (inline, header)
// 0x10001030 ?CallBack@MfCallBack@@UAEHPAX00@Z          (inline, header)
// 0x10001040 ??0MfCallBack@@QAE@ABV0@@Z                 (implicit)
// 0x10001050 ??_EMfCallBack@@UAEPAXI@Z                  (compiler-generated)
// 0x100010a0 ??0MfStringSize@@QAE@H@Z                   (inline, header)
// 0x100010b0 ??0MfStringCursor@@QAE@H@Z                 (inline, header)
// 0x100010d0 ??0MfStringCursor@@QAE@ABV0@@Z             (implicit)
// 0x100010f0 ??4MfStringCursor@@QAEAAV0@ABV0@@Z         (inline, header)
// 0x10001110 ?Reset@MfStringCursor@@QAEXXZ              (inline, header)
// 0x10001120 ?OutsideString@MfStringCursor@@QBEHXZ      (inline, header)
// 0x10001130 ??_FMfStringCursor@@QAEXXZ                 (compiler-generated)
// 0x10001150 ?IncrementCount@MfStringRef@@QAEHXZ        (inline, header)
// 0x10001160 ?DecrementCount@MfStringRef@@QAEHXZ        (inline, header)
// 0x10001170 ??0MfStringRef@@QAE@XZ                     (inline, header)
// 0x100011a0 ??_EMfStringRef@@UAEPAXI@Z                 (compiler-generated)
// 0x10001200 ??0MfStringRef@@QAE@PBD@Z                  (inline, header)
// 0x10001230 ??0MfStringRef@@QAE@PBG@Z                  (inline, header)
// 0x10001260 ??0MfStringRef@@QAE@PBDH@Z                 (inline, header)
// 0x100012b0 ??0MfStringRef@@QAE@ABV0@@Z                (inline, header)
// 0x100012e0 ??0MfStringRef@@QAE@H@Z                    (inline, header)
// 0x10001320 ??1MfStringRef@@UAE@XZ                     (inline, header)
// 0x10001340 ?CopyOnWrite@MfStringRef@@QAEPAV1@XZ       (inline, header)
// 0x100013b0 ?CopyToInternal@MfStringRef@@QAEXPBG@Z     (inline, header)
// 0x100013e0 ?CopyToInternal@MfStringRef@@QAEXPBD@Z     (inline, header)
// 0x10001410 ?AppendToInternal@MfStringRef@@QAEXPBGH@Z  (inline, header)
// 0x10001440 ?AppendToInternal@MfStringRef@@QAEXPBDH@Z  (inline, header)
// 0x10001480-0x100014f0 ??YMfString@@...                (inline, header)
// 0x10001510 ??AMfString@@QBEDH@Z                       (inline, header)
// 0x10001520 ??0MfStringTokenizerCallBack@@QAE@XZ       (inline, header)
// 0x10001530 ??1MfStringTokenizerCallBack@@UAE@XZ       (inline, header)
// 0x10001540 ?CallBack@MfStringTokenizerCallBack@@UAEHABVMfString@@AAV2@@Z (inline, header)
// 0x10001550 ??0MfStringTokenizerCallBack@@QAE@ABV0@@Z  (implicit)
// 0x10001560 ??_EMfStringTokenizerCallBack@@UAEPAXI@Z   (compiler-generated)
// EQUIV 0x100015b0 ?CopyOnWrite@MfString@@AAEXH@Z: reviewed: the constant 1 is kept in ebx instead of immediates; same compares, stores and calls
// 0x10001690 ?Dereference@MfString@@AAEXXZ              (inline, header)
// 0x100016c0-0x10001820 MfString inline members        (inline, header)
// 0x10001830 ??0MfCopyFolder@@QAE@ABV0@@Z               (implicit)
// 0x100018a0 ??4MfCopyFolder@@QAEAAV0@ABV0@@Z           (implicit)
// 0x100018e0 ??_EMfCopyFolder@@UAEPAXI@Z                (compiler-generated)

// 0x10001930 ??0MfCopyFolder@@QAE@ABVMfString@@00@Z
MfCopyFolder::MfCopyFolder(const MfString &source, const MfString &folder, const MfString &dest)
    : msSource(source), msFolder(folder), msDest(dest)
{
}

// 0x100019a0 ??1MfCopyFolder@@UAE@XZ
MfCopyFolder::~MfCopyFolder()
{
}

// 0x10001a00 ?Exists@MfCopyFolder@@SAHABVMfString@@@Z
int MfCopyFolder::Exists(const MfString &path)
{
    return GetFileAttributesA(path) != 0xFFFFFFFF;
}

// 0x10001a20 ?CreateFolder@MfCopyFolder@@SAHABVMfString@@@Z
int MfCopyFolder::CreateFolder(const MfString &path)
{
    MfString full(path);
    if (full[full.Length() - 1] != '\\')
        full << '\\';
    if (Exists(full))
        return 1;

    int i = 0;
    MfString sep("/");
    while (i >= 0) {
        i = full.HaveCharactersInSet(sep, i);
        if (i >= 0) {
            full.SetCharacter(i, '\\');
            i++;
        }
    }

    sep = "\\";
    i = full.HaveCharactersInSet(sep, 0);
    if (i == 0 || full[i - 1] == ':')
        i = full.HaveCharactersInSet(sep, i + 1);

    MfString part(full);
    part.Truncate(i);
    do {
        if (!Exists(part) && !CreateDirectoryA(part, NULL))
            return 0;
        part = full;
        i = full.HaveCharactersInSet(sep, i + 1);
        part.Truncate(i);
    } while (i >= 0);
    return 1;
}

// EQUIV 0x10001de0 ?Copy@MfCopyFolder@@QAEHH@Z: reviewed: only the stack slots of the MfString locals differ (a consistent slot mapping, also in the EH funclets)
int MfCopyFolder::Copy(int bSilent)
{
    MfString dest(msDest);
    MfString source(msSource);
    dest << msFolder;
    source << msFolder;

    if (!Exists(dest) && !CreateFolder(dest))
        return 0;

    if (!Exists(source)) {
        if (!bSilent) {
            MfString msg("The source directory doesn't exist :\n");
            msg << source;
            MessageBoxA(NULL, msg, NULL, MB_ICONERROR);
        }
        return bSilent;
    }

    MfString pattern;
    WIN32_FIND_DATAA fd;
    pattern << source << "\\*.*";
    HANDLE h = FindFirstFileA(pattern, &fd);
    if (h == INVALID_HANDLE_VALUE) {
        MessageBoxA(NULL, "No files in source folder!", NULL, MB_ICONERROR);
    } else {
        MfString from;
        MfString to;
        BOOL bMore = TRUE;
        while (bMore == TRUE) {
            if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                from = source;
                from << "\\" << fd.cFileName;
                to = dest;
                to << "\\" << fd.cFileName;
                if (!CopyFileA(from, to, FALSE)) {
                    MfString msg("Error copying the file: \n");
                    msg << from;
                    MessageBoxA(NULL, msg, NULL, MB_ICONERROR);
                }
            }
            bMore = FindNextFileA(h, &fd);
        }
    }
    return 1;
}


// Matched by this code:
// 0x10001480 ??YMfString@@QAEAAV0@ABV0@@Z
// 0x10001490 ??YMfString@@QAEAAV0@PBD@Z
// 0x100014a0 ??YMfString@@QAEAAV0@PBG@Z
// 0x100014b0 ??YMfString@@QAEAAV0@D@Z
// 0x100014c0 ??YMfString@@QAEAAV0@G@Z
// 0x100014d0 ??YMfString@@QAEAAV0@H@Z
// 0x100014e0 ??YMfString@@QAEAAV0@M@Z
// 0x100014f0 ??YMfString@@QAEAAV0@N@Z
// 0x100016c0 ?Length@MfString@@QBEHXZ
// 0x100016d0 ?MaxLength@MfString@@QBEHXZ
// 0x100016e0 ??BMfString@@QBEHXZ
// 0x100016f0 ??BMfString@@QBEJXZ
// 0x10001700 ?ConvertToUpper@MfString@@QAEXXZ
// 0x10001720 ?ConvertToLower@MfString@@QAEXXZ
// 0x10001740 ?Truncate@MfString@@QAEXH@Z
// 0x10001760 ?SetCharacter@MfString@@QAEHHD@Z
// 0x10001780 ?StripWhiteSpace@MfString@@QAEXXZ
// 0x100017a0 ?NoLeadingWhiteSpace@MfString@@QAEXXZ
// 0x100017c0 ?NoTrailingWhiteSpace@MfString@@QAEXXZ
// 0x100017e0 ?DuplicateUnicode@MfString@@QBEPAGXZ
// 0x100017f0 ?DuplicateAscii@MfString@@QBEPADXZ
// 0x10001800 ?ReadFromFile@MfString@@QAEHABV1@@Z
// 0x10001820 ?WriteToFile@MfString@@QBEHABV1@@Z
// plugins/pscdebugger.dll 0x10003c40 ?IncrementCount@MfStringRef@@QAEHXZ
// runtimelib.dll 0x1005d2a0 ?IncrementCount@MfStringRef@@QAEHXZ