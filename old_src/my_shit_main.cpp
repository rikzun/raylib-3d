#define GLFW_INCLUDE_VULKAN
#define VULKAN_HPP_NO_EXCEPTIONS
#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <thread>

#define printStart(value) std::cout << value << std::endl
#define print1(value) std::cout << "  " << value << std::endl
#define print2(value) std::cout << "    " << value << std::endl
#define print3(value) std::cout << "      " << value << std::endl
#define printEnd(value) std::cout << value << std::endl << std::endl

vk::Instance make_instance()
{
	printStart("Making an instance");
	uint32_t version = vk::enumerateInstanceVersion().value;

	print1("Installed Vulkan");
	print2(
		VK_API_VERSION_MAJOR(version) << "." <<
		VK_API_VERSION_MINOR(version) << "." <<
		VK_API_VERSION_PATCH(version)
	);

	vk::ApplicationInfo appInfo = {};
	appInfo.pApplicationName = "mrrr";
	appInfo.applicationVersion = 1;
	appInfo.apiVersion = version;

	uint32_t glfwRequiredExtensionCount = 0;
	const char** glfwRequiredExtensions = glfwGetRequiredInstanceExtensions(&glfwRequiredExtensionCount);
	std::vector<const char*> glfwEnabledExtensions(glfwRequiredExtensions, glfwRequiredExtensions + glfwRequiredExtensionCount);
	glfwEnabledExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

	for (uint32_t i = 0; i < glfwRequiredExtensionCount; ++i) {
		glfwEnabledExtensions[i] = glfwRequiredExtensions[i];
	}

	print1("Instance support extensions");
	std::vector<vk::ExtensionProperties> supportExtensions = vk::enumerateInstanceExtensionProperties().value;
	for (auto& supportedExtension : supportExtensions) {
		print2(supportedExtension.extensionName);
	}

	print1("Enabled extensions");
	for (uint32_t i = 0; i < glfwEnabledExtensions.size(); ++i) {
		const char* extension = glfwEnabledExtensions[i];
		const char* status = " (NOT SUPPORTED)";

		for (auto& supportedExtension : supportExtensions) {
			if (strcmp(extension, supportedExtension.extensionName) != 0) continue;
			status = " (SUPPORTED)";
			break;
		}

		print2(extension << status);
	}

	std::vector<const char*> enableLayers = { "VK_LAYER_KHRONOS_validation" };

	print1("Instance support layers");
	std::vector<vk::LayerProperties> supportLayers = vk::enumerateInstanceLayerProperties().value;
	for (auto& supportedLayer : supportLayers) {
		print2(supportedLayer.layerName);
	}

	print1("Enabled extensions");
	for (uint32_t i = 0; i < enableLayers.size(); ++i) {
		const char* layer = enableLayers[i];
		const char* status = " (NOT SUPPORTED)";

		for (auto& supportedLayer : supportLayers) {
			if (strcmp(layer, supportedLayer.layerName) != 0) continue;
			status = " (SUPPORTED)";
			break;
		}

		print2(layer << status);
	}

	vk::InstanceCreateInfo instanceCreateInfo = {};
	instanceCreateInfo.flags = vk::InstanceCreateFlags();
	instanceCreateInfo.pApplicationInfo = &appInfo;
	instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(glfwEnabledExtensions.size());
	instanceCreateInfo.ppEnabledExtensionNames = glfwEnabledExtensions.data();
	instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(enableLayers.size());
	instanceCreateInfo.ppEnabledLayerNames = enableLayers.data();

	vk::ResultValue<vk::Instance> instanceResult = vk::createInstance(instanceCreateInfo);
	if (instanceResult.result != vk::Result::eSuccess) {
		print1("Failed to create instance");
		return nullptr;
	}

	printEnd("Instance is created");
	return instanceResult.value;
}

VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
	vk::DebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	vk::DebugUtilsMessageTypeFlagsEXT messageType,
	const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData
) {
	std::cerr << "Vulkan_" << vk::to_string(messageSeverity) << " " << pCallbackData->pMessage << std::endl;
	return VK_FALSE;
}

