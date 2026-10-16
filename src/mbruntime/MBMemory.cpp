// The muffin heap: low-memory handlers and the debug heap (MBmalloc_debug and
// friends), which wraps every block in a header and guard bytes and keeps all
// live blocks in a list for leak and damage reports.
#include "MBRuntime.h"
#include <map>

// Runtime/MBNew.h: the global operators route to the release heap.
inline void *__cdecl operator new(unsigned int size)
{
    return MBmalloc_release(size);
}

inline void __cdecl operator delete(void *p)
{
    if (p)
        MBfree_release(p);
}

extern "C" {
void __cdecl MBDebugStr(const char *fmt, ...);
void __cdecl MBatexit(void (__cdecl *func)(void));
void __cdecl MBmemcpy(void *dest, const void *src, unsigned int cnt);
void __cdecl MBmemset(void *dest, unsigned char fill, unsigned int cnt);
void __cdecl MBmemclr(void *dest, unsigned int cnt);
bool __cdecl MBCheckHeap(void);
}

typedef bool (__cdecl *MBLowMemoryHandler)(unsigned long sizeNeeded);

// Debug block: header, 16 guard bytes, the user block, 16 guard bytes.
struct MBDebugHeapHeader {
    long dummy;
    long id;                    // 'MBDN'
    unsigned long size;         // user block size
    MBDebugHeapHeader *prev;
    MBDebugHeapHeader *next;
    unsigned long index;        // allocation number
    unsigned long filenameIndex;    // key into sFilenameMap (the file name pointer)
    unsigned long line;
};

enum {
    kMBDebugHeapID = 'MBDN',
    kMBDebugHeapGuardSize = 16,
    kMBDebugHeapUserOffset = sizeof(MBDebugHeapHeader) + kMBDebugHeapGuardSize,
    kMBDebugHeapOverhead = kMBDebugHeapUserOffset + kMBDebugHeapGuardSize,
    kMBDebugHeapGuardFill = 0xbc,
    kMBDebugHeapFreedFill = 0xbd,
    kMBMaxLowMemHandlers = 10,
    kMBMaxLeaksPrinted = 100
};

typedef std::map<unsigned long, std::string> MBFilenameMap;

static CMBCriticalSection *sCritSec;
static bool sCheckHeap;
static MBDebugHeapHeader sRoot;
static MBLowMemoryHandler loMemHandlers[kMBMaxLowMemHandlers];
static MBFilenameMap *sFilenameMap;
static bool sForceReleaseAlloc;

// 0x10001fc0 _MBAddLowMemoryHandler
extern "C" bool __cdecl MBAddLowMemoryHandler(MBLowMemoryHandler handler)
{
    for (long i = 0; i < kMBMaxLowMemHandlers; i++) {
        if (loMemHandlers[i] == 0) {
            loMemHandlers[i] = handler;
            return true;
        }
    }
    return false;
}

// 0x10001ff0 _MBRemoveLowMemoryHandler
extern "C" bool __cdecl MBRemoveLowMemoryHandler(MBLowMemoryHandler handler)
{
    for (long i = 0; i < kMBMaxLowMemHandlers; i++) {
        if (loMemHandlers[i] == handler) {
            loMemHandlers[i] = 0;
            return true;
        }
    }
    return false;
}

// 0x10002020 ?InvokeLowMemHandlers@@YA_NK@Z
bool InvokeLowMemHandlers(unsigned long sizeNeeded)
{
    bool freed = false;
    for (long i = 0; i < kMBMaxLowMemHandlers; i++) {
        if (loMemHandlers[i])
            freed = freed || loMemHandlers[i](sizeNeeded);
    }
    return freed;
}

// 0x10002060 ?MBDebugHeapLogPointerOrigin@@YAXPAUMBDebugHeapHeader@@@Z
void MBDebugHeapLogPointerOrigin(MBDebugHeapHeader *ptr)
{
    if (ptr && ptr->filenameIndex && sFilenameMap) {
        MBFilenameMap::iterator it = sFilenameMap->find(ptr->filenameIndex);
        MBDebugStr("%s(%d)", (*it).second.c_str(), ptr->line);
    }
}

