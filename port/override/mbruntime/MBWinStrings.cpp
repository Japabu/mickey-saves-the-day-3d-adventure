// PORT: C++ <string.h> returns const char * from strstr/strchr/strrchr/strpbrk for a const argument; cast back, as the C functions return.
#include "MBWinStrings.h"

// 0x10001cc0 _MBstrcpy
extern "C" char *__cdecl MBstrcpy(char *strDestination, const char *strSource)
{
	return strcpy(strDestination, strSource);
}

// 0x10001cf0 _MBstrncpy
extern "C" char *__cdecl MBstrncpy(char *strDestination, const char *strSource, unsigned int count)
{
	return strncpy(strDestination, strSource, count);
}

// 0x10001d10 _MBstrcat
extern "C" char *__cdecl MBstrcat(char *strDestination, const char *strSource)
{
	return strcat(strDestination, strSource);
}

// 0x10001d50 _MBstrcmp
extern "C" long __cdecl MBstrcmp(const char *string1, const char *string2)
{
	return strcmp(string1, string2);
}

// 0x10001d90 _MBstricmp
extern "C" long __cdecl MBstricmp(const char *string1, const char *string2)
{
	return _stricmp(string1, string2);
}

// 0x10001db0 _MBstrlen
extern "C" unsigned long __cdecl MBstrlen(const char *string)
{
	return strlen(string);
}

// 0x10001dd0 _MBstrstr
extern "C" char *__cdecl MBstrstr(const char *string, const char *strCharSet)
{
	return (char *)strstr(string, strCharSet);
}

// 0x10001df0 _MBstrchr
extern "C" char *__cdecl MBstrchr(const char *string, int c)
{
	return (char *)strchr(string, c);
}

// 0x10001e10 _MBstrrchr
extern "C" char *__cdecl MBstrrchr(const char *string, int c)
{
	return (char *)strrchr(string, c);
}

// 0x10001e30 _MBstrspn
extern "C" unsigned long __cdecl MBstrspn(const char *string, const char *strCharSet)
{
	return strspn(string, strCharSet);
}

// 0x10001e50 _MBstrcspn
extern "C" unsigned long __cdecl MBstrcspn(const char *string, const char *strCharSet)
{
	return strcspn(string, strCharSet);
}

// 0x10001e70 _MBstrpbrk
extern "C" char *__cdecl MBstrpbrk(const char *string, const char *strCharSet)
{
	return (char *)strpbrk(string, strCharSet);
}

// 0x10001e90 _MBstrset
extern "C" char *__cdecl MBstrset(char *string, int c)
{
	return _strset(string, c);
}

// 0x10001ed0 _MBstrupr
extern "C" char *__cdecl MBstrupr(char *string)
{
	return _strupr(string);
}

// 0x10001ee0 _MBstrlwr
extern "C" char *__cdecl MBstrlwr(char *string)
{
	return _strlwr(string);
}

// 0x10001ef0 _MBstrrev
extern "C" char *__cdecl MBstrrev(char *string)
{
	return _strrev(string);
}

// 0x10001f00 _MBstrtok
extern "C" char *__cdecl MBstrtok(char *strToken, const char *strDelimit)
{
	return strtok(strToken, strDelimit);
}


// Matched by this code:
// 0x10001f20 _$E20
// 0x10001f40 _$E19