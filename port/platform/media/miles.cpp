// miles.cpp - the Miles Sound System calls of the game (mss32.dll) on SDL3 audio: the
// "quick" API for sounds loaded from memory (MBMilesSoundEngine, CMBMilesSoundManager)
// and file streams (the background music of SteamboatNode, MBMilesStreamingSoundEngine).
//
// A voice is an SDL audio stream bound to the output device. SDL asks for data through
// the stream's get-callback; the voice hands out decoded float stereo from its sound
// (decoded whole at load) or from its file (decoded as it plays), looping as told. The
// stream is locked while the callback runs, so the game's calls lock it too.
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
//   volume is the device gain, so it applies to the movies' sound too, as in the original
//   where Bink and Smacker played through Miles.
#include "media.h"

#include <mutex>
#include <string.h>

#pragma GCC visibility push(hidden)

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

struct Voice {
    enum State { Idle, Playing, Paused, Done };

    ~Voice()
    {
        if (stream)
            SDL_DestroyAudioStream(stream);  // unbinds; no callback runs after this
        delete media;
    }

    bool init(int r)
    {
        rate = speed = r > 0 ? r : 22050;
        SDL_AudioSpec spec = media::float_spec(rate);
        stream = SDL_CreateAudioStream(&spec, 0);
        if (!stream) {
            set_error(std::string("SDL_CreateAudioStream: ") + SDL_GetError());
            return false;
        }
        SDL_SetAudioStreamGetCallback(stream, feed, this);
        return true;
    }

    void bind(bool on)
    {
        SDL_AudioDeviceID dev = media::audio_device();
        if (!dev || on == bound)
            return;
        if (on)
            bound = SDL_BindAudioStream(dev, stream);
        else {
            SDL_UnbindAudioStream(stream);
            bound = false;
        }
    }

    // Frames SDL has been given but not played yet.
    long queued_frames() const
    {
        int q = SDL_GetAudioStreamQueued(stream);
        return q > 0 ? q / (long)(sizeof(float) * media::CHANNELS) : 0;
    }

    State status()
    {
        if (state == Playing && fed_all && queued_frames() == 0)
            state = Done;
        return state;
    }

    long length_frames() const { return (long)(pcm.size() / media::CHANNELS); }

    // Position of what is being heard, in frames from the start of the sound.
    long heard_frames() const
    {
        long p = pos - queued_frames();
        long len = length_frames();
        if (p < 0 && len > 0)
            p += len;  // the queue reaches back over a loop
        return p < 0 ? 0 : p;
    }

