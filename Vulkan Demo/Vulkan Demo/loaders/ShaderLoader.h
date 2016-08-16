#pragma once

#include <map>
#include <vector>
#include <string>
#include <filesystem>
#include <vulkan\vulkan.h>

#include "vkhelpers\VkWrapper.h"

namespace fs = std::tr2::sys;

class App;

class ShaderLoader {
public:
	ShaderLoader(App& app);
	~ShaderLoader();

	void loadShaders();

	VkPipelineShaderStageCreateInfo getShaderInfo(const std::string& name);

private:
	App& appRef;

	std::map<std::string, VkWrapper<VkShaderModule>> shaders;

	std::vector<char> readBinaryFile(fs::path path);
	void createShaderModule(std::vector<char> data, VkWrapper<VkShaderModule>& module);
	VkPipelineShaderStageCreateInfo createShaderStage(VkShaderModule module);
};

