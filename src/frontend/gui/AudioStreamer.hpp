#pragma once
#include "AudioConstants.hpp"
#include <SDL3/SDL_audio.h>
#include <span>

class AudioStreamer {
    public:
        AudioStreamer();
        ~AudioStreamer();
        void queueAudioData(std::span<const float> data);

    private:
        SDL_AudioStream* audioStream = nullptr;
        const SDL_AudioSpec streamFormat {
            SDL_AUDIO_F32,
            2,
            Audio::OUTPUT_SAMPLE_RATE
        };
};