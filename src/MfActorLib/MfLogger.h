#pragma once
// MfLogger, imported from mfutil.dll (see mfutil/MfLogger.h).
#include "MfString.h"
class MfStringList;

enum { MF_LOG_INFORMATION, MF_LOG_WARNING, MF_LOG_ERROR };

class __declspec(dllimport) MfLogger {
public:
    static void Log(int, const MfString &, const MfString &, const MfString &);
    static void Log(int, const MfString &, const MfString &, const MfStringList &);
    static void Log(int, const MfString &, const MfString &, int, ...);
};
