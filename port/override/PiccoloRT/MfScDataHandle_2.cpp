// PORT: the handle diagnostics print pointers with %p (the source printed their low 32 bits with %x).
// PiccoloRT.dll 0x100050f0-0x100072ff: MfScDataHandle allocation, garbage collection, diagnostics.
#include "MfScDataHandle.h"
#include "MfScHandleHeap.h"
#include "MfScObject.h"
#include "MfThread.h"
#include "MfScGCWork.h"
#include "MfVariant.h"
#include "MfScRoutine.h"
#include "MfScDataArray.h"
#include "MfScBytecodeRoutine.h"
#include "MfScHandleEnum.h"
#include "MfScVmStack.h"
#include "MfScGcThread.h"
#include "MfScDiagnostics.h"
#include "MfStopWatch.h"
#include "MfSleeper.h"
#include "MfHPTimer.h"
#include "MfScThreadManager.h"
#include "MfScPiccolo.h"
#include "MfAiFunctionRegistry.h"
#include "MfScLinkData.h"
#include "MfScData.h"
#include <typeinfo.h>
#include <stdio.h>

extern __declspec(dllimport) MfNullString NULL_STRING;

MfCriticalSection MfScDataHandle::csGCGuard;
MfScHandleHeap *MfScDataHandle::aphhp[3];          // 0x10034f90
unsigned int MfScDataHandle::dwGCTick;
MfThread *MfScDataHandle::ptGCThread;
MfScGCWork *MfScDataHandle::pgcwWork;
MfThread *MfScDataHandle::ptGCTimerThread;
MfScGCTimerWork *MfScDataHandle::pgctwWork;
int MfScDataHandle::lGCTimeOut;
unsigned int MfScDataHandle::udwHints;
int MfScDataHandle::lDisableGC;
int MfScDataHandle::lGCrunning;                   // 0x10034fbc

// 0x10005140 ??0MfScDataHandle@@QAE@XZ
MfScDataHandle::MfScDataHandle()
{
    m_refCount = 1;
    m_flags = 2;
    m_data = 0;
}

// 0x10005160 ??1MfScDataHandle@@QAE@XZ
MfScDataHandle::~MfScDataHandle()
{
    if (IsType(0xfc0) && m_data)
        delete m_data;
    if (IsType(0x1000) && m_pointer)
        m_pointer->Release();
    m_flags = 1;
}

// 0x100051a0 ?ShutdownGCTimeoutThread@MfScDataHandle@@SAXXZ
void __cdecl MfScDataHandle::ShutdownGCTimeoutThread()
{
    if (ptGCTimerThread) {
        ptGCTimerThread->SignalShutdown(0);
        ptGCTimerThread = 0;
    }
    if (pgctwWork) {
        delete pgctwWork;
        pgctwWork = 0;
    }
}

// 0x100051e0 ?Shutdown@MfScDataHandle@@SAXXZ
void __cdecl MfScDataHandle::Shutdown()
{
    ptGCThread->SignalShutdown(0);
    pgcwWork->Quit();
    int tries = 10;
    while (tries--) {
        if (ptGCThread->GetState() & 0x10)
            break;
        Sleep(100);
    }
    ShutdownGCTimeoutThread();
    AddGCHint(2);
    GarbageCollect();
    if (ptGCThread)
        delete ptGCThread;
    ptGCThread = 0;
    if (pgcwWork)
        delete pgcwWork;
    pgcwWork = 0;
    for (int i = 0; i < 3; i++) {
        delete aphhp[i];
        aphhp[i] = 0;
    }
}

// 0x100052a0 ?Package@MfScDataHandle@@QAEPAV1@XZ
MfScDataHandle *MfScDataHandle::Package()
{
    MfScDataHandle *h = new(1) MfScDataHandle;
    AddRef();
    h->SetType(0x1000);
    h->SetPointer(this);
    return h;
}

