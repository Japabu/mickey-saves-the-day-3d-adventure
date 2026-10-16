// movie.h - what Bink and Smacker share: a movie read from a Win32 file handle, decoded
// frame by frame on the caller's thread, its sound played through the audio device.
#ifndef PORT_MEDIA_MOVIE_H
#define PORT_MEDIA_MOVIE_H

#include "media.h"

#include <deque>

namespace media {

class Movie {
public:
    ~Movie();
    // Reads from the handle's current position; the handle stays the caller's.
    bool open(HANDLE h, const char *what);

    long width() const { return w; }
    long height() const { return h; }
    long frames() const { return count; }
    AVRational rate() const { return fps; }

    // Makes `frame` hold picture `index` (0-based): decodes forward, or starts over for an
    // earlier one. False if the file ends first.
    bool decode(long index);
    AVFrame *frame = 0;
    long decoded = -1;  // the index `frame` holds

    // The clock: the sound starts and time 0 is set by start(); due(index) says whether
    // picture `index` should be on screen by now.
    void start();
    bool started() const { return running; }
    bool due(long index) const;
    // Back to the first picture, sound and clock included (start() runs again).
    void rewind();

private:
    bool fill(bool want_video);  // reads one packet; false at the end
    void lead_audio();          // reads ahead until the sound is half a second ahead
    void drop_packets();
    double seconds_queued() const;
    Media *m = 0;
    std::deque<AVPacket *> video;
    SDL_AudioStream *sound = 0;
    std::vector<float> pcm;
    long w = 0, h = 0, count = 0;
    AVRational fps{10, 1};
    bool running = false, eof = false;
    uint64_t start_ns = 0;
    std::string name;
};

// Picture conversion for BinkCopyToBuffer: scales `src` to dw x dh pixels of `fmt` and
// copies what fits into dest (pitch bytes per row, rows rows, at x, y).
void blit_frame(AVFrame *src, AVPixelFormat fmt, int dw, int dh, uint8_t *dest, long pitch, long rows, long x,
                long y);

}  // namespace media

#endif