vk::DebugUtilsMessengerEXT make_debug_utils_messenger(vk::Instance& instance, vk::detail::DispatchLoaderDynamic& dld)
{
	printStart("Making debug messenger");

	vk::DebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo = {};
	debugMessengerCreateInfo.flags = vk::DebugUtilsMessengerCreateFlagsEXT();
	debugMessengerCreateInfo.messageSeverity =
		vk::DebugUtilsMessageSeverityFlagBitsEXT::eError |
		vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo |
		vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning;
	debugMessengerCreateInfo.messageType =
		vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
		vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
		vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation;
	debugMessengerCreateInfo.pfnUserCallback = debugCallback;

	vk::ResultValue<vk::DebugUtilsMessengerEXT> debugMessenger = instance.createDebugUtilsMessengerEXT(debugMessengerCreateInfo, nullptr, dld);
	if (debugMessenger.result != vk::Result::eSuccess) {
		print1("Failed to create debug messenger");
		return nullptr;
	}

	printEnd("Debug messenger is created");
	return debugMessenger.value;
}

vk::PhysicalDevice choose_physical_device(vk::Instance& instance)
{
	printStart("Choosing a physical device");

	//TODO: check VK_EXT_SHADER_OBJECT_EXTENSION_NAME too
	const char* requiredExtension = VK_KHR_SWAPCHAIN_EXTENSION_NAME;

	vk::ResultValue<std::vector<vk::PhysicalDevice>> availableDevices = instance.enumeratePhysicalDevices();
	if (availableDevices.result != vk::Result::eSuccess) {
		print1("Failed to get physical devices list");
		return nullptr;
	}

	print1("List of physical devices");
	for (auto& device : availableDevices.value)
	{
		vk::PhysicalDeviceProperties properties = device.getProperties();
		std::ostringstream os;
		os << properties.deviceName << " " << "(" << properties.deviceID << ")";
		std::string deviceTitle = os.str();
		
		print2(deviceTitle);
		print3(vk::to_string(properties.deviceType));
		vk::ResultValue<std::vector<vk::ExtensionProperties>> supportedExtensions = device.enumerateDeviceExtensionProperties();
		if (supportedExtensions.result != vk::Result::eSuccess) {
			print3("Failed to get extension list from " << deviceTitle);
			continue;
		}

		for (auto& supportedExtension : supportedExtensions.value) {
			if (strcmp(requiredExtension, supportedExtension.extensionName) == 0) {
				print3("SUPPORTS " << requiredExtension);
				break;
			}
		}
	}

	for (auto& device : availableDevices.value)
	{
		vk::ResultValue<std::vector<vk::ExtensionProperties>> supportedExtensions = device.enumerateDeviceExtensionProperties();
		if (supportedExtensions.result != vk::Result::eSuccess) continue;

		for (auto& supportedExtension : supportedExtensions.value) {
			if (strcmp(requiredExtension, supportedExtension.extensionName) == 0) {
				vk::PhysicalDeviceProperties properties = device.getProperties();
				std::ostringstream os;
				os << properties.deviceName << " " << "(" << properties.deviceID << ")";

				printEnd("Selected " << os.str());
				return device;
			}
		}
	}

	printEnd("Supported device was not found");
	return nullptr;
}

uint32_t get_queue_graphics_family_index(vk::PhysicalDevice physicalDevice)
{
	std::vector<vk::QueueFamilyProperties> queueFamilies = physicalDevice.getQueueFamilyProperties();

	for (uint32_t i = 0; i < queueFamilies.size(); i++) {
		vk::QueueFamilyProperties queueFamily = queueFamilies[i];

		if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics) {
			return i;
		}
	}

	return UINT32_MAX;
}

uint32_t get_queue_present_family_index(vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface)
{
	std::vector<vk::QueueFamilyProperties> queueFamilies = physicalDevice.getQueueFamilyProperties();

	for (uint32_t i = 0; i < queueFamilies.size(); i++) {
		vk::QueueFamilyProperties queueFamily = queueFamilies[i];

		if (physicalDevice.getSurfaceSupportKHR(i, surface).result == vk::Result::eSuccess) {
			return i;
		}
	}

	return UINT32_MAX;
}

