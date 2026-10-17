// CMBFileStreamLocator unit declarations (MBEngine.dll)
#ifndef CMBFILESTREAMLOCATOR_H
#define CMBFILESTREAMLOCATOR_H

#include <fstream> // evidence: original TU has $E19/$E20 for std::ctype<unsigned short>::id (same as CMBResourceMgr)

extern "C" __declspec(dllimport) void *__cdecl MBmalloc_release(unsigned int size);
extern "C" __declspec(dllimport) void __cdecl MBfree_release(void *p);

inline void *__cdecl operator new(unsigned int size) { return MBmalloc_release(size); }
inline void __cdecl operator delete(void *p) { MBfree_release(p); }

#include "../MBCom/MBGUID.h"

#include "../MBCom/IMBUnknown.h"
static const MBGUID MBIID_IMBStreamLocator = { 0x89cc0fe0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBFileStreamLocator = { 0x89de6620, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };


class IMBStream;

// only the slot used here is named (vtable +0x2c)
// own copy: this unit has no MBString for the slots +0x0c..+0x28, which it does not call
class IMBFileSpec : public IMBUnknown {
public:
    virtual void v0c(void) = 0;
    virtual void v10(void) = 0;
    virtual void v14(void) = 0;
    virtual void v18(void) = 0;
    virtual void v1c(void) = 0;
    virtual void v20(void) = 0;
    virtual void v24(void) = 0;
    virtual void v28(void) = 0;
    virtual unsigned long Hash(void) const = 0;
};

extern "C" long __cdecl MBOpenFile(IMBFileSpec *spec, unsigned long mode, IMBStream **stream);

// smart pointer (subset of the one in CMBResourceStreamLocator.h)
// own copy: which members are inline and which are only declared differs per unit (the original's inline budget; NOTES.md)
template <class T>
class MBComPtr {
public:
    MBComPtr(void) : mInterface(0) {}
    ~MBComPtr(void) { if (mInterface) mInterface->Release(); }
    operator T *(void) const { return mInterface; }
    T *operator->(void) const { return mInterface; }
    MBComPtr &operator=(T *p) {
        T *old = mInterface;
        mInterface = p;
        if (p) p->AddRef();
        if (old) old->Release();
        return *this;
    }
private:
    T *mInterface;
};

// own copy: carries the sUnknownStreamLocatorStatic $E stand-in (see NOTES.md)
class IMBStreamLocator : public IMBUnknown {
public:
    virtual long GetStream(IMBStream **stream) = 0;
    virtual unsigned long GetID(void) = 0;
    // stand-in: the original's ctype id $E20/$E19 are one past the <fstream> baseline ($E19/$E18);
    // the obj's PDB data has no guard/static, and every STL unit including the IMBStreamLocator
    // header has the +1, so one static data member declaration there takes it (name unknown;
    // never defined or used)
    static const unsigned long sUnknownStreamLocatorStatic;
};

// own copy: derives from this unit's own IMBStreamLocator
class IMBFileStreamLocator : public IMBStreamLocator {
public:
    virtual long SetFile(IMBFileSpec *file) = 0;
};

class CMBFileStreamLocator : public IMBFileStreamLocator {
public:
    CMBFileStreamLocator(void);
    ~CMBFileStreamLocator(void);
    virtual long QueryInterface(const MBGUID &iid, void **obj);
    virtual unsigned long AddRef(void);
    virtual unsigned long Release(void);
    virtual long GetStream(IMBStream **stream);
    virtual unsigned long GetID(void);
    virtual long SetFile(IMBFileSpec *file);
protected:
    unsigned long _mRefCnt;
    MBComPtr<IMBFileSpec> mFile;
};

IMBUnknown *MakeCMBFileStreamLocator(void);

#endif