// 0x100052e0 ?IsValidHandle@MfScDataHandle@@ABEHXZ
int MfScDataHandle::IsValidHandle() const
{
    MfScHandleHeap *heap = 0;
    aphhp[0]->m_cs.Enter();
    if (aphhp[0]->Contains((MfScDataHandle *)this))
        heap = aphhp[0];
    aphhp[0]->m_cs.Leave();
    aphhp[1]->m_cs.Enter();
    if (!heap && aphhp[1]->Contains((MfScDataHandle *)this))
        heap = aphhp[1];
    aphhp[1]->m_cs.Leave();
    aphhp[2]->m_cs.Enter();
    if (!heap && aphhp[2]->Contains((MfScDataHandle *)this))
        heap = aphhp[2];
    aphhp[2]->m_cs.Leave();
    if (!heap)
        return 0;
    heap->m_cs.Enter();
    heap->m_cs.Leave();
    return 1;
}

// EQUIV 0x100053a0 ?InfoShort@MfScDataHandle@@QAEXPAVMfString@@@Z: reviewed: the boxed float is passed through fld/fstp in the original and copied as a dword in ours; same calls and branches
void MfScDataHandle::InfoShort(MfString *info)
{
    *info = NULL_STRING;
    if (IsType(0x10))
        *info << (int)m_long;
    else if (IsType(0x20))
        *info << m_float;
    else if (IsType(0x100))
        *info << *(MfString *)(MfScObjString *)m_data;
    else if (IsType(0x40))
        *info << ((MfScObjInteger *)m_data)->mValue;
    else if (IsType(0x80))
        *info << ((MfScObjFloat *)m_data)->Value();
    else
        *info << "<OBJECT>";
}

// EQUIV 0x10005460 ?InfoLong@MfScDataHandle@@QAEXPAVMfString@@@Z: reviewed: as InfoShort, the boxed float is passed through fld/fstp in the original and copied as a dword in ours; the later branches differ only in register choice; same calls, arguments and branches
void MfScDataHandle::InfoLong(MfString *info)
{
    char address[24]; // PORT: room for a 64-bit %p
    info->Format("\tHandle %p(%s): Count = %d: ", (void *)this, /* PORT: %p */ IsReadOnly() ? "RO" : "RW", m_refCount);
    if (IsType(0x10))
        *info << "DATA_INT: " << (int)m_long;
    else if (IsType(0x20))
        *info << "DATA_FLOAT: " << m_float;
    else if (IsType(0x100))
        *info << "DATA_STRING: '" << *(MfString *)(MfScObjString *)m_data << "'";
    else if (IsType(0x40))
        *info << "DATA_OINT: " << ((MfScObjInteger *)m_data)->mValue;
    else if (IsType(0x80))
        *info << "DATA_OFLOAT: " << ((MfScObjFloat *)m_data)->mValue;
    else if (IsType(0x1000)) {
        sprintf(address, "%p", (void *)m_pointer); // PORT: %p
        *info << "DATA_POINTER: --> " << address;
    } else if (IsType(0x800))
        *info << "DATA_METHOD: " << m_data->Name();
    else if (IsType(0x200))
        *info << "DATA_OBJECT: " << m_data->Name();
    *info << " TYPE: " << TypeInfo();
}

// 0x10005610 ?CreateData@MfScDataHandle@@SAPAV1@H@Z
MfScDataHandle * __cdecl MfScDataHandle::CreateData(int value)
{
    MfScDataHandle *h = new(0) MfScDataHandle;
    h->SetType(0x10);
    h->SetInt(value);
    return h;
}

// 0x10005650 ?CreateData@MfScDataHandle@@SAPAV1@ABVMfScObjInteger@@@Z
MfScDataHandle * __cdecl MfScDataHandle::CreateData(MfScObjInteger const &value)
{
    MfScDataHandle *h = new(1) MfScDataHandle;
    MfScObjInteger *object = new MfScObjInteger(value.Value());
    h->SetType(0x40);
    h->SetData(object);
    return h;
}

// 0x100056d0 ?CreateData@MfScDataHandle@@SAPAV1@M@Z
MfScDataHandle * __cdecl MfScDataHandle::CreateData(float value)
{
    MfScDataHandle *h = new(0) MfScDataHandle;
    h->SetType(0x20);
    h->SetFloat(value);
    return h;
}

// 0x10005710 ?CreateData@MfScDataHandle@@SAPAV1@ABVMfScObjFloat@@@Z
MfScDataHandle * __cdecl MfScDataHandle::CreateData(MfScObjFloat const &value)
{
    MfScDataHandle *h = new(1) MfScDataHandle;
    MfScObjFloat *object = new MfScObjFloat(value.Value());
    h->SetType(0x80);
    h->SetData(object);
    return h;
}

