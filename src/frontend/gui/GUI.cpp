#include "GUI.hpp"
#include "GraphicsConstants.hpp"
#include "Renderer.hpp"
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_video.h>
#include <cstdint>
#include <format>
#include <stdexcept>
#include <array>
#include <string>

GUI::GUI() {
    using std::string_literals::operator""s;
    
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS)) {
        throw std::runtime_error("SDL failed to initialize: "s + SDL_GetError());
    }
    SDL_SetHint(SDL_HINT_MAIN_CALLBACK_RATE, "waitevent");

    window = SDL_CreateWindow(windowTitle.c_str(), Graphics::LCD_WIDTH, Graphics::LCD_HEIGHT, SDL_WINDOW_RESIZABLE);
    if (!window) {
        throw std::runtime_error("GUI failed to initialize: "s + SDL_GetError());
    }
    SDL_SetWindowMinimumSize(window, Graphics::LCD_WIDTH, Graphics::LCD_HEIGHT);
    auto display = SDL_GetDisplayForWindow(window);
    auto* displayMode = SDL_GetDesktopDisplayMode(display);
    size_t width = displayMode->w / 2, height = displayMode->h / 2;
    uint8_t scale = Renderer::getMaxGameScale(width, height);
    SDL_SetWindowSize(window, scale * Graphics::LCD_WIDTH, scale * Graphics::LCD_HEIGHT);

    renderer = std::make_unique<Renderer>(window);
    audioStreamer = std::make_unique<AudioStreamer>();
    inputManager = std::make_unique<InputManager>(*this);

    gb = std::make_unique<DMG>(std::bind(&InputManager::readInput, std::ref(*inputManager))
                             , std::bind(&AudioStreamer::queueAudioData, std::ref(*audioStreamer), std::placeholders::_1)
                             , std::bind(&Renderer::renderFrame, std::ref(*renderer), std::placeholders::_1));
}

GUI::~GUI() {
    SDL_DestroyWindow(window);
}

SDL_AppResult GUI::handleIterate() {
    if (running) [[ likely ]] {
        gb->frameAdvance();
        if (!speedUnlocked) gb->synchronizeClock();
        updateFps();
    }
    else {
        renderInterface();
    }
    
    return SDL_APP_CONTINUE;
}

SDL_AppResult GUI::handleEvent(SDL_Event* event) {
    switch (event->type) {
        case SDL_EVENT_WINDOW_RESIZED:
            renderer->updateRenderRegion();
        break;

        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            if (running) return SDL_APP_CONTINUE;
            static constexpr std::array<SDL_DialogFileFilter, 2> filters = {{
                {"GameBoy and GameBoy Color ROMs", "gb;gbc"},
                {"All Files", "*"}
            }};
            SDL_ShowOpenFileDialog(fileLoadCallback
                                  , this
                                  , window
                                  , filters.data()
                                  , filters.size()
                                  , NULL
                                  , false);
        break;

        case SDL_EVENT_DROP_FILE:
            loadFile(event->drop.data);
        break;

        case SDL_EVENT_QUIT: return SDL_APP_SUCCESS;

        default:
            inputManager->handleInput(event);
        break;
    }
    return SDL_APP_CONTINUE;
}

template<bool Unlocked>
void GUI::updateSpeed() {
    speedUnlocked = Unlocked;
    gb->resetClock();
}

void GUI::loadFile(const std::filesystem::path file) {
    if (!std::filesystem::exists(file)) return;
    using std::string_literals::operator""s;
    auto metadata = gb->loadRom(file);
    windowTitle = "PoBoy: "s + metadata.title.data();
    SDL_SetWindowTitle(window, windowTitle.c_str());
    running = true;
    SDL_ResetHint(SDL_HINT_MAIN_CALLBACK_RATE);
    SDL_Event wakeEvent;
    SDL_zero(wakeEvent);
    wakeEvent.type = SDL_EVENT_USER;
    SDL_PushEvent(&wakeEvent);
}

void GUI::fileLoadCallback(void* userdata, const char * const * filelist, int filter [[ maybe_unused ]]) {
    if (!filelist || !*filelist) return;
    auto* gui = static_cast<GUI*>(userdata);
    const std::filesystem::path file = filelist[0];
    gui->loadFile(file);
}

void GUI::updateFps() {
    uint64_t currentTime = SDL_GetTicks();
    frameCount++;
    if (currentTime - lastTime >= 1000) {
        float fps = frameCount * 1000.0 / (currentTime - lastTime);
        auto newTitle = std::format("{}\tFPS: {:.2f}", windowTitle, fps);
        SDL_SetWindowTitle(window, newTitle.c_str());
        lastTime = currentTime;
        frameCount = 0;
    }
}

void GUI::renderInterface() {
    static constexpr std::array<uint8_t, Graphics::FRAMEBUFFER_SIZE> blank{}; // white screen for now until a proper interface is made
    renderer->renderFrame(blank);
}

template void GUI::updateSpeed<true>();
template void GUI::updateSpeed<false>();