#include "render.h"

void Render::selectPhysicalDevice() {
    #ifdef DEBUG_BUILD
        print("Select Physical Device");
    #endif

    vk::ResultValue<std::vector<vk::PhysicalDevice>> physicalDevices = m_Instance.enumeratePhysicalDevices();
    VK_FAILED_ERROR(physicalDevices.result, "Physical devices getting caused an error");

    std::vector requiredExtensions {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME
    };

    for (auto& physicalDevice : physicalDevices.value) {
        vk::PhysicalDeviceProperties properties = physicalDevice.getProperties();
        vk::ResultValue<std::vector<vk::ExtensionProperties>> supportedExtensions = physicalDevice.enumerateDeviceExtensionProperties();
        VK_FAILED_ERROR(supportedExtensions.result, "Physical device extensions getting caused an error");

        #ifdef DEBUG_BUILD
            print("Check required extensions for " << properties.deviceName);
        #endif

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
                #ifdef DEBUG_BUILD
                    printi(requiredExtension << " (SUPPORTED)");
                #endif

                requiredExtensionsSupportedCount++;
                continue;
            }
            
            #ifdef DEBUG_BUILD
                else {
                    printi(requiredExtension << " (NOT SUPPORTED)");
                }
            #endif
        }

        if (requiredExtensionsSupportedCount == requiredExtensions.size()) {
            #ifdef DEBUG_BUILD
                print("Selected " << properties.deviceName);
                printi("Type: " << vk::to_string(properties.deviceType));
                printi("ID: " << properties.deviceID);
            #endif
            
            m_PhysicalDevice = physicalDevice;
            return;
        }
    }

    throw std::runtime_error("Suitable physical devise was not found");
}