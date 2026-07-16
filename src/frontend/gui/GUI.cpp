#include "GUI.hpp"
#include "GraphicsConstants.hpp"
#include "SoftwareRenderer.hpp"
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_video.h>
#include <stdexcept>

template<typename RendererType>
GUI<RendererType>::GUI() {
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

template<typename RendererType>
GUI<RendererType>::~GUI() {
    SDL_DestroyWindow(gameWindow);
}

template<typename RendererType>
SDL_Window* GUI<RendererType>::getWindow() {
    return gameWindow;
}

template<typename RendererType>
void GUI<RendererType>::updateWindow(std::string windowName) {
    if (!windowName.empty()) {
        SDL_SetWindowTitle(gameWindow, ("PoBoy: " + windowName).c_str());
    }
    renderer.updateWindow(gameWindow);
}

template<typename RendererType>
void GUI<RendererType>::renderInterface() {
    static std::array<uint8_t, Graphics::FRAMEBUFFER_SIZE> blank{}; // white screen for now until a proper interface is made
    renderer.renderFrame(blank);
}

template<typename RendererType>
void GUI<RendererType>::handleInput(SDL_Event* event) {
    inputManager.handleInput(event);
}

template<typename RendererType>
void GUI<RendererType>::renderFrame(std::array<uint8_t, Graphics::FRAMEBUFFER_SIZE>& framebuffer) {
    renderer.renderFrame(framebuffer);
}

template<typename RendererType>
void GUI<RendererType>::queueAudioData(std::span<const float> data) {
    audioStreamer.queueAudioData(data);
}

template<typename RendererType>
uint8_t GUI<RendererType>::readInput() {
    return inputManager.readInput();
}

template class GUI<SoftwareRenderer>;