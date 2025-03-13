#pragma once
#define VULKAN_HPP_NO_EXCEPTIONS
#include <vulkan/vulkan.hpp>
#include <SDL3/SDL_vulkan.h>
#include <optional>
#include "../logger.h"

#define VK_SUCCESS(result) result == vk::Result::eSuccess
#define VK_FAILED_ERROR(result, message)\
if (result != vk::Result::eSuccess) {\
    throw std::runtime_error(message);\
}

#define COUNT(vector) static_cast<uint32_t>(vector.size())

class Render {
    public:
        Render(SDL_Window* window, Logger& logger);
        ~Render();
        void init();
        void draw();

    private:
        Logger& m_Logger;
        bool m_DebugLayer = false;
        vk::detail::DispatchLoaderDynamic m_Dispatcher;
        vk::DebugUtilsMessengerEXT m_DebugMessenger;

        SDL_Window* m_Window;
        vk::Instance m_Instance;
        vk::SurfaceKHR m_Surface;
        vk::PhysicalDevice m_PhysicalDevice;
        vk::Device m_LogicalDevice;
        vk::SwapchainKHR m_Swapchain;
        vk::CommandPool m_CommandPool;
        vk::CommandBuffer m_CommandBuffer;
        vk::Pipeline m_Pipeline;

        std::vector<vk::Image> m_SwapchainImages;
        std::vector<vk::ImageView> m_SwapchainImagesViews;
        std::vector<vk::ShaderModule> m_Shaders;

        uint32_t m_QueueGraphicFamilyIndex;
        vk::Queue m_GraphicQueue;
        uint32_t m_QueuePresentFamilyIndex;
        vk::Queue m_PresentQueue;

        void createInstance();
        void createDebugMessenger();
        void createSurface();
        void selectPhysicalDevice();
        void selectQueueFamilyIndexes();
        void createLogicalDevice();
        void createSwapchain();
        void fetchSwapcianResources();
        void createShaderModules();
        void createCommandPool();
        void createCommandBuffer();
        void createPipeline();
        void recordCommandBuffer();

        void checkDebugLayerSupport(std::vector<vk::LayerProperties>& instanceSupportedExtensions);
};