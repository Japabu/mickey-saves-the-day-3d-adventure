// mbi18n.h - declarations for MBEngine.dll mbi18n.obj
#ifndef MBI18N_H
#define MBI18N_H

#include <windows.h>
#include <imm.h>
#include "MBScreenUtils.h"
#include <string>
extern "C" __declspec(dllimport) void *__cdecl MBmalloc_release(unsigned int size);
extern "C" __declspec(dllimport) void __cdecl MBfree_release(void *p);

inline void *__cdecl operator new(unsigned int size) { return MBmalloc_release(size); }
inline void __cdecl operator delete(void *p) { if (p) MBfree_release(p); }

template <class T> class MBAllocator : public std::allocator<T> {
public:
    T *allocate(unsigned long n, const void *) { return (T *)MBmalloc_release(n * sizeof(T)); }
    char *_Charalloc(unsigned long n) { return (char *)MBmalloc_release(n); }
    void deallocate(void *p, unsigned long) { if (p) MBfree_release(p); }
};

typedef std::basic_string<char, std::char_traits<char>, MBAllocator<char> > MBString;
typedef std::basic_string<unsigned short, std::char_traits<unsigned short>, MBAllocator<unsigned short> > MBString16;


static const MBGUID MBIID_IMBWinScreen = { 0x7f9eb130, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };

class IMBWinScreen : public IMBScreen {
public:
    virtual void SetHWND(HWND) = 0;
    virtual HWND GetHWND(void) = 0;
};

// CVTUTF.OBJ (Unicode, Inc. ConvertUTF)
extern "C" {
int ConvertUTF16toUTF8(unsigned short **sourceStart, const unsigned short *sourceEnd, unsigned char **targetStart, const unsigned char *targetEnd);
int ConvertUTF8toUTF16(unsigned char **sourceStart, unsigned char *sourceEnd, unsigned short **targetStart, const unsigned short *targetEnd);
}

extern "C" {
void MB_utf8_to_ucs2(const MBString &utf8string, MBString16 &unicode);
void MB_ucs2_to_utf8(const MBString16 &unicode, MBString &utf8);
bool MB_unichar_isalnum(unsigned short c);
bool MB_unichar_isalpha(unsigned short c);
bool MB_unichar_isascii(unsigned short c);
bool MB_unichar_iscntrl(unsigned short c);
bool MB_unichar_isupper(unsigned short c);
bool MB_unichar_islower(unsigned short c);
void MBEnableInternationalCharacterWindows(void);
void MBDisableInternationalCharacterWindows(void);
bool MBInternationalCharacterWindowsEnabled(void);
}

#endif
