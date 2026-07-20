#define SDL_MAIN_USE_CALLBACKS
#include <array>
#include "DMG.hpp"
#include "GUI.hpp"
#include <filesystem>
#include <SDL3/SDL_dialog.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_hints.h>
#include <functional>

struct AppState {
    GUI gui;
    DMG gb{std::bind(&GUI::readInput, std::ref(gui))
         , std::bind(&GUI::queueAudioData, std::ref(gui), std::placeholders::_1)
         , std::bind(&GUI::renderFrame, std::ref(gui), std::placeholders::_1)};
    bool running = false;
};

void loadRom(void* userdata, const char * const * filelist, int filter [[ maybe_unused ]]) {
    if (!filelist || !*filelist) return;
    auto* app = static_cast<AppState*>(userdata);
    const std::filesystem::path romFile = filelist[0];
    if (std::filesystem::exists(romFile)) {
        auto& gb = app->gb;
        auto metadata = gb.loadRom(romFile);
        app->gui.updateWindow(metadata.title.data());
        app->running = true;
        SDL_ResetHint(SDL_HINT_MAIN_CALLBACK_RATE);
        SDL_Event wakeEvent;
        SDL_zero(wakeEvent);
        wakeEvent.type = SDL_EVENT_USER;
        SDL_PushEvent(&wakeEvent);
    }
}

SDL_AppResult SDL_AppInit(void** appstate, int argc, char** argv) {
    auto* app = new AppState();
    *appstate = app;
    SDL_SetHint(SDL_HINT_MAIN_CALLBACK_RATE, "waitevent");
    if (argc > 1) loadRom(app, &argv[1], 0);
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate) {
    auto* app = static_cast<AppState*>(appstate);
    if (app->running) [[ likely ]] app->gb.frameAdvance();
    else app->gui.renderInterface();
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
            static constexpr std::array<SDL_DialogFileFilter, 2> filters = {{
                {"GameBoy and GameBoy Color ROMs", "gb;gbc"},
                {"All Files", "*"}
            }};
            SDL_ShowOpenFileDialog(loadRom, app, gui.getWindow(), filters.data(), filters.size(), NULL, false);
        break;

        case SDL_EVENT_QUIT: return SDL_APP_SUCCESS;
    }
    gui.handleInput(event);
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result [[maybe_unused ]]) {
    if (appstate != NULL) delete static_cast<AppState*>(appstate);
}