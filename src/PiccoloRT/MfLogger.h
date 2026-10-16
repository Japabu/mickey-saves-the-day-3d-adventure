#pragma once
class MfString;
class MfStringList;

// RuntimeLib: the message log.
class __declspec(dllimport) MfLogger {
public:
    MfLogger();
    virtual ~MfLogger();
    static void Log(int level, const MfString &source, const MfString &title, const MfString &text);
    static void Log(int level, const MfString &source, const MfString &title, const MfStringList &lines);
};
