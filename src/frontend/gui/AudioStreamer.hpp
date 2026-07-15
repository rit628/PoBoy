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
        static constexpr SDL_AudioSpec streamFormat {
            SDL_AUDIO_F32,
            2,
            Audio::OUTPUT_SAMPLE_RATE
        };

        /* 250 ms audio buffer cap */
        static constexpr size_t STREAM_QUEUE_MAX = Audio::OUTPUT_SAMPLE_RATE * sizeof(float) / 4;

        SDL_AudioStream* audioStream = nullptr;
};