// 0x100020d0 ?MBDebugHeapDumpLeaks@@YAXXZ
void MBDebugHeapDumpLeaks(void)
{
    CMBCriticalSectionLock lock(*sCritSec);
    long count = 0;
    MBDebugHeapHeader *header = sRoot.next;
    if (header != &sRoot) {
        MBDebugStr("\n********************************************************\n*\n"
                   "* hey, guess what...you've got MEMORY LEAKS\n*\n"
                   "********************************************************\n\n");
        do {
            if (count < kMBMaxLeaksPrinted) {
                MBDebugHeapLogPointerOrigin(header);
                MBDebugStr(": ERROR: MBDebugHeap: %d bytes leaked!\n     0x%08X (index %d)\n",
                           header->size, (char *)header + kMBDebugHeapUserOffset, header->index);
            }
            header = header->next;
            count++;
        } while (header != &sRoot);
        if (count > kMBMaxLeaksPrinted)
            MBDebugStr("WARNING: Leak count exceeds 100...only first 100 leaks printed.  Total leaks: %d.\n", count);
        MBDebugStr("\n********************************************************\n");
    }
}

// 0x10002160 ?MBDebugHeapTerminate@@YAXXZ
void MBDebugHeapTerminate(void)
{
    sForceReleaseAlloc = true;
    delete sCritSec;
    sCritSec = 0;
    delete sFilenameMap;
    sFilenameMap = 0;
    sForceReleaseAlloc = false;
}

// 0x100022d0 ?MBDebugHeapInit@@YAXXZ
void MBDebugHeapInit(void)
{
    sForceReleaseAlloc = true;
    sCritSec = new CMBCriticalSection;
    CMBCriticalSectionLock lock(*sCritSec);
    sFilenameMap = new MBFilenameMap;
    sForceReleaseAlloc = false;
    MBmemclr(&sRoot, sizeof(sRoot));
    sRoot.next = sRoot.prev = &sRoot;
    MBDebugStr(NULL);
    MBatexit(MBDebugHeapTerminate);
    MBatexit(MBDebugHeapDumpLeaks);
}

// 0x10002400 ?MBDebugHeapInitHeader@@YAJPAXKPBDJ@Z
long MBDebugHeapInitHeader(void *ptr, unsigned long requestSize, const char *file, long line)
{
    static long sIndex = 1;
    static bool sInited = false;

    if (!sInited) {
        sInited = true;
        MBDebugHeapInit();
    }
    MBDebugHeapHeader *header = (MBDebugHeapHeader *)ptr;
    MBmemclr(header, sizeof(MBDebugHeapHeader));
    header->id = kMBDebugHeapID;
    header->index = sIndex++;
    header->size = requestSize;
    header->filenameIndex = (unsigned long)file;
    header->line = line;

    CMBCriticalSectionLock lock(*sCritSec);
    header->prev = sRoot.prev;
    sRoot.prev->next = header;
    sRoot.prev = header;
    header->next = &sRoot;
    sForceReleaseAlloc = true;
    sFilenameMap->insert(std::pair<unsigned long, std::string>(header->filenameIndex, std::string(file)));
    sForceReleaseAlloc = false;
    return 1;
}

// 0x100025e0 ?MBDebugHeapFindHeaderFromPtr@@YAPAUMBDebugHeapHeader@@PAX@Z
MBDebugHeapHeader *MBDebugHeapFindHeaderFromPtr(void *ptr)
{
    if (!ptr)
        return 0;
    MBDebugHeapHeader *header = (MBDebugHeapHeader *)((char *)ptr - kMBDebugHeapUserOffset);
    return header->id == kMBDebugHeapID ? header : 0;
}

// 0x10002600 ?MBDebugHeapCheckHeaderValid@@YAPAUMBDebugHeapHeader@@PAX@Z
MBDebugHeapHeader *MBDebugHeapCheckHeaderValid(void *ptr)
{
    MBDebugHeapHeader *header = MBDebugHeapFindHeaderFromPtr(ptr);
    if (!header) {
        MBDebugStr(": ERROR: MBDebugHeap: Pointer 0x%08X header trashed.\n", ptr);
        MBDebugStr("                   There are 2 major ways to end up here, in order of likelihood:\n");
        MBDebugStr("                   1) you didn't allocate this pointer with new.\n");
        MBDebugStr("                   2) you have code that ran past an array boundary.\n");
        MBDebugBreak();
        return 0;
    }
    unsigned long size = header->size;
    unsigned long count;
    unsigned char *p;

    // a freed block is filled with kMBDebugHeapFreedFill
    for (count = header->size, p = (unsigned char *)ptr; count--; ) {
        if (*p++ != kMBDebugHeapFreedFill)
            break;
    }
    if (count == (unsigned long)-1 && size != 0) {
        MBDebugHeapLogPointerOrigin(header);
        MBDebugStr(": WARN: MBDebugHeap: Pointer 0x%08X was already deleted! (I think).\n", ptr);
        MBDebugBreak();
        return 0;
    }
    for (count = kMBDebugHeapGuardSize, p = (unsigned char *)(header + 1); count--; ) {
        if (*p++ != kMBDebugHeapGuardFill)
            break;
    }
    if (count != (unsigned long)-1) {
        MBDebugHeapLogPointerOrigin(header);
        MBDebugStr(": ERROR: MBDebugHeap: Damage to front of pointer block 0x%08X!.\n", ptr);
        MBDebugBreak();
        return 0;
    }
    for (count = kMBDebugHeapGuardSize, p = (unsigned char *)ptr + size; count--; ) {
        if (*p++ != kMBDebugHeapGuardFill)
            break;
    }
    if (count != (unsigned long)-1) {
        MBDebugHeapLogPointerOrigin(header);
        MBDebugStr(": ERROR: MBDebugHeap: Damage to end of pointer block 0x%08X!.\n", ptr);
        MBDebugBreak();
        return 0;
    }
    return header;
}