vk::Device create_logical_device(vk::PhysicalDevice physicalDevice, int queueGraphicFamilyIndex)
{
	printStart("Creating a logical device");
	float queuePriority = 1.0f;

	vk::DeviceQueueCreateInfo queueCreateInfo = {};
	queueCreateInfo.flags = vk::DeviceQueueCreateFlags();
	queueCreateInfo.queueFamilyIndex = queueGraphicFamilyIndex;
	queueCreateInfo.queueCount = 1;
	queueCreateInfo.pQueuePriorities = &queuePriority;

	vk::PhysicalDeviceFeatures deviceFeatures = {};
	vk::PhysicalDeviceShaderObjectFeaturesEXT deviceShaderFeatures = { VK_TRUE };
	vk::PhysicalDeviceDynamicRenderingFeaturesKHR deviceDynamicRenderingFeatures = { VK_TRUE };
	deviceShaderFeatures.pNext = &deviceDynamicRenderingFeatures;

	std::vector<const char*> requiredExtensions {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME,
		VK_EXT_SHADER_OBJECT_EXTENSION_NAME,
		VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME
	};

	vk::DeviceCreateInfo deviceCreateInfo = {};
	deviceCreateInfo.flags = vk::DeviceCreateFlags();
	deviceCreateInfo.queueCreateInfoCount = 1;
	deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
	deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
	deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
	deviceCreateInfo.ppEnabledExtensionNames = requiredExtensions.data();
	deviceCreateInfo.pNext = &deviceShaderFeatures;

	vk::ResultValue<vk::Device> logicalDevice = physicalDevice.createDevice(deviceCreateInfo);
	if (logicalDevice.result != vk::Result::eSuccess) {
		print1("Failed to create logical device");
		return nullptr;
	}

	printEnd("Logical device was created");
	return logicalDevice.value;
}

vk::Queue get_queue(vk::Device logicalDevice, int queueGraphicIndex)
{
	return logicalDevice.getQueue(queueGraphicIndex, 0);
}

vk::SwapchainKHR create_swapchain(
	vk::PhysicalDevice physicalDevice,
	vk::Device logicalDevice,
	vk::SurfaceKHR surface,
	uint32_t width,
	uint32_t height,
	int queueGraphicFamilyIndex,
	int queueKHRFamilyIndex
)
{
	printStart("Swapchain creation");
	vk::SurfaceCapabilitiesKHR capabilities = physicalDevice.getSurfaceCapabilitiesKHR(surface).value;
	std::vector<vk::SurfaceFormatKHR> formats = physicalDevice.getSurfaceFormatsKHR(surface).value;
	std::vector<vk::PresentModeKHR> presentModes = physicalDevice.getSurfacePresentModesKHR(surface).value;

	vk::SurfaceFormatKHR finalFormat = formats[0];
	for (vk::SurfaceFormatKHR format : formats) {
		if (format.format == vk::Format::eB8G8R8A8Unorm && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
			finalFormat = format;
			break;
		}
	}

	vk::PresentModeKHR finalPresentMode = vk::PresentModeKHR::eFifo;
	for (vk::PresentModeKHR presentMode : presentModes) {
		if (presentMode == vk::PresentModeKHR::eMailbox) {
			finalPresentMode = presentMode;
		}
	}

	vk::Extent2D extent = { width, height };
	if (capabilities.currentExtent.width != UINT32_MAX) {
		extent = capabilities.currentExtent;
	} else {
		extent.width = std::min(
			capabilities.maxImageExtent.width,
			std::max(capabilities.minImageExtent.width, extent.width)
		);

		extent.height = std::min(
			capabilities.maxImageExtent.height,
			std::max(capabilities.minImageExtent.height, extent.height)
		);
	}

	uint32_t imageCount = capabilities.minImageCount + 1;
	if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount) {
		imageCount = capabilities.maxImageCount;
	}

	vk::SwapchainCreateInfoKHR swapchainCreateInfo = {};
	swapchainCreateInfo.flags = vk::SwapchainCreateFlagsKHR();
	swapchainCreateInfo.surface = surface;
	swapchainCreateInfo.minImageCount = imageCount;
	swapchainCreateInfo.imageFormat = finalFormat.format;
	swapchainCreateInfo.imageColorSpace = finalFormat.colorSpace;
	swapchainCreateInfo.imageExtent = extent;
	swapchainCreateInfo.imageArrayLayers = 1;
	swapchainCreateInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;
	swapchainCreateInfo.preTransform = capabilities.currentTransform;
	swapchainCreateInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
	swapchainCreateInfo.presentMode = finalPresentMode;
	swapchainCreateInfo.clipped = VK_TRUE;

	uint32_t queue[] = {
		static_cast<uint32_t>(queueGraphicFamilyIndex),
		static_cast<uint32_t>(queueKHRFamilyIndex)
	};

	if (queueGraphicFamilyIndex != queueKHRFamilyIndex) {
		swapchainCreateInfo.imageSharingMode = vk::SharingMode::eConcurrent;
		swapchainCreateInfo.queueFamilyIndexCount = 2;
		swapchainCreateInfo.pQueueFamilyIndices = queue;
	} else {
		swapchainCreateInfo.imageSharingMode = vk::SharingMode::eExclusive;
		swapchainCreateInfo.queueFamilyIndexCount = 1;
		swapchainCreateInfo.pQueueFamilyIndices = &queue[0];
	}

	vk::ResultValue<vk::SwapchainKHR> swapchain = logicalDevice.createSwapchainKHR(swapchainCreateInfo, nullptr);
	if (swapchain.result != vk::Result::eSuccess) {
		print1("Failed swapchain creation");
		return nullptr;
	}

	printEnd("Swapchain created");
	return swapchain.value;
}

