// MfLogger (mfutil.dll): the log window. Only what RuntimeLib calls.
#ifndef RUNTIMELIB_MFLOGGER_H
#define RUNTIMELIB_MFLOGGER_H
class MfString;

class __declspec(dllimport) MfLogger {
public:
    // level 1: a warning
    static void Log(int level, const MfString &source, const MfString &title, const MfString &text);
};

#endif
