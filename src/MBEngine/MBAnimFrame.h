// MBAnimFrame unit declarations (MBEngine.dll)
#ifndef MBANIMFRAME_H
#define MBANIMFRAME_H

#include <string>

extern "C" __declspec(dllimport) void *__cdecl MBmalloc_release(unsigned int size);
extern "C" __declspec(dllimport) void __cdecl MBfree_release(void *p);
extern "C" __declspec(dllimport) unsigned long __cdecl MBHashString(const char *);

inline void *__cdecl operator new(unsigned int size) { return MBmalloc_release(size); }
inline void __cdecl operator delete(void *p) { MBfree_release(p); }

struct MBGUID {
    unsigned long data1;
    unsigned short data2;
    unsigned short data3;
    unsigned char data4[8];
};

class IMBUnknown {
public:
    virtual long QueryInterface(const MBGUID &, void **) = 0;
    virtual unsigned long AddRef(void) = 0;
    virtual unsigned long Release(void) = 0;
};

template <class T> class MBAllocator : public std::allocator<T> {
public:
    typedef unsigned long SizeType;
    typedef T *PointerType;
    T *allocate(unsigned long n, const void *) { return (T *)MBmalloc_release(n * sizeof(T)); }
    char *_Charalloc(unsigned long n) { return (char *)MBmalloc_release(n); }
    void deallocate(void *p, unsigned long) { if (p) MBfree_release(p); }
    void construct(T *p, const T &v) { new ((void *)p) T(v); }
    void destroy(T *p) { p->~T(); }
};

typedef std::basic_string<char, std::char_traits<char>, MBAllocator<char> > MBString;

struct MBPointStruct {
    long x;
    long y;
};

class MBPoint : public MBPointStruct {
public:
    MBPoint(void) { x = 0; y = 0; }
    MBPoint(long ax, long ay) { x = ax; y = ay; }
};

class IMBScriptData;

enum MBFrameProperty {
    kFrameNotify = 0,
    kFrameDelay = 1,
    kFrameStartSound = 2,
    kFrameStartAnim = 3,
    kFrameStartAnimName = 4,
    kFrameStartAnimSequence = 5,
    kFrameStopAnim = 6,
    kFrameStartScript = 7,
    kFrameStopScript = 8,
    kFrameCastMemberOffset = 9
};

class MBAnimFrame : public IMBUnknown {
public:
    MBAnimFrame(void);
protected:
    unsigned long _mRefCnt;
public:
    virtual long QueryInterface(const MBGUID &iid, void **obj);
    virtual unsigned long AddRef(void);
    virtual unsigned long Release(void);
    long SetFrameProperty(MBFrameProperty property, bool value);
    long SetFrameProperty(MBFrameProperty property, unsigned short value);
    long SetFrameProperty(MBFrameProperty property, unsigned long value);
    long SetFrameProperty(MBFrameProperty property, const MBString &value);
    long SetFrameProperty(MBFrameProperty property, const MBPoint &value);
    long GetFrameProperty(MBFrameProperty property, bool &value) const;
    long GetFrameProperty(MBFrameProperty property, unsigned short &value) const;
    long GetFrameProperty(MBFrameProperty property, unsigned long &value) const;
    long GetFrameProperty(MBFrameProperty property, MBPoint &value) const;
    long UseProperty(MBFrameProperty property, bool use);
    bool HasProperty(MBFrameProperty property) const;
    long GetCastID(void) const;
    void SetCastID(long castID);
protected:
    long mCastID;
    unsigned long mProperties;
    MBPoint mCastMemberOffset;
    unsigned long mStartSound;
    unsigned long mStartAnim;
    unsigned long mStopAnim;
    unsigned long mStartScript;
    unsigned long mStopScript;
    unsigned short mFrameDelay;
    unsigned short mStartAnimSequence;
    bool mNotify;
    IMBScriptData *mScriptData;
private:
    ~MBAnimFrame(void);
};

#endif
