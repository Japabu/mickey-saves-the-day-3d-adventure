// PscContentInfo_api.h - plugins/PscContentInfo.dll's exports: PSCCONTENTINFO_API exports a declaration when plugins/PscContentInfo.dll is
// compiled (PSCCONTENTINFO_BUILD, src/options.txt) and imports it everywhere else.
#ifndef PSCCONTENTINFO_API
#ifdef PSCCONTENTINFO_BUILD
#define PSCCONTENTINFO_API __declspec(dllexport)
#else
#define PSCCONTENTINFO_API __declspec(dllimport)
#endif
#endif
