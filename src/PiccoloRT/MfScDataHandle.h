// MfScDataHandle: a reference-counted, typed value of the script VM.
#pragma once
#include "PiccoloRT_api.h"
#include "MfCriticalSection.h"
class MfScData;
class MfScDataArray;
class MfScDataObject;
class MfScGCTimerWork;
class MfScGCWork;
class MfScHandleEnum;
class MfScHandleHeap;
class MfScObjFloat;
class MfScObjInteger;
class MfString;
class MfThread;
class MfVariant;

// A reference-counted, typed value of the script VM (8 bytes, allocated from
// the handle heaps by operator new).
class PICCOLORT_API MfScDataHandle {
public:
    // thrown by operator new when a heap is exhausted even after collecting
    class MfScMemoryException {
    public:
        ~MfScMemoryException() {}
    };

    MfScDataHandle();
    ~MfScDataHandle();

    unsigned int GetType() const { return m_flags & 0x1ff0; }
    int IsType(unsigned long type) const { return m_flags & type; }
    void SetType(unsigned long type) { m_flags |= (type & 0x1ff0); }
    int IsReadOnly() const { return m_readOnly; }
    void SetReadOnly() { m_readOnly = 1; }
    void SetReadWrite() { m_readOnly = 0; }
    int IsVisiting() const { return m_visiting; }
    void SetVisiting() { m_visiting = 1; }
    void ResetVisiting() { m_visiting = 0; }
    float GetFloat() const { return m_float; }
    float GetNumber() const { return (m_flags & 0x10) ? (float)m_long : m_float; }
    int GetInt() const { return m_long; }
    MfScData *GetData() const { return (m_flags & 0xfc0) ? m_data : 0; }
    MfScDataHandle *GetPointer() const { return m_pointer; }
    void SetData(MfScData *data) { m_data = data; }
    void SetFloat(float f) { m_float = f; }
    void SetInt(int i) { m_long = i; }
    void SetPointer(MfScDataHandle *pointer) { m_pointer = pointer; }
    unsigned short GetRefCount() const { return m_refCount; }
    unsigned short AddRef() { return ++m_refCount; }
    unsigned short Release() { return m_refCount > 0 ? --m_refCount : m_refCount; }

    MfScDataHandle *Package();
    MfString TypeInfo();
    void InfoLong(MfString *);
    void InfoShort(MfString *);

    static MfScDataHandle * __cdecl CreateBytecodeFunction(MfString const &, int);
    static MfScDataHandle * __cdecl CreateData(MfScData *, MfScDataHandle * (MfScData::*)(MfScDataHandle *,int,MfScDataHandle * *), MfString const &, int);
    static MfScDataHandle * __cdecl CreateData(MfScData *, int, MfScDataHandle * (MfScData::*)(MfScDataHandle *,int,MfScDataHandle * *), MfString const &, int);
    static MfScDataHandle * __cdecl CreateData(MfScDataHandle * (MfScData::*)(MfScDataHandle *,int,MfScDataHandle * *), MfString const &, int);
    static MfScDataHandle * __cdecl CreateData(MfScDataHandle * (__cdecl*)(MfScDataHandle *,int,MfScDataHandle * *), MfString const &, int);
    static MfScDataHandle * __cdecl CreateData(MfScDataObject *);
    static MfScDataHandle * __cdecl CreateData(MfScObjFloat const &);
    static MfScDataHandle * __cdecl CreateData(MfScObjInteger const &);
    static MfScDataHandle * __cdecl CreateData(MfString const &, int);
    static MfScDataHandle * __cdecl CreateData(MfVariant const &, int);
    static MfScDataHandle * __cdecl CreateData(float);
    static MfScDataHandle * __cdecl CreateData(int);
    static MfScDataHandle * __cdecl CreateData(int, MfScDataHandle * (MfScData::*)(MfScDataHandle *,int,MfScDataHandle * *), MfString const &, int);
    static MfScDataHandle * __cdecl CreateDataArray(int);
    static MfScDataHandle * __cdecl CreateDataArrayHandle(MfScDataArray *);
    static MfString __cdecl HeapTypeName(int);
    static int __cdecl GarbageCollect();
    static int __cdecl GetNumHandles(int);
    static int __cdecl WaitForGC(unsigned int);
    static void * __cdecl operator new(unsigned int, int);
    static void __cdecl operator delete(void *);
    static void __cdecl DumpPool();
    static void __cdecl DumpPool(int);
    static void __cdecl EnumHandles(int, MfScHandleEnum &);
    static void __cdecl Shutdown();
    static void __cdecl ShutdownGCTimeoutThread();

    static void AddGCHint(unsigned long hint) { udwHints |= hint; }
    static void RemoveGCHint(unsigned long hint) { udwHints &= ~hint; }
    static unsigned long GetGCHints() { return udwHints; }
    static void DisableGCForVM() { InterlockedIncrement(&lDisableGC); }
    static void EnableGCForVM() { InterlockedDecrement(&lDisableGC); }
    static int IsGCRunning() { return lGCrunning; }
    static void LockGC() { csGCGuard.Enter(); }
    static void UnlockGC() { csGCGuard.Leave(); }

private:
    friend class MfScThreadManager;
    friend class MfScGCWork;
    friend class MfScGCTimerWork;
    MfScDataHandle(const MfScDataHandle &) {}
    MfScDataHandle &operator=(const MfScDataHandle &) { return *this; }
    int IsValidHandle() const;
    static void SignalGCIsRunning() { InterlockedIncrement(&lGCrunning); }
    static void SignalGCIsQuitting() { InterlockedDecrement(&lGCrunning); }
    static MfScDataHandle * __cdecl GrowHeap(int);
    static int __cdecl GarbageCollectContainer(MfScHandleHeap *, int, int);
    static int __cdecl GarbageCollectNotContainer(MfScHandleHeap *, int, int);
    static void __cdecl LockHeap();
    static void __cdecl UnlockHeap();

    unsigned short m_refCount; // +0
    union {                    // +2
        unsigned short m_flags;
        struct {
            unsigned short m_bit0 : 1;
            unsigned short m_bit1 : 1;
            unsigned short m_visiting : 1; // 0x4
            unsigned short m_readOnly : 1; // 0x8
            // 0x10 integer, 0xfc0 data (object) types, 0x1ff0 type mask
        };
    };
    union {                    // +4
        float m_float;
        long m_long;
        MfScData *m_data;
        MfScDataHandle *m_pointer;
    };

    static MfScHandleHeap *aphhp[3];
    static MfCriticalSection csGCGuard;
    static unsigned long dwGCTick;
    static long lDisableGC;
    static long lGCTimeOut;
    static long lGCrunning;
    static MfScGCTimerWork * pgctwWork;
    static MfScGCWork * pgcwWork;
    static MfThread * ptGCThread;
    static MfThread * ptGCTimerThread;
    static unsigned long udwHints;
};

// The value a variable pool stores for a declared but undefined variable.
// Every source file that includes this header has its own copy.
static MfScDataHandle *pdhUndefined = (MfScDataHandle *)-1;