// 0x10002710 _MBCheckHeap
extern "C" bool __cdecl MBCheckHeap(void)
{
    if (sRoot.next) {
        CMBCriticalSectionLock lock(*sCritSec);
        for (MBDebugHeapHeader *header = sRoot.next; header != &sRoot; header = header->next) {
            if (!MBDebugHeapCheckHeaderValid((char *)header + kMBDebugHeapUserOffset)) {
                MBDebugHeapLogPointerOrigin(header);
                MBDebugStr("ERROR: Heap damage found.\n");
                MBDebugBreak();
                return false;
            }
        }
    }
    return true;
}

// 0x10002790 _MBCheckHeapOnAlloc
extern "C" bool __cdecl MBCheckHeapOnAlloc(bool doCheck)
{
    bool old = sCheckHeap;
    sCheckHeap = doCheck;
    return old;
}

// 0x100027a0 _MBmalloc_debug
extern "C" void *__cdecl MBmalloc_debug(unsigned int cb, const char *file, int line)
{
    if (sForceReleaseAlloc)
        return MBmalloc_release(cb);
    if (sCheckHeap)
        MBCheckHeap();
    unsigned long total = cb + kMBDebugHeapOverhead;
    void *block = MBmalloc_release(total);
    if (!block) {
        MBDebugStr("ERROR: MBmalloc_debug: OUT OF MEMORY!\n");
        MBDebugBreak();
        return 0;
    }
    MBmemset(block, kMBDebugHeapGuardFill, total);
    MBDebugHeapInitHeader(block, cb, file, line);
    if (sCheckHeap)
        MBCheckHeap();
    return (char *)block + kMBDebugHeapUserOffset;
}

// 0x10002830 _MBfree_debug
extern "C" void __cdecl MBfree_debug(void *ptr)
{
    if (sForceReleaseAlloc) {
        MBfree_release(ptr);
        return;
    }
    if (sCheckHeap)
        MBCheckHeap();
    MBDebugHeapHeader *header = MBDebugHeapCheckHeaderValid(ptr);
    if (header) {
        MBmemset(ptr, kMBDebugHeapFreedFill, header->size);
        if (sCritSec) {
            CMBCriticalSectionLock lock(*sCritSec);
            header->next->prev = header->prev;
            header->prev->next = header->next;
        } else {
            header->next->prev = header->prev;
            header->prev->next = header->next;
        }
        MBfree_release(header);
        if (sCheckHeap)
            MBCheckHeap();
    }
}

// 0x100028e0 _MBrealloc_debug
extern "C" void *__cdecl MBrealloc_debug(void *ptr, unsigned int sizeReq, const char *file, int line)
{
    if (sForceReleaseAlloc)
        return MBrealloc_release(ptr, sizeReq);
    if (sCheckHeap)
        MBCheckHeap();
    MBDebugHeapHeader *header = MBDebugHeapCheckHeaderValid(ptr);
    if (!header)
        return 0;
    MBDebugHeapHeader safeHeader = *header;
    unsigned char *block = (unsigned char *)MBrealloc_release(header, sizeReq + kMBDebugHeapOverhead);
    if (!block) {
        MBDebugStr("ERROR: MBrealloc_debug: OUT OF MEMORY!\n");
        MBDebugBreak();
        return 0;
    }
    unsigned char *newUserPtr = block + kMBDebugHeapUserOffset;
    if (sizeReq > header->size)
        MBmemset(newUserPtr + header->size, kMBDebugHeapGuardFill, sizeReq - header->size);
    safeHeader.size = sizeReq;
    safeHeader.filenameIndex = (unsigned long)file;
    safeHeader.line = line;
    sFilenameMap->insert(std::pair<unsigned long, std::string>(safeHeader.filenameIndex, std::string(file)));
    MBmemcpy(block, &safeHeader, sizeof(safeHeader));
    MBmemset(block + sizeof(MBDebugHeapHeader), kMBDebugHeapGuardFill, kMBDebugHeapGuardSize);
    MBmemset(block + kMBDebugHeapUserOffset + sizeReq, kMBDebugHeapGuardFill, kMBDebugHeapGuardSize);
    if (sCheckHeap)
        MBCheckHeap();
    return newUserPtr;
}


