// VisMisc.cpp: reading and writing the scene library's values (Text, vectors,
// matrices) to a scene file stream.
#include "VisMisc.h"
#include "Graph_Stream.h"
#include "AVec.h"
#include "Vec3.h"
#include "Vec4.h"
#include "TMatrix4.h"

// 0x10104910 ?writeText@@YAXABVText@@PAVStream@Graph@@@Z
void writeText(const Text &text, Graph::Stream *stream)
{
    int length = strlen(text);
    stream->Write(length);
    stream->Write((const unsigned char *)(const char *)text, length);
}

// 0x10104950 ?readText@@YA?AVText@@PAVStream@Graph@@@Z
Text readText(Graph::Stream *stream)
{
    int length = 0;
    stream->Read(&length);
    Text text(length + 1);
    char *buffer = text;
    stream->Read((unsigned char *)buffer, length);
    buffer[length] = 0;
    return text;
}

#pragma optimize("a", on) // evidence: the vector writers reuse the length read before the Write call (no-alias code)
// 0x10104a40 ?writeVeci@@YAXABV?$IVec@H@@PAVStream@Graph@@@Z
void writeVeci(const IVec<int> &v, Graph::Stream *stream)
{
    int n = v.length();
    const int *e = v;
    stream->Write(n);
    for (int i = 0; i < n; i++)
        stream->Write(e[i]);
}

// 0x10104a80 ?writeVecdw@@YAXABV?$Vec@K@@PAVStream@Graph@@@Z
void writeVecdw(const Vec<unsigned long> &v, Graph::Stream *stream)
{
    int n = v.length();
    const unsigned long *e = v;
    stream->Write(n);
    for (int i = 0; i < n; i++)
        stream->Write(e[i]);
}

// 0x10104ac0 ?readVecdw@@YAXAAV?$Vec@K@@PAVStream@Graph@@@Z
void readVecdw(Vec<unsigned long> &v, Graph::Stream *stream)
{
    int n = 0;
    stream->Read(&n);
    v.setLength(n);
    unsigned long *e = v;
    for (int i = 0; i < n; i++)
        stream->Read(&e[i]);
}

// 0x10104ba0 ?readVeci@@YAXAAV?$IVec@H@@PAVStream@Graph@@@Z
void readVeci(IVec<int> &v, Graph::Stream *stream)
{
    int n = 0;
    stream->Read(&n);
    v.setLength(n);
    int *e = v;
    for (int i = 0; i < n; i++)
        stream->Read(&e[i]);
}

// 0x10104c80 ?readVecf@@YAXAAV?$RVec@M@@PAVStream@Graph@@@Z
void readVecf(RVec<float> &v, Graph::Stream *stream)
{
    int n = 0;
    stream->Read(&n);
    v.setLength(n);
    float *e = v;
    for (int i = 0; i < n; i++)
        stream->Read(&e[i]);
}

// 0x10104d60 ?writeVecf@@YAXABV?$RVec@M@@PAVStream@Graph@@@Z
void writeVecf(const RVec<float> &v, Graph::Stream *stream)
{
    int n = v.length();
    const float *e = v;
    stream->Write(n);
    for (int i = 0; i < n; i++)
        stream->Write(e[i]);
}

// 0x10104da0 ?writeVec3f@@YAXABV?$Vec3@M@@PAVStream@Graph@@@Z
void writeVec3f(const Vec3<float> &v, Graph::Stream *stream)
{
    for (int i = 0; i < 3; i++)
        stream->Write(v(i));
}

// 0x10104dd0 ?readVec3f@@YAXAAV?$Vec3@M@@PAVStream@Graph@@@Z
void readVec3f(Vec3<float> &v, Graph::Stream *stream)
{
    for (int i = 0; i < 3; i++)
        stream->Read(&v[i]);
}

// 0x10104e00 ?writeVec4f@@YAXABV?$Vec4@M@@PAVStream@Graph@@@Z
void writeVec4f(const Vec4<float> &v, Graph::Stream *stream)
{
    for (int i = 0; i < 4; i++)
        stream->Write(v(i));
}

// 0x10104e30 ?readVec4f@@YAXAAV?$Vec4@M@@PAVStream@Graph@@@Z
void readVec4f(Vec4<float> &v, Graph::Stream *stream)
{
    for (int i = 0; i < 4; i++)
        stream->Read(&v[i]);
}

// 0x10104e60 ?writeVVec3f@@YAXABV?$AVec@V?$Vec3@M@@@@PAVStream@Graph@@@Z
void writeVVec3f(const AVec<Vec3<float> > &v, Graph::Stream *stream)
{
    int n = v.length();
    stream->Write(n);
    const Vec3<float> *e = v;
    for (int i = 0; i < n; i++)
        writeVec3f(e[i], stream);
}

// 0x10104ea0 ?readVVec3f@@YAXAAV?$AVec@V?$Vec3@M@@@@PAVStream@Graph@@@Z
void readVVec3f(AVec<Vec3<float> > &v, Graph::Stream *stream)
{
    int n = 0;
    stream->Read(&n);
    v.setLength(n);
    Vec3<float> *e = v;
    for (int i = 0; i < n; i++)
        readVec3f(e[i], stream);
}

// 0x10104f60 ?writeVVVec3f@@YAXABV?$Vec@V?$AVec@V?$Vec3@M@@@@@@PAVStream@Graph@@@Z
void writeVVVec3f(const Vec<AVec<Vec3<float> > > &v, Graph::Stream *stream)
{
    int n = v.length();
    stream->Write(n);
    const AVec<Vec3<float> > *e = v;
    for (int i = 0; i < n; i++)
        writeVVec3f(e[i], stream);
}

// 0x10104fa0 ?readVVVec3f@@YAXAAV?$Vec@V?$AVec@V?$Vec3@M@@@@@@PAVStream@Graph@@@Z
void readVVVec3f(Vec<AVec<Vec3<float> > > &v, Graph::Stream *stream)
{
    int n = 0;
    stream->Read(&n);
    v.setLength(n);
    AVec<Vec3<float> > *e = v;
    for (int i = 0; i < n; i++)
        readVVec3f(e[i], stream);
}

// 0x10105020 ?readMatrix3f@@YAXAAV?$TMatrix3@M@@PAVStream@Graph@@@Z
void readMatrix3f(TMatrix3<float> &m, Graph::Stream *stream)
{
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            stream->Read(&m[i][j]);
}

// 0x10105070 ?writeMatrix3f@@YAXABV?$TMatrix3@M@@PAVStream@Graph@@@Z
void writeMatrix3f(const TMatrix3<float> &m, Graph::Stream *stream)
{
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            stream->Write(m[i][j]);
}

// 0x101050c0 ?writeMatrix4f@@YAXABV?$TMatrix4@M@@PAVStream@Graph@@@Z
void writeMatrix4f(const TMatrix4<float> &m, Graph::Stream *stream)
{
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            stream->Write(m[i][j]);
}

// 0x10105110 ?readMatrix4f@@YAXAAV?$TMatrix4@M@@PAVStream@Graph@@@Z
void readMatrix4f(TMatrix4<float> &m, Graph::Stream *stream)
{
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            stream->Read(&m[i][j]);
}
