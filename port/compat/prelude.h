// prelude.h - force-included before every source file of the port (clang -include).
//
// The decompiled source is written for Visual C++ 5.0/6.0 on 32-bit Windows. This
// file maps the Microsoft language extensions it uses onto clang on macOS; the
// Windows, DirectX and runtime APIs come from the headers in compat/include.
#ifndef PORT_PRELUDE_H
#define PORT_PRELUDE_H

// Each original DLL is built as a dylib with hidden symbols; what the DLL exported is
// exported, an import is an ordinary declaration.
#define __declspec(x) PORT_DECLSPEC_##x
#define PORT_DECLSPEC_dllexport __attribute__((visibility("default")))
#define PORT_DECLSPEC_dllimport
#define PORT_DECLSPEC_export __attribute__((visibility("default")))
#define PORT_DECLSPEC_noreturn __attribute__((noreturn))
#define PORT_DECLSPEC_novtable
#define PORT_DECLSPEC_selectany __attribute__((weak))
#define PORT_DECLSPEC_naked
#define PORT_DECLSPEC_thread __thread
#define PORT_DECLSPEC_noinline __attribute__((noinline))

// Calling conventions do not exist on arm64.
#define __stdcall
#define __cdecl
#define __fastcall
#define __pascal
#define _stdcall
#define _cdecl
#define _fastcall
#define __export

#ifndef __forceinline
#define __forceinline inline __attribute__((always_inline))
#endif

// The 32-bit Windows integer model: these names are always 32 bits wide, unlike the
// C `long` of macOS (64 bits). Windows types are defined on top of them.
typedef int port_i32;
typedef unsigned int port_u32;

#endif
