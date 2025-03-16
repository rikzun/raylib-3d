#include "render.h"

void Render::selectSwapcianResources() {
    m_Logger.info("Selecting Swapchain Resources");
    m_Logger.info("  Selecting Swapchain Images");

    std::vector<vk::Image> swapchainImages = VK_ERROR_AND_EMPRY_CHECK(
        m_LogicalDevice.getSwapchainImagesKHR(m_Swapchain),
        "Swapchain Images selecting caused an error",
        "Swapchain Images selecting returned no results"
    );

    std::vector<vk::ImageView> swapchainImagesViews {};
    swapchainImagesViews.reserve(swapchainImages.size());

    m_Logger.info("  Creating Swapchain ImagesViews");
    for (vk::Image& image : swapchainImages) {
        vk::ImageViewCreateInfo viewInfo {};
        viewInfo.image = image;
        viewInfo.viewType = vk::ImageViewType::e2D;
        viewInfo.format = vk::Format::eB8G8R8A8Unorm;
        viewInfo.components = vk::ComponentSwizzle::eIdentity;
        viewInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        vk::ImageView imageView = VK_ERROR_CHECK(
            m_LogicalDevice.createImageView(viewInfo),
            "Swapchain ImagesViews creating caused an error"
        );

        swapchainImagesViews.push_back(imageView);
    }

    m_SwapchainImages = swapchainImages;
    m_SwapchainImagesViews = swapchainImagesViews;

    m_Logger.info("Swapchain Resources selected successfully");
}