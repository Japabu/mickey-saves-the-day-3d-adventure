// movie.cpp - decoding Bink and Smacker movies with FFmpeg (see movie.h).
#include "movie.h"

#include <string.h>

#pragma GCC visibility push(hidden)

namespace media {

// How far the sound is decoded ahead of the picture.
static const double SOUND_LEAD = 0.5;
// Pictures read ahead while looking for sound, at most.
static const size_t MAX_QUEUED = 64;

Movie::~Movie()
{
    if (sound)
        SDL_DestroyAudioStream(sound);
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
    if (m->audio && audio_device()) {
        SDL_AudioSpec spec = float_spec(m->rate);
        sound = SDL_CreateAudioStream(&spec, 0);
    }
    MEDIA_LOG("%s: %ldx%ld, %ld frames at %d/%d fps, %s\n", what, w, h, count, fps.num, fps.den,
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
    int q = SDL_GetAudioStreamQueued(sound);
    return q > 0 ? q / (double)(sizeof(float) * CHANNELS * m->rate) : 0;
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
                SDL_PutAudioStreamData(sound, pcm.data(), (int)(pcm.size() * sizeof(float)));
            SDL_FlushAudioStream(sound);
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
            SDL_PutAudioStreamData(sound, pcm.data(), (int)(pcm.size() * sizeof(float)));
    }
    av_packet_free(&pkt);
    return true;
}

void Movie::lead_audio()
{
    while (sound && !eof && video.size() < MAX_QUEUED && seconds_queued() < SOUND_LEAD)
        fill(false);
}

bool Movie::decode(long index)
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
    start_ns = SDL_GetTicksNS();
    lead_audio();
    if (sound)
        SDL_BindAudioStream(audio_device(), sound);
}

bool Movie::due(long index) const
{
    if (!running)
        return true;
    double t = (SDL_GetTicksNS() - start_ns) / 1e9;
    return t * fps.num >= (double)index * fps.den;
}

void Movie::rewind()
{
    if (sound) {
        SDL_UnbindAudioStream(sound);
        SDL_ClearAudioStream(sound);
    }
    drop_packets();
    if (m->rewind())
        eof = false;
    decoded = -1;
    running = false;
}

void blit_frame(AVFrame *src, AVPixelFormat fmt, int dw, int dh, uint8_t *dest, long pitch, long rows, long x,
                long y)
{
    if (!src || !dest || dw <= 0 || dh <= 0)
        return;
    int bpp = fmt == AV_PIX_FMT_BGR0 ? 4 : 2;
    // 2x is pixel doubling, as RAD's copies did it
    SwsContext *sws = sws_getContext(src->width, src->height, (AVPixelFormat)src->format, dw, dh, fmt,
                                     dw == src->width ? SWS_BILINEAR : SWS_POINT, 0, 0, 0);
    if (!sws)
        return;
    long rowbytes = (long)dw * bpp;
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
        long n = rowbytes;
        if (x * bpp + n > pitch)
            n = pitch - x * bpp;
        for (long r = 0; r < dh && y + r < rows && n > 0; r++)
            if (y + r >= 0)
                memcpy(dest + (y + r) * pitch + x * bpp, &tmp[(size_t)r * rowbytes], (size_t)n);
    }
    sws_freeContext(sws);
}

}  // namespace media
