// mbruntime_api.h - mbruntime.dll's exports: MBRUNTIME_API exports a declaration when mbruntime.dll is
// compiled (MBRUNTIME_BUILD, src/options.txt) and imports it everywhere else.
#ifndef MBRUNTIME_API
#ifdef MBRUNTIME_BUILD
#define MBRUNTIME_API __declspec(dllexport)
#else
#define MBRUNTIME_API __declspec(dllimport)
#endif
#endif
