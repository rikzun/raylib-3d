#include "render.h"

VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
	vk::DebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	vk::DebugUtilsMessageTypeFlagsEXT messageType,
	const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData
) {
	if (messageSeverity & vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo) {
		static_cast<Logger*>(pUserData)->info(std::format("[VULKAN] {}", pCallbackData->pMessage));
	} else if (messageSeverity & vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning) {
		static_cast<Logger*>(pUserData)->warning(std::format("[VULKAN] {}", pCallbackData->pMessage));
	} else if (messageSeverity & vk::DebugUtilsMessageSeverityFlagBitsEXT::eError) {
		static_cast<Logger*>(pUserData)->error(std::format("[VULKAN] {}", pCallbackData->pMessage));
	}

	return VK_FALSE;
}

void Render::createDebugMessenger() {
    if (!m_DebugLayer) return;

	m_Logger.info("Creating Debug Messenger");
	m_Dispatcher.init(m_Instance, vkGetInstanceProcAddr);

    vk::DebugUtilsMessengerCreateInfoEXT createInfo {};
	createInfo.pfnUserCallback = debugCallback;
	createInfo.pUserData = &m_Logger;
	createInfo.messageSeverity =
		vk::DebugUtilsMessageSeverityFlagBitsEXT::eError |
		vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo |
		vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning;
	createInfo.messageType =
		vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
		vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
		vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation;

	vk::DebugUtilsMessengerEXT debugMessenger = VK_ERROR_CHECK(
		m_Instance.createDebugUtilsMessengerEXT(createInfo, nullptr, m_Dispatcher),
		"Debug Messenger creating caused an error"
	);

	m_Logger.info("Debug Messenger was created successfully");
    m_DebugMessenger = debugMessenger;
}