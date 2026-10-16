// PscCompiler.dll 0x10001000-0x10001050: DLL entry point and Psc* exports.
#ifndef PSCCOMPILER_10001000_H
#define PSCCOMPILER_10001000_H

class MfNullString;

class __declspec(dllimport) MfString {
public:
    MfString &operator=(const MfNullString &);
};

extern __declspec(dllimport) MfNullString NULL_STRING;

class __declspec(dllimport) MfPcCompiler {
public:
    static void Initialize();
    static void Shutdown();
};

#endif
