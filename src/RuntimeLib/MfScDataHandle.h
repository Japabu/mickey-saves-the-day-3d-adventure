// MfScDataHandle (PiccoloRT.dll): a reference-counted handle to a Piccolo
// value. The small accessors are inline in the original header (see
// src/PiccoloRT/MfScDataHandle_unit.h).
#ifndef RUNTIMELIB_MFSCDATAHANDLE_H
#define RUNTIMELIB_MFSCDATAHANDLE_H
class MfCriticalSection;
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

class __declspec(dllimport) MfScDataHandle {
public:
    MfScData *GetData() const { return (mFlags & 0xfc0) ? mData : 0; }
    MfScDataHandle * GetPointer() const;
    MfScDataHandle * Package();
    MfScDataHandle();
    MfString TypeInfo();
    float GetFloat() const { return mFloat; }
    float GetNumber() const { return (mFlags & 0x10) ? (float)mLong : mFloat; }
    int GetInt() const;
    int IsReadOnly() const { return mReadOnly; }
    int IsType(unsigned long type) const { return mFlags & type; }
    int IsVisiting() const { return mVisiting; }
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
    static int __cdecl IsGCRunning();
    static int __cdecl WaitForGC(unsigned int);
    static unsigned long __cdecl GetGCHints();
    static void * __cdecl operator new(unsigned int, int);
    static void __cdecl AddGCHint(unsigned long);
    static void __cdecl DisableGCForVM();
    static void __cdecl DumpPool();
    static void __cdecl DumpPool(int);
    static void __cdecl EnableGCForVM();
    static void __cdecl EnumHandles(int, MfScHandleEnum &);
    static void __cdecl LockGC();
    static void __cdecl RemoveGCHint(unsigned long);
    static void __cdecl Shutdown();
    static void __cdecl ShutdownGCTimeoutThread();
    static void __cdecl UnlockGC();
    static void __cdecl operator delete(void *);
    unsigned int GetType() const;
    unsigned short AddRef() { return ++mRefCount; }
    unsigned short GetRefCount() const;
    unsigned short Release() { return mRefCount > 0 ? --mRefCount : mRefCount; }
    void InfoLong(MfString *);
    void InfoShort(MfString *);
    void ResetVisiting() { mVisiting = 0; }
    void SetData(MfScData *);
    void SetFloat(float);
    void SetInt(int);
    void SetPointer(MfScDataHandle *);
    void SetReadOnly() { mReadOnly = 1; }
    void SetReadWrite() { mReadOnly = 0; }
    void SetType(unsigned long type) { mFlags |= (type & 0x1ff0); }
    void SetVisiting() { mVisiting = 1; }
    ~MfScDataHandle();
private:
    MfScDataHandle & operator=(MfScDataHandle const &);
    MfScDataHandle(MfScDataHandle const &);
    int IsValidHandle() const;
    static MfScDataHandle * __cdecl GrowHeap(int);
    static int __cdecl GarbageCollectContainer(MfScHandleHeap *, int, int);
    static int __cdecl GarbageCollectNotContainer(MfScHandleHeap *, int, int);
    static void __cdecl LockHeap();
    static void __cdecl SignalGCIsQuitting();
    static void __cdecl SignalGCIsRunning();
    static void __cdecl UnlockHeap();
    static MfScHandleHeap * * aphhp;
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
public:
    unsigned short mRefCount;                               // 0x00
    union {                                                 // 0x02
        unsigned short mFlags;  // 0x10 integer, 0xfc0 data (object) types, 0x1ff0 type mask
        struct {
            unsigned short mBit0 : 1;
            unsigned short mBit1 : 1;
            unsigned short mVisiting : 1;
            unsigned short mReadOnly : 1;
        };
    };
    union {                                                 // 0x04
        float mFloat;
        long mLong;
        MfScData *mData;
    };
};

#endif
