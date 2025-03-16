#include "render.h"

void Render::createInstance() {
    m_Logger.info("Creating Instance");

    uint32_t version = VK_ERROR_CHECK(
        vk::enumerateInstanceVersion(),
        "Version enumeration caused an error"
    );

    vk::ApplicationInfo appInfo {};
    appInfo.pApplicationName = PROJECT_NAME;
    appInfo.applicationVersion = VK_MAKE_VERSION(PROJECT_VERSION_MAJOR, PROJECT_VERSION_MINOR, PROJECT_VERSION_PATCH);
    appInfo.apiVersion = VK_MAKE_API_VERSION(0, PROJECT_VK_VERSION_MAJOR, PROJECT_VK_VERSION_MINOR, PROJECT_VK_VERSION_PATCH);

    m_Logger.info(
        std::format(
            "Vulkan sys: {}.{}.{} app: {}.{}.{}",
            VK_API_VERSION_MAJOR(version),
            VK_API_VERSION_MINOR(version),
            VK_API_VERSION_PATCH(version),
            PROJECT_VK_VERSION_MAJOR,
            PROJECT_VK_VERSION_MINOR,
            PROJECT_VK_VERSION_PATCH
        )
    );

    std::vector<vk::LayerProperties> instanceSupportedLayers = VK_ERROR_AND_EMPRY_CHECK(
        vk::enumerateInstanceLayerProperties(),
        "Instance support layers enumeration caused an error",
        "Instance support layers enumeration returned no results"
    );

    std::vector<vk::ExtensionProperties> instanceSupportExtensions = VK_ERROR_AND_EMPRY_CHECK(
        vk::enumerateInstanceExtensionProperties(),
        "Instance support extensions enumeration caused an error",
        "Instance support extensions enumeration returned no results"
    );

    std::unordered_set<std::string_view> supportedLayers;
    INSERT_ELEMENTS_M(supportedLayers, instanceSupportedLayers, layerName);

    std::unordered_set<std::string_view> supportedExtensions;
    INSERT_ELEMENTS_M(supportedExtensions, instanceSupportExtensions, extensionName);

    std::vector<const char*> requiredLayers {};
    std::vector<const char*> requiredExtensions {};

    m_DebugLayer = supportedLayers.contains("VK_LAYER_KHRONOS_validation");

    if (m_DebugLayer) {
        m_Logger.info("Debug layer supported");
        requiredExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        requiredLayers.push_back("VK_LAYER_KHRONOS_validation");
    } else {
        m_Logger.info("Debug layer not supported");
    }

    uint32_t sdlRequiredExtensionsCount = 0;
    const char* const* sdlRequiredExtensions = SDL_Vulkan_GetInstanceExtensions(&sdlRequiredExtensionsCount);
    for (uint32_t i = 0; i < sdlRequiredExtensionsCount; i++) {
        requiredExtensions.push_back(sdlRequiredExtensions[i]);
    }

    m_Logger.info("Check required layers support");
    bool requiredLayersSupported = true;

    for (std::string_view requiredLayer : requiredLayers) {
        if (supportedLayers.contains(requiredLayer)) {
            m_Logger.info(std::format("  + {}", requiredLayer));
        } else {
            m_Logger.info(std::format("  - {}", requiredLayer));
            requiredLayersSupported = false;
        }
    }

    m_Logger.info("Check required extensions support");
    bool requiredExtensionsSupported = true;

    for (std::string_view requiredExtension : requiredExtensions) {
        if (supportedExtensions.contains(requiredExtension)) {
            m_Logger.info(std::format("  + {}", requiredExtension));
        } else {
            m_Logger.info(std::format("  - {}", requiredExtension));
            requiredExtensionsSupported = false;
        }
    }

    if (!requiredLayersSupported) throw std::runtime_error("Not all required layers are supported");
    if (!requiredExtensionsSupported) throw std::runtime_error("Not all required extensions are supported");

    vk::InstanceCreateInfo createInfo {};
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledLayerCount = CONTAINER_COUNT(requiredLayers);
    createInfo.ppEnabledLayerNames = requiredLayers.data();
    createInfo.enabledExtensionCount = CONTAINER_COUNT(requiredExtensions);
    createInfo.ppEnabledExtensionNames = requiredExtensions.data();

    vk::Instance instance = VK_ERROR_CHECK(
        vk::createInstance(createInfo),
        "Instance creating caused an error"
    );

    m_Logger.info("Instance was created successfully");
    m_Instance = instance;
}