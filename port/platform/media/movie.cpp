// movie.cpp - decoding Bink and Smacker movies with FFmpeg (see movie.h).
#include "movie.h"

#include <chrono>
#include <string.h>

namespace media {

static uint64_t now_ns()
{
    return (uint64_t)std::chrono::duration_cast<std::chrono::nanoseconds>(
               std::chrono::steady_clock::now().time_since_epoch()).count();
}

// How far the sound is decoded ahead of what is heard. The sound is fed from the game's
// thread (with the pictures), so this is how long that thread may stall before the sound
// runs dry (the mixer then plays silence, and the clock below waits with it).
static const double SOUND_LEAD = 1.0;
// Pictures read ahead while looking for sound, at most.
static const size_t MAX_QUEUED = 64;

Movie::~Movie()
{
    delete sound;
    drop_packets();
    av_frame_free(&frame);
    delete m;
}

bool Movie::open(HANDLE handle, const char *what)
{
    name = what;
    m = Media::open(handle_bytes(handle, false), what);
    if (!m || !m->video) {
        MEDIA_LOG("%s: no video\n", what);
        return false;
    }
    w = m->video->width;
    h = m->video->height;
    count = m->frame_count();
    fps = m->frame_rate();
    if (!fps.num || !fps.den)
        fps = AVRational{10, 1};
    frame = av_frame_alloc();
    if (m->audio && audio_open())
        sound = new Stream(m->rate);  // fed before binding
    MEDIA_LOG("%s: %dx%d, %d frames at %d/%d fps, %s\n", what, w, h, count, fps.num, fps.den,
              sound ? "with sound" : "silent");
    return true;
}

void Movie::drop_packets()
{
    for (AVPacket *p : video)
        av_packet_free(&p);
    video.clear();
}

double Movie::seconds_queued() const
{
    if (!sound)
        return 1e9;
    return sound->queued() / (double)m->rate;
}

bool Movie::fill(bool want_video)
{
    (void)want_video;
    if (eof)
        return false;
    AVPacket *pkt = av_packet_alloc();
    if (!m->read(pkt)) {
        av_packet_free(&pkt);
        eof = true;
        if (sound) {
            pcm.clear();
            m->decode_audio(0, pcm);
            if (!pcm.empty())
                push_sound();
        }
        return false;
    }
    if (pkt->stream_index == m->video_index) {
        video.push_back(pkt);
        return true;
    }
    if (sound && pkt->stream_index == m->audio_index) {
        pcm.clear();
        m->decode_audio(pkt, pcm);
        if (!pcm.empty())
            push_sound();
    }
    av_packet_free(&pkt);
    return true;
}

void Movie::lead_audio()
{
    if (sound && running && !eof && seconds_queued() == 0)
        MEDIA_LOG("%s: the sound ran dry (the game's thread stalled for over %.1f s)\n", name.c_str(), SOUND_LEAD);
    while (sound && !eof && video.size() < MAX_QUEUED && seconds_queued() < SOUND_LEAD)
        fill(false);
}

bool Movie::decode(int index)
{
    if (index < decoded) {
        // Back to the start; the clock and sound are the caller's business (rewind()).
        drop_packets();
        if (!m->rewind())
            return false;
        eof = false;
        decoded = -1;
    }
    while (decoded < index) {
        while (video.empty())
            if (!fill(true))
                return false;
        AVPacket *pkt = video.front();
        video.pop_front();
        bool got = m->decode_video(pkt, frame);
        av_packet_free(&pkt);
        if (got)
            decoded++;
    }
    lead_audio();
    return true;
}

void Movie::start()
{
    running = true;
    start_ns = now_ns();
    lead_audio();
    if (sound)
        sound->bind(true);
}

void Movie::push_sound()
{
    if (pcm.empty())
        return;
    sound->put(pcm.data(), (int)(pcm.size() / CHANNELS));
    pushed += (int64_t)(pcm.size() / CHANNELS);
}

// The movie's time: what has been heard of its sound, so that the pictures wait when the
// sound was starved and both stay together; the wall clock for a silent movie, and after
// the sound has ended.
double Movie::now() const
{
    double wall = (now_ns() - start_ns) / 1e9;
    if (!sound)
        return wall;
    int64_t queued = sound->queued();
    double heard = (double)(pushed - queued) / m->rate;
    if (eof && queued == 0) {
        if (!sound_end_ns) {
            sound_end_ns = now_ns();
            sound_end_t = heard;
        }
        return sound_end_t + (now_ns() - sound_end_ns) / 1e9;
    }
    // Starved before the end: the sound is only fed when a frame is decoded, and a frame
    // only when it is due, so a clock that waits for the sound would wait forever. The
    // time goes on from where the sound stopped until decoding refills it.
    if (!eof && queued == 0 && running) {
        if (!starve_ns) {
            starve_ns = now_ns();
            starve_t = heard;
        }
        return starve_t + (now_ns() - starve_ns) / 1e9;
    }
    if (starve_ns) {
        if (heard < starve_t + (now_ns() - starve_ns) / 1e9 - 0.05)
            return starve_t + (now_ns() - starve_ns) / 1e9;  // let the sound catch up
        starve_ns = 0;
    }
    return heard;
}

bool Movie::due(int index) const
{
    if (!running)
        return true;
    return now() * fps.num >= (double)index * fps.den;
}

void Movie::rewind()
{
    if (sound) {
        sound->bind(false);
        sound->clear();
    }
    drop_packets();
    if (m->rewind())
        eof = false;
    decoded = -1;
    running = false;
    pushed = 0;
    sound_end_ns = 0;
}

void blit_frame(AVFrame *src, AVPixelFormat fmt, int dw, int dh, uint8_t *dest, int pitch, int rows, int x,
                int y)
{
    if (!src || !dest || dw <= 0 || dh <= 0)
        return;
    int bpp = fmt == AV_PIX_FMT_BGR0 ? 4 : 2;
    // 2x is pixel doubling, as RAD's copies did it
    SwsContext *sws = sws_getContext(src->width, src->height, (AVPixelFormat)src->format, dw, dh, fmt,
                                     dw == src->width ? SWS_BILINEAR : SWS_POINT, 0, 0, 0);
    if (!sws)
        return;
    int rowbytes = (int)dw * bpp;
    if (x >= 0 && y >= 0 && y + dh <= rows && x * bpp + rowbytes <= pitch) {
        uint8_t *dst[4] = {dest + y * pitch + x * bpp, 0, 0, 0};
        int dstride[4] = {(int)pitch, 0, 0, 0};
        sws_scale(sws, src->data, src->linesize, 0, src->height, dst, dstride);
    } else {
        // only part of it fits: convert aside, copy the part
        std::vector<uint8_t> tmp((size_t)rowbytes * dh);
        uint8_t *dst[4] = {tmp.data(), 0, 0, 0};
        int dstride[4] = {(int)rowbytes, 0, 0, 0};
        sws_scale(sws, src->data, src->linesize, 0, src->height, dst, dstride);
        int n = rowbytes;
        if (x * bpp + n > pitch)
            n = pitch - x * bpp;
        for (int r = 0; r < dh && y + r < rows && n > 0; r++)
            if (y + r >= 0)
                memcpy(dest + (y + r) * pitch + x * bpp, &tmp[(size_t)r * rowbytes], (size_t)n);
    }
    sws_freeContext(sws);
}

}  // namespace media
