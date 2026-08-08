#include "AudioStreamer.hpp"
#include <SDL3/SDL_audio.h>
#include <SDL3/SDL_init.h>
#include <cstdlib>
#include <iostream>
#include <print>

AudioStreamer::AudioStreamer() {
    audioStream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &streamFormat, NULL, NULL);
    if (!audioStream || !SDL_ResumeAudioStreamDevice(audioStream)) {
        std::println(std::cerr, "Audio streamer failed to initialize: {}", SDL_GetError());
        exit(EXIT_FAILURE);
    }
}

AudioStreamer::~AudioStreamer() {
    SDL_DestroyAudioStream(audioStream);
}

void AudioStreamer::queueAudioData(std::span<const float> data) {
    /* drop old audio buffer in case of desync */
    if (size_t(SDL_GetAudioStreamQueued(audioStream)) > STREAM_QUEUE_MAX) {
        SDL_ClearAudioStream(audioStream);
    }
    /* SDL can handle downsampling */
    SDL_PutAudioStreamData(audioStream, data.data(), sizeof(float) * data.size());
}