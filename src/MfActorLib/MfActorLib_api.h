// MfActorLib_api.h - MfActorLib.dll's exports: MFACTOR_API exports a declaration when MfActorLib.dll is
// compiled (MFACTOR_BUILD, src/options.txt) and imports it everywhere else.
#ifndef MFACTOR_API
#ifdef MFACTOR_BUILD
#define MFACTOR_API __declspec(dllexport)
#else
#define MFACTOR_API __declspec(dllimport)
#endif
#endif
