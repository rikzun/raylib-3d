#include "render.h"

void Render::createSurface() {
    VkSurfaceKHR surface;

    #ifdef DEBUG_BUILD
        print("Creating Surface");

        if (!SDL_Vulkan_CreateSurface(m_Window, m_Instance, nullptr, &surface)) {
            throw std::runtime_error("Surface creating caused an error");
        }

        print("Surface was created successfully");
    #else
        SDL_Vulkan_CreateSurface(m_Window, m_Instance, nullptr, &surface);
    #endif
    
    m_Surface = surface;
}