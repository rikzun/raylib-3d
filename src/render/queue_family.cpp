#include "render.h"

void Render::selectQueueFamilyIndexes() {
    #ifdef DEBUG_BUILD
        print("Select queue family indexes");
    #endif

    std::vector<vk::QueueFamilyProperties> queueFamilies = m_PhysicalDevice.getQueueFamilyProperties();
    if (queueFamilies.empty()) throw std::runtime_error("Queue family getting caused an error");

	for (uint32_t i = 0; i < queueFamilies.size(); i++) {
		const vk::QueueFamilyProperties& queueFamily = queueFamilies[i];

		if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics) {
            if (!m_QueueGraphicFamilyIndex.has_value()) {
                #ifdef DEBUG_BUILD
                    printi("Graphic index " << i);
                #endif

                m_QueueGraphicFamilyIndex.emplace(i);
            }

            if (!m_QueuePresentFamilyIndex.has_value()) {
                if(VK_SUCCESS(m_PhysicalDevice.getSurfaceSupportKHR(i, m_Surface).result)) {
                    #ifdef DEBUG_BUILD
                        printi("Present index " << i);
                    #endif

                    m_QueuePresentFamilyIndex.emplace(i);
                }
            }
		}
	}

    #ifdef DEBUG_BUILD
        if (!m_QueueGraphicFamilyIndex.has_value() && !m_QueuePresentFamilyIndex.has_value()) {
            print("Suitable queue family was not found");
        }
    #endif
}