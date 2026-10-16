#pragma once
#include <windows.h>
#include "MfString.h"

// A plugin or extension DLL of the runtime: exports _PscLoad@0,
// _PscInitialize@8, _PscShutdown@4 and _PscUnload@0.
class MfScPiccoloDll {
public:
    MfScPiccoloDll(const MfString &name);
    ~MfScPiccoloDll();
    // loads the DLL and initializes it with the callback; errors get the reason
    // why not, *initialized whether PscInitialize succeeded
    int Load(int (*callback)(const MfString &, const MfString &, MfString *), MfString &errors,
             int *initialized);
    // shuts the DLL down and calls its PscUnload
    int Unload(MfString &errors);
    // an export of the DLL, 0 if it is not loaded
    FARPROC GetProc(const MfString &name);

    MfString mName;                 // +0x00
    HMODULE mModule;                // +0x04
};
