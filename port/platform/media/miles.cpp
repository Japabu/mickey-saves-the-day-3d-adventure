// miles.cpp - the Miles Sound System calls of the game (mss32.dll) on the port's mixer: the
// "quick" API for sounds loaded from memory (MBMilesSoundEngine, CMBMilesSoundManager)
// and file streams (the background music of SteamboatNode, MBMilesStreamingSoundEngine).
//
// A voice is a media::Stream (mix.cpp) bound to the output; the mixer runs on the host's
// sound thread, which must never wait: a late voice delays them all (the glitches of a
// stalled mixer). So:
// - a sound loaded from memory (decoded whole at load) is handed out through the stream's
//   pull callback, a copy from memory; the game's calls take the stream's lock for the
//   few instructions they change, which is also what the callback runs under;
// - a file stream is decoded by a thread of its own that keeps about 3/4 s queued in the
//   stream, so file reads, MP3 decoding and the rewinds of a loop never happen on the
//   sound thread; the game's calls and that thread share the voice's own mutex.
// When a voice runs dry the mixer plays silence for it (no stale buffer repeats).
//
// Behaviour the game relies on:
// - loop counts: 0 plays forever, n plays n times; AIL_sample_loop_count is the number of
//   passes left, which AIL_quick_play takes back (MBMilesSoundEngine::Resume).
// - AIL_quick_play starts at the position AIL_quick_set_ms_position chose since the last
//   halt (the engine pauses with ms_position + halt and resumes with set_ms_position +
//   play), otherwise at the start.
// - AIL_quick_status is QSTAT_DONE once a sound has played out or was halted;
//   AIL_stream_status is SMP_DONE once a stream has played out.
// - volumes are 0..127, the pan ("extravol") 0..127 with 64 the centre; the master
//   volume is the mixer's, so it applies to the movies' sound too, as in the original
//   where Bink and Smacker played through Miles.
// - the options panel's slider of a sound's channel is a float factor on top of the
//   volume (port_sound_next_gain): in the 0..127 steps the quiet end of a slider would
//   be a handful of loud steps and then silence.
#include "media.h"
#include "../port.h"

#include <chrono>
#include <condition_variable>
#include <atomic>
#include <mutex>
#include <string.h>
#include <thread>

namespace {

std::string s_error;
HWND__ *s_hwnd;
bool s_started;
char s_dig_token;  // what the _DIG_DRIVER handle points at

void set_error(const std::string &e)
{
    s_error = e;
    MEDIA_LOG("miles: %s\n", e.c_str());
}

// The slider gain the next volume call takes (port_sound_next_gain), -1 when none: the
// game computes a channel's volume and hands it to its sound in one go, on one thread.
thread_local float t_next_gain = -1;

struct Voice {
    enum State { Idle, Playing, Paused, Done };
    float slider = 1;                // its channel's slider gain (port_sound_next_gain)
    std::atomic<bool> music{false};  // the background music (port_music_stream): the music slider applies
    float stream_volume = 1;         // a file stream's own volume (AIL_set_stream_volume), 0..1
    float stream_gain = -1;          // what the stream has: stream_volume times the music slider

    void apply_stream_gain()
    {
        float g = stream_volume * (music ? port_music_gain() : 1.0f);
        if (g != stream_gain) {
            stream_gain = g;
            stream->set_gain(g);
        }
    }

    ~Voice()
    {
        if (feeder.joinable()) {
            {
                std::lock_guard<std::mutex> l(mu);
                quit = true;
            }
            cv.notify_all();
            feeder.join();
        }
        delete stream;  // out of the mixer; no callback runs after this
        delete media;
    }

    // A file stream (media set) gets its decoding thread, a sound the get-callback.
    bool init(int r)
    {
        rate = speed = r > 0 ? r : 22050;
        stream = new media::Stream(rate);
        if (media)
            feeder = std::thread([this] { feed_loop(); });
        else
            stream->set_pull(feed, this);
        return true;
    }

