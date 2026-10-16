#pragma once
// MfFileTime: a Win32 FILETIME with conversions and comparisons. Reconstructed
// (no PDB): like MfSystemTime it derives from the Win32 structure. The implicit
// copy constructor and copy assignment are exported too; the linker folded them
// with the FILETIME ones (identical code).
#include <windows.h>

class MfString;

class __declspec(dllexport) MfFileTime : public FILETIME {
public:
    MfFileTime();
    MfFileTime(const FILETIME &ft);
    MfFileTime(const SYSTEMTIME &st);
    MfFileTime &operator=(const FILETIME &ft);
    MfFileTime &operator=(const SYSTEMTIME &st);
    int operator==(const FILETIME &ft) const { return !CompareFileTime(this, &ft); }
    int operator!=(const FILETIME &ft) const { return CompareFileTime(this, &ft) != 0; }
    int operator<=(const FILETIME &ft) const { return CompareFileTime(this, &ft) <= 0; }
    int operator<(const FILETIME &ft) const { return CompareFileTime(this, &ft) < 0; }
    int operator>=(const FILETIME &ft) const { return CompareFileTime(this, &ft) >= 0; }
    int operator>(const FILETIME &ft) const { return CompareFileTime(this, &ft) > 0; }
    void Convert(SYSTEMTIME *st) const;
    BOOL SetFromFile(const MfString &path);
};
