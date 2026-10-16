// MfFileProgress: the progress display of file loads (an imported static;
// signature from the decorated name). Stages as RuntimeLib passes them:
// 0 the file name, 3 begin, 2 end.
#ifndef RUNTIMELIB_MFFILEPROGRESS_H
#define RUNTIMELIB_MFFILEPROGRESS_H
#include "MfString.h"

class __declspec(dllimport) MfFileProgress {
public:
    static void SetProgress(int stage, MfString text);
};

#endif
