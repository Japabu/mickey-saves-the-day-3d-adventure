// Squish image library declarations (MBEngine.dll: Squish, SquishImageFrame, SquishMovie)
#ifndef SQUISH_H
#define SQUISH_H

#include <string> // evidence: each Squish TU has $E19/$E18 for std::ctype<unsigned short>::id (<string> baseline 18/19)
#include <vector>

extern "C" __declspec(dllimport) void *__cdecl MBmalloc_release(unsigned int size);
extern "C" __declspec(dllimport) void __cdecl MBfree_release(void *p);
extern "C" __declspec(dllimport) void *MBmemcpy(void *dst, const void *src, size_t n);

inline void *__cdecl operator new(unsigned int size) { return MBmalloc_release(size); }
inline void __cdecl operator delete(void *p) { MBfree_release(p); }

// No construct/destroy: SquishPalette.obj instantiates std::_Construct for its maps' pairs
template <class T> class MBAllocator : public std::allocator<T> {
public:
    typedef unsigned long SizeType;
    typedef T *PointerType;
    T *allocate(unsigned long n, const void *) { return (T *)MBmalloc_release(n * sizeof(T)); }
    char *_Charalloc(unsigned long n) { return (char *)MBmalloc_release(n); }
    void deallocate(void *p, unsigned long) { if (p) MBfree_release(p); }
};

template <class T> class MBVector : public std::vector<T, MBAllocator<T> > {
};

#include "SquishFileOpenFlags.h"

#include "SquishFileSeekValues.h"

// pixel formats (SquishPixel::GetPixel/SetPixel switch values)
enum {
    kSquishPixFmtIndexed = 1,
    kSquishPixFmtRGBA5551 = 2,
    kSquishPixFmtARGB1555 = 4,
    kSquishPixFmtRGB565 = 8,
    kSquishPixFmtBGRA5551 = 16,
    kSquishPixFmtBGR565 = 32,
    kSquishPixFmtARGB8888 = 64,
    kSquishPixFmtBGRA8888 = 128
};

enum {
    kSquishPixFmtMask = 0x1ff,
    kSquishCompFmtMask = 0xe00,
    kSquishFlagTrans = 0x1000,
    kSquishFlagPalette = 0x2000
};

#include "SquishFile.h"

#include "SquishPixel.h"

class SquishPalette {
public:
    static long __cdecl CreateSeed(unsigned long size, SquishPixel *pixels);
    static SquishPalette *__cdecl Create(unsigned long size, SquishPixel *pixels);
    static SquishPalette *__cdecl StreamIn(SquishFile *file);
    static SquishPalette *__cdecl Find(long seed);
    static void __cdecl BeginStreamOut(void);
    static void __cdecl EndStreamOut(void);
    SquishPalette(void);
    ~SquishPalette(void);
    unsigned long AddRef(void) { return ++mRefCount; }
    unsigned long Release(void) { return --mRefCount; }
    bool StreamOut(SquishFile *file) const;
    unsigned long GetSize(void) const { return mSize; }
    unsigned long GetPixel(unsigned long index, unsigned long format) const;
    SquishPixel GetPixel(unsigned long index) const;
    void SetPixel(unsigned long index, unsigned long pixel, unsigned long format);
    long GetIndex(SquishPixel pixel) const;
    long GetNearestIndex(SquishPixel pixel) const;
    long GetSeed(void) const { return mSeed; }
    void UpdateSeed(void);
    const SquishPixel *GetEntries(void) const { return mEntries; }
protected:
    bool Resize(unsigned long newSize);
    void Reset(void);
    void SetSeed(unsigned long seed) { mSeed = seed; }
    unsigned long mSize;
    unsigned long mRefCount;
    long mSeed;
    SquishPixel *mEntries;
};

#include "SquishImageDescriptor.h"

template <class T> inline const T &SquishMax(const T &a, const T &b) { return (a > b) ? a : b; }
template <class T> inline const T &SquishMin(const T &a, const T &b) { return (a < b) ? a : b; }

