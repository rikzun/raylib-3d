#include <SDL3/SDL.h>
#include "render/render.h"
#include "logger.h"

int main() {
    SDL_Window* window = SDL_CreateWindow("Vulkan in my ass", 800, 600, SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);

    Logger logger("log.txt");
    Render render(window, logger);
    render.init();

    bool running = true;
    while(running) {
        render.draw();
        
        for (SDL_Event event; SDL_PollEvent(&event);)
        switch (event.type) {
            case SDL_EVENT_QUIT:
                running = false;
                break;

            case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
                break;

            default: break;
        }

        SDL_Delay(1);
    }
}