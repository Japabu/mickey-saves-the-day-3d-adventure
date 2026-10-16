// sync.h - waitable kernel objects (sync.cpp): events, mutexes, semaphores, threads and
// processes all wait through the one shared lock and condition variable.
#ifndef PORT_WIN32_SYNC_H
#define PORT_WIN32_SYNC_H

#include "internal.h"
#include <pthread.h>
#include <vector>

namespace win32 {

extern pthread_mutex_t g_wait_lock;
extern pthread_cond_t g_wait_cond;
// Wakes every waiter to re-check its objects; call with g_wait_lock held.
void signal_all();

struct Waitable : Object {
    explicit Waitable(Kind k) : Object(k) {}
    // Both are called with g_wait_lock held.
    virtual bool signalled() = 0;
    virtual void acquire() {}
    bool wait(DWORD ms) override;
};

}  // namespace win32

#endif
