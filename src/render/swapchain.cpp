#include "render.h"

vk::Extent2D getWindowSize(SDL_Window* window) {
    int width, height;
    SDL_GetWindowSizeInPixels(window, &width, &height);

    return vk::Extent2D {
        static_cast<uint32_t>(width),
        static_cast<uint32_t>(height)
    };
}

void Render::createSwapchain() {
    m_Logger.info("Creating Swapchain");

    vk::SurfaceCapabilitiesKHR surfaceCapabilities = VK_ERROR_CHECK(
        m_PhysicalDevice.getSurfaceCapabilitiesKHR(m_Surface),
        "Physical device capabilities getting caused an error"
    );

    uint32_t minImageCount = surfaceCapabilities.minImageCount + 1;
    if (surfaceCapabilities.maxImageCount > 0 && minImageCount > surfaceCapabilities.maxImageCount) {
        minImageCount = surfaceCapabilities.maxImageCount;
    }

    vk::Extent2D extent = surfaceCapabilities.currentExtent;
    if (extent.height == UINT32_MAX || extent.width == UINT32_MAX) {
        vk::Extent2D window = getWindowSize(m_Window);

        extent.width = std::min(
            surfaceCapabilities.maxImageExtent.width,
            std::max(surfaceCapabilities.minImageExtent.width, window.width)
        );

        extent.height = std::min(
            surfaceCapabilities.maxImageExtent.height,
            std::max(surfaceCapabilities.minImageExtent.height, window.height)
        );
    }

    vk::SwapchainCreateInfoKHR swapchainCreateInfo {};
    swapchainCreateInfo.surface = m_Surface;
    swapchainCreateInfo.minImageCount = minImageCount;
    swapchainCreateInfo.imageFormat = vk::Format::eB8G8R8A8Unorm;
    swapchainCreateInfo.imageColorSpace = vk::ColorSpaceKHR::eSrgbNonlinear;
    swapchainCreateInfo.imageExtent = extent;
    swapchainCreateInfo.imageArrayLayers = 1;
    swapchainCreateInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferDst;
    swapchainCreateInfo.preTransform = surfaceCapabilities.currentTransform;
    swapchainCreateInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
    swapchainCreateInfo.presentMode = vk::PresentModeKHR::eMailbox;
    swapchainCreateInfo.clipped = VK_TRUE;
    swapchainCreateInfo.oldSwapchain = m_Swapchain;

    if (m_QueueGraphicFamilyIndex != m_QueuePresentFamilyIndex) {
        uint32_t queue[2] = {
            static_cast<uint32_t>(m_QueueGraphicFamilyIndex),
            static_cast<uint32_t>(m_QueuePresentFamilyIndex)
        };
        
        swapchainCreateInfo.imageSharingMode = vk::SharingMode::eConcurrent;
        swapchainCreateInfo.queueFamilyIndexCount = 2;
        swapchainCreateInfo.pQueueFamilyIndices = queue;
    } else {
        swapchainCreateInfo.imageSharingMode = vk::SharingMode::eExclusive;
        swapchainCreateInfo.queueFamilyIndexCount = 1;
        swapchainCreateInfo.pQueueFamilyIndices = &m_QueueGraphicFamilyIndex;
    }

    vk::SwapchainKHR swapchainResult = VK_ERROR_CHECK(
        m_LogicalDevice.createSwapchainKHR(swapchainCreateInfo),
        "Swapchain creating caused an error"
    );

    m_Logger.info("Swapchain was created successfully");
    m_Swapchain = swapchainResult;
}