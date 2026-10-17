// MfTimer (mfutil.dll): millisecond timer based on GetTickCount.
// Reconstructed declaration (no PDB): layout from the code, signatures from the export names.
#ifndef MFUTIL_MFTIMER_H
#define MFUTIL_MFTIMER_H

#include <windows.h>

#include "mfutil_api.h"

// size 4; implicit operator= (COMDAT-folded with MfLock/MfStringSize)
class MFUTIL_API MfTimer {
public:
    MfTimer();
    void reset();
    DWORD time();
private:
    DWORD m_start; // +0
};

#endif
