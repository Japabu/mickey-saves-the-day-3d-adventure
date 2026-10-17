// mflinalg_api.h - mflinalg.dll's exports: MFLINALG_API exports a declaration when mflinalg.dll is
// compiled (MFLINALG_BUILD, src/options.txt) and imports it everywhere else.
#ifndef MFLINALG_API
#ifdef MFLINALG_BUILD
#define MFLINALG_API __declspec(dllexport)
#else
#define MFLINALG_API __declspec(dllimport)
#endif
#endif
