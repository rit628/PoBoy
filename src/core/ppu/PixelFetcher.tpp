#pragma once
#include "PixelFetcher.hpp"

namespace Graphics {
    
    template<typename Self>
    inline void PixelFetcher::tick(this Self&& self) {
        self.preTick();
        switch (self.state) {
            using enum STATE;
            case GET_TILE:
                self.executeGetTile();
            break;
    
            case GET_TILE_DATA_LO:
                self.executeGetTileDataLo();
            break;
    
            case GET_TILE_DATA_HI:
                self.executeGetTileDataHi();
            break;
            
            case SLEEP:
                self.executeSleep();
            break;
    
            case PUSH:
                self.executePush();
            break;
        }
        self.onSecondDot = !self.onSecondDot;
    }
    
    inline Pixel PixelFetcher::fifoPop() {
        return pixelFifo.pop();
    }
    
    inline bool PixelFetcher::fifoEmpty() {
        return pixelFifo.empty();
    }

    inline void PixelFetcher::resetState() {
        state = STATE::GET_TILE;
        onSecondDot = false;
    }

    template<typename Self>
    inline void PixelFetcher::preTick(this Self&& self) {
        self.preTick();
    }

    template<typename Self>
    inline void PixelFetcher::executeGetTile(this Self&& self) {
        if (!self.onSecondDot) return; // wait for one dot
        self.getTile();
        self.state = STATE::GET_TILE_DATA_LO;
    }
    
    template<typename Self>
    inline void PixelFetcher::executeGetTileDataLo(this Self&& self) {
        if (!self.onSecondDot) return; // wait for one dot
        self.getTileDataLo();
        self.state = STATE::GET_TILE_DATA_HI;
    }
    
    template<typename Self>
    inline void PixelFetcher::executeGetTileDataHi(this Self&& self) {
        if (!self.onSecondDot) return; // wait for one dot
        self.getTileDataHi();
        self.state = STATE::SLEEP;
        self.executeSleep();
    }
    
    template<typename Self>
    inline void PixelFetcher::executeSleep(this Self&& self) {
        self.sleep();
        if (self.state == STATE::PUSH) self.executePush();
    }
    
    template<typename Self>
    inline void PixelFetcher::executePush(this Self&& self) {
        self.push();
        self.state = STATE::GET_TILE;
        self.onSecondDot = false;
    }

}