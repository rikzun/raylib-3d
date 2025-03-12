#include "render.h"

void Render::createLogicalDevice() {
    #ifdef DEBUG_BUILD
        print("Creating Logical device");
    #endif

    const float queuePriority = 1.0f;
    std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;

    vk::DeviceQueueCreateInfo graphicQueueCreateInfo {};
    graphicQueueCreateInfo.flags = vk::DeviceQueueCreateFlags();
	graphicQueueCreateInfo.queueFamilyIndex = m_QueueGraphicFamilyIndex.value();
	graphicQueueCreateInfo.queueCount = 1;
	graphicQueueCreateInfo.pQueuePriorities = &queuePriority;
    queueCreateInfos.push_back(graphicQueueCreateInfo);

    if (m_QueuePresentFamilyIndex.value() != m_QueueGraphicFamilyIndex.value()) {
        vk::DeviceQueueCreateInfo presentQueueCreateInfo {};
        presentQueueCreateInfo.flags = vk::DeviceQueueCreateFlags();
        presentQueueCreateInfo.queueFamilyIndex = m_QueuePresentFamilyIndex.value();
        presentQueueCreateInfo.queueCount = 1;
        presentQueueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(graphicQueueCreateInfo);
    }

    vk::PhysicalDeviceDynamicRenderingFeaturesKHR deviceDynamicRenderingFeatures = { VK_TRUE };
    std::vector<const char*> requiredExtensions { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

    vk::DeviceCreateInfo deviceCreateInfo = {};
	deviceCreateInfo.flags = vk::DeviceCreateFlags();
	deviceCreateInfo.queueCreateInfoCount = COUNT(queueCreateInfos);
	deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
	deviceCreateInfo.enabledExtensionCount = COUNT(requiredExtensions);
	deviceCreateInfo.ppEnabledExtensionNames = requiredExtensions.data();
	deviceCreateInfo.pNext = &deviceDynamicRenderingFeatures;

    vk::ResultValue<vk::Device> logicalDevice = m_PhysicalDevice.createDevice(deviceCreateInfo);
    VK_FAILED_ERROR(logicalDevice.result, "Logical device creating caused an error");

    m_LogicalDevice = logicalDevice.value;
    m_GraphicQueue = m_LogicalDevice.getQueue(m_QueueGraphicFamilyIndex.value(), 0);
    m_PresentQueue = m_LogicalDevice.getQueue(m_QueuePresentFamilyIndex.value(), 0);
}