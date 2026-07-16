#include <SDL3/SDL_events.h>
#define SDL_MAIN_USE_CALLBACKS
#include "DMG.hpp"
#include "GUI.hpp"
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_hints.h>
#include <functional>
#include <iostream>
#include <print>
#include <thread>


struct AppState {
    GUI<> gui;
    DMG gb{std::bind(&GUI<>::readInput, std::ref(gui))
         , std::bind(&GUI<>::queueAudioData, std::ref(gui), std::placeholders::_1)
         , std::bind(&GUI<>::renderFrame, std::ref(gui), std::placeholders::_1)};
    std::jthread emulatorThread;
};

SDL_AppResult SDL_AppInit(void** appstate, int argc, char** argv) {
    const std::filesystem::path romFile = (argc > 1) ? argv[1] : "test.gb";
    std::println(std::cerr, "Running {}", romFile.filename().c_str());
    auto* app = new AppState();
    auto& gb = app->gb;
    app->emulatorThread = std::jthread(
    [&gb](std::stop_token stoken, const std::filesystem::path& romFile) {
        gb.run(stoken, romFile);
    }, romFile);
    *appstate = app;
    SDL_SetHint(SDL_HINT_MAIN_CALLBACK_RATE, "waitevent");
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate [[maybe_unused ]]) {
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {
    auto& gui = static_cast<AppState*>(appstate)->gui;
    switch (event->type) {
        case gui.SDL_EVENT_RENDER_FRAME: return SDL_APP_CONTINUE;
        case SDL_EVENT_WINDOW_RESIZED:
            gui.updateWindow();
            return SDL_APP_CONTINUE;
        break;
        case SDL_EVENT_QUIT: return SDL_APP_SUCCESS;
    }
    gui.handleInput(event);
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result [[maybe_unused ]]) {
    delete static_cast<AppState*>(appstate);
}