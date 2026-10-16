#pragma once
#include "Text.h"
namespace Graph { class Stream; }

// Text as stored in a scene file: a length and the characters.
void writeText(const Text &, Graph::Stream *);
Text readText(Graph::Stream *);
template <class T0> class RVec;
template <class T0> class Vec3;
void writeVecf(const RVec<float> &, Graph::Stream *);
void readVecf(RVec<float> &, Graph::Stream *);
void writeVec3f(const Vec3<float> &, Graph::Stream *);
void readVec3f(Vec3<float> &, Graph::Stream *);
template <class T0> class TMatrix4;
void writeMatrix4f(const TMatrix4<float> &, Graph::Stream *);
void readMatrix4f(TMatrix4<float> &, Graph::Stream *);
template <class T0> class Vec4;
void writeVec4f(const Vec4<float> &, Graph::Stream *);
void readVec4f(Vec4<float> &, Graph::Stream *);
template <class T0> class IVec;
template <class T0> class Vec;
template <class T0> class AVec;
template <class T0> class TMatrix3;
void writeVeci(const IVec<int> &, Graph::Stream *);
void readVeci(IVec<int> &, Graph::Stream *);
void writeVecdw(const Vec<unsigned long> &, Graph::Stream *);
void readVecdw(Vec<unsigned long> &, Graph::Stream *);
void writeVVec3f(const AVec<Vec3<float> > &, Graph::Stream *);
void readVVec3f(AVec<Vec3<float> > &, Graph::Stream *);
void writeVVVec3f(const Vec<AVec<Vec3<float> > > &, Graph::Stream *);
void readVVVec3f(Vec<AVec<Vec3<float> > > &, Graph::Stream *);
void writeMatrix3f(const TMatrix3<float> &, Graph::Stream *);
void readMatrix3f(TMatrix3<float> &, Graph::Stream *);
