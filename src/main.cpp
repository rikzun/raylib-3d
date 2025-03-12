#include <SDL3/SDL.h>
#include "render/render.h"

int main() {
    SDL_Window* window = SDL_CreateWindow("Vulkan in my ass", 800, 600, SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);
    Render render {window};
    render.init();

    bool running = true;
    while(running) {
        for (SDL_Event event; SDL_PollEvent(&event);)
        switch (event.type) {
            case SDL_EVENT_QUIT:
                running = false;
                break;

            default: break;
        }

        SDL_Delay(1);
    }
}