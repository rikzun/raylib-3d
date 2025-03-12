#include "render.h"

Render::Render(SDL_Window* window) {
    m_Window = window;
}

Render::~Render() {

}

void Render::init() {
    try {
        Render::createInstance();
        Render::createDebugMessenger();
        Render::createSurface();
        Render::selectPhysicalDevice();
        Render::selectQueueFamilyIndexes();
        Render::createLogicalDevice();
    } catch(const std::runtime_error& error) {
        std::cout << error.what() << std::endl;
    }
}