    // The decoding thread of a file stream: tops the stream up to 3/4 s, in chunks.
    void feed_loop()
    {
        std::unique_lock<std::mutex> l(mu);
        while (!quit) {
            apply_stream_gain();  // follows the music slider while it moves
            if (rewind_pending) {
                rewind_pending = false;
                media->rewind();
            }
            if (state == Playing && !fed_all && queued_frames() < rate * 3 / 4) {
                if (fed_some && bound && queued_frames() == 0)
                    MEDIA_LOG("miles: stream ran dry (decoding fell behind)\n");
                fed_some = true;
                enum { CHUNK = 4096 };
                scratch.resize((size_t)CHUNK * media::CHANNELS);
                int got = produce(scratch.data(), CHUNK);
                if (got > 0)
                    stream->put(scratch.data(), got);
                continue;
            }
            cv.wait_for(l, std::chrono::milliseconds(20));
        }
    }

    void bind(bool on)
    {
        if (!media::audio_open() || on == bound)
            return;
        stream->bind(on);
        bound = on;
    }

    // Frames the stream has been given but not played yet.
    int queued_frames() const { return stream->queued(); }

    State status()
    {
        if (state == Playing && fed_all && queued_frames() == 0)
            state = Done;
        return state;
    }

    int length_frames() const { return (int)(pcm.size() / media::CHANNELS); }

    // Position of what is being heard, in frames from the start of the sound.
    int heard_frames() const
    {
        int p = pos - queued_frames();
        int len = length_frames();
        if (p < 0 && len > 0)
            p += len;  // the queue reaches back over a loop
        return p < 0 ? 0 : p;
    }

    void start(int loop_count)
    {
        stream->clear();
        if (media) {
            fed_some = false;
            if (pos || fed_all)
                rewind_pending = true;  // done by the decoding thread
            pending.clear();
            pending_at = 0;
            pos = 0;
        } else if (pos >= length_frames()) {
            pos = 0;
        }
        loops = loop_count;
        fed_all = false;
        state = Playing;
    }

    void halt()
    {
        state = Done;
        stream->clear();
        pos = 0;
    }

    void set_volume(int volume, int pan)
    {
        if (t_next_gain >= 0) {
            slider = t_next_gain;
            t_next_gain = -1;
        }
        float v = (volume < 0 ? 0 : volume > 127 ? 127 : volume) / 127.0f * slider;
        if (media) {
            // a stream is always centred; the gain applies to what is queued too
            stream_volume = v;
            apply_stream_gain();
            return;
        }
        if (pan < 0)
            pan = 0;
        if (pan > 127)
            pan = 127;
        gain_l = v * (pan <= 64 ? 1.0f : (127 - pan) / 63.0f);
        gain_r = v * (pan >= 64 ? 1.0f : pan / 64.0f);
    }

    void set_speed(int s)
    {
        if (s <= 0)
            return;
        speed = s;
        stream->set_ratio((float)s / (float)rate);
    }

    // Fills up to `frames` frames; returns how many it produced.
    int produce(float *out, int frames)
    {
        int done = 0;
        while (done < frames && !fed_all) {
            if (!media) {
                int len = length_frames();
                if (!len) {
                    fed_all = true;
                    break;
                }
                int n = len - pos < frames - done ? len - pos : frames - done;
                if (n > 0) {
                    memcpy(out + done * media::CHANNELS, &pcm[(size_t)pos * media::CHANNELS],
                           (size_t)n * media::CHANNELS * sizeof(float));
                    pos += n;
                    done += n;
                }
                if (pos >= len && !next_pass())
                    break;
                continue;
            }
            size_t avail = (pending.size() - pending_at) / media::CHANNELS;
            if (!avail) {
                pending.clear();
                pending_at = 0;
                if (!refill() && !next_pass())
                    break;
                continue;
            }
            int n = (int)avail < frames - done ? (int)avail : frames - done;
            memcpy(out + done * media::CHANNELS, &pending[pending_at], (size_t)n * media::CHANNELS * sizeof(float));
            pending_at += (size_t)n * media::CHANNELS;
            pos += n;
            done += n;
        }
        return done;
    }

