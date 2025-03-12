#include "render.h"

void Render::createInstance() {
    #ifdef DEBUG_BUILD
        print("Creating Instance");

        vk::ResultValue<uint32_t> version = vk::enumerateInstanceVersion();
        VK_FAILED_ERROR(version.result, "Version getting caused an error");

        print(
            "Vulkan"
            << " " << VK_API_VERSION_MAJOR(version.value)
            << "." << VK_API_VERSION_MINOR(version.value)
            << "." << VK_API_VERSION_PATCH(version.value)
        );
    #endif

    vk::ResultValue<std::vector<vk::ExtensionProperties>> instanceSupportExtensions = vk::enumerateInstanceExtensionProperties();
    VK_FAILED_ERROR(instanceSupportExtensions.result, "Instance supported extensions getting caused an error");

    #ifdef DEBUG_BUILD
        for (auto& ext : instanceSupportExtensions.value) {
            if (strcmp(ext.extensionName, VK_EXT_DEBUG_UTILS_EXTENSION_NAME) != 0) continue;
            m_DebugLayer = true;
            break;
        }

        if (m_DebugLayer) print("Debug layer SUPPORTED");
        else print("Debug layer NOT SUPPORTED");
    #endif

    std::vector<const char*> enableExtensions {};
    if (m_DebugLayer) enableExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    uint32_t sdlRequiredExtensionsCount = 0;
    const char* const* sdlRequiredExtensions = SDL_Vulkan_GetInstanceExtensions(&sdlRequiredExtensionsCount);

    for (uint32_t i = 0; i < sdlRequiredExtensionsCount; i++) {
        const char* sdlRequiredExtension = sdlRequiredExtensions[i];
        enableExtensions.push_back(sdlRequiredExtension);
    }

    std::vector<const char*> enableLayers {};
    if (m_DebugLayer) enableLayers.push_back("VK_LAYER_KHRONOS_validation");

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

    #ifdef DEBUG_BUILD
        print("Check required extensions support");

        for (uint32_t i = 0; i < createInfo.enabledExtensionCount; i++) {
            const char* requiredExtension = createInfo.ppEnabledExtensionNames[i];
            bool supported = false;

            for (auto& ext : instanceSupportExtensions.value) {
                if (strcmp(ext.extensionName, requiredExtension) != 0) continue;
                supported = true;
                break;
            }

            if (supported) {
                printi("+ " << requiredExtension);
            } else {
                printi("- " << requiredExtension);
                return;
            }
        }

        print("Check required layers support");
        vk::ResultValue<std::vector<vk::LayerProperties>> instanceSupportedLayers = vk::enumerateInstanceLayerProperties();
        VK_FAILED_ERROR(instanceSupportedLayers.result, "Instance support layers getting caused an error");

        for (uint32_t i = 0; i < createInfo.enabledLayerCount; i++) {
            const char* requiredLayer = createInfo.ppEnabledLayerNames[i];
            bool supported = false;

            for (auto& layer : instanceSupportedLayers.value) {
                if (strcmp(layer.layerName, requiredLayer) != 0) continue;
                supported = true;
                break;
            }

            if (supported) {
                printi("+ " << requiredLayer);
            } else {
                printi("- " << requiredLayer);
                return;
            }
        }
    #endif

    vk::ResultValue<vk::Instance> instance = vk::createInstance(createInfo);
    VK_FAILED_ERROR(instance.result, "Instance creating getting caused an error")
    #ifdef DEBUG_BUILD
        print("Instance was created successfully");
    #endif

    m_Instance = instance.value;
}