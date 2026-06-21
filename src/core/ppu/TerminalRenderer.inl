#include "GraphicsConstants.hpp"
#include <array>
#include <cstdint>
#include <iostream>
#include <codecvt>

namespace {
    using namespace Graphics;
    // temporary rendering function to test out stuff for now
    void renderFrame(std::array<uint8_t, FRAMEBUFFER_SIZE>&& framebuffer) {
        static std::array<std::u16string, 4> palette = { u"\x1b[38;2;230;230;230m██\x1b[0m"
                                                       , u"\x1b[38;2;184;184;184m██\x1b[0m"
                                                       , u"\x1b[38;2;120;120;120m██\x1b[0m"
                                                       , u"\x1b[38;2;40;40;40m██\x1b[0m"
                                                    };
        std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
        uint8_t currentColumn = 0;
        auto renderPixel = [&](uint8_t paletteIndex) {
            auto pixel = convert.to_bytes(palette.at(paletteIndex));
            std::print(std::cout, "{}", pixel);
            if (++currentColumn == LCD_WIDTH) {
                currentColumn = 0;
                std::println(std::cout);
            }
        };

        for (auto&& byte : framebuffer) {
            for (uint8_t pixelBit = 0; pixelBit < 8; pixelBit += BITS_PER_PIXEL) {
                renderPixel((byte >> pixelBit) & 0b11); // shift down and grab bottom two bits
            }  
        }
        
        std::println(std::cout, "\033[H");
        std::cout.flush();
    }

    void flushVRAM(uint8_t bgp, std::array<uint8_t, VRAM_SIZE>& vram) {
        std::cout.write(reinterpret_cast<char*>(&bgp), 1);
        std::cout.write(reinterpret_cast<const char *>(vram.data()), vram.size());
    }

}