class SquishRect {
public:
    long x;
    long y;
    long w;
    long h;
    SquishRect(long ax, long ay, long aw, long ah) : x(ax), y(ay), w(aw), h(ah) {}
    SquishRect(void) { x = y = w = h = 0; }
    void Set(long ax, long ay, long aw, long ah) { x = ax; y = ay; w = aw; h = ah; }
    long Left(void) const { return x; }
    long Top(void) const { return y; }
    long Right(void) const { return x + w; }
    long Bottom(void) const { return y + h; }
    long Width(void) const { return w; }
    long Height(void) const { return h; }
    bool Intersect(const SquishRect &r, SquishRect &out)
    {
        if (Right() < r.Left() || Left() > r.Right() || Bottom() < r.Top() || Top() > r.Bottom())
            return false;
        long right = Right();
        long rright = r.Right();
        long bottom = Bottom();
        long rbottom = r.Bottom();
        out.x = SquishMax(x, r.x);
        out.y = SquishMax(y, r.y);
        out.w = SquishMin(right, rright) - out.x;
        out.h = SquishMin(bottom, rbottom) - out.y;
        return true;
    }
    void Clip(const SquishRect &r)
    {
        x = SquishMax(x, r.x);
        y = SquishMax(y, r.y);
        long right = Right();
        long rright = r.Right();
        long bottom = Bottom();
        long rbottom = r.Bottom();
        w = SquishMin(right, rright) - x;
        h = SquishMin(bottom, rbottom) - y;
    }
    void Move(long dx, long dy) { x += dx; y += dy; }
    void Place(long ax, long ay) { x = ax; y = ay; }
    void Scale(float sx, float sy) { x = ScaledLong(x, sx); y = ScaledLong(y, sy); w = ScaledLong(w, sx); h = ScaledLong(h, sy); }
protected:
    static long __cdecl ScaledLong(long val, float scale)
    {
        float f = val * scale;
        long r = (long)(f + 0.5f);
        long t = (long)f;
        if (t != r)
            return r;
        return t;
    }
};

class SquishFileFilter;

#include "SquishImageBlock.h"

// Locks a block's data for the lifetime of the object
struct ImageDataPtr {
    ImageDataPtr(SquishImageBlock *block) : mBlock(block)
    {
        mData = 0;
        if (mBlock)
            mData = mBlock->Lock();
    }
    ~ImageDataPtr(void) { Unlock(); }
    void Unlock(void)
    {
        if (mBlock)
            mBlock->Unlock(mData);
        mBlock = 0;
        mData = 0;
    }
    operator unsigned char *(void) { return mData; }
    operator const unsigned char *(void) const { return mData; }
    unsigned char *mData;
    SquishImageBlock *mBlock;
};

struct _iobuf;

class SquishFileFilter {
public:
    SquishFileFilter(const char *path);
    SquishFileFilter(_iobuf *file);
    virtual ~SquishFileFilter(void);
    virtual bool Describe(SquishImageDescriptor *desc) = 0;
    virtual bool Read(SquishImageDescriptor *desc, unsigned char *data) = 0;
    virtual bool Write(SquishImageBlock *block) = 0;
protected:
    _iobuf *file;
    bool bCloseFileHandle;
};

// SquishBlit.obj
void SquishFill(unsigned char *dest, unsigned long fill, int width, int height, int depth, int rowbytes);
void SquishBlit(unsigned char *dest, const unsigned char *src, const SquishImageDescriptor *destFmt,
                const SquishImageDescriptor *srcFmt);
void SquishBlitTrans(unsigned char *dest, const unsigned char *src, const SquishImageDescriptor *destFmt,
                     const SquishImageDescriptor *srcFmt);
void SquishBlitBlock(unsigned char *dest, const unsigned char *src, const SquishImageDescriptor *destFmt,
                     const SquishImageDescriptor *srcFmt, const SquishRect *destRect, const SquishRect *srcRect);
void SquishBlitBlockTrans(unsigned char *dest, const unsigned char *src, const SquishImageDescriptor *destFmt,
                          const SquishImageDescriptor *srcFmt, const SquishRect *destRect, const SquishRect *srcRect);

// SquishDecode.obj
void RLEBlit(unsigned char *dest, const unsigned char *src, const SquishImageDescriptor *destFmt,
             const SquishImageDescriptor *srcFmt);
void RLEBlitTrans(unsigned char *dest, const unsigned char *src, const SquishImageDescriptor *destFmt,
                  const SquishImageDescriptor *srcFmt);
void RLEBlitBlock(unsigned char *dest, const unsigned char *src, const SquishImageDescriptor *destFmt,
                  const SquishImageDescriptor *srcFmt, const SquishRect *destRect, const SquishRect *srcRect);
void RLEBlitBlockTrans(unsigned char *dest, const unsigned char *src, const SquishImageDescriptor *destFmt,
                       const SquishImageDescriptor *srcFmt, const SquishRect *destRect, const SquishRect *srcRect);
void RLEBlitScaledBlock(unsigned char *dest, const unsigned char *src, const SquishImageDescriptor *destFmt,
                        const SquishImageDescriptor *srcFmt, const SquishRect *destRect, const SquishRect *srcRect);
void RLEBlitScaledBlockTrans(unsigned char *dest, const unsigned char *src, const SquishImageDescriptor *destFmt,
                             const SquishImageDescriptor *srcFmt, const SquishRect *destRect, const SquishRect *srcRect);

// SquishEncode.obj
void RLEEncodeBlock(unsigned char **dest, const unsigned char *src, SquishImageDescriptor *destFmt,
                    const SquishImageDescriptor *srcFmt, long srcx, long srcy, long srcw, long srch);
void RLEEncodeBlockTrans(unsigned char **dest, const unsigned char *src, SquishImageDescriptor *destFmt,
                         const SquishImageDescriptor *srcFmt, long srcx, long srcy, long srcw, long srch);

#include "SquishImageFrame.h"

#include "SquishMovie.h"

#endif
