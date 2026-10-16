// memory.cpp - Global/Local/Heap memory on malloc, Virtual memory on mmap, the
// pointer probes, and the system memory and processor reports.
#include <map>
#include <mutex>
#include "internal.h"

#include <mach/mach.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/sysctl.h>
#include <unistd.h>

namespace {

// A Global/Local/Heap block: the size sits in front of the memory. A handle and its
// locked pointer are the same address (GMEM_MOVEABLE memory never moves here).
struct Block {
    size_t size;
    size_t pad;  // keeps the memory 16-byte aligned
};

void *block_alloc(size_t n, bool zero)
{
    Block *b = (Block *)(zero ? calloc(1, sizeof(Block) + n) : malloc(sizeof(Block) + n));
    if (!b) {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        return 0;
    }
    b->size = n;
    return b + 1;
}

Block *block_of(const void *p) { return (Block *)p - 1; }

void *block_realloc(void *p, size_t n, bool zero)
{
    if (!p)
        return block_alloc(n, zero);
    size_t old = block_of(p)->size;
    Block *b = (Block *)realloc(block_of(p), sizeof(Block) + n);
    if (!b) {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        return 0;
    }
    b->size = n;
    if (zero && n > old)
        memset((char *)(b + 1) + old, 0, n - old);
    return b + 1;
}

struct HeapObject : win32::Object {
    HeapObject() : Object(Heap) {}
};
HeapObject s_process_heap;

}  // namespace

