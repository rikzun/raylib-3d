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

void transition_image_layout(vk::CommandBuffer commandBuffer, vk::Image image,
    vk::ImageLayout oldLayout, vk::ImageLayout newLayout,
	vk::AccessFlags srcAccessMask, vk::AccessFlags dstAccessMask,
	vk::PipelineStageFlags srcStage, vk::PipelineStageFlags dstStage) {

	vk::ImageSubresourceRange access;
	access.aspectMask = vk::ImageAspectFlagBits::eColor;
	access.baseMipLevel = 0;
	access.levelCount = 1;
	access.baseArrayLayer = 0;
	access.layerCount = 1;

	vk::ImageMemoryBarrier barrier;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = image;
	barrier.subresourceRange = access;

	vk::PipelineStageFlags sourceStage, destinationStage;

	barrier.srcAccessMask = srcAccessMask;
	barrier.dstAccessMask = dstAccessMask;

	commandBuffer.pipelineBarrier(
		srcStage, dstStage, vk::DependencyFlags(), nullptr, nullptr, barrier);

}

void Render::recordCommandBuffer(int imageIndex) {
    auto& commandBuffer = m_CommandBuffers[imageIndex];

    // TODO: fix later
    vk::Extent2D frameSize {800, 600};

    vk::RenderingAttachmentInfo colorAttachment {};
    colorAttachment.imageView = m_SwapchainImagesViews[imageIndex];
    colorAttachment.imageLayout = vk::ImageLayout::eColorAttachmentOptimal;
    colorAttachment.loadOp = vk::AttachmentLoadOp::eClear;
    colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
    colorAttachment.clearValue = vk::ClearValue(vk::ClearColorValue(std::array<float, 4>{0.1f, 0.1f, 0.1f, 1.0f}));

    vk::RenderingInfo renderInfo {};
    renderInfo.renderArea = vk::Rect2D({0, 0}, frameSize);
    renderInfo.layerCount = 1;
    renderInfo.colorAttachmentCount = 1;
    renderInfo.pColorAttachments = &colorAttachment;

	vk::CommandBufferBeginInfo beginInfo = {};
	commandBuffer.begin(beginInfo);

	transition_image_layout(commandBuffer, m_SwapchainImages[imageIndex],
		vk::ImageLayout::eUndefined, vk::ImageLayout::eAttachmentOptimal,
		vk::AccessFlagBits::eNone, vk::AccessFlagBits::eColorAttachmentWrite,
		vk::PipelineStageFlagBits::eTopOfPipe, vk::PipelineStageFlagBits::eColorAttachmentOutput);

	vk::Viewport viewport = vk::Viewport(0.0f, 0.0f, frameSize.width, frameSize.height, 0.0f, 1.0f);
	commandBuffer.setViewportWithCount(viewport);

	vk::Rect2D scissor = vk::Rect2D({ 0,0 }, frameSize);
	commandBuffer.setScissorWithCount(scissor);

	commandBuffer.setRasterizerDiscardEnable(0);
	commandBuffer.setPolygonModeEXT(vk::PolygonMode::eFill, m_Dispatcher);
	commandBuffer.setRasterizationSamplesEXT(vk::SampleCountFlagBits::e1, m_Dispatcher);
	uint32_t sampleMask = 1;
	commandBuffer.setSampleMaskEXT(vk::SampleCountFlagBits::e1, sampleMask, m_Dispatcher);
	commandBuffer.setAlphaToCoverageEnableEXT(0, m_Dispatcher);
	commandBuffer.setCullMode(vk::CullModeFlagBits::eNone);
	commandBuffer.setDepthTestEnable(0);
	commandBuffer.setDepthWriteEnable(0);
	commandBuffer.setDepthBiasEnable(0);
	commandBuffer.setStencilTestEnable(0);
	commandBuffer.setPrimitiveTopology(vk::PrimitiveTopology::eTriangleList);
	commandBuffer.setPrimitiveRestartEnable(0);
	uint32_t colorBlendEnable = 0;
	commandBuffer.setColorBlendEnableEXT(0, colorBlendEnable, m_Dispatcher);
	vk::ColorBlendEquationEXT equation;
	equation.colorBlendOp = vk::BlendOp::eAdd;
	equation.dstColorBlendFactor = vk::BlendFactor::eZero;
	equation.srcColorBlendFactor = vk::BlendFactor::eOne;
	commandBuffer.setColorBlendEquationEXT(0, equation, m_Dispatcher);
	vk::ColorComponentFlags colorWriteMask = vk::ColorComponentFlagBits::eR
		| vk::ColorComponentFlagBits::eG
		| vk::ColorComponentFlagBits::eB
		| vk::ColorComponentFlagBits::eA;
	commandBuffer.setColorWriteMaskEXT(0, colorWriteMask, m_Dispatcher);
    commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, m_Pipeline);

	commandBuffer.beginRenderingKHR(renderInfo, m_Dispatcher);

	vk::ShaderStageFlagBits stages[2] = {
		vk::ShaderStageFlagBits::eVertex,
		vk::ShaderStageFlagBits::eFragment
	};

	commandBuffer.draw(3, 1, 0, 0);

	commandBuffer.endRenderingKHR(m_Dispatcher);

	transition_image_layout(commandBuffer, m_SwapchainImages[imageIndex],
		vk::ImageLayout::eAttachmentOptimal, vk::ImageLayout::ePresentSrcKHR,
		vk::AccessFlagBits::eColorAttachmentWrite, vk::AccessFlagBits::eNone,
		vk::PipelineStageFlagBits::eColorAttachmentOutput, vk::PipelineStageFlagBits::eBottomOfPipe);

	commandBuffer.end();
}