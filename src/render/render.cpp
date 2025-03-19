#include "render.h"

Render::Render(SDL_Window* window, Logger& logger) : m_Logger(logger) { m_Window = window; }
Render::~Render() {}

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
    m_LogicalDevice.waitForFences(m_RenderFinishedFence, false, UINT64_MAX);
	m_LogicalDevice.resetFences(m_RenderFinishedFence);

    uint32_t imageIndex = m_LogicalDevice.acquireNextImageKHR(m_Swapchain, UINT64_MAX, m_ImageAvailableSemaphore, nullptr).value;
    recordCommandBuffer(imageIndex);

    vk::SubmitInfo submitInfo {};
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &m_CommandBuffers[imageIndex];
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = &m_ImageAvailableSemaphore;
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &m_RenderFinishedSemaphore;
	vk::PipelineStageFlags waitStage = vk::PipelineStageFlagBits::eColorAttachmentOutput;
	submitInfo.pWaitDstStageMask = &waitStage;
	m_GraphicQueue.submit(submitInfo, m_RenderFinishedFence, m_Dispatcher);

    vk::PresentInfoKHR presentInfo {};
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &m_Swapchain;
	presentInfo.pImageIndices = &imageIndex;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &m_RenderFinishedSemaphore;

	m_GraphicQueue.presentKHR(presentInfo);
}