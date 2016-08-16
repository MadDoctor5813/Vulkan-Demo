#include "ShaderLoader.h"

#include <filesystem>
#include <iostream>
#include <fstream>
#include <stdexcept>

#include "App.h"

ShaderLoader::ShaderLoader(App& app) : appRef(app) {
}


ShaderLoader::~ShaderLoader() {
}

void ShaderLoader::loadShaders() {
	for (auto iter = fs::directory_iterator("shaders/"); iter != fs::directory_iterator(); iter++) {
		auto binaryData = readBinaryFile(iter->path());
		VkWrapper<VkShaderModule> shaderModule{ appRef.getDeviceHelper().getDeviceWrapper(), vkDestroyShaderModule };
		shaders.emplace(iter->path().string(), shaderModule);
		createShaderModule(binaryData, shaders.at(iter->path().string()));
	}
}

VkPipelineShaderStageCreateInfo ShaderLoader::getShaderInfo(const std::string & name) {
	return VkPipelineShaderStageCreateInfo();
}

std::vector<char> ShaderLoader::readBinaryFile(fs::path path) {
	std::ifstream fStream(path.string(), std::ios::ate | std::ios::binary);
	std::vector<char> data(fStream.tellg());
	fStream.seekg(0);
	fStream.read(data.data(), data.size());
	fStream.close();
	return data;
}

void ShaderLoader::createShaderModule(std::vector<char> data, VkWrapper<VkShaderModule>& module) {
	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = data.size();
	createInfo.pCode = (uint32_t*) data.data();
	if (vkCreateShaderModule(appRef.getDeviceHelper().getDevice(), &createInfo, nullptr, &module) != VK_SUCCESS) {
		throw std::runtime_error("Could not create shader module.");
	}
}

VkPipelineShaderStageCreateInfo ShaderLoader::createShaderStage(VkShaderModule module) {
	return VkPipelineShaderStageCreateInfo();
}
