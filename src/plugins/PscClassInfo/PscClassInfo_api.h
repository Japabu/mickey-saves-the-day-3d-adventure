// PscClassInfo_api.h - plugins/PscClassInfo.dll's exports: PSCCLASSINFO_API exports a declaration when plugins/PscClassInfo.dll is
// compiled (PSCCLASSINFO_BUILD, src/options.txt) and imports it everywhere else.
#ifndef PSCCLASSINFO_API
#ifdef PSCCLASSINFO_BUILD
#define PSCCLASSINFO_API __declspec(dllexport)
#else
#define PSCCLASSINFO_API __declspec(dllimport)
#endif
#endif