// 0x10005790 ?CreateData@MfScDataHandle@@SAPAV1@ABVMfString@@H@Z
MfScDataHandle * __cdecl MfScDataHandle::CreateData(MfString const &value, int readOnly)
{
    MfScDataHandle *h = new(1) MfScDataHandle;
    MfScObjString *object = new MfScObjString(readOnly);
    *(MfString *)object = value;
    h->SetType(0x100);
    h->SetData(object);
    return h;
}

// 0x10005830 ?CreateData@MfScDataHandle@@SAPAV1@ABVMfVariant@@H@Z
MfScDataHandle * __cdecl MfScDataHandle::CreateData(MfVariant const &value, int readOnly)
{
    if (value.IsInt())
        return CreateData((int)value);
    if (value.IsFloat())
        return CreateData((float)value);
    return CreateData(MfString(value), readOnly);
}

// 0x100058f0 ?CreateData@MfScDataHandle@@SAPAV1@PAVMfScDataObject@@@Z
MfScDataHandle * __cdecl MfScDataHandle::CreateData(MfScDataObject *object)
{
    MfScDataHandle *h;
    if (object->Flags() & 0x400)
        h = new(2) MfScDataHandle;
    else
        h = new(1) MfScDataHandle;
    h->SetData(object);
    h->SetType(0x200);
    return h;
}

// 0x10005950 ?CreateData@MfScDataHandle@@SAPAV1@P6APAV1@PAV1@HPAPAV1@@ZABVMfString@@H@Z
MfScDataHandle * __cdecl MfScDataHandle::CreateData(MfScDataHandle *(__cdecl *function)(MfScDataHandle *, int, MfScDataHandle **),
                                                    MfString const &name, int numArgs)
{
    MfScDataHandle *h = new(1) MfScDataHandle;
    MfScFunctionRoutine *routine = new MfScFunctionRoutine(function, numArgs);
    routine->Name() = name;
    h->SetType(0x800);
    h->SetData(routine);
    return h;
}

// 0x100059f0 ?CreateData@MfScDataHandle@@SAPAV1@P8MfScData@@AEPAV1@PAV1@HPAPAV1@@ZABVMfString@@H@Z
MfScDataHandle * __cdecl MfScDataHandle::CreateData(MfScDataHandle *(MfScData::*method)(MfScDataHandle *, int, MfScDataHandle **),
                                                    MfString const &name, int numArgs)
{
    MfScDataHandle *h = new(1) MfScDataHandle;
    MfScMethodRoutine *routine = new MfScMethodRoutine(method, numArgs);
    routine->Name() = name;
    h->SetType(0x800);
    h->SetData(routine);
    return h;
}

// 0x10005aa0 ?CreateData@MfScDataHandle@@SAPAV1@HP8MfScData@@AEPAV1@PAV1@HPAPAV1@@ZABVMfString@@H@Z
MfScDataHandle * __cdecl MfScDataHandle::CreateData(int ordinal, MfScDataHandle *(MfScData::*method)(MfScDataHandle *, int, MfScDataHandle **),
                                                    MfString const &name, int numArgs)
{
    MfScDataHandle *h = new(1) MfScDataHandle;
    MfScMethodRoutine *routine = new MfScMethodRoutine(ordinal, method, numArgs);
    routine->Name() = name;
    h->SetType(0x800);
    h->SetData(routine);
    return h;
}

// 0x10005b50 ?CreateData@MfScDataHandle@@SAPAV1@PAVMfScData@@P82@AEPAV1@PAV1@HPAPAV1@@ZABVMfString@@H@Z
MfScDataHandle * __cdecl MfScDataHandle::CreateData(MfScData *object, MfScDataHandle *(MfScData::*method)(MfScDataHandle *, int, MfScDataHandle **),
                                                    MfString const &name, int numArgs)
{
    MfScDataHandle *h = new(1) MfScDataHandle;
    MfScExtRoutine *routine = new MfScExtRoutine(object, method, numArgs);
    routine->Name() = name;
    h->SetType(0x800);
    h->SetData(routine);
    return h;
}

