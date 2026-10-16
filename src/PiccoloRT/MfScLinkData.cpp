#include "MfScLinkData.h"
int g_MfScLinkDataCount;
void *g_MfScLinkDataPool;
// 0x10008b30 ?Count@MfScLinkData@@QAEAAHXZ
int & MfScLinkData::Count()
{
    return mCount;
}

// 0x1000bd80 ??2MfScLinkData@@SAPAXI@Z
void * __cdecl MfScLinkData::operator new(unsigned int)
{
    g_MfScLinkDataCount = 1;
    return &g_MfScLinkDataPool;
}

// 0x1000bd90 ??3MfScLinkData@@SAXPAX@Z
void __cdecl MfScLinkData::operator delete(void *)
{
    g_MfScLinkDataCount = 0;
}