    void start(long loop_count)
    {
        SDL_ClearAudioStream(stream);
        if (media) {
            if (pos || fed_all)
                media->rewind();
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
        SDL_ClearAudioStream(stream);
        pos = 0;
    }

    void set_volume(long volume, long pan)
    {
        float v = (volume < 0 ? 0 : volume > 127 ? 127 : volume) / 127.0f;
        if (pan < 0)
            pan = 0;
        if (pan > 127)
            pan = 127;
        gain_l = v * (pan <= 64 ? 1.0f : (127 - pan) / 63.0f);
        gain_r = v * (pan >= 64 ? 1.0f : pan / 64.0f);
    }

    void set_speed(long s)
    {
        if (s <= 0)
            return;
        speed = s;
        float ratio = (float)s / (float)rate;
        if (ratio < 0.01f)
            ratio = 0.01f;
        if (ratio > 100.0f)
            ratio = 100.0f;
        SDL_SetAudioStreamFrequencyRatio(stream, ratio);
    }

    // Fills up to `frames` frames; returns how many it produced.
    long produce(float *out, long frames)
    {
        long done = 0;
        while (done < frames && !fed_all) {
            if (!media) {
                long len = length_frames();
                if (!len) {
                    fed_all = true;
                    break;
                }
                long n = len - pos < frames - done ? len - pos : frames - done;
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
            long n = (long)avail < frames - done ? (long)avail : frames - done;
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

    static void SDLCALL feed(void *userdata, SDL_AudioStream *s, int additional, int)
    {
        Voice *v = (Voice *)userdata;
        if (v->state != Playing || additional <= 0)
            return;
        long frames = additional / (long)(sizeof(float) * media::CHANNELS) + 1;
        v->scratch.resize((size_t)frames * media::CHANNELS);
        long got = v->produce(v->scratch.data(), frames);
        for (long i = 0; i < got; i++) {
            v->scratch[(size_t)i * 2] *= v->gain_l;
            v->scratch[(size_t)i * 2 + 1] *= v->gain_r;
        }
        if (got > 0)
            SDL_PutAudioStreamData(s, v->scratch.data(), (int)(got * (long)sizeof(float) * media::CHANNELS));
        if (v->fed_all)
            SDL_FlushAudioStream(s);  // the resampler's tail too, so the queue runs empty
    }

    SDL_AudioStream *stream = 0;
    bool bound = false;
    int rate = 22050;
    long speed = 22050;
    std::vector<float> pcm;          // a whole sound
    media::Media *media = 0;         // or a file
    std::vector<float> pending;      // decoded from the file, not handed out yet
    size_t pending_at = 0;
    std::vector<float> scratch;
    long pos = 0;                    // frames handed out since the start of the pass
    long loops = 1;                  // passes left, the current one included; 0 = forever
    bool fed_all = false;
    State state = Idle;
    float gain_l = 1, gain_r = 1;
};

// A sound of the quick API: the game reads `handle` (the HSAMPLE) out of AUDIO_TYPE.
struct Quick {
    AUDIO_TYPE pub;
    Voice voice;
};

Quick *quick(HAUDIO a) { return (Quick *)a; }
Voice *sample(HSAMPLE s) { return (Voice *)s; }
Voice *stream_voice(HSTREAM s) { return (Voice *)s; }

struct Lock {
    explicit Lock(Voice *v) : s(v->stream) { SDL_LockAudioStream(s); }
    ~Lock() { SDL_UnlockAudioStream(s); }
    SDL_AudioStream *s;
};

}  // namespace

#pragma GCC visibility push(default)

extern "C" {

long __stdcall AIL_quick_startup(long use_digital, long use_MIDI, unsigned long output_rate, long output_bits,
                                 long output_channels)
{
    (void)use_MIDI;
    (void)output_rate;
    (void)output_bits;
    (void)output_channels;
    // The device mixes at its own rate: the game's 22050 Hz, 16-bit request was what 1999
    // sound cards did well; the sounds are resampled per voice instead.
    if (!use_digital)
        return 0;
    if (!media::audio_device()) {
        set_error("no audio device");
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

long __stdcall AIL_set_preference(unsigned long number, long value)
{
    (void)number;
    (void)value;  // driver choices (DirectSound or waveOut) mean nothing here
    return 0;
}

HWND__ *__stdcall AIL_HWND(void) { return s_hwnd; }

long __stdcall AIL_set_DirectSound_HWND(_DIG_DRIVER *dig, HWND__ *wnd)
{
    (void)dig;
    s_hwnd = wnd;
    return 1;
}

void __stdcall AIL_set_digital_master_volume(_DIG_DRIVER *dig, long master_volume)
{
    (void)dig;
    media::set_master_volume(master_volume);
}

long __stdcall AIL_digital_master_volume(_DIG_DRIVER *dig)
{
    (void)dig;
    return media::master_volume();
}

char *__stdcall AIL_last_error() { return (char *)s_error.c_str(); }

HAUDIO __stdcall AIL_quick_load_mem(const void *mem, unsigned long size)
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
    q->pub.data = (const unsigned long *)mem;
    q->pub.size = (long)size;
    q->pub.type = 1;  // digital
    q->pub.handle = &q->voice;
    q->pub.status = QSTAT_LOADED;
    q->pub.speed = rate;
    q->pub.volume = 127;
    q->pub.extravol = 64;
    q->pub.milliseconds = (long)(q->voice.length_frames() * 1000LL / rate);
    return &q->pub;
}

long __stdcall AIL_quick_play(HAUDIO audio, unsigned long loop_count)
{
    if (!audio)
        return 0;
    Voice &v = quick(audio)->voice;
    {
        Lock l(&v);
        if (v.pos == 0 || v.pos >= v.length_frames())
            v.set_speed(v.rate);  // a fresh start plays at the sound's own rate
        v.start((long)loop_count);
    }
    v.bind(true);
    audio->status = QSTAT_PLAYING;
    return media::audio_device() != 0;
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

long __stdcall AIL_quick_status(HAUDIO audio)
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

void __stdcall AIL_quick_set_volume(HAUDIO audio, long volume, long extravol)
{
    if (!audio)
        return;
    Voice &v = quick(audio)->voice;
    Lock l(&v);
    v.set_volume(volume, extravol);
    audio->volume = volume;
    audio->extravol = extravol;
}

void __stdcall AIL_quick_set_speed(HAUDIO audio, long speed)
{
    if (!audio)
        return;
    Voice &v = quick(audio)->voice;
    Lock l(&v);
    v.set_speed(speed);
    audio->speed = speed;
}

long __stdcall AIL_quick_ms_position(HAUDIO audio)
{
    if (!audio)
        return 0;
    Voice &v = quick(audio)->voice;
    Lock l(&v);
    return (long)(v.heard_frames() * 1000LL / v.rate);
}

void __stdcall AIL_quick_set_ms_position(HAUDIO audio, long milliseconds)
{
    if (!audio)
        return;
    Voice &v = quick(audio)->voice;
    Lock l(&v);
    long p = (long)((long long)milliseconds * v.rate / 1000);
    if (p < 0)
        p = 0;
    if (p > v.length_frames())
        p = v.length_frames();
    SDL_ClearAudioStream(v.stream);
    v.pos = p;
}

unsigned long __stdcall AIL_sample_playback_rate(HSAMPLE S) { return S ? (unsigned long)sample(S)->speed : 0; }

long __stdcall AIL_sample_loop_count(HSAMPLE S) { return S ? sample(S)->loops : 0; }

HSTREAM __stdcall AIL_open_stream(_DIG_DRIVER *dig, const char *filename, long stream_mem)
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

void __stdcall AIL_start_stream(HSTREAM stream)
{
    if (!stream)
        return;
    Voice *v = stream_voice(stream);
    {
        Lock l(v);
        long loops = v->loops;
        v->start(loops);
    }
    v->bind(true);
}

void __stdcall AIL_pause_stream(HSTREAM stream, long onoff)
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

long __stdcall AIL_stream_status(HSTREAM stream)
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

void __stdcall AIL_set_stream_volume(HSTREAM stream, long volume)
{
    if (!stream)
        return;
    Voice *v = stream_voice(stream);
    Lock l(v);
    v->set_volume(volume, 64);
}

void __stdcall AIL_set_stream_loop_count(HSTREAM stream, long count)
{
    if (!stream)
        return;
    Voice *v = stream_voice(stream);
    Lock l(v);
    v->loops = count;
}

long __stdcall AIL_auto_service_stream(HSTREAM stream, long onoff)
{
    (void)stream;
    (void)onoff;  // SDL's callback always services the stream
    return 1;
}

}  // extern "C"
