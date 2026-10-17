// mfutil_api.h - mfutil.dll's exports: MFUTIL_API exports a declaration when mfutil.dll is
// compiled (MFUTIL_BUILD, src/options.txt) and imports it everywhere else.
#ifndef MFUTIL_API
#ifdef MFUTIL_BUILD
#define MFUTIL_API __declspec(dllexport)
#else
#define MFUTIL_API __declspec(dllimport)
#endif
#endif
