// MfScDataHandle (PiccoloRT.dll): a reference-counted handle to a Piccolo
// value. Members from the SteamboatApp PDB; the small accessors are inline in
// the original header (MfPoLibrary imports none of them).
#pragma once
class MfCriticalSection;
class MfScData;
class MfScDataArray;
class MfScDataHandle;
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

// a method a script object registers: (args, number of args, return value)
typedef MfScDataHandle *(MfScData::*MfScMethod)(MfScDataHandle *, int, MfScDataHandle **);

class __declspec(dllimport) MfScDataHandle {
    enum {
        GC_NO_HINTS = 0,
        GC_GARBAGE_COLLECT = 1,
        GC_SHUTDOWN = 2,
        GC_DISABLE_THREAD = 4
    };
public:
    enum {
        HF_VISITING = 0x4,
        HF_READ_ONLY = 0x8,
        HT_INTEGER = 0x10,
        HT_FLOAT = 0x20,
        HT_NUMBER = 0x30,
        HT_STRING = 0x100,
        HT_OBJECT = 0x200,
        HT_ARRAY = 0x400,
        HT_DATA = 0xfc0
    };
    MfScDataHandle();
    ~MfScDataHandle();
    unsigned short AddRef() { return ++udwRefCount; }
    unsigned short Release() { return udwRefCount > 0 ? --udwRefCount : udwRefCount; }
    unsigned short GetRefCount() const { return udwRefCount; }
    static int __cdecl GetNumHandles(int);
    static void __cdecl DumpPool(int);
    static void __cdecl DumpPool();
    static void __cdecl EnumHandles(int, MfScHandleEnum &);
    static MfString __cdecl HeapTypeName(int);
    void InfoShort(MfString *);
    void InfoLong(MfString *);
    unsigned int GetType() const;
    int IsType(unsigned long type) const { return udwFlags & type; }
    void SetType(unsigned long);
    int IsReadOnly() const { return udwFlags & HF_READ_ONLY; }
    void SetReadOnly() { udwFlags |= HF_READ_ONLY; }
    void SetReadWrite() { udwFlags &= ~HF_READ_ONLY; }
    int IsVisiting() const { return udwFlags & HF_VISITING; }
    void SetVisiting() { udwFlags |= HF_VISITING; }
    void ResetVisiting() { udwFlags &= ~HF_VISITING; }
    int GetInt() const { return dataInt; }
    void SetInt(int i) { dataInt = i; }
    float GetFloat() const { return dataFloat; }
    void SetFloat(float f) { dataFloat = f; }
    float GetNumber() const { return (udwFlags & HT_INTEGER) ? (float)dataInt : dataFloat; }
    MfScData *GetData() const { return (udwFlags & HT_DATA) ? pdData : 0; }
    void SetData(MfScData *data) { pdData = data; }
    MfScDataHandle *GetPointer() const;
    void SetPointer(MfScDataHandle *pointer) { hpPointer = pointer; }
    static void *__cdecl operator new(unsigned int, int);
    static void __cdecl operator delete(void *);
    MfScDataHandle *Package();
    static MfScDataHandle *__cdecl CreateData(MfScData *, int, MfScMethod, const MfString &, int);
    static MfScDataHandle *__cdecl CreateData(MfScData *, MfScMethod, const MfString &, int);
    static MfScDataHandle *__cdecl CreateData(int, MfScMethod, const MfString &, int);
    static MfScDataHandle *__cdecl CreateData(MfScMethod, const MfString &, int);
    static MfScDataHandle *__cdecl CreateData(MfScDataHandle *(__cdecl *)(MfScDataHandle *, int, MfScDataHandle **), const MfString &, int);
    static MfScDataHandle *__cdecl CreateData(MfScDataObject *);
    static MfScDataHandle *__cdecl CreateData(const MfVariant &, int);
    static MfScDataHandle *__cdecl CreateData(const MfString &, int);
    static MfScDataHandle *__cdecl CreateData(const MfScObjFloat &);
    static MfScDataHandle *__cdecl CreateData(float);
    static MfScDataHandle *__cdecl CreateData(const MfScObjInteger &);
    static MfScDataHandle *__cdecl CreateData(int);
    static MfScDataHandle *__cdecl CreateBytecodeFunction(const MfString &, int);
    static MfScDataHandle *__cdecl CreateDataArray(int);
    static MfScDataHandle *__cdecl CreateDataArrayHandle(MfScDataArray *);
    static void __cdecl Shutdown();
    static void __cdecl ShutdownGCTimeoutThread();
    static void __cdecl AddGCHint(unsigned long);
    static void __cdecl RemoveGCHint(unsigned long);
    static unsigned long __cdecl GetGCHints();
    MfString TypeInfo();
    static void __cdecl DisableGCForVM();
    static void __cdecl EnableGCForVM();
    static int __cdecl WaitForGC(unsigned int);
    static int __cdecl IsGCRunning();
    static void __cdecl LockGC();
    static void __cdecl UnlockGC();
    static int __cdecl GarbageCollect();
private:
    MfScDataHandle(const MfScDataHandle &);
    MfScDataHandle &operator=(const MfScDataHandle &);
    int IsValidHandle() const;
    static unsigned long udwHints;
    static long lDisableGC;
    static void __cdecl SignalGCIsRunning();
    static void __cdecl SignalGCIsQuitting();
    static long lGCrunning;
    static MfScHandleHeap *aphhp[3];
    static unsigned long dwGCTick;
    static MfThread *ptGCThread;
    static MfScGCWork *pgcwWork;
    static MfThread *ptGCTimerThread;
    static MfScGCTimerWork *pgctwWork;
    static long lGCTimeOut;
    static MfScDataHandle *__cdecl GrowHeap(int);
    static void __cdecl LockHeap();
    static void __cdecl UnlockHeap();
    static int __cdecl GarbageCollectNotContainer(MfScHandleHeap *, int, int);
    static int __cdecl GarbageCollectContainer(MfScHandleHeap *, int, int);

    unsigned short udwRefCount;                             // 0x00
    unsigned short udwFlags;                                // 0x02
    union {                                                 // 0x04
        MfScData *pdData;
        MfScDataHandle *pdhNext;
        MfScDataHandle *hpPointer;
        int dataInt;
        float dataFloat;
        unsigned int dataBoolean;
    };
};
