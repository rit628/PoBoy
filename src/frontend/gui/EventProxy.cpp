#include <SDL3/SDL_events.h>
#include <SDL3/SDL_stdinc.h>

extern "C" {

    void pushFileEvent(const char *filename) {
        SDL_Event e;
        SDL_zero(e);
        e.type = SDL_EVENT_DROP_FILE;
        e.drop.data = SDL_strdup(filename);
        SDL_PushEvent(&e);
        SDL_free(&e.drop.data);
    }

    void pushKeyboardEvent(int scancode, bool pressed) {
        SDL_Event e;
        SDL_zero(e);
        e.type = pressed ? SDL_EVENT_KEY_DOWN : SDL_EVENT_KEY_UP;
        e.key.scancode = static_cast<SDL_Scancode>(scancode);
        e.key.key = SDL_GetKeyFromScancode(static_cast<SDL_Scancode>(scancode), SDL_KMOD_NONE, true);
        e.key.mod = SDL_KMOD_NONE;
        e.key.down = pressed;
        e.key.repeat = false;
        SDL_PushEvent(&e);
    }

}