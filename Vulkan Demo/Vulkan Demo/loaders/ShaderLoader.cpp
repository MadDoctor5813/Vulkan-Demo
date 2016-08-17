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
		shaders.emplace(std::piecewise_construct, std::forward_as_tuple(iter->path()), std::forward_as_tuple(VkWrapper<VkShaderModule> {appRef.getDeviceHelper().getDeviceWrapper(), vkDestroyShaderModule}, VkPipelineShaderStageCreateInfo()));
		auto binaryData = readBinaryFile(iter->path());
		createShaderModule(binaryData, shaders.at(iter->path()).module);
		auto stageInfo = createShaderStageInfo(shaders.at(iter->path()).module, getStageFromPath(iter->path()));
		shaders.at(iter->path()).info = stageInfo;
	}
}

Shader& ShaderLoader::getShader(const std::string & name) {
	fs::path namePath(name);
	return shaders.at(namePath);
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

VkPipelineShaderStageCreateInfo ShaderLoader::createShaderStageInfo(VkShaderModule module, VkShaderStageFlagBits stage) {
	VkPipelineShaderStageCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	createInfo.module = module;
	createInfo.pName = "main";
	createInfo.stage = stage;
	return createInfo;
}

VkShaderStageFlagBits ShaderLoader::getStageFromPath(fs::path path) {
	std::string stageStr = path.filename().stem().extension().string();
	if (stageStr == ".vert") {
		return VK_SHADER_STAGE_VERTEX_BIT;
	}
	if (stageStr == ".frag") {
		return VK_SHADER_STAGE_FRAGMENT_BIT;
	}
}