// 0x10005c00 ?CreateData@MfScDataHandle@@SAPAV1@PAVMfScData@@HP82@AEPAV1@PAV1@HPAPAV1@@ZABVMfString@@H@Z
MfScDataHandle * __cdecl MfScDataHandle::CreateData(MfScData *object, int ordinal,
                                                    MfScDataHandle *(MfScData::*method)(MfScDataHandle *, int, MfScDataHandle **),
                                                    MfString const &name, int numArgs)
{
    MfScDataHandle *h = new(1) MfScDataHandle;
    MfScExtRoutine *routine = new MfScExtRoutine(object, ordinal, method, numArgs);
    routine->Name() = name;
    h->SetType(0x800);
    h->SetData(routine);
    return h;
}

// 0x10005cb0 ?CreateBytecodeFunction@MfScDataHandle@@SAPAV1@ABVMfString@@H@Z
MfScDataHandle * __cdecl MfScDataHandle::CreateBytecodeFunction(MfString const &name, int numArgs)
{
    MfScDataHandle *h = new(1) MfScDataHandle;
    MfScBytecodeRoutine *routine = new MfScBytecodeRoutine(numArgs);
    routine->Name() = name;
    h->SetType(0x800);
    h->SetData(routine);
    return h;
}

// 0x10005d40 ?CreateDataArray@MfScDataHandle@@SAPAV1@H@Z
MfScDataHandle * __cdecl MfScDataHandle::CreateDataArray(int size)
{
    MfScDataHandle *h = new(2) MfScDataHandle;
    MfScDataArrayImp *array = new MfScDataArrayImp(size);
    h->SetType(0x400);
    h->SetData(array);
    return h;
}

// 0x10005dc0 ?CreateDataArrayHandle@MfScDataHandle@@SAPAV1@PAVMfScDataArray@@@Z
MfScDataHandle * __cdecl MfScDataHandle::CreateDataArrayHandle(MfScDataArray *array)
{
    MfScDataHandle *h;
    if (array->IsValid())
        h = new(2) MfScDataHandle;
    else
        h = new(1) MfScDataHandle;
    h->SetData(array);
    h->SetType(0x400);
    return h;
}

// EQUIV 0x10005e20 ??2MfScDataHandle@@SAPAXIH@Z: reviewed: the two throw sites are laid out in another order (the original merges their tails) and its frame is 4 bytes smaller (both throw temporaries share the argument slot); same calls, conditions, EH states and thrown type
void * __cdecl MfScDataHandle::operator new(size_t, int heapIndex)
{
    if (!pgctwWork) {
        pgctwWork = new MfScGCTimerWork;
        ptGCTimerThread = new MfThread(2);
        ptGCThread = new MfScGcThread(0);
        pgcwWork = new MfScGCWork;
        ptGCThread->AssignWork(pgcwWork);
        MfHPTimer::Initialize();
    }
    if (!aphhp[heapIndex]) {
        aphhp[0] = new MfScHandleHeap;
        aphhp[1] = new MfScHandleHeap;
        aphhp[2] = new MfScHandleHeap;
    }
    MfScHandleHeap *heap = aphhp[heapIndex];
    for (;;) {
        heap->m_cs.Enter();
        if (heap->m_numFree || GrowHeap(heapIndex))
            break;
        heap->m_cs.Leave();
        if (heapIndex == 0 || heapIndex == 1) {
            LockGC();
            GarbageCollectNotContainer(aphhp[heapIndex], heapIndex, 0);
            UnlockGC();
            if (heap->m_numFree <= 0)
                throw MfScMemoryException();
        } else {
            LockGC();
            GarbageCollectContainer(aphhp[heapIndex], heapIndex, 1);
            UnlockGC();
            throw MfScMemoryException();
        }
    }
    MfScDataHandle *handle = heap->m_free;
    if (handle)
        heap->m_free = handle->m_pointer;
    else
        handle = heap->m_nextUnused++;
    heap->m_numFree--;
    handle->m_refCount = 1;
    handle->m_flags = 2;
    handle->m_long = 0;
    heap->m_cs.Leave();
    return handle;
}

// 0x10006100 ??1MfScGCWork@@UAE@XZ
MfScGCWork::~MfScGCWork()
{
}

// 0x10006150 ??_GMfScGCWork@@UAEPAXI@Z
// 0x10006110 ??_GMfThreadWork@@UAEPAXI@Z

