#include "render.h"

void Render::createInstance() {
    m_Logger.info("Creating Instance");

    vk::ResultValue<uint32_t> version = vk::enumerateInstanceVersion();
    VK_FAILED_ERROR(version.result, "Version getting caused an error");

    m_Logger.info(
        std::format(
            "Vulkan {}.{}.{}",
            VK_API_VERSION_MAJOR(version.value),
            VK_API_VERSION_MINOR(version.value),
            VK_API_VERSION_PATCH(version.value)
        )
    );

    vk::ResultValue<std::vector<vk::ExtensionProperties>> instanceSupportExtensions = vk::enumerateInstanceExtensionProperties();
    VK_FAILED_ERROR(instanceSupportExtensions.result, "Instance supported extensions getting caused an error");

    std::vector<const char*> enableExtensions {};
    uint32_t sdlRequiredExtensionsCount = 0;
    const char* const* sdlRequiredExtensions = SDL_Vulkan_GetInstanceExtensions(&sdlRequiredExtensionsCount);

    for (uint32_t i = 0; i < sdlRequiredExtensionsCount; i++) {
        const char* sdlRequiredExtension = sdlRequiredExtensions[i];
        enableExtensions.push_back(sdlRequiredExtension);
    }

    vk::ResultValue<std::vector<vk::LayerProperties>> instanceSupportedLayers = vk::enumerateInstanceLayerProperties();
    VK_FAILED_ERROR(instanceSupportedLayers.result, "Instance support layers getting caused an error");

    std::vector<const char*> enableLayers {};
    checkDebugLayerSupport(instanceSupportedLayers.value);

    if (m_DebugLayer) {
        m_Logger.info("Debug layer supported");
        enableExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        enableLayers.push_back("VK_LAYER_KHRONOS_validation");
    } else {
        m_Logger.info("Debug layer not supported");
    }

    vk::ApplicationInfo appInfo {};
	appInfo.pApplicationName = PROJECT_NAME;
	appInfo.applicationVersion = VK_MAKE_VERSION(PROJECT_VERSION_MAJOR, PROJECT_VERSION_MINOR, PROJECT_VERSION_PATCH);
	appInfo.apiVersion = VK_MAKE_API_VERSION(0, 1, 4, 304);

    vk::InstanceCreateInfo createInfo {};
    createInfo.pApplicationInfo = &appInfo;
    createInfo.ppEnabledExtensionNames = enableExtensions.data();
    createInfo.enabledExtensionCount = COUNT(enableExtensions);
    createInfo.ppEnabledLayerNames = enableLayers.data();
    createInfo.enabledLayerCount = COUNT(enableLayers);

    m_Logger.info("Check required extensions support");

    for (uint32_t i = 0; i < createInfo.enabledExtensionCount; i++) {
        const char* requiredExtension = createInfo.ppEnabledExtensionNames[i];
        bool supported = false;

        for (auto& ext : instanceSupportExtensions.value) {
            if (strcmp(ext.extensionName, requiredExtension) != 0) continue;
            supported = true;
            break;
        }

        if (supported) {
            m_Logger.info(std::format("  (SUPPORTED) {}", requiredExtension));
        } else {
            m_Logger.info(std::format("  (NOT SUPPORTED) {}", requiredExtension));
            return;
        }
    }

    m_Logger.info("Check required layers support");

    for (uint32_t i = 0; i < createInfo.enabledLayerCount; i++) {
        const char* requiredLayer = createInfo.ppEnabledLayerNames[i];
        bool supported = false;

        for (auto& layer : instanceSupportedLayers.value) {
            if (strcmp(layer.layerName, requiredLayer) != 0) continue;
            supported = true;
            break;
        }

        if (supported) {
            m_Logger.info(std::format("  (SUPPORTED) {}", requiredLayer));
        } else {
            m_Logger.info(std::format("  (NOT SUPPORTED) {}", requiredLayer));
            return;
        }
    }

    vk::ResultValue<vk::Instance> instance = vk::createInstance(createInfo);
    VK_FAILED_ERROR(instance.result, "Instance creating getting caused an error")
    m_Logger.info("Instance was created successfully");
    m_Instance = instance.value;
}