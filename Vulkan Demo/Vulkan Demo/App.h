#pragma once

#include <GLFW\glfw3.h>
#include <vulkan\vulkan.h>
#include <vector>
#include <string>

#include "vkhelpers\VkWrapper.h"
#include "vkhelpers\ExtensionHelper.h"
#include "vkhelpers\LayerHelper.h"

class App {

public:
	App();
	~App();

	void runLoop();

	VkWrapper<VkInstance> getInstance() { return vkInstance; }

private:
	GLFWwindow * window;
	VkWrapper<VkInstance> vkInstance{ vkDestroyInstance };

	std::vector<const char*> reqExtensions;
	std::vector<const char*> reqLayers;

	ExtensionHelper extHelper;
	LayerHelper layerHelper;

	const int WIDTH = 720;
	const int HEIGHT = 480;

	void initGLFW();
	void initVulkan();
	
	void createVkInstance();
	void setReqExtensions();
	void setReqLayers();

};

