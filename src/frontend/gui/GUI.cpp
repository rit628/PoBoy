#include "GUI.hpp"
#include "GraphicsConstants.hpp"
#include "Renderer.hpp"
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_video.h>
#include <stdexcept>

GUI::GUI() {
    using namespace std::string_literals;
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
        throw std::runtime_error("SDL failed to initialize: "s + SDL_GetError());
    }

    gameWindow = SDL_CreateWindow("PoBoy", Graphics::LCD_WIDTH, Graphics::LCD_HEIGHT, SDL_WINDOW_RESIZABLE);
    SDL_SetWindowMinimumSize(gameWindow, Graphics::LCD_WIDTH, Graphics::LCD_HEIGHT);
    auto display = SDL_GetDisplayForWindow(gameWindow);
    auto* displayMode = SDL_GetDesktopDisplayMode(display);
    size_t width = displayMode->w / 2, height = displayMode->h / 2;
    uint8_t scale = renderer.getMaxGameScale(width, height);
    SDL_SetWindowSize(gameWindow, scale * Graphics::LCD_WIDTH, scale * Graphics::LCD_HEIGHT);
    renderer.updateWindow(gameWindow);

    if (!gameWindow) {
        throw std::runtime_error("GUI failed to initialize: "s + SDL_GetError());
    }
}

GUI::~GUI() {
    SDL_DestroyWindow(gameWindow);
}

SDL_Window* GUI::getWindow() {
    return gameWindow;
}

void GUI::updateWindow(std::string windowName) {
    if (!windowName.empty()) {
        SDL_SetWindowTitle(gameWindow, ("PoBoy: " + windowName).c_str());
    }
    renderer.updateWindow(gameWindow);
}

void GUI::renderInterface() {
    static constexpr std::array<uint8_t, Graphics::FRAMEBUFFER_SIZE> blank{}; // white screen for now until a proper interface is made
    renderer.renderFrame(blank);
}

void GUI::handleInput(SDL_Event* event) {
    inputManager.handleInput(event);
}

void GUI::renderFrame(std::span<const uint8_t> framebuffer) {
    renderer.renderFrame(framebuffer);
}

void GUI::queueAudioData(std::span<const float> data) {
    audioStreamer.queueAudioData(data);
}

uint8_t GUI::readInput() {
    return inputManager.readInput();
}