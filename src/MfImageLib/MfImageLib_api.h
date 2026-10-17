// MfImageLib_api.h - MfImageLib.dll's exports: MFIMAGE_API exports a declaration when MfImageLib.dll is
// compiled (MFIMAGE_BUILD, src/options.txt) and imports it everywhere else.
#ifndef MFIMAGE_API
#ifdef MFIMAGE_BUILD
#define MFIMAGE_API __declspec(dllexport)
#else
#define MFIMAGE_API __declspec(dllimport)
#endif
#endif
