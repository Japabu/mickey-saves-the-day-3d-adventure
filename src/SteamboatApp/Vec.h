#pragma once
#include "BasicObject.h"

class ostream;

// n limited to [lower, upper].
inline int clamp(int n, int lower, int upper)
{
    return (upper <= lower) ? lower : ((n >= upper) ? upper : ((n <= lower) ? lower : n));
}

// v limited to [lo, hi].
inline float clamp(float v, float lo, float hi)
{
    return (hi <= lo) ? lo : ((v >= hi) ? hi : ((v <= lo) ? lo : v));
}

// Growable array of the scene-graph library. Storage grows by the factor
// amult (clamped to 1..2) whenever an append or verify runs out of room.
template <class T>
class Vec : public BasicObject {
protected:
    T *e;
    int len;
    int alloclen;
    float amult;

    void reallocMem(int n, int copy, int init)
    {
        alloclen = n;
        T *ne = new T[n];
        if (copy) {
            for (int i = 0; i < len; i++)
                ne[i] = e[i];
            if (e)
                delete [] e;
            e = ne;
            if (init)
                initElements(len, alloclen);
        } else {
            if (e)
                delete [] e;
            e = ne;
            if (init)
                initElements(len, alloclen);
        }
    }
    void initElements(int from, int to)
    {
        T zero = T();
        for (int i = from; i < to; i++)
            e[i] = zero;
    }
    int checkBounds(int) const;
public:
    Vec(const Vec<T> &v)
    {
        e = 0;
        len = 0;
        alloclen = 0;
        amult = v.amult;
        if (v.len) {
            reallocMem(v.len, 0, 0);
            for (int i = 0; i < v.len; i++)
                e[i] = v.e[i];
            len = v.len;
        }
    }
    Vec(const T *, int, float);
    Vec(int n = 0, float mult = 2.0f)
    {
        e = 0;
        len = 0;
        alloclen = 0;
        amult = clamp(mult, 1.0f, 2.0f);
        if (n > 0) {
            reallocMem(n, 0, 0);
            len = n;
        }
    }
    virtual ~Vec() { delete [] e; }
    void setAllocMult(float);
    int allocated() { return alloclen; }
    void setLength(int n)
    {
        if (n < 0)
            n = 0;
        if (n > alloclen)
            reallocMem(n, 1, 1);
        else if (n > len)
            initElements(len, n);
        len = n;
    }
    operator T *() { return len ? e : 0; }
    operator const T *() const { return len ? e : 0; }
    int length() const { return len; }
    int operator!() const { return len == 0; }
    Vec<T> &operator=(const Vec<T> &v)
    {
        if (alloclen < v.len)
            reallocMem(v.len, 0, 0);
        for (int i = 0; i < v.len; i++)
            e[i] = v.e[i];
        len = v.len;
        return *this;
    }
    T &operator[](int i) { return e[i]; }
    void setValueVerify(int i, const T &t) { e[verify(i)] = t; }
    void setValues(const T *, int);
    const T &operator()(unsigned int i) const { return e[i]; }
    const T &operator()(int i) const { return e[i]; }
    const T &getValue(int i) const { return e[i]; }
    void append(const Vec<T> &);
    void append(const T &t)
    {
        int n = len + 1;
        if (n > alloclen) {
            int grown = (int)(alloclen * amult);
            reallocMem(n > grown ? n : grown, 1, 0);
        }
        e[len++] = t;
    }
    void append(const T *, int);
    ostream &write(ostream &) const;
    void insert(const Vec<T> &, int);
    void insert(const T &, int);
    void insert(const T *, int, int);
    int extract(Vec<T> &, int, int) const;
    int remove(int pos, int n)
    {
        if (pos < 0 || pos >= len)
            return 0;
        if (n <= len - pos) {
            for (int i = pos + n; i < len; i++)
                e[i - n] = e[i];
            len -= n;
        } else {
            setLength(pos);
        }
        return 1;
    }
    int mvLastTo(int);
    void shrink()
    {
        if (len == 0) {
            delete [] e;
            e = 0;
            alloclen = 0;
        } else if (alloclen != len) {
            reallocMem(len, 1, 0);
        }
    }
    int verify(int i)
    {
        i = (i < 0) ? 0 : i;
        if (i >= len) {
            if (i >= alloclen) {
                int n = i + 1;
                int grown = (int)(alloclen * amult);
                reallocMem(n > grown ? n : grown, 1, 1);
            } else {
                initElements(len, i + 1);
            }
            len = i + 1;
        }
        return i;
    }
    int operator==(const Vec<T> &) const;
};

template <class T>
void Vec<T>::append(const T *p, int n)
{
    if (n > 0) {
        int newlen = len + n;
        if (newlen > alloclen) {
            int grown = (int)(alloclen * amult);
            reallocMem(newlen > grown ? newlen : grown, 1, 0);
        }
        for (int i = 0; i < n; i++)
            e[len++] = p[i];
    }
}

template <class T>
void Vec<T>::insert(const T *p, int n, int pos)
{
    int oldlen = len;
    verify(len + n - 1);
    for (int i = oldlen - 1; i >= pos; i--)
        e[i + n] = e[i];
    for (int j = 0; j < n; j++)
        e[pos + j] = p[j];
}
