// SBCRTNew.cpp: the CRT's debug form of operator new (the one _CRTDBG_MAP_ALLOC's new
// expressions call), sent to the engine's tracking allocator.
extern "C" __declspec(dllimport) void *__cdecl MBmalloc_debug(unsigned long size, const char *fileName, int line);

// 0x10044ca0 ??2@YAPAXIHPBDH@Z
void *__cdecl operator new(unsigned int cb, int nBlockUse, const char *szFileName, int nLine)
{
    return MBmalloc_debug(cb, szFileName, nLine);
}
