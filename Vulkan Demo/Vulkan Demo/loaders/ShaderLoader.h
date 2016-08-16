#pragma once

#include <map>
#include <vector>
#include <string>
#include <filesystem>
#include <vulkan\vulkan.h>

namespace fs = std::tr2::sys;

class ShaderLoader {
public:
	ShaderLoader();
	~ShaderLoader();

	void loadShaders();

	VkPipelineShaderStageCreateInfo getShaderInfo(const std::string& name);

private:
	std::map<std::string, VkPipelineShaderStageCreateInfo> shaders;

	std::vector<char> readBinaryFile(fs::path path);
	VkShaderModule createShaderModule(std::vector<char> data);
	VkPipelineShaderStageCreateInfo createShaderStage(VkShaderModule module);
};

