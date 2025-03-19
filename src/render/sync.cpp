#include "render.h"

void Render::createSyncObjects() {
    m_ImageAvailableSemaphore = VK_ERROR_CHECK(
        m_LogicalDevice.createSemaphore(vk::SemaphoreCreateInfo {}),
        "m_ImageAvailableSemaphore"
    );

    m_RenderFinishedSemaphore = VK_ERROR_CHECK(
        m_LogicalDevice.createSemaphore(vk::SemaphoreCreateInfo {}),
        "m_RenderFinishedSemaphore"
    );

    m_RenderFinishedFence = VK_ERROR_CHECK(
        m_LogicalDevice.createFence(vk::FenceCreateInfo { vk::FenceCreateFlagBits::eSignaled }),
        "m_RenderFinishedFence"
    );
}