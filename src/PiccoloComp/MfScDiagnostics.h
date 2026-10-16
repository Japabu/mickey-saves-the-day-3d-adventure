#pragma once
#include "MfDblList.h"
class MfScDiagnosticCallBack;
class MfString;

// PiccoloRT.dll: diagnostic output of the runtime. Active() is inline here:
// the compiler reads the output list and the level itself.
class __declspec(dllimport) MfScDiagnostics {
public:
    enum { kAlways = 5 };
    static int Active(int level)
    {
        if (plOutputList.GetCount() > 0 && (level == kAlways || level <= iLevel))
            return 1;
        return 0;
    }
    static int GetLevel();
    static void SetLevel(int);
    static void AddOutput(MfScDiagnosticCallBack *);
    static int RemoveOutput(MfScDiagnosticCallBack *);
    static void ClearAllOutput();
    static void Print(int, MfString const &);
    static void PrintNl(int, MfString const &);
private:
    static int iLevel;
    static MfDblList plOutputList;
};
