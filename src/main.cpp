#define SDL_MAIN_USE_CALLBACKS
#include "GUI.hpp"
#include <SDL3/SDL_main.h>

SDL_AppResult SDL_AppInit(void** appstate, int argc, char** argv) {
    auto* gui = new GUI();
    *appstate = gui;
    if (argc > 1) gui->loadFile(argv[1]);
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate) {
    return static_cast<GUI*>(appstate)->handleIterate();
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {
    return static_cast<GUI*>(appstate)->handleEvent(event);
}

void SDL_AppQuit(void* appstate, SDL_AppResult result [[maybe_unused ]]) {
    if (appstate != NULL) delete static_cast<GUI*>(appstate);
}