// PORT: operator new hands out a static buffer big enough for the link data objects (the source returned one pointer-sized global).
#include "MfScLinkData.h"
int g_MfScLinkDataCount;
// PORT: the one live link data object needs room for an MfScListLinkData (24 bytes on
// 64-bit); the source had a single pointer-sized slot
alignas(16) static unsigned char g_MfScLinkDataPool[64];
// 0x10008b30 ?Count@MfScLinkData@@QAEAAHXZ
int & MfScLinkData::Count()
{
    return mCount;
}

// 0x1000bd80 ??2MfScLinkData@@SAPAXI@Z
void * __cdecl MfScLinkData::operator new(size_t)
{
    g_MfScLinkDataCount = 1;
    return g_MfScLinkDataPool; // PORT: the buffer above
}

// 0x1000bd90 ??3MfScLinkData@@SAXPAX@Z
void __cdecl MfScLinkData::operator delete(void *)
{
    g_MfScLinkDataCount = 0;
}
