#pragma once
// MfFileProgress: the progress of file loading (a state and a name) reported
// to a callback.
#include "MfActorLib_api.h"
#include "MfString.h"
#include "MfCriticalSection.h"

class MFACTOR_API MfFileProgress {
public:
    static void GetProgress(int &, MfString &);
    static void SetProgress(int, MfString);
    static void SetProgressCB(void (*)());
    static void UpdateProgress();
protected:
    static MfCriticalSection lock;
    static void (*callback)();
};
