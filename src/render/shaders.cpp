#include "render.h"
#include <iostream>
#include <sstream>
#include <fstream>

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

void Render::createShaderModules() {
	m_Logger.info("Creating Shaders Modules");

    std::vector<char> vertexContent = loadFile("../../shaders/basic_vert.spv");
	std::vector<char> fragmentContent = loadFile("../../shaders/basic_frag.spv");

	vk::ShaderModuleCreateInfo vertexInfo {};
	vertexInfo.codeSize = vertexContent.size();
	vertexInfo.pCode = reinterpret_cast<uint32_t*>(vertexContent.data());

	vk::ShaderModuleCreateInfo fragmentInfo {};
	fragmentInfo.codeSize = fragmentContent.size();
	fragmentInfo.pCode = reinterpret_cast<uint32_t*>(fragmentContent.data());

	vk::ResultValue<vk::ShaderModule> vertexShader = m_LogicalDevice.createShaderModule(vertexInfo);
	vk::ResultValue<vk::ShaderModule> fragmentShader = m_LogicalDevice.createShaderModule(fragmentInfo);

	VK_FAILED_ERROR(vertexShader.result, "Vertex Shader Module creating caused an error");
	VK_FAILED_ERROR(fragmentShader.result, "Fragment Shader Module creating caused an error");

	m_Logger.info("Shaders Modules was created successfully");
	m_Shaders = { vertexShader.value, fragmentShader.value };

    // vk::ShaderCreateInfoEXT vertexInfo {};
	// vertexInfo.flags = vk::ShaderCreateFlagBitsEXT::eLinkStage;
	// vertexInfo.stage = vk::ShaderStageFlagBits::eVertex;
	// vertexInfo.codeType = vk::ShaderCodeTypeEXT::eSpirv;
	// vertexInfo.nextStage = vk::ShaderStageFlagBits::eFragment;
	// vertexInfo.codeSize = vertexContent.size();
	// vertexInfo.pCode = vertexContent.data();
	// vertexInfo.pName = "main";

	// vk::ShaderCreateInfoEXT fragmentInfo {};
	// fragmentInfo.flags = vk::ShaderCreateFlagBitsEXT::eLinkStage;
	// fragmentInfo.stage = vk::ShaderStageFlagBits::eFragment;
	// fragmentInfo.codeType = vk::ShaderCodeTypeEXT::eSpirv;
	// fragmentInfo.codeSize = fragmentContent.size();
	// fragmentInfo.pCode = fragmentContent.data();
	// fragmentInfo.pName = "main";

    // vk::ResultValue<std::vector<vk::ShaderEXT>> shaders = m_LogicalDevice.createShadersEXT({vertexInfo, fragmentInfo}, nullptr, m_Dispatcher);
    // VK_FAILED_ERROR(shaders.result, "aboba");
}