    // Decodes the next packets of the file; false at its end.
    bool refill()
    {
        AVPacket *pkt = av_packet_alloc();
        bool more = false;
        while (pending.empty() && media->read(pkt)) {
            if (pkt->stream_index == media->audio_index)
                media->decode_audio(pkt, pending);
            av_packet_unref(pkt);
            more = true;
        }
        if (!more)
            media->decode_audio(0, pending);  // what the decoder still holds
        av_packet_free(&pkt);
        return !pending.empty();
    }

    // At the end of the sound: start the next pass, or finish.
    bool next_pass()
    {
        if (loops == 1) {
            fed_all = true;
            return false;
        }
        if (loops > 1)
            loops--;
        pos = 0;
        if (media && !media->rewind()) {
            fed_all = true;
            return false;
        }
        return true;
    }

    static void feed(void *userdata, media::Stream *s, int frames)
    {
        Voice *v = (Voice *)userdata;
        if (v->state != Playing || frames <= 0)
            return;
        v->scratch.resize((size_t)frames * media::CHANNELS);
        int got = v->produce(v->scratch.data(), frames);
        float music = v->music ? port_music_gain() : 1.0f;
        for (int i = 0; i < got; i++) {
            v->scratch[(size_t)i * 2] *= v->gain_l * music;
            v->scratch[(size_t)i * 2 + 1] *= v->gain_r * music;
        }
        if (got > 0)
            s->put(v->scratch.data(), got);
    }

    media::Stream *stream = 0;
    bool bound = false;
    int rate = 22050;
    int speed = 22050;
    std::vector<float> pcm;          // a whole sound
    media::Media *media = 0;         // or a file
    std::vector<float> pending;      // decoded from the file, not handed out yet
    size_t pending_at = 0;
    std::vector<float> scratch;
    int pos = 0;                    // frames handed out since the start of the pass
    int loops = 1;                  // passes left, the current one included; 0 = forever
    bool fed_all = false;
    State state = Idle;
    float gain_l = 1, gain_r = 1;
    // file streams: the decoding thread and what it shares with the game's calls
    std::thread feeder;
    std::mutex mu;
    std::condition_variable cv;
    bool quit = false, rewind_pending = false, fed_some = false;
};

// A sound of the quick API: the game reads `handle` (the HSAMPLE) out of AUDIO_TYPE.
struct Quick {
    AUDIO_TYPE pub;
    Voice voice;
};

Quick *quick(HAUDIO a) { return (Quick *)a; }
Voice *sample(HSAMPLE s) { return (Voice *)s; }
Voice *stream_voice(HSTREAM s) { return (Voice *)s; }

// The game's side of a voice: a file stream's mutex (and a wake-up for its thread when
// done), a sound's stream lock.
struct Lock {
    explicit Lock(Voice *v) : v(v)
    {
        if (v->media)
            v->mu.lock();
        else
            v->stream->lock();
    }
    ~Lock()
    {
        if (v->media) {
            v->mu.unlock();
            v->cv.notify_all();
        } else {
            v->stream->unlock();
        }
    }
    Voice *v;
};

}  // namespace