vk::ImageView create_image_view(vk::Device logicalDevice, vk::Image image, vk::Format format)
{
	printStart("Creating ImageView");

	vk::ImageViewCreateInfo imageViewCreateInfo {};
	imageViewCreateInfo.image = image;
	imageViewCreateInfo.viewType = vk::ImageViewType::e2D;
	imageViewCreateInfo.format = format;
	imageViewCreateInfo.components.r = vk::ComponentSwizzle::eIdentity;
	imageViewCreateInfo.components.g = vk::ComponentSwizzle::eIdentity;
	imageViewCreateInfo.components.b = vk::ComponentSwizzle::eIdentity;
	imageViewCreateInfo.components.a = vk::ComponentSwizzle::eIdentity;
	imageViewCreateInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
	imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
	imageViewCreateInfo.subresourceRange.levelCount = 1;
	imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
	imageViewCreateInfo.subresourceRange.layerCount = 1;

	vk::ResultValue<vk::ImageView> imageView = logicalDevice.createImageView(imageViewCreateInfo);

	if (imageView.result != vk::Result::eSuccess) {
		print1("ImageView creation caused an error");
		return nullptr;
	}

	printEnd("ImageView was created successfully");
	return imageView.value;
}

// utils function
std::vector<char> loadFile(const char* filePath) {
    std::ifstream file = std::ifstream(filePath, std::ios::binary | std::ios::ate);

    if (!file.is_open()) {
		std::ostringstream os;
		os << "Failed to open file" << filePath;

		throw std::runtime_error(os.str());
	}

    size_t fileSize { static_cast<size_t>(file.tellg()) };
    std::vector<char> buffer(fileSize);

    file.seekg(0, std::ios::beg);
    file.read(buffer.data(), fileSize);
    file.close();

    return buffer;
}

