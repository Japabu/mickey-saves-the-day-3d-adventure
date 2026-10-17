// MfScDiagnostics: the diagnostic output of the runtime.
#pragma once
#ifndef PICCOLORT_API
#define PICCOLORT_API __declspec(dllimport)
#endif
#include "MfDblList.h"
class MfScDiagnosticCallBack;
class MfString;

// Diagnostic output of the runtime: messages up to the current level go to
// every registered MfScDiagnosticCallBack; level 5 always gets through.
class PICCOLORT_API MfScDiagnostics {
public:
    enum { kAlways = 5 };
    static int __cdecl Active(int);
    // like Active, without the kAlways exception
    static int IsOn(int level) { return plOutputList.GetCount() > 0 && level <= iLevel; }
    static int __cdecl GetLevel();
    static void __cdecl SetLevel(int);
    static void __cdecl AddOutput(MfScDiagnosticCallBack *);
    static int __cdecl RemoveOutput(MfScDiagnosticCallBack *);
    static void __cdecl ClearAllOutput();
    static void __cdecl Print(int, MfString const &);
    static void __cdecl PrintNl(int, MfString const &);
private:
    MfScDiagnostics() {}
    ~MfScDiagnostics() {}
    static int iLevel;
    static MfDblList plOutputList;
};
