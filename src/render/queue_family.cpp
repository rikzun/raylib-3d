#include "render.h"

void Render::selectQueueFamilyIndexes() {
    m_Logger.info("Select Queue Family indexes");

    std::vector<vk::QueueFamilyProperties> queueFamilies = m_PhysicalDevice.getQueueFamilyProperties();
    if (queueFamilies.empty()) throw std::runtime_error("Queue Family getting returned no results");

    std::optional<uint32_t> localQueueGraphicFamilyIndex;
    std::optional<uint32_t> localQueuePresentFamilyIndex;

	for (uint32_t i = 0; i < queueFamilies.size(); i++) {
		const vk::QueueFamilyProperties& queueFamily = queueFamilies[i];

		if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics) {
            if (!localQueueGraphicFamilyIndex) {
                m_Logger.info(std::format("Graphic index {}", i));
                localQueueGraphicFamilyIndex.emplace(i);
            }

            if (!localQueuePresentFamilyIndex) {
                if(m_PhysicalDevice.getSurfaceSupportKHR(i, m_Surface).result == vk::Result::eSuccess) {
                    m_Logger.info(std::format("Present index {}", i));
                    localQueuePresentFamilyIndex.emplace(i);
                }
            }
		}
	}

    if (!localQueueGraphicFamilyIndex && !localQueuePresentFamilyIndex) {
        throw std::runtime_error("Suitable Queue Family was not found");
    }

    m_QueueGraphicFamilyIndex = localQueueGraphicFamilyIndex.value();
    m_QueuePresentFamilyIndex = localQueuePresentFamilyIndex.value_or(localQueueGraphicFamilyIndex.value());
}