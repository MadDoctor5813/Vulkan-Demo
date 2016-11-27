#pragma once

#include <vulkan\vulkan.h>
#include <GLFW\glfw3.h>
#include <vector>
#include <string>
#include <functional>

#include "vkhelpers\VkWrapper.h"
#include "vkhelpers\VkQueryHelper.h"
#include "vkhelpers\DeviceHelper.h"
#include "vkhelpers\GraphicsPipelineHelper.h"
#include "vkhelpers\VulkanContext.h"

#include "loaders\ShaderLoader.h"
#include "loaders\MeshLoader.h"

class App {

public:
	App();
	~App();

	void runLoop();

	ShaderLoader& getShaderLoader() { return shaderLoader; }
	MeshLoader& getMeshLoader() { return meshLoader; }
	VulkanContext& getVulkanContext() { return vulkanContext; }
	GLFWwindow* getWindow() { return window; }


	int windowWidth = 720;
	int windowHeight = 480;

private:
	GLFWwindow * window;
	VulkanContext vulkanContext;
	ShaderLoader shaderLoader;
	MeshLoader meshLoader;

	VkWrapper<VkSemaphore> imageAvailableSemaphore{ vulkanContext.getDeviceHelper().getDeviceWrapper(), vkDestroySemaphore };
	VkWrapper<VkSemaphore> renderFinishedSemaphore{ vulkanContext.getDeviceHelper().getDeviceWrapper(), vkDestroySemaphore };

	void initGLFW();
	void initVulkan();
	void drawFrame();

	void createSemaphores();

	static void onWindowResize(GLFWwindow * window, int width, int height);
	
};

