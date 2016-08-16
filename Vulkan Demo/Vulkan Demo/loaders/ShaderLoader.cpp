#include "ShaderLoader.h"

#include <filesystem>
#include <iostream>

namespace fs = std::tr2::sys;

ShaderLoader::ShaderLoader() {
}


ShaderLoader::~ShaderLoader() {
}

void ShaderLoader::loadShaders() {
	for (auto iter = fs::directory_iterator("shaders/"); iter != fs::directory_iterator(); iter++) {
		std::cout << iter->path() << std::endl;
	}
}

VkPipelineShaderStageCreateInfo ShaderLoader::getShaderInfo(const std::string & name) {
	return VkPipelineShaderStageCreateInfo();
}

std::vector<char> ShaderLoader::readBinaryFile(const std::string & path) {
	return std::vector<char>();
}

VkShaderModule ShaderLoader::createShaderModule(std::vector<char> data) {
	return VkShaderModule();
}

VkPipelineShaderStageCreateInfo ShaderLoader::createShaderStage(VkShaderModule module) {
	return VkPipelineShaderStageCreateInfo();
}