// 0x10006190 ??1MfScGCTimerWork@@UAE@XZ (compiler-generated)

// 0x10006170 ??_GMfScGCTimerWork@@UAEPAXI@Z
// 0x100061d0 ??_GMfThread@@UAEPAXI@Z

// 0x100061f0 ??3MfScDataHandle@@SAXPAX@Z
void __cdecl MfScDataHandle::operator delete(void *)
{
}

// 0x10006200 ?GrowHeap@MfScDataHandle@@CAPAV1@H@Z
MfScDataHandle * __cdecl MfScDataHandle::GrowHeap(int idx)
{
    return aphhp[idx]->GrowHeap();
}

// 0x10006210 ?WaitForGC@MfScDataHandle@@SAHI@Z
int __cdecl MfScDataHandle::WaitForGC(unsigned int timeout)
{
    if (!IsGCRunning())
        return 1;
    MfStopWatch watch;
    watch.Start(timeout);
    while (IsGCRunning() && !watch.HasExpired())
        MfSleeper::Snooze(10);
    return !IsGCRunning();
}

// 0x100062d0 ?GarbageCollect@MfScDataHandle@@SAHXZ
int __cdecl MfScDataHandle::GarbageCollect()
{
    int pause = 0;
    LockGC();
    int containers = GarbageCollectContainer(aphhp[2], 2, 1);
    int objects = GarbageCollectNotContainer(aphhp[1], 1, 1);
    if (containers || objects) {
        if (containers && (!objects || containers <= objects))
            pause = containers;
        if (objects && (!containers || objects <= containers))
            pause = objects;
    }
    GarbageCollectNotContainer(aphhp[0], 0, 1);
    UnlockGC();
    return pause;
}

// EQUIV 0x10006350 ?GarbageCollectNotContainer@MfScDataHandle@@CAHPAVMfScHandleHeap@@HH@Z: reviewed: the original stores collected = 0 both on entry and again after starting the timer (nothing changes it in between); otherwise the same instructions
int __cdecl MfScDataHandle::GarbageCollectNotContainer(MfScHandleHeap *heap, int heapIndex, int compact)
{
    int collected = 0;
    int released;
    int pause;      // left unset when nothing was collected
    if (!heap)
        return collected;
    static MfString line((MfStringSize(512)));
    if (MfScDiagnostics::IsOn(3)) {
        line = "MfScDataHandle::GarbageCollect(";
        line << HeapTypeName(heapIndex) << "): Collecting freed handles.";
        MfScDiagnostics::PrintNl(3, line);
    }
    heap->m_cs.Enter();
    MfHPTimer timer;
    timer.Start();

    // destroy the handles nobody refers to any more
    MfScDataHandle *first = (MfScDataHandle *)heap->m_stack->Base();
    MfScDataHandle *last = heap->m_nextUnused;
    if (!last)
        last = (MfScDataHandle *)heap->m_stack->CommittedEnd();
    last--;
    MfScDataHandle *handle;
    for (handle = last; handle >= first; handle--) {
        if (handle->IsType(2) && (handle->m_refCount == 0 || (udwHints & 2))) {
            handle->~MfScDataHandle();
            heap->m_numFree++;
            collected++;
        }
    }

    // chain the free handles, lowest first
    if (collected > 0) {
        heap->m_free = 0;
        for (handle = last; handle >= first; handle--) {
            if (handle->IsType(1)) {
                handle->m_pointer = heap->m_free;
                heap->m_free = handle;
            }
        }
        pause = 5;
    }

    // give the unused pages at the top back to the system
    released = 0;
    if (compact && (!heap->m_nextUnused ||
                    heap->m_numFree > MfScVmStack::PageSize() * 16 / sizeof(MfScDataHandle))) {
        for (handle = last; handle >= first; handle--) {
            if (!handle->IsType(1) && handle->m_flags)
                break;
        }
        if (handle < last - 1) {
            handle++;
            MfScDataHandle *minimum = first + MfScVmStack::PageSize();
            if (handle < minimum)
                handle = minimum;
            unsigned int decommitted;
            unsigned int size = heap->m_stack->CommittedEnd() - (char *)handle;
            heap->m_stack->Decommit(size, &decommitted);
            if (decommitted > 0) {
                released = decommitted / sizeof(MfScDataHandle);
                heap->m_numFree -= released;
                heap->m_numHandles -= released;
                heap->m_nextUnused = handle;
                if (heap->m_free == handle)
                    heap->m_free = 0;
                else {
                    MfScDataHandle *free = heap->m_free;
                    while (free && free->m_pointer < handle)
                        free = free->m_pointer;
                    if (free)
                        free->m_pointer = 0;
                }
            }
        }
    }
    timer.Stop();
    heap->m_cs.Leave();

    if (MfScDiagnostics::IsOn(3)) {
        line = "MfScDataHandle::GarbageCollect(";
        line << HeapTypeName(heapIndex) << "): " << collected << " handles in " << timer.GetElapsedTimeInMs()
             << " ms.";
        MfScDiagnostics::PrintNl(3, line);
        if (released > 0) {
            line = "MfScDataHandle::GarbageCollect(";
            line << HeapTypeName(heapIndex) << "): ";
            line << released << " handles were returned to the OS.";
            MfScDiagnostics::PrintNl(3, line);
        }
        line = "MfScDataHandle::GarbageCollect(";
        line << HeapTypeName(heapIndex) << "): There are now ";
        line << (int)heap->m_numFree << "/" << (int)heap->m_numHandles << " free handles available.";
        MfScDiagnostics::PrintNl(3, line);
    }
    return pause;
}

