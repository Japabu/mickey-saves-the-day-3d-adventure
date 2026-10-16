#include "Graph_Stream.h"

// 0x100e2120 ??0Stream@Graph@@QAE@XZ
Graph::Stream::Stream()
{
}

// 0x100e2150 ??1Stream@Graph@@UAE@XZ
Graph::Stream::~Stream()
{
}

// 0x100e2160 ?ByteSwap@Stream@Graph@@SAXPAF@Z
void __cdecl Graph::Stream::ByteSwap(short *p)
{
    ByteSwap((unsigned char *)p, 2);
}

// 0x100e2170 ?ByteSwap@Stream@Graph@@SAXPAH@Z
void __cdecl Graph::Stream::ByteSwap(int *p)
{
    ByteSwap((unsigned char *)p, 4);
}

// 0x100e2180 ?ByteSwap@Stream@Graph@@SAXPAM@Z
void __cdecl Graph::Stream::ByteSwap(float *p)
{
    ByteSwap((unsigned char *)p, 4);
}

// 0x100e2190 ?ByteSwap@Stream@Graph@@SAXPAN@Z
void __cdecl Graph::Stream::ByteSwap(double *p)
{
    ByteSwap((unsigned char *)p, 8);
}

// 0x100e21a0 ?ByteSwap@Stream@Graph@@SAXPAK@Z
void __cdecl Graph::Stream::ByteSwap(unsigned long *p)
{
    ByteSwap((unsigned char *)p, 4);
}

// 0x100e21b0 ?ByteSwap@Stream@Graph@@SAXPAEH@Z
void __cdecl Graph::Stream::ByteSwap(unsigned char *p, int size)
{
    int i = 0;
    int j = size - 1;
    while (i < j) {
        unsigned char c = p[i];
        p[i++] = p[j];
        p[j--] = c;
    }
}

// Matched by this code:
// 0x100e2130 ??_GStream@Graph@@UAEPAXI@Z