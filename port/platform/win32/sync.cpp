// sync.cpp - critical sections, events, mutexes, semaphores, waiting, and thread-local
// storage.
//
// All waitable objects share one lock and one condition variable: a state change
// broadcasts, and every waiter re-checks the objects it waits for. The game waits on
// few objects, so this keeps WaitForMultipleObjects simple and correct.
#include "internal.h"
#include "sync.h"

#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>

namespace win32 {

pthread_mutex_t g_wait_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t g_wait_cond = PTHREAD_COND_INITIALIZER;

void signal_all() { pthread_cond_broadcast(&g_wait_cond); }

// Waits on the shared condition until pred() holds or ms pass; the lock is held.
template <class P> static bool wait_until(P pred, DWORD ms)
{
    if (pred())
        return true;
    if (ms == 0)
        return false;
    if (ms == INFINITE) {
        while (!pred())
            pthread_cond_wait(&g_wait_cond, &g_wait_lock);
        return true;
    }
    struct timeval now;
    gettimeofday(&now, 0);
    struct timespec until;
    long long ns = (long long)now.tv_usec * 1000 + (long long)(ms % 1000) * 1000000LL;
    until.tv_sec = now.tv_sec + ms / 1000 + (time_t)(ns / 1000000000LL);
    until.tv_nsec = (long)(ns % 1000000000LL);
    while (!pred()) {
        if (pthread_cond_timedwait(&g_wait_cond, &g_wait_lock, &until) == ETIMEDOUT)
            return pred();
    }
    return true;
}

bool Waitable::wait(DWORD ms)
{
    pthread_mutex_lock(&g_wait_lock);
    bool ok = wait_until([this] { return signalled(); }, ms);
    if (ok)
        acquire();
    pthread_mutex_unlock(&g_wait_lock);
    return ok;
}

struct EventObject : Waitable {
    EventObject(bool manual, bool state) : Waitable(Event), manual(manual), state(state) {}
    bool signalled() override { return state; }
    void acquire() override
    {
        if (!manual)
            state = false;
    }
    bool manual, state;
};

struct MutexObject : Waitable {
    MutexObject() : Waitable(Mutex), owner(0), count(0) {}
    bool signalled() override { return count == 0 || owner == pthread_self(); }
    void acquire() override
    {
        owner = pthread_self();
        count++;
    }
    pthread_t owner;
    int count;
};

struct SemaphoreObject : Waitable {
    SemaphoreObject(LONG n, LONG limit) : Waitable(Semaphore), count(n), limit(limit) {}
    bool signalled() override { return count > 0; }
    void acquire() override { count--; }
    LONG count, limit;
};

// Named objects (the game's single-instance mutex): one per name in this process.
struct Named {
    std::string name;
    Object *object;
};
static std::vector<Named> s_named;

static Object *find_named(const char *name, Object::Kind kind)
{
    if (!name)
        return 0;
    for (auto &n : s_named)
        if (n.name == name && n.object->kind == kind)
            return n.object;
    return 0;
}

}  // namespace win32

using namespace win32;