// Matched by this code:
// 0x10002b60 ?assign@?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@QAEAAV12@ABV12@II@Z
// 0x10002d70 ?_Tidy@?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AAEX_N@Z
// 0x10002dc0 ?begin@?$_Tree@KU?$pair@$$CBKV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@std@@U_Kfn@?$map@KV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@U?$less@K@2@V?$allocator@V?$basic_string@DU?$char_traits@D@std@@V?$allocato6dd3d01b
// 0x10002dd0 ?insert@?$_Tree@KU?$pair@$$CBKV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@std@@U_Kfn@?$map@KV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@U?$less@K@2@V?$allocator@V?$basic_string@DU?$char_traits@D@std@@V?$allocate7f0c07a
// 0x10003070 ?_Eos@?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AAEXI@Z
// 0x10003090 ?_Grow@?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AAE_NI_N@Z
// 0x10003240 ?erase@?$_Tree@KU?$pair@$$CBKV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@std@@U_Kfn@?$map@KV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@U?$less@K@2@V?$allocator@V?$basic_string@DU?$char_traits@D@std@@V?$allocatoe652acf7
// 0x10003750 ?_Erase@?$_Tree@KU?$pair@$$CBKV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@std@@U_Kfn@?$map@KV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@U?$less@K@2@V?$allocator@V?$basic_string@DU?$char_traits@D@std@@V?$allocate91a9cdc
// 0x100037e0 ?_Insert@?$_Tree@KU?$pair@$$CBKV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@std@@U_Kfn@?$map@KV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@U?$less@K@2@V?$allocator@V?$basic_string@DU?$char_traits@D@std@@V?$alloca7db5069b
// 0x10003ac0 ??Eiterator@?$_Tree@KU?$pair@$$CBKV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@std@@U_Kfn@?$map@KV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@U?$less@K@2@V?$allocator@V?$basic_string@DU?$char_traits@D@std@@V?$allcaefec3f
// 0x10003b50 ?max_size@?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@QBEIXZ
// 0x10003b60 ?_Copy@?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AAEXI@Z
// 0x10003c10 ?_Split@?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AAEXXZ
// 0x10003ce0 ?_Lbound@?$_Tree@KU?$pair@$$CBKV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@std@@U_Kfn@?$map@KV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@U?$less@K@2@V?$allocator@V?$basic_string@DU?$char_traits@D@std@@V?$alloca43a6cc42
// 0x10003d30 ?_Lrotate@?$_Tree@KU?$pair@$$CBKV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@std@@U_Kfn@?$map@KV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@U?$less@K@2@V?$allocator@V?$basic_string@DU?$char_traits@D@std@@V?$alloca2f7f720
// 0x10003d90 ?_Rrotate@?$_Tree@KU?$pair@$$CBKV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@std@@U_Kfn@?$map@KV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@U?$less@K@2@V?$allocator@V?$basic_string@DU?$char_traits@D@std@@V?$alloc8a89ab64
// 0x10003df0 ?_Buynode@?$_Tree@KU?$pair@$$CBKV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@std@@U_Kfn@?$map@KV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@U?$less@K@2@V?$allocator@V?$basic_string@DU?$char_traits@D@std@@V?$alloc8d4ad41d
// 0x10003e10 ?_Dec@const_iterator@?$_Tree@KU?$pair@$$CBKV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@std@@U_Kfn@?$map@KV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@U?$less@K@2@V?$allocator@V?$basic_string@DU?$char_traits@D@st68f90c53
// 0x10003eb0 ?_Inc@const_iterator@?$_Tree@KU?$pair@$$CBKV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@std@@U_Kfn@?$map@KV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@U?$less@K@2@V?$allocator@V?$basic_string@DU?$char_traits@D@st90bcd6ae
// 0x10003f40 ?_Construct@std@@YAXPAU?$pair@$$CBKV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@1@ABU21@@Z
// 0x10004070 _$E22
// 0x10004090 _$E21