// 0x10006790 ?GarbageCollectContainer@MfScDataHandle@@CAHPAVMfScHandleHeap@@HH@Z
// collects the container handles: besides the unreferenced ones, data
// objects whose only references are circular links; gives up early when the
// timer thread says the collection takes too long
int __cdecl MfScDataHandle::GarbageCollectContainer(MfScHandleHeap *heap, int heapIndex, int compact)
{
    int suspended;
    int pause;
    pause = 0;
    suspended = 0;
    static int nSkipped;
    if (nSkipped >= 10) {
        gThreadManager->SuspendAll();
        suspended = 1;
    } else if (lDisableGC) {
        nSkipped++;
        return 5;
    }
    nSkipped = 0;
    SignalGCIsRunning();
    static MfString line((MfStringSize(512)));
    if (MfScDiagnostics::IsOn(3)) {
        line = "MfScDataHandle::GarbageCollect(";
        line << HeapTypeName(heapIndex) << "): Collecting freed handles.";
        MfScDiagnostics::PrintNl(3, line);
    }
    MfAiFunctionRegistry *registry = MfScPiccolo::GetFunctionRegistry();
    if (registry)
        registry->CheckOrphans();
    heap->m_cs.Enter();
    MfHPTimer timer;
    timer.Start();
    int collected = 0;
    InterlockedExchange(&lGCTimeOut, 0);
    if (ptGCTimerThread && pgctwWork)
        ptGCTimerThread->AssignWork(pgctwWork);

    // destroy what nobody refers to and break the circular links
    MfScDataHandle *first = (MfScDataHandle *)heap->m_stack->Base();
    MfScDataHandle *last = heap->m_nextUnused;
    if (!last)
        last = (MfScDataHandle *)heap->m_stack->CommittedEnd();
    last--;
    MfScDataHandle *handle;
    handle = last;
    while (handle >= first) {
        if (handle->IsType(2)) {
            if (handle->m_refCount == 0 || (udwHints & 2)) {
                handle->~MfScDataHandle();
                heap->m_numFree++;
                collected++;
            } else if (!handle->IsReadOnly() && handle->IsType(0x7c0)) {
                MfScData *data = handle->m_data;
                data->Lock();
                MfScLinkData *links = data->CountCircLinks(handle);
                if (links->mCount == handle->m_refCount)
                    data->ReleaseCircLinks(links);
                data->Unlock();
                delete links;
            }
        }
        handle--;
        if (InterlockedExchange(&lGCTimeOut, lGCTimeOut)) {
            InterlockedDecrement(&lGCTimeOut);
            if (collected) {
                pause = 5;
                break;
            }
            InterlockedExchange(&lGCTimeOut, 0);
            if (ptGCTimerThread && pgctwWork)
                ptGCTimerThread->AssignWork(pgctwWork);
        }
    }

    // chain the free handles, lowest first
    if (collected > 0) {
        handle = last;
        heap->m_free = 0;
        for (; handle >= first; handle--) {
            if (handle->IsType(1)) {
                handle->m_pointer = heap->m_free;
                heap->m_free = handle;
            }
        }
        pause = 5;
    }

    // give the unused pages at the top back to the system
    int released = 0;
    if (compact && (!heap->m_nextUnused ||
                    heap->m_numFree > MfScVmStack::PageSize() * 16 / sizeof(MfScDataHandle))) {
        for (handle = last; handle >= first; handle--) {
            if (!handle->IsType(1) && handle->m_flags)
                break;
        }
        if (handle < last - 1) {
            handle++;
            MfScDataHandle *minimum = first + MfScVmStack::PageSize();
            if (handle < minimum)
                handle = minimum;
            unsigned int decommitted;
            unsigned int size = heap->m_stack->CommittedEnd() - (char *)handle;
            heap->m_stack->Decommit(size, &decommitted);
            if (decommitted > 0) {
                released = decommitted / sizeof(MfScDataHandle);
                heap->m_numFree -= released;
                heap->m_numHandles -= released;
                heap->m_nextUnused = handle;
                if (heap->m_free == handle)
                    heap->m_free = 0;
                else {
                    MfScDataHandle *free = heap->m_free;
                    while (free && free->m_pointer < handle)
                        free = free->m_pointer;
                    if (free)
                        free->m_pointer = 0;
                }
            }
        }
    }
    if (pgctwWork)
        pgctwWork->WakeUp();
    timer.Stop();
    heap->m_cs.Leave();

    if (MfScDiagnostics::IsOn(3)) {
        line = "MfScDataHandle::GarbageCollect(";
        line << HeapTypeName(heapIndex) << "): freed " << collected << " handles in "
             << timer.GetElapsedTimeInMs() << " ms.";
        MfScDiagnostics::PrintNl(3, line);
        if (released > 0) {
            line = "MfScDataHandle::GarbageCollect(";
            line << HeapTypeName(heapIndex) << "): ";
            line << released << " handles were returned to the OS.";
            MfScDiagnostics::PrintNl(3, line);
        }
        line = "MfScDataHandle::GarbageCollect(";
        line << HeapTypeName(heapIndex) << "): There are now ";
        line << (int)heap->m_numFree << "/" << (int)heap->m_numHandles << " free handles available.";
        MfScDiagnostics::PrintNl(3, line);
    }
    SignalGCIsQuitting();
    if (suspended)
        gThreadManager->ResumeAll();
    return pause;
}

