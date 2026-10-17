// sound.cpp - the web host's sound output: a thread of its own mixes into a ring of
// stereo float frames in shared memory, which the page's AudioWorklet plays
// (port/web/sound-worklet.js), keeping about 64 ms ahead of it.
#include "../host.h"
#include "shared.h"

#include <atomic>
#include <pthread.h>
#include <stdlib.h>
#include <thread>
#include <time.h>

namespace host {

using namespace web;

enum { RING_FRAMES = 16384, AHEAD = 3072, CHUNK = 512 };

static SoundFill s_fill;
static void *s_user;
static float *s_ring;
static std::atomic<bool> s_quit;
static std::thread s_thread;

static void run()
{
    std::atomic<int32_t> *f = shared().f;
    float chunk[CHUNK * 2];
    while (!s_quit) {
        uint32_t written = (uint32_t)f[SOUND_WRITE], played = (uint32_t)f[SOUND_READ];
        if (written - played >= AHEAD) {
            timespec ts = {0, 2000000};
            nanosleep(&ts, 0);
            continue;
        }
        s_fill(chunk, CHUNK, s_user);
        for (int i = 0; i < CHUNK; i++) {
            uint32_t at = (written + (uint32_t)i) % RING_FRAMES;
            s_ring[at * 2] = chunk[i * 2];
            s_ring[at * 2 + 1] = chunk[i * 2 + 1];
        }
        f[SOUND_WRITE] = (int32_t)(written + CHUNK);
    }
}

int sound_open(SoundFill fill, void *user)
{
    std::atomic<int32_t> *f = shared().f;
    int rate = f[SOUND_RATE];
    if (!rate)
        return 0;  // the page has no AudioContext
    if (s_thread.joinable())
        return rate;
    s_fill = fill;
    s_user = user;
    if (!s_ring)
        s_ring = (float *)calloc(RING_FRAMES * 2, sizeof(float));
    f[SOUND_FRAMES] = RING_FRAMES;
    f[SOUND_WRITE] = f[SOUND_READ].load();
    f[SOUND_RING] = (int32_t)(uintptr_t)s_ring;
    s_quit = false;
    s_thread = std::thread(run);
    return rate;
}

void sound_close()
{
    if (!s_thread.joinable())
        return;
    s_quit = true;
    s_thread.join();
    shared().f[SOUND_RING] = 0;
    // the worklet may still look at the ring for one block: it stays allocated
}

}  // namespace host
