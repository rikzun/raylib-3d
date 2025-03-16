#include "render.h"

void Render::createCommandPool() {
    m_Logger.info("Creating Command Pool");

    vk::CommandPoolCreateInfo createInfo {};
    createInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
    createInfo.queueFamilyIndex = m_QueueGraphicFamilyIndex;

    vk::CommandPool commandPool = VK_ERROR_CHECK(
        m_LogicalDevice.createCommandPool(createInfo),
        "Command Pool creating caused an error"
    );

    m_Logger.info("Command Pool was created successfully");
    m_CommandPool = commandPool;
}

void Render::createCommandBuffers() {
    m_Logger.info("Creating Command Buffers");

    vk::CommandBufferAllocateInfo allocInfo {};
    allocInfo.commandPool = m_CommandPool;
    allocInfo.level = vk::CommandBufferLevel::ePrimary;
    allocInfo.commandBufferCount = CONTAINER_COUNT(m_SwapchainImages);

    std::vector<vk::CommandBuffer> commandBuffers = VK_ERROR_AND_EMPRY_CHECK(
        m_LogicalDevice.allocateCommandBuffers(allocInfo),
        "Command Buffer creating caused an error",
        "Command Buffer creating returned no results"

    );

    m_Logger.info("Command Buffer was created successfully");
    m_CommandBuffers = commandBuffers;
}

void Render::recordCommandBuffer(int index) {
    auto& commandBuffer = m_CommandBuffers[index];
    auto imageIndex = index;

    vk::CommandBufferBeginInfo beginInfo = {};
    commandBuffer.begin(beginInfo);

    vk::ImageMemoryBarrier barrier = {};
    barrier.oldLayout = vk::ImageLayout::eUndefined;
    barrier.newLayout = vk::ImageLayout::eColorAttachmentOptimal;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = m_SwapchainImages[imageIndex];
    barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    vk::PipelineStageFlags sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
    vk::PipelineStageFlags destinationStage = vk::PipelineStageFlagBits::eColorAttachmentOutput;

    commandBuffer.pipelineBarrier(sourceStage, destinationStage, {}, {}, {}, barrier);

    vk::RenderingAttachmentInfo colorAttachment = {};
    colorAttachment.imageView = m_SwapchainImagesViews[imageIndex];
    colorAttachment.imageLayout = vk::ImageLayout::eColorAttachmentOptimal;
    colorAttachment.loadOp = vk::AttachmentLoadOp::eClear;
    colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
    colorAttachment.clearValue = vk::ClearValue(vk::ClearColorValue(std::array<float, 4>{0.1f, 0.1f, 0.1f, 1.0f}));

    vk::RenderingInfo renderInfo = {};
    renderInfo.renderArea = vk::Rect2D({0, 0}, {800, 600});
    renderInfo.layerCount = 1;
    renderInfo.colorAttachmentCount = 1;
    renderInfo.pColorAttachments = &colorAttachment;

    commandBuffer.beginRendering(renderInfo);

    commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, m_Pipeline, m_Dispatcher);

    vk::Viewport viewport = {0.0f, 0.0f, 800.0f, 600.0f, 0.0f, 1.0f};
    vk::Rect2D scissor = {{0, 0}, {800, 600}};
    commandBuffer.setViewport(0, viewport);
    commandBuffer.setScissor(0, scissor);

    commandBuffer.draw(3, 1, 0, 0);
    
    commandBuffer.endRendering();
    commandBuffer.end();
}