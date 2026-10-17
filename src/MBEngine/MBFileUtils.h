// MBFileUtils.h - declarations for MBEngine.dll MBFileUtils.obj: directory helpers over IMBFileSpec
#ifndef MBFILEUTILS_H
#define MBFILEUTILS_H

#include <windows.h>
#include "MBStl.h"
#include <string>
#include <vector>

extern "C" __declspec(dllimport) void *__cdecl MBmalloc_release(unsigned int size);
extern "C" __declspec(dllimport) void __cdecl MBfree_release(void *p);
extern "C" __declspec(dllimport) char *__cdecl MBstrcpy(char *dst, const char *src);
extern "C" __declspec(dllimport) char *__cdecl MBstrcat(char *dst, const char *src);
extern "C" __declspec(dllimport) char *__cdecl MBstrchr(const char *s, int c);
extern "C" __declspec(dllimport) unsigned int __cdecl MBstrlen(const char *s);

inline void *__cdecl operator new(unsigned int size) { return MBmalloc_release(size); }
inline void __cdecl operator delete(void *p) { MBfree_release(p); }

#include "../MBCom/MBGUID.h"

#include "../MBCom/IMBUnknown.h"
static const MBGUID MBIID_IMBFileSpec = { 0x897e2150, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_CMBWinFileSpec = { 0x89843e10, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };


extern "C" long MBComCreateInstance(IMBUnknown *pOuter, const MBGUID &guid, IMBUnknown **unknown);

// No construct/destroy: the original instantiates std::_Construct for the file vector
template <class T> class MBAllocator : public std::allocator<T> {
public:
    T *allocate(unsigned long n, const void *) { return (T *)MBmalloc_release(n * sizeof(T)); }
    char *_Charalloc(unsigned long n) { return (char *)MBmalloc_release(n); }
    void deallocate(void *p, unsigned long) { if (p) MBfree_release(p); }
};

typedef std::basic_string<char, std::char_traits<char>, MBAllocator<char> > MBString;

template <class T> class MBVector : public std::vector<T, MBAllocator<T> > {
};

#include "IMBFileSpec.h"

// own copy: which members are inline and which are only declared differs per unit (the original's inline budget; NOTES.md)
template <class T> class MBComPtr {
public:
    MBComPtr(void) : mInterface(0) {}
    MBComPtr(const MBGUID &clsid) : mInterface(0) { CreateObject(clsid); }
    MBComPtr(const MBComPtr &p) : mInterface(p.mInterface) { if (mInterface) mInterface->AddRef(); }
    ~MBComPtr(void) { Release(); }
    operator T *(void) const { return mInterface; }
    T *operator->(void) const { return mInterface; }
    MBComPtr &operator=(T *p) {
        if (mInterface != p) {
            T *old = mInterface;
            mInterface = p;
            if (p) p->AddRef();
            if (old) old->Release();
        }
        return *this;
    }
    MBComPtr &operator=(const MBComPtr &p) { return *this = p.mInterface; }
    void Release(void) {
        do {
            if (mInterface != 0)
                mInterface->Release();
        } while (0);
        mInterface = 0;
    }
    void Attach(T *p) {
        Release();
        mInterface = p;
    }
    long CreateObject(const MBGUID &clsid) {
        IMBUnknown *unk;
        Release();
        MBComCreateInstance(0, clsid, &unk);
        if (unk) {
            long hr = _QueryInterface(unk);
            unk->Release();
            return hr;
        }
        return -2;
    }
private:
    long _QueryInterface(IMBUnknown *const &p) {
        long hr;
        if (p) {
            T *pInterface;
            hr = p->QueryInterface(T::GetGUID(), (void **)&pInterface);
            if (hr) {
                Attach(0);
                return hr;
            }
            Attach(pInterface);
        } else {
            *this = 0;
            hr = -3;
        }
        return hr;
    }
    T *mInterface;
};

// MBFileSystemWin.obj
extern "C" {
bool MBFileSpecExists(const IMBFileSpec *spec);
bool MBIsFileSpecAFolder(const IMBFileSpec *spec);
long MBDeleteFile(const IMBFileSpec *fileSpec);
long MBSetFileReadOnly(const IMBFileSpec *fileSpec, bool isReadOnly);
}

extern char kMBDirectorySeparator;
extern char kMBExtensionSeparator;

extern "C" {
bool MBGetCurrentDirectory(IMBFileSpec **path);
bool MBGetDesktopDirectory(IMBFileSpec **out);
void MBGetFilesInDirectory(const IMBFileSpec *directory, MBVector<MBComPtr<IMBFileSpec> > &kids,
                           unsigned long flags, long recursionDepth);
bool MBMakeCDPath(const char *partialPath, IMBFileSpec **spec);
}
bool MBSetCurrentDirectory(MBComPtr<IMBFileSpec> spec);
bool MBMakeDirectory(MBComPtr<IMBFileSpec> spec);
bool MBRemoveDirectory(MBComPtr<IMBFileSpec> spec, bool remove_files);
bool MBCopyFile(MBComPtr<IMBFileSpec> source, MBComPtr<IMBFileSpec> dest);

#endif
