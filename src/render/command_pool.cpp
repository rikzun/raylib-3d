#include "render.h"

void Render::createCommandPool() {
    m_Logger.info("Creating Command Pool");

    vk::CommandPoolCreateInfo createInfo {};
    createInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
    createInfo.queueFamilyIndex = m_QueueGraphicFamilyIndex;

    vk::ResultValue<vk::CommandPool> commandPool = m_LogicalDevice.createCommandPool(createInfo);
    VK_FAILED_ERROR(commandPool.result, "Command Pool creating caused an error");

    m_Logger.info("Command Pool was created successfully");
    m_CommandPool = commandPool.value;
}

void Render::recordCommandBuffer() {
    vk::CommandBufferBeginInfo beginInfo{};
    beginInfo.flags = vk::CommandBufferUsageFlagBits::eSimultaneousUse;

    m_CommandBuffer.begin(beginInfo);

    int currentFrame = 0;
    int width0, height0;
    SDL_GetWindowSizeInPixels(m_Window, &width0, &height0);
    uint32_t width = static_cast<uint32_t>(width0);
    uint32_t height = static_cast<uint32_t>(height0);

    vk::RenderingInfo renderingInfo{};
    renderingInfo.renderArea = vk::Rect2D{{0, 0}, {width, height}};
    renderingInfo.layerCount = 1;

    vk::RenderingAttachmentInfo colorAttachment{};
    colorAttachment.imageView = m_SwapchainImagesViews[currentFrame];
    colorAttachment.imageLayout = vk::ImageLayout::eColorAttachmentOptimal;
    colorAttachment.loadOp = vk::AttachmentLoadOp::eClear;
    colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
    colorAttachment.clearValue = vk::ClearColorValue(std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f});

    renderingInfo.colorAttachmentCount = 1;
    renderingInfo.pColorAttachments = &colorAttachment;

    m_CommandBuffer.beginRendering(renderingInfo);

    vk::Viewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(width);
    viewport.height = static_cast<float>(height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    m_CommandBuffer.setViewport(0, 1, &viewport);

    vk::Rect2D scissor{};
    scissor.offset.setX(0).setY(0);
    scissor.extent.setWidth(width).setHeight(height);

    m_CommandBuffer.setScissor(0, 1, &scissor);

    m_CommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, m_Pipeline);

    m_CommandBuffer.draw(3, 1, 0, 0);

    m_CommandBuffer.endRendering();

    m_CommandBuffer.end();

    std::cout << "Command buffer recorded successfully!" << std::endl;
}


void Render::createCommandBuffer() {
    m_Logger.info("Creating Command Buffer");

    vk::CommandBufferAllocateInfo allocInfo {};
    allocInfo.commandPool = m_CommandPool;
    allocInfo.level = vk::CommandBufferLevel::ePrimary;
    allocInfo.commandBufferCount = 1;

    vk::ResultValue<std::vector<vk::CommandBuffer>> commandBuffers = m_LogicalDevice.allocateCommandBuffers(allocInfo);
    VK_FAILED_ERROR(commandBuffers.result, "Command Buffer creating caused an error")

    m_Logger.info("Command Buffer was created successfully");
    m_CommandBuffer = commandBuffers.value[0];
}