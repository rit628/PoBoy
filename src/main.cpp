#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_dialog.h>
#include <filesystem>
#include "DMG.hpp"
#include "GUI.hpp"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_hints.h>
#include <functional>
#include <thread>


struct AppState {
    GUI<> gui;
    DMG gb{std::bind(&GUI<>::readInput, std::ref(gui))
         , std::bind(&GUI<>::queueAudioData, std::ref(gui), std::placeholders::_1)
         , std::bind(&GUI<>::renderFrame, std::ref(gui), std::placeholders::_1)};
    std::jthread emulatorThread;
};

void loadRom(void* userdata, const char * const * filelist, int filter [[ maybe_unused ]]) {
    if (!filelist) return;
    auto* app = static_cast<AppState*>(userdata);
    const std::filesystem::path romFile = filelist[0];
    if (std::filesystem::exists(romFile)) {
        auto& gb = app->gb;
        if (app->emulatorThread.joinable()) {
            app->emulatorThread.request_stop();
            app->emulatorThread.join();
        }
        auto metadata = gb.loadRom(romFile);
        app->emulatorThread = std::jthread([&gb](std::stop_token stoken) { gb.run(stoken); });
        app->gui.updateWindow(metadata.title.data());
    }
}

SDL_AppResult SDL_AppInit(void** appstate, int argc, char** argv) {
    auto* app = new AppState();
    *appstate = app;
    SDL_SetHint(SDL_HINT_MAIN_CALLBACK_RATE, "waitevent");
    if (argc > 1) loadRom(app, &argv[1], 0);
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate [[maybe_unused ]]) {
    auto* app = static_cast<AppState*>(appstate);
    if (!app->emulatorThread.joinable()) {
        app->gui.renderInterface();
    }
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {
    auto* app = static_cast<AppState*>(appstate);
    auto& gui = app->gui;
    switch (event->type) {
        case SDL_EVENT_WINDOW_RESIZED:
            gui.updateWindow();
        break;

        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            SDL_ShowOpenFileDialog(loadRom, app, gui.getWindow(), NULL, 0, NULL, false);
        break;

        case gui.SDL_EVENT_RENDER_FRAME: return SDL_APP_CONTINUE;
        case SDL_EVENT_QUIT: return SDL_APP_SUCCESS;
    }
    gui.handleInput(event);
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result [[maybe_unused ]]) {
    delete static_cast<AppState*>(appstate);
}