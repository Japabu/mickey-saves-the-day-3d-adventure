// MBCom_api.h - MBCom.dll's exports: MBCOM_API exports a declaration when MBCom.dll is
// compiled (MBCOM_BUILD, src/options.txt) and imports it everywhere else.
#ifndef MBCOM_API
#ifdef MBCOM_BUILD
#define MBCOM_API __declspec(dllexport)
#else
#define MBCOM_API __declspec(dllimport)
#endif
#endif
