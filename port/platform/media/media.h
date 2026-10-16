// media.h - shared pieces of the media layer: the audio output that Miles, Bink and
// Smacker play through (SDL3), and reading audio and video with FFmpeg.
#ifndef PORT_MEDIA_MEDIA_H
#define PORT_MEDIA_MEDIA_H

#include "../win32/internal.h"
#include "rad.h"

#include <SDL3/SDL.h>
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>
}

#include <stdint.h>
#include <string>
#include <vector>

namespace media {

#define MEDIA_LOG(...) win32::log("media", __VA_ARGS__)

// ---- audio output ----

// The output device, opened on first use (AIL_quick_startup or a movie); 0 if there is
// no audio. Every voice is an SDL audio stream bound to it, SDL mixes them.
SDL_AudioDeviceID audio_device();
void audio_shutdown();
// Master volume, 0..127 (Miles' digital master volume): the device gain.
void set_master_volume(long volume);
long master_volume();

// Decoded audio is always interleaved 32-bit float stereo, at the file's own rate.
enum { CHANNELS = 2 };
SDL_AudioSpec float_spec(int rate);

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
    long frame_count() const;

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
