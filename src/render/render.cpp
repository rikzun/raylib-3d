#include "render.h"

Render::Render(SDL_Window* window, Logger& logger) : m_Logger(logger) {
    m_Window = window;
}

Render::~Render() {

}

void Render::init() {
    try {
        m_Logger.info("Vulkan init");

        Render::createInstance();
        Render::createDebugMessenger();
        Render::createSurface();
        Render::selectPhysicalDevice();
        Render::selectQueueFamilyIndexes();
        Render::createLogicalDevice();
        Render::createSwapchain();
        Render::selectSwapcianResources();
        Render::createShaderModules();
        Render::createCommandPool();
        Render::createCommandBuffers();
        
        Render::createSyncObjects();
        Render::createPipeline();
    } catch(const std::runtime_error& error) {
        m_Logger.error(error.what());
    }
}

void Render::draw() {
    for (uint32_t i = 0; i < m_CommandBuffers.size(); i++) {
        m_LogicalDevice.waitForFences(1, &m_InFlightFences[i], VK_TRUE, UINT64_MAX);

        uint32_t imageIndex = m_LogicalDevice.acquireNextImageKHR(m_Swapchain, UINT64_MAX, m_ImageAvailableSemaphores[i], VK_NULL_HANDLE).value;

        m_LogicalDevice.resetFences(1, &m_InFlightFences[i]);

        Render::recordCommandBuffer(imageIndex);

        vk::Semaphore waitSemaphores[] = { m_ImageAvailableSemaphores[i] };
        vk::Semaphore signalSemaphores[] = { m_RenderFinishedSemaphores[i] };
        vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };

        vk::SubmitInfo submitInfo {};
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &m_CommandBuffers[imageIndex];
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        m_GraphicQueue.submit(submitInfo, m_InFlightFences[imageIndex]);
        m_GraphicQueue.waitIdle();

        vk::PresentInfoKHR presentInfo {};
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &m_Swapchain;
        presentInfo.pImageIndices = &imageIndex;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        m_PresentQueue.presentKHR(presentInfo);
        m_PresentQueue.waitIdle();
    }
}