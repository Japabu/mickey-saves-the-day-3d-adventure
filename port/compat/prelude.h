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


// libc++ decides its ABI from _WIN32 && _MSC_VER (both defined for the source) and would
// then expect the Microsoft runtime's headers (<new.h>, <vcruntime_exception.h>). Its
// configuration is read once, here, with the two hidden, so it sees plain macOS.
#ifdef __cplusplus
#pragma push_macro("_WIN32")
#pragma push_macro("_MSC_VER")
#undef _WIN32
#undef _MSC_VER
#include <cstddef>
#include <new>
#include <exception>
#include <typeinfo>
#pragma pop_macro("_MSC_VER")
#pragma pop_macro("_WIN32")
#endif


// Visual C++ binds temporaries to non-const references; the port's copies of such calls
// pass the temporary through this (see port/rewrite.py).
#ifdef __cplusplus
template <class T> inline T &port_lvalue(const T &t) { return const_cast<T &>(t); }
// VC6's vector iterators are element pointers; code that uses them as such gets these.
template <class V> inline typename V::value_type *port_begin(V &v) { return v.empty() ? 0 : &v[0]; }
template <class V> inline typename V::value_type *port_end(V &v) { return v.empty() ? 0 : &v[0] + v.size(); }
#endif

// Structured exception handling does not exist on macOS: a __try block is a try block,
// and its handler catches what RaiseException throws (platform); the filter expression
// is dropped.
#ifdef __cplusplus
#define __try try
#define __except(filter) catch (...)
#endif

// the Microsoft C runtime names the source uses (_stricmp, _snprintf, _isnan, ...)
#include <port_crt.h>
#endif
