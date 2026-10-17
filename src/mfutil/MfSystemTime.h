// MfSystemTime: a Win32 SYSTEMTIME that knows whether it holds local time or UTC.
// Declaration from the SteamboatApp PDB.
#ifndef MFUTIL_MFSYSTEMTIME_H
#define MFUTIL_MFSYSTEMTIME_H

#ifndef MFUTIL_API
#define MFUTIL_API __declspec(dllimport)
#endif
#include <windows.h>
#include <time.h>
#include "MfTime.h"

class MFUTIL_API MfSystemTime : public SYSTEMTIME {
    enum {
        STM_LOCAL = 0,
        STM_UNIVERSAL = 1
    };
public:
    MfSystemTime(int mode = STM_LOCAL);
    MfSystemTime(const MfSystemTime &t, int mode = STM_LOCAL);
    MfSystemTime(const FILETIME &ft, int mode);
    MfSystemTime(const tm &t, int mode);
    MfSystemTime(const MfTime &t, int mode);
    operator MfTime() const;
    void Convert(tm *t) const;
    void Convert(FILETIME *ft) const;
    void GetLocalTime() { ::GetLocalTime(this); }
    void GetSystemTime() { ::GetSystemTime(this); }
    MfSystemTime &operator=(const MfTime &t);
    MfSystemTime &operator=(const tm &t);
    MfSystemTime &operator=(const FILETIME &ft);
    MfSystemTime &operator=(const MfSystemTime &t);
private:
    int iMode;  // +0x10
};

#endif
