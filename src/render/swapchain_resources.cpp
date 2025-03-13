#include "render.h"

void Render::fetchSwapcianResources() {
    m_Logger.info("Fetching Swapchain Resources");
    m_Logger.info("  Fetching Swapchain Images");

    vk::ResultValue<std::vector<vk::Image>> swapchainImages = m_LogicalDevice.getSwapchainImagesKHR(m_Swapchain);
    VK_FAILED_ERROR(swapchainImages.result, "Swapchain Images fetching caused an error");

    std::vector<vk::ImageView> swapchainImagesViews {};
    m_Logger.info("  Creating Swapchain ImagesViews");

    for (vk::Image& image : swapchainImages.value) {
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

        vk::ResultValue<vk::ImageView> imageView = m_LogicalDevice.createImageView(viewInfo);
        VK_FAILED_ERROR(imageView.result, "Swapchain ImagesViews creating caused an error");

        swapchainImagesViews.push_back(imageView.value);
    }

    if (!swapchainImagesViews.empty()) {
        m_SwapchainImages = swapchainImages.value;
        m_SwapchainImagesViews = swapchainImagesViews;

        m_Logger.info("Swapchain Resources was feched successfully");
        return;
    }

    throw std::runtime_error("Suitable physical devise was not found");
}