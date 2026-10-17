// mix.cpp - the sound output every voice of the game plays through: the streams (see
// media.h), resampled to the host's output rate and mixed on the host's sound thread
// (host::sound_open).
//
// The mixer must never wait long: a late output plays a gap for every voice. It takes
// each stream's lock only for the copy of its frames (and its pull callback, which
// copies from memory); the game's calls hold those locks for a few instructions.
#include "media.h"
#include "../host/host.h"

#include <algorithm>
#include <atomic>
#include <math.h>
#include <string.h>

namespace media {

struct Mixer {
    std::mutex mu;                 // the list; taken before any stream's lock
    std::vector<Stream *> streams;
    int rate = 0;                  // the output's
    bool open = false, tried = false;
    std::atomic<int> master{127};

    static void fill(float *out, int frames, void *user) { ((Mixer *)user)->mix(out, frames); }

    void mix(float *out, int frames)
    {
        memset(out, 0, (size_t)frames * CHANNELS * sizeof(float));
        std::lock_guard<std::mutex> l(mu);
        for (Stream *s : streams)
            add(s, out, frames);
        float m = master / 127.0f;
        for (int i = 0; i < frames * CHANNELS; i++) {
            float v = out[i] * m;
            out[i] = v > 1 ? 1 : v < -1 ? -1 : v;
        }
    }

    // Adds a stream's next frames to out, resampled from its rate (times its ratio) to
    // the output's, by linear interpolation.
    void add(Stream *s, float *out, int frames)
    {
        std::lock_guard<std::recursive_mutex> l(s->mu);
        if (!s->bound)
            return;
        double step = (double)s->rate * s->ratio / rate;
        int need = (int)ceil(s->frac + frames * step) + 1;
        int have = (int)((s->queue.size() - s->head) / CHANNELS);
        if (have < need && s->pull) {
            s->pull(s->pull_user, s, need - have);
            have = (int)((s->queue.size() - s->head) / CHANNELS);
        }
        const float *q = s->queue.data() + s->head;
        double pos = s->frac;
        int i = 0;
        for (; i < frames; i++) {
            int at = (int)pos;
            if (at >= have)
                break;
            float t = (float)(pos - at);
            int next = at + 1 < have ? at + 1 : at;   // the last frame holds to its end
            for (int c = 0; c < CHANNELS; c++) {
                float a = q[at * CHANNELS + c], b = q[next * CHANNELS + c];
                out[i * CHANNELS + c] += (a + (b - a) * t) * s->gain;
            }
            pos += step;
        }
        int used = (int)pos;
        if (used > have)
            used = have;
        s->head += (size_t)used * CHANNELS;
        s->frac = used < have ? pos - used : 0;
        // the played part goes once it is a good share of what is kept
        if (s->head >= 4096 && s->head * 2 >= s->queue.size()) {
            s->queue.erase(s->queue.begin(), s->queue.begin() + (long)s->head);
            s->head = 0;
        }
    }
};

static Mixer &mixer()
{
    static Mixer m;
    return m;
}

Stream::Stream(int r) : rate(r > 0 ? r : 22050)
{
    Mixer &m = mixer();
    std::lock_guard<std::mutex> l(m.mu);
    m.streams.push_back(this);
}

Stream::~Stream()
{
    Mixer &m = mixer();
    std::lock_guard<std::mutex> l(m.mu);  // the mixer is not in this stream afterwards
    m.streams.erase(std::remove(m.streams.begin(), m.streams.end(), this), m.streams.end());
}

void Stream::put(const float *frames, int count)
{
    std::lock_guard<std::recursive_mutex> l(mu);
    if (count > 0)
        queue.insert(queue.end(), frames, frames + (size_t)count * CHANNELS);
}

void Stream::clear()
{
    std::lock_guard<std::recursive_mutex> l(mu);
    queue.clear();
    head = 0;
    frac = 0;
}

int Stream::queued()
{
    std::lock_guard<std::recursive_mutex> l(mu);
    return (int)((queue.size() - head) / CHANNELS);
}

void Stream::set_gain(float g)
{
    std::lock_guard<std::recursive_mutex> l(mu);
    gain = g;
}

void Stream::set_ratio(float r)
{
    std::lock_guard<std::recursive_mutex> l(mu);
    ratio = r < 0.01f ? 0.01f : r > 100.0f ? 100.0f : r;
}

void Stream::set_pull(Pull p, void *user)
{
    std::lock_guard<std::recursive_mutex> l(mu);
    pull = p;
    pull_user = user;
}

void Stream::bind(bool on)
{
    std::lock_guard<std::recursive_mutex> l(mu);
    bound = on;
}

void Stream::lock() { mu.lock(); }
void Stream::unlock() { mu.unlock(); }

bool audio_open()
{
    Mixer &m = mixer();
    if (m.open || m.tried)
        return m.open;
    m.tried = true;
    m.rate = host::sound_open(Mixer::fill, &m);
    m.open = m.rate > 0;
    if (!m.open)
        MEDIA_LOG("no sound output\n");
    return m.open;
}

void audio_shutdown()
{
    Mixer &m = mixer();
    if (m.open)
        host::sound_close();
    m.open = false;
    m.tried = false;
}

void set_master_volume(int volume) { mixer().master = volume < 0 ? 0 : volume > 127 ? 127 : volume; }

int master_volume() { return mixer().master; }

}  // namespace media