// 0x10006d50 ?LockHeap@MfScDataHandle@@CAXXZ
void __cdecl MfScDataHandle::LockHeap()
{
    LockGC();
    aphhp[0]->m_cs.Enter();
    aphhp[2]->m_cs.Enter();
    aphhp[1]->m_cs.Enter();
}

// 0x10006d90 ?UnlockHeap@MfScDataHandle@@CAXXZ
void __cdecl MfScDataHandle::UnlockHeap()
{
    aphhp[2]->m_cs.Leave();
    aphhp[1]->m_cs.Leave();
    aphhp[0]->m_cs.Leave();
    UnlockGC();
}

// 0x10006dd0 ?GetNumHandles@MfScDataHandle@@SAHH@Z
int __cdecl MfScDataHandle::GetNumHandles(int heapIndex)
{
    if (heapIndex >= 0)
        return aphhp[heapIndex]->m_numFree;
    return aphhp[2]->m_numFree + aphhp[1]->m_numFree + aphhp[0]->m_numFree;
}

// 0x10006e10 ?TypeInfo@MfScDataHandle@@QAE?AVMfString@@XZ
MfString MfScDataHandle::TypeInfo()
{
    MfString type;
    if (IsType(0x10))
        type = typeid(int).name();
    else if (IsType(0x20))
        type = typeid(float).name();
    else
        type = typeid(*m_data).name();
    MfString prefix = type.LeftString(5);
    if (prefix == "class")
        type = type.RightString(type.Length() - 6);
    return type;
}

// 0x10006f30 ?DumpPool@MfScDataHandle@@SAXXZ
void __cdecl MfScDataHandle::DumpPool()
{
    DumpPool(0);
    DumpPool(1);
    DumpPool(2);
}

