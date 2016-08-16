#include "ShaderLoader.h"

#include <filesystem>
#include <iostream>

ShaderLoader::ShaderLoader() {
}


ShaderLoader::~ShaderLoader() {
}

void ShaderLoader::loadShaders() {
	for (auto iter = fs::directory_iterator("shaders/"); iter != fs::directory_iterator(); iter++) {
		readBinaryFile(iter->path());
	}
}

VkPipelineShaderStageCreateInfo ShaderLoader::getShaderInfo(const std::string & name) {
	return VkPipelineShaderStageCreateInfo();
}

std::vector<char> ShaderLoader::readBinaryFile(fs::path path) {
	return std::vector<char>();
}

VkShaderModule ShaderLoader::createShaderModule(std::vector<char> data) {
	return VkShaderModule();
}

VkPipelineShaderStageCreateInfo ShaderLoader::createShaderStage(VkShaderModule module) {
	return VkPipelineShaderStageCreateInfo();
}
