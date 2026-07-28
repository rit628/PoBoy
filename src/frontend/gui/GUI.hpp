#pragma once
#include "AudioStreamer.hpp"
#include "DMG.hpp"
#include "InputManager.hpp"
#include "Renderer.hpp"
#include <SDL3/SDL.h>
#include <SDL3/SDL_init.h>
#include <cstdint>
#include <memory>

class GUI {
    public:
        GUI();
        ~GUI();

        SDL_AppResult handleIterate();
        SDL_AppResult handleEvent(SDL_Event* event);
        
        template<bool Unlocked>
        void updateSpeed();
        void loadFile(const std::filesystem::path file);

    private:
        static void fileLoadCallback(void* userdata, const char * const * filelist, int filter);

        void updateFps();
        void renderInterface();

        uint64_t frameCount = 0;
        uint64_t lastTime = 0;
        bool running = false;
        bool speedUnlocked = false;
        
        SDL_Window* window;
        std::string windowTitle = "PoBoy";
        std::unique_ptr<Renderer> renderer;
        std::unique_ptr<AudioStreamer> audioStreamer;
        std::unique_ptr<InputManager> inputManager;

        std::unique_ptr<DMG> gb;
};