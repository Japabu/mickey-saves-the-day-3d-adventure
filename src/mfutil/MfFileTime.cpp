// MfFileTime (mfutil.dll 0x10003b50-0x10003c20)
#include "MfString.h"
#include "MfFileTime.h"

// 0x10003b50 ??0MfFileTime@@QAE@XZ
MfFileTime::MfFileTime()
{
    dwLowDateTime = 0;
    dwHighDateTime = 0;
}

// 0x10003b60 ??0MfFileTime@@QAE@ABU_FILETIME@@@Z (SetFromFile calls it as operator=; folded with ??0MfFileTime@@QAE@ABV0@@Z,
// ??4MfFileTime@@QAEAAV0@ABU_FILETIME@@@Z, ??4MfFileTime@@QAEAAV0@ABV0@@Z)
MfFileTime::MfFileTime(const FILETIME &ft)
{
    FILETIME::operator=(ft);
}

MfFileTime &MfFileTime::operator=(const FILETIME &ft)
{
    FILETIME::operator=(ft);
    return *this;
}

// 0x10003b80 ??0MfFileTime@@QAE@ABU_SYSTEMTIME@@@Z (folded with
// ??4MfFileTime@@QAEAAV0@ABU_SYSTEMTIME@@@Z)
MfFileTime::MfFileTime(const SYSTEMTIME &st)
{
    SystemTimeToFileTime(&st, this);
}

MfFileTime &MfFileTime::operator=(const SYSTEMTIME &st)
{
    SystemTimeToFileTime(&st, this);
    return *this;
}

// 0x10003ba0 ?Convert@MfFileTime@@QBEXPAU_SYSTEMTIME@@@Z
void MfFileTime::Convert(SYSTEMTIME *st) const
{
    FileTimeToSystemTime(this, st);
}

// 0x10003bb0 ?SetFromFile@MfFileTime@@QAEHABVMfString@@@Z
BOOL MfFileTime::SetFromFile(const MfString &path)
{
    HANDLE h = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING,
                           FILE_ATTRIBUTE_NORMAL, 0);
    if (h == INVALID_HANDLE_VALUE)
        return FALSE;
    BY_HANDLE_FILE_INFORMATION info;
    GetFileInformationByHandle(h, &info);
    CloseHandle(h);
    *this = info.ftLastWriteTime;
    return TRUE;
}
