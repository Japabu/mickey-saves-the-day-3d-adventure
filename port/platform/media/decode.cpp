// decode.cpp - reading audio and video files with FFmpeg, from memory or a Win32 handle.
#include "media.h"

#include <string.h>

namespace media {

namespace {

struct MemoryBytes : Bytes {
    MemoryBytes(const void *d, size_t n) : data((const uint8_t *)d), size(n) {}
    int read(uint8_t *buf, int n) override
    {
        if (pos >= size)
            return AVERROR_EOF;
        size_t k = size - pos < (size_t)n ? size - pos : (size_t)n;
        memcpy(buf, data + pos, k);
        pos += k;
        return (int)k;
    }
    int64_t seek(int64_t off, int whence) override
    {
        if (whence == AVSEEK_SIZE)
            return (int64_t)size;
        int64_t p = whence == SEEK_SET ? off : whence == SEEK_CUR ? (int64_t)pos + off : (int64_t)size + off;
        if (p < 0)
            return -1;
        pos = (size_t)p;
        return p;
    }
    const uint8_t *data;
    size_t size;
    size_t pos = 0;
};

struct HandleBytes : Bytes {
    HandleBytes(HANDLE h, bool owned) : h(h), owned(owned)
    {
        LONG high = 0;
        DWORD low = SetFilePointer(h, 0, &high, FILE_CURRENT);
        base = low == INVALID_SET_FILE_POINTER && GetLastError() != NO_ERROR ? 0 : ((int64_t)high << 32 | low);
        DWORD sizeHigh = 0;
        DWORD sizeLow = GetFileSize(h, &sizeHigh);
        int64_t total = ((int64_t)sizeHigh << 32) | sizeLow;
        size = total > base ? total - base : 0;
    }
    ~HandleBytes() override
    {
        if (owned)
            CloseHandle(h);
    }
    int read(uint8_t *buf, int n) override
    {
        if (pos >= size)
            return AVERROR_EOF;
        if (n > size - pos)
            n = (int)(size - pos);
        if (!place(base + pos))
            return AVERROR(EIO);
        DWORD got = 0;
        if (!ReadFile(h, buf, (DWORD)n, &got, 0))
            return AVERROR(EIO);
        if (got == 0)
            return AVERROR_EOF;
        pos += got;
        return (int)got;
    }
    int64_t seek(int64_t off, int whence) override
    {
        if (whence == AVSEEK_SIZE)
            return size;
        int64_t p = whence == SEEK_SET ? off : whence == SEEK_CUR ? pos + off : size + off;
        if (p < 0)
            return -1;
        pos = p;
        return p;
    }
    bool place(int64_t at)
    {
        LONG high = (LONG)(at >> 32);
        DWORD low = SetFilePointer(h, (LONG)(at & 0xffffffff), &high, FILE_BEGIN);
        return !(low == INVALID_SET_FILE_POINTER && GetLastError() != NO_ERROR);
    }
    HANDLE h;
    bool owned;
    int64_t base = 0, size = 0, pos = 0;
};

int io_read(void *opaque, uint8_t *buf, int n) { return ((Bytes *)opaque)->read(buf, n); }
int64_t io_seek(void *opaque, int64_t off, int whence)
{
    return ((Bytes *)opaque)->seek(off, whence & ~AVSEEK_FORCE);
}

AVCodecContext *open_decoder(AVStream *st)
{
    const AVCodec *codec = avcodec_find_decoder(st->codecpar->codec_id);
    if (!codec)
        return 0;
    AVCodecContext *c = avcodec_alloc_context3(codec);
    if (!c)
        return 0;
    if (avcodec_parameters_to_context(c, st->codecpar) < 0 || avcodec_open2(c, codec, 0) < 0) {
        avcodec_free_context(&c);
        return 0;
    }
    c->pkt_timebase = st->time_base;
    return c;
}

}  // namespace

Bytes *memory_bytes(const void *data, size_t size) { return new MemoryBytes(data, size); }
Bytes *handle_bytes(HANDLE h, bool close_handle) { return new HandleBytes(h, close_handle); }

Media *Media::open(Bytes *bytes, const char *what)
{
    Media *m = new Media;
    m->bytes = bytes;
    m->what = what ? what : "?";
    if (!m->open_streams()) {
        delete m;
        return 0;
    }
    return m;
}

Media::~Media()
{
    close_streams();
    delete bytes;
}

bool Media::open_streams()
{
    enum { BUFSIZE = 32768 };
    bytes->seek(0, SEEK_SET);
    uint8_t *buf = (uint8_t *)av_malloc(BUFSIZE);
    io = avio_alloc_context(buf, BUFSIZE, 0, bytes, io_read, 0, io_seek);
    fmt = avformat_alloc_context();
    if (!io || !fmt) {
        MEDIA_LOG("%s: out of memory\n", what.c_str());
        return false;
    }
    fmt->pb = io;
    fmt->flags |= AVFMT_FLAG_CUSTOM_IO;
    if (avformat_open_input(&fmt, 0, 0, 0) < 0) {
        MEDIA_LOG("%s: not a file FFmpeg can read\n", what.c_str());
        fmt = 0;  // freed by avformat_open_input
        return false;
    }
    if (avformat_find_stream_info(fmt, 0) < 0)
        MEDIA_LOG("%s: no stream information\n", what.c_str());
    audio_index = av_find_best_stream(fmt, AVMEDIA_TYPE_AUDIO, -1, -1, 0, 0);
    video_index = av_find_best_stream(fmt, AVMEDIA_TYPE_VIDEO, -1, -1, 0, 0);
    if (audio_index >= 0 && !(audio = open_decoder(fmt->streams[audio_index]))) {
        MEDIA_LOG("%s: no decoder for its audio\n", what.c_str());
        audio_index = -1;
    }
    if (video_index >= 0 && !(video = open_decoder(fmt->streams[video_index]))) {
        MEDIA_LOG("%s: no decoder for its video\n", what.c_str());
        video_index = -1;
    }
    if (audio) {
        rate = audio->sample_rate;
        AVChannelLayout stereo = AV_CHANNEL_LAYOUT_STEREO;
        if (swr_alloc_set_opts2(&swr, &stereo, AV_SAMPLE_FMT_FLT, rate, &audio->ch_layout, audio->sample_fmt,
                                rate, 0, 0) < 0 ||
            swr_init(swr) < 0) {
            MEDIA_LOG("%s: cannot convert its audio\n", what.c_str());
            swr_free(&swr);
            avcodec_free_context(&audio);
            audio_index = -1;
        }
        aframe = av_frame_alloc();
    }
    return audio_index >= 0 || video_index >= 0;
}

void Media::close_streams()
{
    swr_free(&swr);
    av_frame_free(&aframe);
    avcodec_free_context(&audio);
    avcodec_free_context(&video);
    if (fmt)
        avformat_close_input(&fmt);
    if (io) {
        av_freep(&io->buffer);
        avio_context_free(&io);
    }
    audio_index = video_index = -1;
}

bool Media::rewind()
{
    // A fresh open is the one way back to the start that every demuxer supports.
    close_streams();
    return open_streams();
}

bool Media::read(AVPacket *pkt)
{
    for (;;) {
        if (av_read_frame(fmt, pkt) < 0)
            return false;
        if (pkt->stream_index == audio_index || pkt->stream_index == video_index)
            return true;
        av_packet_unref(pkt);
    }
}

void Media::convert(AVFrame *f, std::vector<float> &out)
{
    int max = swr_get_out_samples(swr, f->nb_samples);
    if (max <= 0)
        return;
    size_t at = out.size();
    out.resize(at + (size_t)max * CHANNELS);
    uint8_t *dst = (uint8_t *)&out[at];
    int got = swr_convert(swr, &dst, max, (const uint8_t **)f->extended_data, f->nb_samples);
    out.resize(at + (size_t)(got > 0 ? got : 0) * CHANNELS);
}

void Media::decode_audio(const AVPacket *pkt, std::vector<float> &out)
{
    if (!audio)
        return;
    if (avcodec_send_packet(audio, pkt) < 0)
        return;
    while (avcodec_receive_frame(audio, aframe) == 0) {
        convert(aframe, out);
        av_frame_unref(aframe);
    }
}

bool Media::decode_video(const AVPacket *pkt, AVFrame *frame)
{
    if (!video)
        return false;
    if (avcodec_send_packet(video, pkt) < 0)
        return false;
    return avcodec_receive_frame(video, frame) == 0;
}

AVRational Media::frame_rate() const
{
    if (video_index < 0)
        return AVRational{0, 1};
    AVStream *st = fmt->streams[video_index];
    AVRational r = st->avg_frame_rate.num ? st->avg_frame_rate : st->r_frame_rate;
    if (!r.num || !r.den)
        r = AVRational{st->time_base.den, st->time_base.num ? st->time_base.num : 1};
    return r;
}

int Media::frame_count() const
{
    if (video_index < 0)
        return 0;
    AVStream *st = fmt->streams[video_index];
    if (st->nb_frames > 0)
        return (int)st->nb_frames;
    if (st->duration > 0)
        return (int)st->duration;  // the RAD demuxers count the duration in frames
    return 0;
}

bool decode_all(Bytes *bytes, const char *what, std::vector<float> &out, int &rate)
{
    Media *m = Media::open(bytes, what);
    if (!m)
        return false;
    bool ok = m->audio != 0;
    if (ok) {
        rate = m->rate;
        AVPacket *pkt = av_packet_alloc();
        while (m->read(pkt)) {
            if (pkt->stream_index == m->audio_index)
                m->decode_audio(pkt, out);
            av_packet_unref(pkt);
        }
        m->decode_audio(0, out);  // drain
        av_packet_free(&pkt);
    }
    delete m;
    return ok;
}

}  // namespace media
