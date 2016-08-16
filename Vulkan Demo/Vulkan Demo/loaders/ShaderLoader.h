#pragma once

#include <map>
#include <vector>
#include <string>
#include <vulkan\vulkan.h>

class ShaderLoader {
public:
	ShaderLoader();
	~ShaderLoader();

	void loadShaders();

	VkPipelineShaderStageCreateInfo getShaderInfo(const std::string& name);

private:
	std::map<std::string, VkPipelineShaderStageCreateInfo> shaders;

	std::vector<char> readBinaryFile(const std::string& path);
	VkShaderModule createShaderModule(std::vector<char> data);
	VkPipelineShaderStageCreateInfo createShaderStage(VkShaderModule module);
};

