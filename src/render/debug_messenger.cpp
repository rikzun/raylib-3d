#include "render.h"

VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
	vk::DebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	vk::DebugUtilsMessageTypeFlagsEXT messageType,
	const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData
) {
	std::cerr << "Vulkan_" << vk::to_string(messageSeverity) << " " << pCallbackData->pMessage << std::endl;
	return VK_FALSE;
}

void Render::createDebugMessenger() {
    if (!m_DebugLayer) return;
    #ifdef DEBUG_BUILD
        print("Creating Debug Messenger");
    #endif

    m_dispatcher = vk::detail::DispatchLoaderDynamic(m_Instance, vkGetInstanceProcAddr);

    vk::DebugUtilsMessengerCreateInfoEXT createInfo {};
	createInfo.messageSeverity =
		vk::DebugUtilsMessageSeverityFlagBitsEXT::eError |
		vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo |
		vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning;
	createInfo.messageType =
		vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
		vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
		vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation;
    createInfo.flags = vk::DebugUtilsMessengerCreateFlagsEXT();
	createInfo.pfnUserCallback = debugCallback;

    vk::ResultValue<vk::DebugUtilsMessengerEXT> debugMessenger = m_Instance.createDebugUtilsMessengerEXT(createInfo, nullptr, m_dispatcher);
    VK_FAILED_ERROR(debugMessenger.result, "Debug Messenger creating caused an error")

	#ifdef DEBUG_BUILD
        print("Debug Messenger was created successfully");
    #endif
    m_DebugMessenger = debugMessenger.value;
}