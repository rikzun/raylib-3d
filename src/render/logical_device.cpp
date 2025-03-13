#include "render.h"

void Render::createLogicalDevice() {
    m_Logger.info("Creating Logical Device");

    const float queuePriority = 1.0f;
    std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;

    vk::DeviceQueueCreateInfo graphicQueueCreateInfo {};
    graphicQueueCreateInfo.flags = vk::DeviceQueueCreateFlags();
	graphicQueueCreateInfo.queueFamilyIndex = m_QueueGraphicFamilyIndex;
	graphicQueueCreateInfo.queueCount = 1;
	graphicQueueCreateInfo.pQueuePriorities = &queuePriority;
    queueCreateInfos.push_back(graphicQueueCreateInfo);

    if (m_QueuePresentFamilyIndex != m_QueueGraphicFamilyIndex) {
        vk::DeviceQueueCreateInfo presentQueueCreateInfo {};
        presentQueueCreateInfo.flags = vk::DeviceQueueCreateFlags();
        presentQueueCreateInfo.queueFamilyIndex = m_QueuePresentFamilyIndex;
        presentQueueCreateInfo.queueCount = 1;
        presentQueueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(graphicQueueCreateInfo);
    }

    // vk::PhysicalDeviceFeatures deficeFeatures;
    // vk::PhysicalDeviceShaderObjectFeaturesEXT deviceShaderFeatures = { VK_TRUE };
    vk::PhysicalDeviceDynamicRenderingFeaturesKHR deviceDynamicRenderingFeatures = { VK_TRUE };
    // deviceShaderFeatures.pNext = &deviceDynamicRenderingFeatures;

    std::vector<const char*> requiredExtensions {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        // VK_EXT_SHADER_OBJECT_EXTENSION_NAME
    };

    vk::DeviceCreateInfo deviceCreateInfo = {};
	deviceCreateInfo.flags = vk::DeviceCreateFlags();
	deviceCreateInfo.queueCreateInfoCount = COUNT(queueCreateInfos);
	deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
	deviceCreateInfo.enabledExtensionCount = COUNT(requiredExtensions);
	deviceCreateInfo.ppEnabledExtensionNames = requiredExtensions.data();
	deviceCreateInfo.pNext = &deviceDynamicRenderingFeatures;

    vk::ResultValue<vk::Device> logicalDevice = m_PhysicalDevice.createDevice(deviceCreateInfo);
    VK_FAILED_ERROR(logicalDevice.result, "Logical Device creating caused an error");

    m_LogicalDevice = logicalDevice.value;
    m_GraphicQueue = m_LogicalDevice.getQueue(m_QueueGraphicFamilyIndex, 0);
    m_PresentQueue = m_LogicalDevice.getQueue(m_QueuePresentFamilyIndex, 0);
    m_Logger.info("Logical Device was created successfully");
}