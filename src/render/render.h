#pragma once
#define VULKAN_HPP_NO_EXCEPTIONS
#define VULKAN_HPP_NO_NODISCARD_WARNINGS
#include <vulkan/vulkan.hpp>
#include <SDL3/SDL_vulkan.h>
#include <optional>
#include <unordered_set>
#include "render_utils.h"
#include <sstream>
#include <iomanip>
#include <sstream>
#include "../logger.h"

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
        std::vector<vk::CommandBuffer> m_CommandBuffers;
        vk::PipelineLayout m_PipelineLayout;
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
        void selectSwapcianResources();
        void createShaderModules();
        void createCommandPool();
        void createCommandBuffers();
        
        void createPipeline();
        void recordCommandBuffer(int index);

        void createSyncObjects();
        std::vector<vk::Semaphore> m_ImageAvailableSemaphores;
        std::vector<vk::Semaphore> m_RenderFinishedSemaphores;
        std::vector<vk::Fence> m_InFlightFences;
};