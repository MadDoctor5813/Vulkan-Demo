#pragma once

#include <GLFW\glfw3.h>
#include <vulkan\vulkan.h>

#include "vkhelpers\VkWrapper.h"
#include "vkhelpers\ExtensionHelper.h"

class App {

public:
	App();
	~App();

	void runLoop();

private:
	GLFWwindow * window;
	VkWrapper<VkInstance> vkInstance{ vkDestroyInstance };
	ExtensionHelper extHelper;


	const int WIDTH = 720;
	const int HEIGHT = 480;

	void initGLFW();
	void initVulkan();
	
	void createVkInstance();

};

