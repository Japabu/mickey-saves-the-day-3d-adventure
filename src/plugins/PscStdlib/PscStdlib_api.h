// PscStdlib_api.h - plugins/PscStdlib.dll's exports: PSCSTDLIB_API exports a declaration when plugins/PscStdlib.dll is
// compiled (PSCSTDLIB_BUILD, src/options.txt) and imports it everywhere else.
#ifndef PSCSTDLIB_API
#ifdef PSCSTDLIB_BUILD
#define PSCSTDLIB_API __declspec(dllexport)
#else
#define PSCSTDLIB_API __declspec(dllimport)
#endif
#endif
