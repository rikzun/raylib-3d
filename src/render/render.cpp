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
        Render::fetchSwapcianResources();
        Render::createShaderModules();
        Render::createCommandPool();
        Render::createCommandBuffer();
        Render::createPipeline();
        Render::recordCommandBuffer();
    } catch(const std::runtime_error& error) {
        m_Logger.error(error.what());
    }
}

void Render::draw() {
    uint32_t imageIndice { 0 };

    vk::SubmitInfo submitInfo {};
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &m_CommandBuffer;

    m_GraphicQueue.submit(submitInfo, nullptr);
    m_GraphicQueue.waitIdle();

    vk::PresentInfoKHR presentInfo {};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &m_Swapchain;
    presentInfo.pImageIndices = &imageIndice;

    m_GraphicQueue.presentKHR(presentInfo);
    m_GraphicQueue.waitIdle();
}