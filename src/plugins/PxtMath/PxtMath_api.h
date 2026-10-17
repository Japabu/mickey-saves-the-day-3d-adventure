// PxtMath_api.h - plugins/PxtMath.dll's exports: PXTMATH_API exports a declaration when plugins/PxtMath.dll is
// compiled (PXTMATH_BUILD, src/options.txt) and imports it everywhere else.
#ifndef PXTMATH_API
#ifdef PXTMATH_BUILD
#define PXTMATH_API __declspec(dllexport)
#else
#define PXTMATH_API __declspec(dllimport)
#endif
#endif
