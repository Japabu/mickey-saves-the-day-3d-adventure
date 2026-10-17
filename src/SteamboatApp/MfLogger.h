#pragma once
// MfLogger.h: Motivate's log (mfutil.dll); the game only switches its window output.
// own copy: the game calls mfutil through its import thunks (no dllimport) and emits its own copies of
// inline members; the owner's header declares the class MFUTIL_API, dllimport here,
// and it opens mfutil's MfString.h (see MfString.h).
class MfLogger {
public:
    static void __cdecl EnableWindowOutput(int);
};