// 0x10006f50 ?DumpPool@MfScDataHandle@@SAXH@Z
void __cdecl MfScDataHandle::DumpPool(int heapIndex)
{
    MfScHandleHeap *heap = aphhp[heapIndex];
    if (!heap || !heap->m_stack)
        return;
    heap->m_cs.Enter();
    static MfString line((MfStringSize(512)));
    line = "MfScDataHandle::DumpPool(): ";
    line << HeapTypeName(heapIndex) << ", There are " << (int)heap->m_numFree << "/" << (int)heap->m_numHandles
         << " free handles available.";
    MfScDiagnostics::PrintNl(1, line);
    MfScDataHandle *handle = (MfScDataHandle *)heap->m_stack->Base();
    MfScDataHandle *end = heap->m_nextUnused;
    if (!end)
        end = (MfScDataHandle *)heap->m_stack->CommittedEnd();
    for (; handle < end; handle++) {
        if (handle->IsType(2)) {
            handle->InfoLong(&line);
            MfScDiagnostics::PrintNl(1, line);
        }
    }
    heap->m_cs.Leave();
}

// 0x100070c0 ?EnumHandles@MfScDataHandle@@SAXHAAVMfScHandleEnum@@@Z
void __cdecl MfScDataHandle::EnumHandles(int heapIndex, MfScHandleEnum &visitor)
{
    LockGC();
    for (int i = 0; i < 3; i++) {
        if (i != heapIndex && heapIndex != 4)
            continue;
        MfScHandleHeap *heap = aphhp[i];
        if (!heap || !heap->m_stack)
            continue;
        heap->m_cs.Enter();
        MfScDataHandle *handle = (MfScDataHandle *)heap->m_stack->Base();
        MfScDataHandle *end = heap->m_nextUnused ? heap->m_nextUnused
                                                 : (MfScDataHandle *)heap->m_stack->CommittedEnd();
        for (; handle < end; handle++) {
            if (handle->IsType(2) && !visitor.CallBack(handle)) {
                heap->m_cs.Leave();
                UnlockGC();
                return;
            }
        }
        heap->m_cs.Leave();
    }
    UnlockGC();
}

// 0x10007190 ?HeapTypeName@MfScDataHandle@@SA?AVMfString@@H@Z
MfString __cdecl MfScDataHandle::HeapTypeName(int heapIndex)
{
    switch (heapIndex) {
    case 0:
        return MfString("HHI_PRIMITIVE");
    case 1:
        return MfString("HHI_NOT_CONTAINER");
    case 2:
        return MfString("HHI_CONTAINER");
    }
    return MfString("HHI_UNKNOWN");
}

// 0x10007210 ?Work@MfScGCWork@@UAEXXZ
void MfScGCWork::Work()
{
    int pause = 0;
    for (;;) {
        int naps = 60;
        if (pause)
            naps = pause * 1000 / 500;
        int start = naps;
        while (naps--) {
            if (InterlockedExchange(&mQuit, mQuit))
                return;
            Sleep(500);
            if ((MfScDataHandle::udwHints & 1) && (start - naps) * 500 >= 5000) {
                MfScDataHandle::udwHints &= ~1;
                break;
            }
        }
        if (MfScDataHandle::udwHints & 4)
            continue;
        MfScDataHandle::dwGCTick = GetTickCount();
        pause = MfScDataHandle::GarbageCollect();
        if (pause && pause * 1000 < 5000)
            pause = 5;
    }
}

// 0x10007300 ?Quit@MfScGCWork@@QAEXXZ
void MfScGCWork::Quit()
{
    InterlockedIncrement(&mQuit);
}

// 0x10007310 ?Work@MfScGCTimerWork@@UAEXXZ
void MfScGCTimerWork::Work()
{
    mWakeUp.Wait(150);
    InterlockedIncrement(&MfScDataHandle::lGCTimeOut);
}


// Matched by this code:
// 0x100050f0 _$E39
// 0x10005100 _$E35
// 0x10005110 _$E38
// 0x10005120 _$E36
// 0x10006780 _$E42 (destructor of GarbageCollectNotContainer's line)
// 0x10006d40 _$E44 (destructor of GarbageCollectContainer's line)
// 0x100070b0 _$E46 (destructor of DumpPool's line)
