#include "render.h"
#include <iostream>
#include <sstream>
#include <iomanip>

void Render::selectPhysicalDevice() {
    m_Logger.info("Select Physical Device");

    vk::ResultValue<std::vector<vk::PhysicalDevice>> physicalDevices = m_Instance.enumeratePhysicalDevices();
    VK_FAILED_ERROR(physicalDevices.result, "Physical Devices getting caused an error");

    std::vector requiredExtensions {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        // VK_EXT_SHADER_OBJECT_EXTENSION_NAME,
        VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME
    };

    std::optional<vk::PhysicalDevice> localPhysicalDevice;

    for (auto& physicalDevice : physicalDevices.value) {
        vk::PhysicalDeviceProperties properties = physicalDevice.getProperties();
        vk::ResultValue<std::vector<vk::ExtensionProperties>> supportedExtensions = physicalDevice.enumerateDeviceExtensionProperties();
        VK_FAILED_ERROR(supportedExtensions.result, "Physical Device extensions getting caused an error");

        m_Logger.info(
            std::format(
                "Check required extensions for {} ({})",
                properties.deviceName.data(),
                properties.deviceID
            )
        );

        uint32_t requiredExtensionsSupportedCount = 0;
        for (auto& requiredExtension : requiredExtensions) {
            bool supported = false;

            for (auto& supportedExtension : supportedExtensions.value) {
                if (strcmp(requiredExtension, supportedExtension.extensionName) == 0) {
                    supported = true;
                    break;
                }
            }

            if (supported) {
                m_Logger.info(std::format("  (SUPPORTED) {}", requiredExtension));
                requiredExtensionsSupportedCount++;
                continue;
            }

            m_Logger.info(std::format("  (NOT SUPPORTED) {}", requiredExtension));
        }

        if (!localPhysicalDevice.has_value() && requiredExtensionsSupportedCount == requiredExtensions.size()) {
            m_Logger.info("  DEVICE SELECTED");
            localPhysicalDevice.emplace(physicalDevice);
        }
    }

    if (localPhysicalDevice.has_value()) {
        m_PhysicalDevice = localPhysicalDevice.value();
        return;
    }

    throw std::runtime_error("Suitable physical devise was not found");
}