extern "C" {

HGLOBAL WINAPI GlobalAlloc(UINT uFlags, SIZE_T dwBytes) { return block_alloc(dwBytes, (uFlags & GMEM_ZEROINIT) != 0); }

HGLOBAL WINAPI GlobalReAlloc(HGLOBAL hMem, SIZE_T dwBytes, UINT uFlags)
{
    if (uFlags & GMEM_MODIFY)
        return hMem;
    return block_realloc(hMem, dwBytes, (uFlags & GMEM_ZEROINIT) != 0);
}

HGLOBAL WINAPI GlobalFree(HGLOBAL hMem)
{
    if (hMem)
        free(block_of(hMem));
    return 0;
}

LPVOID WINAPI GlobalLock(HGLOBAL hMem) { return hMem; }
BOOL WINAPI GlobalUnlock(HGLOBAL) { return TRUE; }
SIZE_T WINAPI GlobalSize(HGLOBAL hMem) { return hMem ? block_of(hMem)->size : 0; }

HLOCAL WINAPI LocalAlloc(UINT uFlags, SIZE_T uBytes) { return block_alloc(uBytes, (uFlags & LMEM_ZEROINIT) != 0); }

HLOCAL WINAPI LocalFree(HLOCAL hMem)
{
    if (hMem)
        free(block_of(hMem));
    return 0;
}

HANDLE WINAPI GetProcessHeap(void) { return &s_process_heap; }

HANDLE WINAPI HeapCreate(DWORD, SIZE_T, SIZE_T) { return new HeapObject; }

BOOL WINAPI HeapDestroy(HANDLE hHeap)
{
    // Blocks are not tracked per heap: what the game did not free stays allocated.
    if (hHeap && hHeap != &s_process_heap)
        win32::release(win32::object(hHeap));
    return TRUE;
}

LPVOID WINAPI HeapAlloc(HANDLE, DWORD dwFlags, SIZE_T dwBytes) { return block_alloc(dwBytes, (dwFlags & HEAP_ZERO_MEMORY) != 0); }

LPVOID WINAPI HeapReAlloc(HANDLE, DWORD dwFlags, LPVOID lpMem, SIZE_T dwBytes)
{
    if (dwFlags & HEAP_REALLOC_IN_PLACE_ONLY) {
        if (block_of(lpMem)->size >= dwBytes) {
            block_of(lpMem)->size = dwBytes;
            return lpMem;
        }
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        return 0;
    }
    return block_realloc(lpMem, dwBytes, (dwFlags & HEAP_ZERO_MEMORY) != 0);
}

BOOL WINAPI HeapFree(HANDLE, DWORD, LPVOID lpMem)
{
    if (lpMem)
        free(block_of(lpMem));
    return TRUE;
}

SIZE_T WINAPI HeapSize(HANDLE, DWORD, LPCVOID lpMem) { return lpMem ? block_of(lpMem)->size : (SIZE_T)-1; }

static int prot_of(DWORD protect)
{
    switch (protect & 0xff) {
    case PAGE_NOACCESS: return PROT_NONE;
    case PAGE_READONLY: return PROT_READ;
    case PAGE_EXECUTE_READ:
    case PAGE_EXECUTE: return PROT_READ;
    default: return PROT_READ | PROT_WRITE;
    }
}

// The reservations VirtualAlloc made ({base: size}): VirtualQuery reports a region's
// size from them (the script VM's stack sizes its reservation that way), and
// VirtualFree(MEM_RELEASE, 0) knows how much to unmap.
static std::mutex s_res_mu;
static std::map<uintptr_t, size_t> s_reservations;

static bool reservation_of(uintptr_t a, uintptr_t *base, size_t *size)
{
    std::lock_guard<std::mutex> lock(s_res_mu);
    auto it = s_reservations.upper_bound(a);
    if (it == s_reservations.begin())
        return false;
    --it;
    if (a >= it->first + it->second)
        return false;
    *base = it->first;
    *size = it->second;
    return true;
}

LPVOID WINAPI VirtualAlloc(LPVOID lpAddress, SIZE_T dwSize, DWORD flAllocationType, DWORD flProtect)
{
    size_t page = (size_t)getpagesize();
    size_t n = (dwSize + page - 1) & ~(page - 1);
    if (lpAddress && (flAllocationType & MEM_COMMIT) && !(flAllocationType & MEM_RESERVE)) {
        // Committing pages of an earlier reservation.
        void *base = (void *)((uintptr_t)lpAddress & ~(uintptr_t)(page - 1));
        if (mprotect(base, n, prot_of(flProtect)) != 0) {
            SetLastError(ERROR_INVALID_PARAMETER);
            return 0;
        }
        return lpAddress;
    }
    int prot = (flAllocationType & MEM_COMMIT) ? prot_of(flProtect) : PROT_NONE;
    void *p = mmap(lpAddress, n, prot, MAP_PRIVATE | MAP_ANON, -1, 0);
    if (p == MAP_FAILED) {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        return 0;
    }
    std::lock_guard<std::mutex> lock(s_res_mu);
    s_reservations[(uintptr_t)p] = n;
    return p;
}

BOOL WINAPI VirtualFree(LPVOID lpAddress, SIZE_T dwSize, DWORD dwFreeType)
{
    if (dwFreeType & MEM_DECOMMIT)
        return mprotect(lpAddress, dwSize, PROT_NONE) == 0;
    // MEM_RELEASE passes size 0: the whole reservation
    uintptr_t base;
    size_t size;
    if (reservation_of((uintptr_t)lpAddress, &base, &size) && base == (uintptr_t)lpAddress) {
        munmap(lpAddress, size);
        std::lock_guard<std::mutex> lock(s_res_mu);
        s_reservations.erase(base);
    } else if (dwSize)
        munmap(lpAddress, dwSize);
    return TRUE;
}

SIZE_T WINAPI VirtualQuery(LPCVOID lpAddress, PMEMORY_BASIC_INFORMATION lpBuffer, SIZE_T dwLength)
{
    if (!lpBuffer || dwLength < sizeof(MEMORY_BASIC_INFORMATION))
        return 0;
    size_t page = (size_t)getpagesize();
    memset(lpBuffer, 0, sizeof(*lpBuffer));
    lpBuffer->BaseAddress = (PVOID)((uintptr_t)lpAddress & ~(uintptr_t)(page - 1));
    lpBuffer->AllocationBase = lpBuffer->BaseAddress;
    lpBuffer->RegionSize = page;
    uintptr_t base;
    size_t size;
    if (reservation_of((uintptr_t)lpAddress, &base, &size)) {
        // the region from the queried page to the end of its reservation
        lpBuffer->AllocationBase = (PVOID)base;
        lpBuffer->RegionSize = base + size - (uintptr_t)lpBuffer->BaseAddress;
    }
    lpBuffer->State = MEM_COMMIT;
    lpBuffer->Protect = PAGE_READWRITE;
    lpBuffer->AllocationProtect = PAGE_READWRITE;
    lpBuffer->Type = MEM_PRIVATE;
    return sizeof(*lpBuffer);
}

// The probes: only null counts as bad (the game uses them as sanity checks).
BOOL WINAPI IsBadReadPtr(const void *lp, UINT_PTR ucb) { return ucb && !lp; }
BOOL WINAPI IsBadWritePtr(LPVOID lp, UINT_PTR ucb) { return ucb && !lp; }
BOOL WINAPI IsBadCodePtr(FARPROC lpfn) { return !lpfn; }

void WINAPI GlobalMemoryStatus(LPMEMORYSTATUS m)
{
    uint64_t total = 0;
    size_t len = sizeof(total);
    sysctlbyname("hw.memsize", &total, &len, 0, 0);
    // The game was written for at most 4 GB: report what fits.
    SIZE_T cap = 0x7FFFFFFF;
    m->dwLength = sizeof(*m);
    m->dwMemoryLoad = 30;
    m->dwTotalPhys = total > cap ? cap : (SIZE_T)total;
    m->dwAvailPhys = m->dwTotalPhys / 2;
    m->dwTotalPageFile = cap;
    m->dwAvailPageFile = cap;
    m->dwTotalVirtual = cap;
    m->dwAvailVirtual = cap;
}

void WINAPI GetSystemInfo(LPSYSTEM_INFO si)
{
    memset(si, 0, sizeof(*si));
    int ncpu = 1;
    size_t len = sizeof(ncpu);
    sysctlbyname("hw.ncpu", &ncpu, &len, 0, 0);
    si->wProcessorArchitecture = 0;  // PROCESSOR_ARCHITECTURE_INTEL, as the game expects
    si->dwPageSize = (DWORD)getpagesize();
    si->lpMinimumApplicationAddress = (LPVOID)0x10000;
    si->lpMaximumApplicationAddress = (LPVOID)0x7FFEFFFF;
    si->dwActiveProcessorMask = 1;
    si->dwNumberOfProcessors = (DWORD)ncpu;
    si->dwProcessorType = 586;  // PROCESSOR_INTEL_PENTIUM
    si->dwAllocationGranularity = 0x10000;
    si->wProcessorLevel = 6;
}

}  // extern "C"