std::vector<vk::ShaderEXT> make_shader_objects(
	vk::Device logicalDevice,
	const char* vertexShaderPath,
	const char* fragmentShaderPath,
	vk::detail::DispatchLoaderDynamic dld
)
{
	printStart("Load shaders");
	print1("Load shader files");
	std::vector<char> vertexContent = loadFile(vertexShaderPath);
	std::vector<char> fragmentContent = loadFile(fragmentShaderPath);
	print1("Loaded successfully");

	vk::ShaderCreateInfoEXT vertexInfo {};
	vertexInfo.flags = vk::ShaderCreateFlagBitsEXT::eLinkStage;
	vertexInfo.stage = vk::ShaderStageFlagBits::eVertex;
	vertexInfo.nextStage = vk::ShaderStageFlagBits::eFragment;
	vertexInfo.codeType = vk::ShaderCodeTypeEXT::eSpirv;
	vertexInfo.codeSize = vertexContent.size();
	vertexInfo.pCode = vertexContent.data();
	vertexInfo.pName = "main";

	vk::ShaderCreateInfoEXT fragmentInfo {};
	fragmentInfo.flags = vk::ShaderCreateFlagBitsEXT::eLinkStage;
	fragmentInfo.stage = vk::ShaderStageFlagBits::eFragment;
	fragmentInfo.codeType = vk::ShaderCodeTypeEXT::eSpirv;
	fragmentInfo.codeSize = fragmentContent.size();
	fragmentInfo.pCode = fragmentContent.data();
	fragmentInfo.pName = "main";

	std::vector<vk::ShaderCreateInfoEXT> shaderInfo = { vertexInfo, fragmentInfo };
	vk::ResultValue<std::vector<vk::ShaderEXT>> shaders = logicalDevice.createShadersEXT(shaderInfo, nullptr, dld);

	if (shaders.result != vk::Result::eSuccess) {
		print1("Shaders loading caused an error");
		return {};
	}

	printEnd("Shaders was successfully loaded");
	return shaders.value;
}

vk::CommandPool createCommandPool(vk::Device logicalDevice, uint32_t queueGraphicFamilyIndex) {
    vk::CommandPoolCreateInfo poolInfo {};
    poolInfo.queueFamilyIndex = queueGraphicFamilyIndex;
    poolInfo.flags = vk::CommandPoolCreateFlags() | vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
	//TODO: check errors
    return logicalDevice.createCommandPool(poolInfo).value;
}

vk::CommandBuffer createCommandBuffer(vk::Device logicalDevice, vk::CommandPool commandPool) {
    vk::CommandBufferAllocateInfo allocInfo {};
    allocInfo.commandBufferCount = 1;
    allocInfo.commandPool = commandPool;
    allocInfo.level = vk::CommandBufferLevel::ePrimary;
    return logicalDevice.allocateCommandBuffers(allocInfo).value[0];
}

void transition_image_layout(vk::CommandBuffer commandBuffer, vk::Image image,
    vk::ImageLayout oldLayout, vk::ImageLayout newLayout,
	vk::AccessFlags srcAccessMask, vk::AccessFlags dstAccessMask,
	vk::PipelineStageFlags srcStage, vk::PipelineStageFlags dstStage) {

	vk::ImageSubresourceRange access;
	access.aspectMask = vk::ImageAspectFlagBits::eColor;
	access.baseMipLevel = 0;
	access.levelCount = 1;
	access.baseArrayLayer = 0;
	access.layerCount = 1;

	vk::ImageMemoryBarrier barrier;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = image;
	barrier.subresourceRange = access;

	vk::PipelineStageFlags sourceStage, destinationStage;

	barrier.srcAccessMask = srcAccessMask;
	barrier.dstAccessMask = dstAccessMask;

	commandBuffer.pipelineBarrier(
		srcStage, dstStage, vk::DependencyFlags(), nullptr, nullptr, barrier);

}

