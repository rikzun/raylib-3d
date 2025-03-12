#pragma once
#define VULKAN_HPP_NO_EXCEPTIONS
#include <vulkan/vulkan.hpp>
#include <SDL3/SDL_vulkan.h>
#include <optional>

#ifdef DEBUG_BUILD
    #include <iostream>

    #define print(value) std::cout << value << std::endl
    #define printi(value) std::cout << "  " << value << std::endl
#endif

#define VK_SUCCESS(result) result == vk::Result::eSuccess
#define VK_FAILED_ERROR(result, message)\
if (result != vk::Result::eSuccess) {\
    throw std::runtime_error(message);\
}

#define COUNT(vector) static_cast<uint32_t>(vector.size())

class Render {
    public:
        Render(SDL_Window* window);
        ~Render();
        void init();

    private:
        bool m_DebugLayer = false;

        SDL_Window* m_Window;
        vk::Instance m_Instance;
        vk::SurfaceKHR m_Surface;
        vk::PhysicalDevice m_PhysicalDevice;
        vk::Device m_LogicalDevice;

        vk::detail::DispatchLoaderDynamic m_dispatcher;
        vk::DebugUtilsMessengerEXT m_DebugMessenger;

        std::optional<uint32_t> m_QueueGraphicFamilyIndex;
        std::optional<uint32_t> m_QueuePresentFamilyIndex;
        vk::Queue m_GraphicQueue;
        vk::Queue m_PresentQueue;

        void createInstance();
        void createDebugMessenger();
        void createSurface();
        void selectPhysicalDevice();
        void selectQueueFamilyIndexes();
        void createLogicalDevice();
};