// PiccoloRT_api.h - PiccoloRT.dll's exports: PICCOLORT_API exports a declaration when PiccoloRT.dll is
// compiled (PICCOLORT_BUILD, src/options.txt) and imports it everywhere else.
#ifndef PICCOLORT_API
#ifdef PICCOLORT_BUILD
#define PICCOLORT_API __declspec(dllexport)
#else
#define PICCOLORT_API __declspec(dllimport)
#endif
#endif
