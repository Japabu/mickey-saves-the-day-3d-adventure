// media.h - shared pieces of the media layer: the sound output that Miles, Bink and
// Smacker play through (mix.cpp, on the host's output), and reading audio and video with
// FFmpeg.
#ifndef PORT_MEDIA_MEDIA_H
#define PORT_MEDIA_MEDIA_H

#include "../win32/internal.h"
#include "rad.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>
}

#include <mutex>
#include <stdint.h>
#include <string>
#include <vector>

namespace media {

#define MEDIA_LOG(...) win32::log("media", __VA_ARGS__)

// ---- sound output ----

// Decoded audio is always interleaved 32-bit float stereo, at the file's own rate.
enum { CHANNELS = 2 };

// A sound being played: frames put into it (or pulled by its callback) are resampled to
// the output, scaled by its gain and mixed with the others while it is bound. The
// mixer calls the pull callback, and reads the stream, under the stream's lock; the
// lock is recursive.
class Stream {
public:
    typedef void (*Pull)(void *user, Stream *s, int frames);
    explicit Stream(int rate);
    ~Stream();
    void put(const float *frames, int count);
    void clear();
    int queued();                       // frames put and not yet played
    void set_gain(float gain);
    void set_ratio(float ratio);        // plays at rate * ratio (pitch)
    void set_pull(Pull pull, void *user);  // asked for frames when the queue runs short
    void bind(bool on);                 // mixed into the output or not
    void lock();
    void unlock();

private:
    friend struct Mixer;
    int rate;
    std::vector<float> queue;           // frames not yet played, from `head`
    size_t head = 0;
    double frac = 0;                    // position between the first two queued frames
    float gain = 1, ratio = 1;
    Pull pull = 0;
    void *pull_user = 0;
    bool bound = false;
    std::recursive_mutex mu;
};

// Opens the host's sound output on first use (AIL_quick_startup or a movie); false if
// there is none.
bool audio_open();
void audio_shutdown();
// Master volume, 0..127 (Miles' digital master volume): scales the whole output.
void set_master_volume(int volume);
int master_volume();

// ---- reading with FFmpeg ----

// Where a file's bytes come from: memory the caller keeps alive, or a Win32 file handle
// (read through ReadFile from a starting offset, so files inside the CD image work).
struct Bytes {
    virtual ~Bytes() {}
    virtual int read(uint8_t *buf, int n) = 0;
    virtual int64_t seek(int64_t offset, int whence) = 0;  // whence: SEEK_*, or AVSEEK_SIZE
};
Bytes *memory_bytes(const void *data, size_t size);
// The handle stays the caller's; reading starts at its current file position.
Bytes *handle_bytes(HANDLE h, bool close_handle);

// One opened file: its container and a decoder for its first audio and first video
// stream (either may be missing). rewind() starts over from the beginning.
class Media {
public:
    // Takes ownership of `bytes`. Returns 0 if FFmpeg cannot read it.
    static Media *open(Bytes *bytes, const char *what);
    ~Media();
    bool rewind();

    // The next packet of either stream; false at the end or on an error.
    bool read(AVPacket *pkt);

    AVFormatContext *fmt = 0;
    int audio_index = -1, video_index = -1;
    AVCodecContext *audio = 0, *video = 0;
    int rate = 0;  // audio sample rate

    // Audio: decodes one packet of the audio stream and appends float stereo frames.
    void decode_audio(const AVPacket *pkt, std::vector<float> &out);
    // Video: decodes one packet of the video stream; true when `frame` holds a picture.
    bool decode_video(const AVPacket *pkt, AVFrame *frame);
    // Video frame rate as a fraction (frames per second = num / den).
    AVRational frame_rate() const;
    int frame_count() const;

private:
    Media() {}
    bool open_streams();
    void close_streams();
    void convert(AVFrame *f, std::vector<float> &out);
    Bytes *bytes = 0;
    AVIOContext *io = 0;
    SwrContext *swr = 0;
    AVFrame *aframe = 0;
    std::string what;
};

// Decodes a whole file's audio (float stereo); false if it has none.
bool decode_all(Bytes *bytes, const char *what, std::vector<float> &out, int &rate);

}  // namespace media

#endif
