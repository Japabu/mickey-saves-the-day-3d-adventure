// PscDebugger_api.h - plugins/PscDebugger.dll's exports: PSCDEBUGGER_API exports a declaration when plugins/PscDebugger.dll is
// compiled (PSCDEBUGGER_BUILD, src/options.txt) and imports it everywhere else.
#ifndef PSCDEBUGGER_API
#ifdef PSCDEBUGGER_BUILD
#define PSCDEBUGGER_API __declspec(dllexport)
#else
#define PSCDEBUGGER_API __declspec(dllimport)
#endif
#endif