void set_command_buffer(
	vk::CommandBuffer commandBuffer,
	std::vector<vk::ShaderEXT>& shaders,
	vk::Image image,
	vk::ImageView imageView,
	vk::Extent2D frameSize,
	vk::detail::DispatchLoaderDynamic dld
) {
	vk::RenderingAttachmentInfoKHR colorAttachment {};
	colorAttachment.imageView = imageView;
	colorAttachment.imageLayout = vk::ImageLayout::eAttachmentOptimal;
	colorAttachment.loadOp = vk::AttachmentLoadOp::eClear;
	colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
	colorAttachment.clearValue = vk::ClearValue({ 0.5f, 0.0f, 0.25f, 1.0f });

	vk::CommandBufferBeginInfo beginInfo {};
	commandBuffer.begin(beginInfo);

	transition_image_layout(commandBuffer, image,
		vk::ImageLayout::eUndefined, vk::ImageLayout::eAttachmentOptimal,
		vk::AccessFlagBits::eNone, vk::AccessFlagBits::eColorAttachmentWrite,
		vk::PipelineStageFlagBits::eTopOfPipe, vk::PipelineStageFlagBits::eFragmentShader);

		vk::Viewport viewport = 
		vk::Viewport(0.0f, 0.0f, frameSize.width, frameSize.height, 0.0f, 1.0f);

	commandBuffer.setViewportWithCount(viewport);

	vk::Rect2D scissor = vk::Rect2D({ 0,0 }, frameSize);
	commandBuffer.setScissorWithCount(scissor);

	commandBuffer.setRasterizerDiscardEnable(0);
	commandBuffer.setPolygonModeEXT(vk::PolygonMode::eFill, dld);
	commandBuffer.setRasterizationSamplesEXT(vk::SampleCountFlagBits::e1, dld);
	uint32_t sampleMask = 1;
	commandBuffer.setSampleMaskEXT(vk::SampleCountFlagBits::e1, sampleMask, dld);
	commandBuffer.setAlphaToCoverageEnableEXT(0, dld);
	commandBuffer.setCullMode(vk::CullModeFlagBits::eNone);
	commandBuffer.setDepthTestEnable(0);
	commandBuffer.setDepthWriteEnable(0);
	commandBuffer.setDepthBiasEnable(0);
	commandBuffer.setStencilTestEnable(0);
	commandBuffer.setPrimitiveTopology(vk::PrimitiveTopology::eTriangleList);
	commandBuffer.setPrimitiveRestartEnable(0);
	uint32_t colorBlendEnable = 0;
	commandBuffer.setColorBlendEnableEXT(0, colorBlendEnable, dld);
	vk::ColorBlendEquationEXT equation;
	equation.colorBlendOp = vk::BlendOp::eAdd;
	equation.dstColorBlendFactor = vk::BlendFactor::eZero;
	equation.srcColorBlendFactor = vk::BlendFactor::eOne;
	commandBuffer.setColorBlendEquationEXT(0, equation, dld);
	vk::ColorComponentFlags colorWriteMask = vk::ColorComponentFlagBits::eR
		| vk::ColorComponentFlagBits::eG
		| vk::ColorComponentFlagBits::eB
		| vk::ColorComponentFlagBits::eA;
	commandBuffer.setColorWriteMaskEXT(0, colorWriteMask, dld);

	vk::RenderingInfoKHR renderingInfo {};
	renderingInfo.flags = vk::RenderingFlagsKHR();
	renderingInfo.renderArea = vk::Rect2D({ 0,0 }, frameSize);
	renderingInfo.layerCount = 1;
	renderingInfo.viewMask = 0;
	renderingInfo.colorAttachmentCount = 1;
	renderingInfo.pColorAttachments = &colorAttachment;

	commandBuffer.beginRenderingKHR(renderingInfo, dld);

	vk::ShaderStageFlagBits stages[2] = {
		vk::ShaderStageFlagBits::eVertex,
		vk::ShaderStageFlagBits::eFragment
	};
	commandBuffer.bindShadersEXT(stages, shaders, dld);

	commandBuffer.draw(3, 1, 0, 0);

	commandBuffer.endRenderingKHR(dld);

	transition_image_layout(commandBuffer, image,
		vk::ImageLayout::eAttachmentOptimal, vk::ImageLayout::ePresentSrcKHR,
		vk::AccessFlagBits::eColorAttachmentWrite, vk::AccessFlagBits::eNone,
		vk::PipelineStageFlagBits::eFragmentShader, vk::PipelineStageFlagBits::eBottomOfPipe);

	commandBuffer.end();
}

void draw(vk::CommandBuffer commandBuffer, vk::SwapchainKHR swapchain, vk::Queue graphicQueue) {
	uint32_t imageIndex{ 0 };

	vk::SubmitInfo submitInfo = {};
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	graphicQueue.submit(submitInfo);
	graphicQueue.waitIdle();

	vk::PresentInfoKHR presentInfo = {};
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &swapchain;
	presentInfo.pImageIndices = &imageIndex;	

	graphicQueue.presentKHR(&presentInfo);
	graphicQueue.waitIdle();
}

