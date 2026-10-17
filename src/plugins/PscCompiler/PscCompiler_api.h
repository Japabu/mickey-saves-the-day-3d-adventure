// PscCompiler_api.h - plugins/PscCompiler.dll's exports: PSCCOMPILER_API exports a declaration when plugins/PscCompiler.dll is
// compiled (PSCCOMPILER_BUILD, src/options.txt) and imports it everywhere else.
#ifndef PSCCOMPILER_API
#ifdef PSCCOMPILER_BUILD
#define PSCCOMPILER_API __declspec(dllexport)
#else
#define PSCCOMPILER_API __declspec(dllimport)
#endif
#endif
