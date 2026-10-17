// MfPoLibrary_api.h - MfPoLibrary.dll's exports: MFPO_API exports a declaration when MfPoLibrary.dll is
// compiled (MFPO_BUILD, src/options.txt) and imports it everywhere else.
#ifndef MFPO_API
#ifdef MFPO_BUILD
#define MFPO_API __declspec(dllexport)
#else
#define MFPO_API __declspec(dllimport)
#endif
#endif
