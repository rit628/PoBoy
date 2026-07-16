#pragma once
#include "PixelFetcher.hpp"

namespace Graphics {
    
    inline void PixelFetcher::tick(this auto&& self) {
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

    inline bool PixelFetcher::asleep() {
        return state == STATE::SLEEP;
    }

    inline void PixelFetcher::preTick(this auto&& self) {
        self.preTick();
    }

    inline void PixelFetcher::executeGetTile(this auto&& self) {
        if (!self.onSecondDot) return; // wait for one dot
        self.getTile();
        self.state = STATE::GET_TILE_DATA_LO;
    }
    
    inline void PixelFetcher::executeGetTileDataLo(this auto&& self) {
        if (!self.onSecondDot) return; // wait for one dot
        self.getTileDataLo();
        self.state = STATE::GET_TILE_DATA_HI;
    }
    
    inline void PixelFetcher::executeGetTileDataHi(this auto&& self) {
        if (!self.onSecondDot) return; // wait for one dot
        self.getTileDataHi();
        self.state = STATE::SLEEP;
        self.executeSleep();
    }
    
    inline void PixelFetcher::executeSleep(this auto&& self) {
        self.sleep();
        if (self.state == STATE::PUSH) self.executePush();
    }
    
    inline void PixelFetcher::executePush(this auto&& self) {
        self.push();
        self.state = STATE::GET_TILE;
        self.onSecondDot = false;
    }

}