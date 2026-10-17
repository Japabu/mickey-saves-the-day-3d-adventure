// PiccoloComp_api.h - PiccoloComp.dll's exports: PICCOLOCOMP_API exports a declaration when PiccoloComp.dll is
// compiled (PICCOLOCOMP_BUILD, src/options.txt) and imports it everywhere else.
#ifndef PICCOLOCOMP_API
#ifdef PICCOLOCOMP_BUILD
#define PICCOLOCOMP_API __declspec(dllexport)
#else
#define PICCOLOCOMP_API __declspec(dllimport)
#endif
#endif
