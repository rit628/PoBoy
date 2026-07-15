#include "AudioStreamer.hpp"
#include <SDL3/SDL_audio.h>
#include <SDL3/SDL_init.h>
#include <stdexcept>

AudioStreamer::AudioStreamer() {
    using namespace std::string_literals;
    SDL_Init(SDL_INIT_AUDIO);
    audioStream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &streamFormat, NULL, NULL);
    if (!audioStream || !SDL_ResumeAudioStreamDevice(audioStream)) {
        throw std::runtime_error("Audio streamer failed to initialize: "s + SDL_GetError());
    }
    SDL_SetAudioStreamGain(audioStream, 0.50f);
}

AudioStreamer::~AudioStreamer() {
    SDL_DestroyAudioStream(audioStream);
}

void AudioStreamer::queueAudioData(std::span<const float> data) {
    /* SDL can handle downsampling */
    SDL_PutAudioStreamData(audioStream, data.data(), sizeof(float) * data.size());
}