void spawn_render_thread(GLFWwindow* window, std::atomic<bool>* done) {
    vk::Instance instance = make_instance();
	vk::detail::DispatchLoaderDynamic dld(instance, vkGetInstanceProcAddr);
	vk::DebugUtilsMessengerEXT debugUtilsMessenger = make_debug_utils_messenger(instance, dld);

	VkSurfaceKHR surface;
	glfwCreateWindowSurface(instance, window, nullptr, &surface);

	vk::PhysicalDevice physicalDevice = choose_physical_device(instance);
	int queueGraphicFamilyIndex = get_queue_graphics_family_index(physicalDevice);
	int queuePresentFamilyIndex = get_queue_present_family_index(physicalDevice, surface);
	vk::Device logicalDevice = create_logical_device(physicalDevice, queueGraphicFamilyIndex);

	vk::Queue graphicQueue = get_queue(logicalDevice, queueGraphicFamilyIndex);
	vk::Queue khrQueue = get_queue(logicalDevice, queuePresentFamilyIndex);

	vk::SwapchainKHR swapchain = create_swapchain(physicalDevice, logicalDevice, surface, 700, 500, queueGraphicFamilyIndex, queuePresentFamilyIndex);
	std::vector<vk::Image> images = logicalDevice.getSwapchainImagesKHR(swapchain).value;

	//TODO: make function for this
	std::vector<vk::SurfaceFormatKHR> formats = physicalDevice.getSurfaceFormatsKHR(surface).value;
	vk::SurfaceFormatKHR finalFormat = formats[0];
	for (vk::SurfaceFormatKHR format : formats) {
		if (format.format == vk::Format::eB8G8R8A8Unorm && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
			finalFormat = format;
			break;
		}
	}
	vk::SurfaceCapabilitiesKHR capabilities = physicalDevice.getSurfaceCapabilitiesKHR(surface).value;
	vk::Extent2D extent = capabilities.currentExtent;

	std::vector<vk::ImageView> imageViews {};
	for (int i = 0; i < images.size(); i++)
	{
		imageViews.push_back(create_image_view(logicalDevice, images[i], finalFormat.format));
	}

	std::vector<vk::ShaderEXT> shaders = make_shader_objects(logicalDevice, "shaders/basic_vert.spv", "shaders/basic_frag.spv", dld);
	vk::CommandPool commandPool = createCommandPool(logicalDevice, queueGraphicFamilyIndex);
	vk::CommandBuffer commandBuffer = createCommandBuffer(logicalDevice, commandPool);

	for (uint32_t i = 0; i < images.size(); ++i) {
		set_command_buffer(commandBuffer, shaders, images[i], imageViews[i], extent, dld);
	}

    while (!*done) {
        draw(commandBuffer, swapchain, graphicQueue);
    }

	// destroying
    for (int i = 0; i < shaders.size(); i++)
	{
		logicalDevice.destroyShaderEXT(shaders[i], nullptr, dld);
	}

	for (int i = 0; i < imageViews.size(); i++)
	{
		logicalDevice.destroyImageView(imageViews[i]);
	}

	logicalDevice.destroySwapchainKHR(swapchain, nullptr, dld);	
	logicalDevice.destroy();
	instance.destroySurfaceKHR(surface, nullptr, dld);
	instance.destroyDebugUtilsMessengerEXT(debugUtilsMessenger, nullptr, dld);
	instance.destroy();
}


//TODO: refactor all shit (use {})
//TODO: pass dld to all shit
//TODO: SDL3
int main() 
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(800, 600, "Test Window", nullptr, nullptr);

	

	
	// vk::Semaphore semaphor1 = createSemaphore(logicalDevice);
	// vk::Semaphore semaphor2 = createSemaphore(logicalDevice);
	// vk::Fence fence = createFence(logicalDevice);

    

	std::atomic<bool> done = false;
    std::thread render_thread(spawn_render_thread, window, &done);

	while(!glfwWindowShouldClose(window)) {
        glfwPollEvents();
		// drawFrame(logicalDevice, graphicQueue, swapchain, commandBuffer, imageViews[0], extent, shaders, semaphor1, semaphor2, fence, dld);
    }

    done = true;
    render_thread.join();

	//destroying
	// logicalDevice.waitIdle();

	
    glfwDestroyWindow(window);
    glfwTerminate();
}