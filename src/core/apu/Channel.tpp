#pragma once
#include "Channel.hpp"
#include "LengthController.hpp"
#include <cstdint>
#include <utility>

namespace Audio {

    template<VOLUME_TYPE VolumeType, TICK_RATE TickRate>
    inline uint8_t Channel<VolumeType, TickRate>::tick(this auto&& self) {
        if (!self.enabled) return 0;
        if (--self.periodTimer == 0) {
            self.resetPeriodTimer();
            self.advanceOutput();
        }
        return self.sample();
    }

    template<VOLUME_TYPE VolumeType, TICK_RATE TickRate>
    inline void Channel<VolumeType, TickRate>::tickLength() {
        lengthController.tick(enabled);
    }
    
    template<VOLUME_TYPE VolumeType, TICK_RATE TickRate>
    inline void Channel<VolumeType, TickRate>::tickEnvelope() requires (VolumeType == VOLUME_TYPE::ENVELOPE) {
        this->envelopeGenerator.tick();
    }

    template<VOLUME_TYPE VolumeType, TICK_RATE TickRate>
    inline bool Channel<VolumeType, TickRate>::dacEnabled(this auto&& self) {
        if constexpr (VolumeType == VOLUME_TYPE::ENVELOPE) {
            return (self.envelopeGenerator.readRegister() & 0xF8) != 0;
        }
        else {
            return self.dacEnable;
        }
    }

    template<VOLUME_TYPE VolumeType, TICK_RATE TickRate>
    inline bool Channel<VolumeType, TickRate>::on(this auto&& self) {
        return self.enabled;
    }

    template<VOLUME_TYPE VolumeType, TICK_RATE TickRate>
    inline void Channel<VolumeType, TickRate>::writeEnvelope(uint8_t value) requires (VolumeType == VOLUME_TYPE::ENVELOPE) {
        this->envelopeGenerator.writeRegister(value);
        if (!dacEnabled()) enabled = false;
    }

    template<VOLUME_TYPE VolumeType, TICK_RATE TickRate>
    inline void Channel<VolumeType, TickRate>::channelControl(this auto&& self, uint8_t value) {
        bool triggered = value >> 7;
        self.lengthController.setState(value & 0x40);
        self.setPeriodHi(value);
        if (triggered) {
            self.enabled = true;
            self.resetPeriodTimer();
            self.lengthController.trigger();
            if constexpr (VolumeType == VOLUME_TYPE::ENVELOPE) self.envelopeGenerator.trigger();
            self.trigger();
        }
    }

    template<VOLUME_TYPE VolumeType, TICK_RATE TickRate>
    inline void Channel<VolumeType, TickRate>::setPeriodLo(uint8_t value) {
        period = (period & 0xFF00) | value;
    }

    template<VOLUME_TYPE VolumeType, TICK_RATE TickRate>
    inline void Channel<VolumeType, TickRate>::setPeriodHi(uint8_t value) {
        period = (period & 0x00FF) | ((value & 0x07) << 8);
    }
    
    template<VOLUME_TYPE VolumeType, TICK_RATE TickRate>
    inline void Channel<VolumeType, TickRate>::resetPeriodTimer() {
        periodTimer = (PERIOD_MAX - period) * std::to_underlying(TickRate);
    }

}