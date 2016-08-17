#pragma once

#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <filesystem>
#include <vulkan\vulkan.h>

#include "vkhelpers\VkWrapper.h"

namespace fs = std::tr2::sys;

struct Shader {
	VkWrapper<VkShaderModule> module;
	VkPipelineShaderStageCreateInfo info;

	Shader(VkWrapper<VkShaderModule> module, VkPipelineShaderStageCreateInfo info) : module(module), info(info) {

	}
};

class App;

class ShaderLoader {
public:
	ShaderLoader(App& app);
	~ShaderLoader();

	void loadShaders();

	Shader& getShader(const std::string& name);

private:
	App& appRef;

	std::map<fs::path, Shader> shaders;

	std::vector<char> readBinaryFile(fs::path path);
	void createShaderModule(std::vector<char> data, VkWrapper<VkShaderModule>& module);
	VkPipelineShaderStageCreateInfo createShaderStageInfo(VkShaderModule module, VkShaderStageFlagBits stage);
	VkShaderStageFlagBits getStageFromPath(fs::path path);
};

