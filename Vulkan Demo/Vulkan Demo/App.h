#pragma once

#include <GLFW\glfw3.h>

#include "VkWrapper.h"

class App {

public:
	App();
	~App();

	void runLoop();

private:
	GLFWwindow * window;
	VkWrapper<VkInstance> vkInstance{ vkDestroyInstance };

	const int WIDTH = 720;
	const int HEIGHT = 480;

	void initGLFW();
	void initVulkan();
	
	void createVkInstance();

};

