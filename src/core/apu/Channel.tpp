#pragma once
#include "Channel.hpp"
#include <utility>

namespace Audio {

    template<VOLUME_TYPE VolumeType, TICK_RATE TickRate>
    inline void Channel<VolumeType, TickRate>::tickLength() {
        lengthController.tick(enabled);
    }
    
    template<VOLUME_TYPE VolumeType, TICK_RATE TickRate>
    inline void Channel<VolumeType, TickRate>::tickEnvelope() requires (VolumeType == VOLUME_TYPE::ENVELOPE) {
        this->envelopeGenerator.tick();
    }
    
    template<VOLUME_TYPE VolumeType, TICK_RATE TickRate>
    inline void Channel<VolumeType, TickRate>::resetPeriodTimer() {
        periodTimer = (PERIOD_MAX - period) * std::to_underlying(TickRate);
    }

}