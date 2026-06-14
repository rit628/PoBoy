#include "GraphicsConstants.hpp"
#include <iostream>
#include <codecvt>

namespace {
    using namespace Graphics;
    // temporary rendering function to test out stuff for now
    void renderFrame(std::array<uint8_t, FRAMEBUFFER_SIZE>&& framebuffer) {
        static constexpr std::array<std::u16string, 4> palette = { u"░░", u"▒▒", u"▓▓", u"██" };
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
}