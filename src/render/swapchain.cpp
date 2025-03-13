#include "render.h"

void Render::createSwapchain() {
    m_Logger.info("Creating Swapchain");
    vk::ResultValue<vk::SurfaceCapabilitiesKHR> surfaceCapabilities = m_PhysicalDevice.getSurfaceCapabilitiesKHR(m_Surface);
    VK_FAILED_ERROR(surfaceCapabilities.result, "Physical device capabilities getting caused an error");

    uint32_t minImageCount = surfaceCapabilities.value.minImageCount + 1;
    if (surfaceCapabilities.value.maxImageCount > 0 && minImageCount > surfaceCapabilities.value.maxImageCount) {
        minImageCount = surfaceCapabilities.value.maxImageCount;
    }

    vk::Extent2D extent = surfaceCapabilities.value.currentExtent;
    if (extent.width == UINT32_MAX) {
        int width, height;
        SDL_GetWindowSizeInPixels(m_Window, &width, &height);

        extent.width = std::min(
            surfaceCapabilities.value.maxImageExtent.width,
            std::max(
                surfaceCapabilities.value.minImageExtent.width,
                static_cast<uint32_t>(width)
            )
        );

        extent.height = std::min(
            surfaceCapabilities.value.maxImageExtent.height,
            std::max(
                surfaceCapabilities.value.minImageExtent.height,
                static_cast<uint32_t>(height)
            )
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
    swapchainCreateInfo.preTransform = surfaceCapabilities.value.currentTransform;
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

    vk::ResultValue<vk::SwapchainKHR> swapchainResult = m_LogicalDevice.createSwapchainKHR(swapchainCreateInfo);
    VK_FAILED_ERROR(swapchainResult.result, "Swapchain creating caused an error");
    m_Logger.info("Swapchain was created successfully");

    m_Swapchain = swapchainResult.value;
}