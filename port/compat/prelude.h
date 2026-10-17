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
#include <cstring>
#include <iosfwd>
#pragma pop_macro("_MSC_VER")
#pragma pop_macro("_WIN32")

// VC6's char_traits worked for any character type; the game's 16-bit strings
// (MBString16, basic_string<unsigned short>) need it for unsigned short, which current
// libc++ no longer gives (the macOS SDK's still had a deprecated generic one).
namespace std {
template <> struct char_traits<unsigned short> {
    typedef unsigned short char_type;
    typedef unsigned int int_type;
    typedef streamoff off_type;
    typedef streampos pos_type;
    typedef mbstate_t state_type;
    static void assign(char_type &a, const char_type &b) noexcept { a = b; }
    static bool eq(char_type a, char_type b) noexcept { return a == b; }
    static bool lt(char_type a, char_type b) noexcept { return a < b; }
    static int compare(const char_type *a, const char_type *b, size_t n)
    {
        for (; n; n--, a++, b++)
            if (*a != *b)
                return *a < *b ? -1 : 1;
        return 0;
    }
    static size_t length(const char_type *s)
    {
        size_t n = 0;
        while (s[n])
            n++;
        return n;
    }
    static const char_type *find(const char_type *s, size_t n, const char_type &c)
    {
        for (; n; n--, s++)
            if (*s == c)
                return s;
        return 0;
    }
    static char_type *move(char_type *d, const char_type *s, size_t n)
    {
        return n ? (char_type *)memmove(d, s, n * sizeof(char_type)) : d;
    }
    static char_type *copy(char_type *d, const char_type *s, size_t n)
    {
        return n ? (char_type *)memcpy(d, s, n * sizeof(char_type)) : d;
    }
    static char_type *assign(char_type *s, size_t n, char_type c)
    {
        for (size_t i = 0; i < n; i++)
            s[i] = c;
        return s;
    }
    static int_type not_eof(int_type c) noexcept { return eq_int_type(c, eof()) ? 0 : c; }
    static char_type to_char_type(int_type c) noexcept { return (char_type)c; }
    static int_type to_int_type(char_type c) noexcept { return c; }
    static bool eq_int_type(int_type a, int_type b) noexcept { return a == b; }
    static int_type eof() noexcept { return (int_type)-1; }
};
}  // namespace std
#endif


// Visual C++ binds temporaries to non-const references; the port's copies of such calls
// pass the temporary through this (see port/rewrite.py).
#ifdef __cplusplus
template <class T> inline T &port_lvalue(const T &t) { return const_cast<T &>(t); }
// VC6's vector iterators are element pointers; code that uses them as such gets these.
template <class V> inline typename V::value_type *port_begin(V &v) { return v.empty() ? 0 : &v[0]; }
template <class V> inline typename V::value_type *port_end(V &v) { return v.empty() ? 0 : &v[0] + v.size(); }
#endif

// The source's classes have members named like Win32 functions that <windows.h> maps
// onto their ANSI versions; the original binaries carry the mapped names
// (?GetClassNameA@MfActor@@...), and every unit saw <windows.h>. The mapping is made
// here for every unit, so that all of them agree on the member names (and on the
// vtable slots of virtual ones: an overrider spelled without the A would not override).
// The names are those the source uses as member functions.
#define GetClassName GetClassNameA
#define SendMessage SendMessageA
#define CreateFont CreateFontA
#define DrawText DrawTextA
#define FindResource FindResourceA
#define GetMessage GetMessageA
#define GetObject GetObjectA
#define GetProfileString GetProfileStringA
#define IsDialogMessage IsDialogMessageA
#define LoadBitmap LoadBitmapA
#define LoadString LoadStringA
#define PlaySound PlaySoundA
#define PostThreadMessage PostThreadMessageA
#define RegisterClass RegisterClassA
#define WriteProfileString WriteProfileStringA

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