extern "C" {

int __stdcall AIL_quick_startup(int use_digital, int use_MIDI, unsigned int output_rate, int output_bits,
                                 int output_channels)
{
    (void)use_MIDI;
    (void)output_rate;
    (void)output_bits;
    (void)output_channels;
    // The output mixes at its own rate: the game's 22050 Hz, 16-bit request was what 1999
    // sound cards did well; the sounds are resampled per voice instead.
    if (!use_digital)
        return 0;
    if (!media::audio_open()) {
        set_error("no sound output");
        return 0;
    }
    s_started = true;
    return 1;
}

void __stdcall AIL_quick_shutdown(void)
{
    s_started = false;
    media::audio_shutdown();
}

void __stdcall AIL_quick_handles(_DIG_DRIVER **pdig, _MDI_DRIVER **pmdi, _DLSDEVICE **pdls)
{
    if (pdig)
        *pdig = s_started ? (_DIG_DRIVER *)&s_dig_token : 0;
    if (pmdi)
        *pmdi = 0;  // no MIDI: the game uses none
    if (pdls)
        *pdls = 0;
}

int __stdcall AIL_set_preference(unsigned int number, int value)
{
    (void)number;
    (void)value;  // driver choices (DirectSound or waveOut) mean nothing here
    return 0;
}

HWND__ *__stdcall AIL_HWND(void) { return s_hwnd; }

int __stdcall AIL_set_DirectSound_HWND(_DIG_DRIVER *dig, HWND__ *wnd)
{
    (void)dig;
    s_hwnd = wnd;
    return 1;
}

void __stdcall AIL_set_digital_master_volume(_DIG_DRIVER *dig, int master_volume)
{
    (void)dig;
    media::set_master_volume(master_volume);
}

int __stdcall AIL_digital_master_volume(_DIG_DRIVER *dig)
{
    (void)dig;
    return media::master_volume();
}

char *__stdcall AIL_last_error() { return (char *)s_error.c_str(); }

HAUDIO __stdcall AIL_quick_load_mem(const void *mem, unsigned int size)
{
    if (!mem || !size) {
        set_error("AIL_quick_load_mem: no data");
        return 0;
    }
    Quick *q = new Quick();
    int rate = 0;
    if (!media::decode_all(media::memory_bytes(mem, size), "sound", q->voice.pcm, rate) || !q->voice.init(rate)) {
        set_error("AIL_quick_load_mem: not a sound the decoder knows");
        delete q;
        return 0;
    }
    memset(&q->pub, 0, sizeof q->pub);
    q->pub.data = (const unsigned int *)mem;
    q->pub.size = (int)size;
    q->pub.type = 1;  // digital
    q->pub.handle = &q->voice;
    q->pub.status = QSTAT_LOADED;
    q->pub.speed = rate;
    q->pub.volume = 127;
    q->pub.extravol = 64;
    q->pub.milliseconds = (int)(q->voice.length_frames() * 1000LL / rate);
    return &q->pub;
}

int __stdcall AIL_quick_play(HAUDIO audio, unsigned int loop_count)
{
    if (!audio)
        return 0;
    Voice &v = quick(audio)->voice;
    {
        Lock l(&v);
        if (v.pos == 0 || v.pos >= v.length_frames())
            v.set_speed(v.rate);  // a fresh start plays at the sound's own rate
        v.start((int)loop_count);
    }
    v.bind(true);
    audio->status = QSTAT_PLAYING;
    return media::audio_open();
}

void __stdcall AIL_quick_halt(HAUDIO audio)
{
    if (!audio)
        return;
    Voice &v = quick(audio)->voice;
    Lock l(&v);
    v.halt();
    audio->status = QSTAT_DONE;
}

int __stdcall AIL_quick_status(HAUDIO audio)
{
    if (!audio)
        return QSTAT_DONE;
    Voice &v = quick(audio)->voice;
    Lock l(&v);
    switch (v.status()) {
    case Voice::Idle: audio->status = QSTAT_LOADED; break;
    case Voice::Playing:
    case Voice::Paused: audio->status = QSTAT_PLAYING; break;
    case Voice::Done: audio->status = QSTAT_DONE; break;
    }
    return audio->status;
}

void __stdcall AIL_quick_unload(HAUDIO audio) { delete quick(audio); }

void __stdcall AIL_quick_set_volume(HAUDIO audio, int volume, int extravol)
{
    if (!audio) {
        t_next_gain = -1;
        return;
    }
    Voice &v = quick(audio)->voice;
    Lock l(&v);
    v.set_volume(volume, extravol);
    audio->volume = volume;
    audio->extravol = extravol;
}

void __stdcall AIL_quick_set_speed(HAUDIO audio, int speed)
{
    if (!audio)
        return;
    Voice &v = quick(audio)->voice;
    Lock l(&v);
    v.set_speed(speed);
    audio->speed = speed;
}

int __stdcall AIL_quick_ms_position(HAUDIO audio)
{
    if (!audio)
        return 0;
    Voice &v = quick(audio)->voice;
    Lock l(&v);
    return (int)(v.heard_frames() * 1000LL / v.rate);
}

void __stdcall AIL_quick_set_ms_position(HAUDIO audio, int milliseconds)
{
    if (!audio)
        return;
    Voice &v = quick(audio)->voice;
    Lock l(&v);
    int p = (int)((long long)milliseconds * v.rate / 1000);
    if (p < 0)
        p = 0;
    if (p > v.length_frames())
        p = v.length_frames();
    v.stream->clear();
    v.pos = p;
}

unsigned int __stdcall AIL_sample_playback_rate(HSAMPLE S) { return S ? (unsigned int)sample(S)->speed : 0; }

int __stdcall AIL_sample_loop_count(HSAMPLE S) { return S ? sample(S)->loops : 0; }

HSTREAM __stdcall AIL_open_stream(_DIG_DRIVER *dig, const char *filename, int stream_mem)
{
    (void)stream_mem;
    if (!dig || !filename) {
        set_error("AIL_open_stream: no driver or file name");
        return 0;
    }
    HANDLE h = CreateFileA(filename, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
    if (h == INVALID_HANDLE_VALUE) {
        set_error(std::string("AIL_open_stream: cannot open ") + filename);
        return 0;
    }
    Voice *v = new Voice();
    v->media = media::Media::open(media::handle_bytes(h, true), filename);
    if (!v->media || !v->media->audio || !v->init(v->media->rate)) {
        set_error(std::string("AIL_open_stream: no sound in ") + filename);
        delete v;
        return 0;
    }
    return (HSTREAM)v;
}

void __stdcall AIL_close_stream(HSTREAM stream) { delete stream_voice(stream); }

void port_sound_next_gain(float gain) { t_next_gain = gain < 0 ? 0 : gain; }

void port_music_stream(void *stream)
{
    if (stream)
        stream_voice((HSTREAM)stream)->music = true;
}

void __stdcall AIL_start_stream(HSTREAM stream)
{
    if (!stream)
        return;
    Voice *v = stream_voice(stream);
    {
        Lock l(v);
        int loops = v->loops;
        v->start(loops);
    }
    v->bind(true);
}

void __stdcall AIL_pause_stream(HSTREAM stream, int onoff)
{
    if (!stream)
        return;
    Voice *v = stream_voice(stream);
    {
        Lock l(v);
        if (onoff && v->state == Voice::Playing)
            v->state = Voice::Paused;
        else if (!onoff && v->state == Voice::Paused)
            v->state = Voice::Playing;
        else
            return;
    }
    v->bind(!onoff);  // unbound, the stream keeps what it holds for the resume
}

int __stdcall AIL_stream_status(HSTREAM stream)
{
    if (!stream)
        return -1;
    Voice *v = stream_voice(stream);
    Lock l(v);
    switch (v->status()) {
    case Voice::Playing: return SMP_PLAYING;
    case Voice::Done: return SMP_DONE;
    default: return SMP_STOPPED;
    }
}

void __stdcall AIL_set_stream_volume(HSTREAM stream, int volume)
{
    if (!stream) {
        t_next_gain = -1;
        return;
    }
    Voice *v = stream_voice(stream);
    Lock l(v);
    v->set_volume(volume, 64);
}

void __stdcall AIL_set_stream_loop_count(HSTREAM stream, int count)
{
    if (!stream)
        return;
    Voice *v = stream_voice(stream);
    Lock l(v);
    v->loops = count;
}

int __stdcall AIL_auto_service_stream(HSTREAM stream, int onoff)
{
    (void)stream;
    (void)onoff;  // the stream's own thread always services it
    return 1;
}

}  // extern "C"
