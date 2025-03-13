#include "render.h"

void Render::createSurface() {
    m_Logger.info("Creating Surface");
    VkSurfaceKHR surface;

    if (!SDL_Vulkan_CreateSurface(m_Window, m_Instance, nullptr, &surface)) {
        throw std::runtime_error("Surface creating caused an error");
    }

    m_Logger.info("Surface was created successfully");
    m_Surface = surface;
}