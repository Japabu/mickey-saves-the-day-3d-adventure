// audio.cpp - the audio output device every voice of the game plays through.
#include "media.h"

#pragma GCC visibility push(hidden)

namespace media {

static SDL_AudioDeviceID s_device;
static bool s_tried;
static int s_master = 127;

SDL_AudioDeviceID audio_device()
{
    if (s_device || s_tried)
        return s_device;
    s_tried = true;
    if (!SDL_InitSubSystem(SDL_INIT_AUDIO)) {
        MEDIA_LOG("no audio: %s\n", SDL_GetError());
        return 0;
    }
    // The device's own format: each voice's stream converts to it, SDL mixes the voices.
    s_device = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, 0);
    if (!s_device) {
        MEDIA_LOG("no audio device: %s\n", SDL_GetError());
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
        return 0;
    }
    SDL_SetAudioDeviceGain(s_device, s_master / 127.0f);
    return s_device;
}

void audio_shutdown()
{
    if (s_device) {
        SDL_CloseAudioDevice(s_device);
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
    }
    s_device = 0;
    s_tried = false;
}

void set_master_volume(int volume)
{
    s_master = volume < 0 ? 0 : volume > 127 ? 127 : volume;
    if (s_device)
        SDL_SetAudioDeviceGain(s_device, s_master / 127.0f);
}

int master_volume() { return s_master; }

}  // namespace media