extern "C" {

// Critical sections: a recursive pthread mutex behind DebugInfo, made on first use
// (some are entered before InitializeCriticalSection in static constructors).
static pthread_mutex_t *cs_mutex(LPCRITICAL_SECTION cs)
{
    static pthread_mutex_t init_lock = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t *m = (pthread_mutex_t *)__atomic_load_n(&cs->DebugInfo, __ATOMIC_ACQUIRE);
    if (m)
        return m;
    pthread_mutex_lock(&init_lock);
    m = (pthread_mutex_t *)cs->DebugInfo;
    if (!m) {
        m = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
        pthread_mutexattr_t a;
        pthread_mutexattr_init(&a);
        pthread_mutexattr_settype(&a, PTHREAD_MUTEX_RECURSIVE);
        pthread_mutex_init(m, &a);
        pthread_mutexattr_destroy(&a);
        __atomic_store_n(&cs->DebugInfo, (PVOID)m, __ATOMIC_RELEASE);
    }
    pthread_mutex_unlock(&init_lock);
    return m;
}

void WINAPI InitializeCriticalSection(LPCRITICAL_SECTION cs)
{
    memset(cs, 0, sizeof(*cs));
    cs_mutex(cs);
}

void WINAPI DeleteCriticalSection(LPCRITICAL_SECTION cs)
{
    pthread_mutex_t *m = (pthread_mutex_t *)cs->DebugInfo;
    if (m) {
        pthread_mutex_destroy(m);
        free(m);
        cs->DebugInfo = 0;
    }
}

void WINAPI EnterCriticalSection(LPCRITICAL_SECTION cs)
{
    pthread_mutex_lock(cs_mutex(cs));
    cs->RecursionCount++;
    cs->OwningThread = (HANDLE)(uintptr_t)GetCurrentThreadId();
}

void WINAPI LeaveCriticalSection(LPCRITICAL_SECTION cs)
{
    if (--cs->RecursionCount == 0)
        cs->OwningThread = 0;
    pthread_mutex_unlock(cs_mutex(cs));
}

HANDLE WINAPI CreateEventA(LPSECURITY_ATTRIBUTES, BOOL bManualReset, BOOL bInitialState, LPCSTR lpName)
{
    pthread_mutex_lock(&g_wait_lock);
    Object *o = find_named(lpName, Object::Event);
    if (o) {
        retain(o);
        SetLastError(ERROR_ALREADY_EXISTS);
    } else {
        o = new EventObject(bManualReset != 0, bInitialState != 0);
        if (lpName)
            s_named.push_back({lpName, o});
        SetLastError(ERROR_SUCCESS);
    }
    pthread_mutex_unlock(&g_wait_lock);
    return handle(o);
}

static BOOL set_event(HANDLE h, int how)
{
    EventObject *e = object_as<EventObject>(h, Object::Event);
    if (!e)
        return FALSE;
    pthread_mutex_lock(&g_wait_lock);
    if (how == 0) {
        e->state = false;
    } else {
        e->state = true;
        signal_all();
        if (how == 2) {
            // PulseEvent: release the current waiters, then reset. Waiters re-check under
            // the lock, so give them a moment to run before the reset.
            pthread_mutex_unlock(&g_wait_lock);
            sched_yield();
            pthread_mutex_lock(&g_wait_lock);
            e->state = false;
        }
    }
    pthread_mutex_unlock(&g_wait_lock);
    return TRUE;
}

BOOL WINAPI SetEvent(HANDLE hEvent) { return set_event(hEvent, 1); }
BOOL WINAPI ResetEvent(HANDLE hEvent) { return set_event(hEvent, 0); }
BOOL WINAPI PulseEvent(HANDLE hEvent) { return set_event(hEvent, 2); }

HANDLE WINAPI CreateMutexA(LPSECURITY_ATTRIBUTES, BOOL bInitialOwner, LPCSTR lpName)
{
    pthread_mutex_lock(&g_wait_lock);
    Object *o = find_named(lpName, Object::Mutex);
    if (o) {
        retain(o);
        SetLastError(ERROR_ALREADY_EXISTS);
    } else {
        MutexObject *m = new MutexObject;
        if (bInitialOwner)
            m->acquire();
        o = m;
        if (lpName)
            s_named.push_back({lpName, o});
        SetLastError(ERROR_SUCCESS);
    }
    pthread_mutex_unlock(&g_wait_lock);
    return handle(o);
}

BOOL WINAPI ReleaseMutex(HANDLE hMutex)
{
    MutexObject *m = object_as<MutexObject>(hMutex, Object::Mutex);
    if (!m)
        return FALSE;
    pthread_mutex_lock(&g_wait_lock);
    BOOL ok = m->count > 0 && m->owner == pthread_self();
    if (ok && --m->count == 0)
        signal_all();
    pthread_mutex_unlock(&g_wait_lock);
    if (!ok)
        SetLastError(288);  // ERROR_NOT_OWNER
    return ok;
}

HANDLE WINAPI CreateSemaphoreA(LPSECURITY_ATTRIBUTES, LONG lInitialCount, LONG lMaximumCount, LPCSTR)
{
    return handle(new SemaphoreObject(lInitialCount, lMaximumCount));
}

BOOL WINAPI ReleaseSemaphore(HANDLE hSemaphore, LONG lReleaseCount, LPLONG lpPreviousCount)
{
    SemaphoreObject *s = object_as<SemaphoreObject>(hSemaphore, Object::Semaphore);
    if (!s)
        return FALSE;
    pthread_mutex_lock(&g_wait_lock);
    if (lpPreviousCount)
        *lpPreviousCount = s->count;
    s->count += lReleaseCount;
    if (s->count > s->limit)
        s->count = s->limit;
    signal_all();
    pthread_mutex_unlock(&g_wait_lock);
    return TRUE;
}

DWORD WINAPI WaitForSingleObject(HANDLE hHandle, DWORD dwMilliseconds)
{
    Object *o = object(hHandle);
    if (!o)
        return WAIT_FAILED;
    return o->wait(dwMilliseconds) ? WAIT_OBJECT_0 : WAIT_TIMEOUT;
}

DWORD WINAPI WaitForMultipleObjects(DWORD nCount, const HANDLE *lpHandles, BOOL bWaitAll, DWORD dwMilliseconds)
{
    std::vector<Waitable *> objs;
    for (DWORD i = 0; i < nCount; i++) {
        Waitable *w = dynamic_cast<Waitable *>(object(lpHandles[i]));
        if (!w)
            return WAIT_FAILED;
        objs.push_back(w);
    }
    DWORD which = WAIT_TIMEOUT;
    pthread_mutex_lock(&g_wait_lock);
    auto ready = [&] {
        if (bWaitAll) {
            for (auto *w : objs)
                if (!w->signalled())
                    return false;
            which = WAIT_OBJECT_0;
            return true;
        }
        for (DWORD i = 0; i < objs.size(); i++)
            if (objs[i]->signalled()) {
                which = WAIT_OBJECT_0 + i;
                return true;
            }
        return false;
    };
    if (wait_until(ready, dwMilliseconds)) {
        if (bWaitAll)
            for (auto *w : objs)
                w->acquire();
        else
            objs[which - WAIT_OBJECT_0]->acquire();
    } else {
        which = WAIT_TIMEOUT;
    }
    pthread_mutex_unlock(&g_wait_lock);
    return which;
}

DWORD WINAPI MsgWaitForMultipleObjects(DWORD nCount, const HANDLE *pHandles, BOOL fWaitAll, DWORD dwMilliseconds,
                                       DWORD)
{
    // Messages arrive by polling the event pump: wait in short slices.
    DWORD start = ticks();
    for (;;) {
        DWORD r = nCount ? WaitForMultipleObjects(nCount, pHandles, fWaitAll, 10) : WAIT_TIMEOUT;
        if (r != WAIT_TIMEOUT)
            return r;
        if (messages_pending())
            return WAIT_OBJECT_0 + nCount;
        if (nCount == 0)
            Sleep(10);
        if (dwMilliseconds != INFINITE && ticks() - start >= dwMilliseconds)
            return WAIT_TIMEOUT;
    }
}

// Thread-local storage.
DWORD WINAPI TlsAlloc(void)
{
    pthread_key_t key;
    if (pthread_key_create(&key, 0) != 0)
        return TLS_OUT_OF_INDEXES;
    return (DWORD)key;
}

BOOL WINAPI TlsFree(DWORD dwTlsIndex) { return pthread_key_delete((pthread_key_t)dwTlsIndex) == 0; }
LPVOID WINAPI TlsGetValue(DWORD dwTlsIndex)
{
    SetLastError(ERROR_SUCCESS);
    return pthread_getspecific((pthread_key_t)dwTlsIndex);
}
BOOL WINAPI TlsSetValue(DWORD dwTlsIndex, LPVOID lpTlsValue)
{
    return pthread_setspecific((pthread_key_t)dwTlsIndex, lpTlsValue) == 0;
}

}  // extern "C"
