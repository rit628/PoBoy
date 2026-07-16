#pragma once
#include "EnvelopeGenerator.hpp"
#include "LengthController.hpp"
#include <cstdint>
#include <utility>

namespace Audio {

    enum class VOLUME_TYPE : bool { SHIFT, ENVELOPE };
    enum class TICK_RATE : uint8_t { M_CYCLE = 4, HALF_M_CYCLE = 2 };

    template<VOLUME_TYPE, TICK_RATE>
    class ChannelBase {};

    template<TICK_RATE TickRate>
    class ChannelBase<VOLUME_TYPE::ENVELOPE, TickRate> {
        protected:
            EnvelopeGenerator envelopeGenerator;  // Manages NRx2 register
    };

    template<VOLUME_TYPE VolumeType = VOLUME_TYPE::ENVELOPE, TICK_RATE TickRate = TICK_RATE::M_CYCLE>
    class Channel : public ChannelBase<VolumeType, TickRate> {
        public:
            Channel();
            void initialize(this auto&& self);
            uint8_t tick(this auto&& self);
            void tickLength();
            void tickEnvelope() requires (VolumeType == VOLUME_TYPE::ENVELOPE);
            bool dacEnabled(this auto&& self);
            bool on(this auto&& self);

        protected:
            void initBase();
            void writeEnvelope(uint8_t value) requires (VolumeType == VOLUME_TYPE::ENVELOPE);
            void channelControl(this auto&& self, uint8_t value);
            void setPeriodLo(uint8_t value);
            void setPeriodHi(uint8_t value);
            void resetPeriodTimer();

            static constexpr uint8_t NRx0 = 0;
            static constexpr uint8_t NRx1 = 1;
            static constexpr uint8_t NRx2 = 2;
            static constexpr uint8_t NRx3 = 3;
            static constexpr uint8_t NRx4 = 4;

            static constexpr uint16_t PERIOD_MAX    = 0x0800;
            static constexpr uint16_t LENGTH_MAX    = 64 << (4 - std::to_underlying(TickRate));

            bool enabled;
            uint16_t periodTimer;
            LengthController<LENGTH_MAX> lengthController;    // Manages NRx1 bits 5-0 and NRx4 bit 6

            /* NRx3 + NRx4 Register Components */
            uint16_t period;    // NRx3 register (bits 7-0) NRx4 register bits 2-0 (bits 10-8)
    };

}

#include "Channel.tpp"