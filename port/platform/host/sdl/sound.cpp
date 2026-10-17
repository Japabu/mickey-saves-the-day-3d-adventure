// sound.cpp - the host's sound output on SDL3: the default playback device, fed from
// SDL's audio thread with what the platform mixes (SDL converts to the device's format).
#include <SDL3/SDL.h>

#include "../host.h"

#include <stdio.h>
#include <vector>

namespace host {

static SDL_AudioStream *s_stream;
static SoundFill s_fill;
static void *s_user;
static std::vector<float> s_buf;
enum { RATE = 48000 };

static void SDLCALL feed(void *, SDL_AudioStream *stream, int additional, int)
{
    int frames = additional / (int)(2 * sizeof(float));
    if (frames <= 0)
        return;
    s_buf.resize((size_t)frames * 2);
    s_fill(s_buf.data(), frames, s_user);
    SDL_PutAudioStreamData(stream, s_buf.data(), frames * (int)(2 * sizeof(float)));
}

int sound_open(SoundFill fill, void *user)
{
    if (s_stream)
        return RATE;
    if (!SDL_InitSubSystem(SDL_INIT_AUDIO)) {
        fprintf(stderr, "no audio: %s\n", SDL_GetError());
        return 0;
    }
    s_fill = fill;
    s_user = user;
    SDL_AudioSpec spec = {SDL_AUDIO_F32, 2, RATE};
    s_stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, feed, 0);
    if (!s_stream) {
        fprintf(stderr, "no audio device: %s\n", SDL_GetError());
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
        return 0;
    }
    SDL_ResumeAudioStreamDevice(s_stream);
    return RATE;
}

void sound_close()
{
    if (!s_stream)
        return;
    SDL_DestroyAudioStream(s_stream);
    s_stream = 0;
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

}  // namespace host
