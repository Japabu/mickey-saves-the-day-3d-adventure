// PiccoloConsole_api.h - PiccoloConsole.dll's exports: PICCOLOCONSOLE_API exports a declaration when PiccoloConsole.dll is
// compiled (PICCOLOCONSOLE_BUILD, src/options.txt) and imports it everywhere else.
#ifndef PICCOLOCONSOLE_API
#ifdef PICCOLOCONSOLE_BUILD
#define PICCOLOCONSOLE_API __declspec(dllexport)
#else
#define PICCOLOCONSOLE_API __declspec(dllimport)
#endif
#endif
