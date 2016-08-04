#pragma once

#include <vulkan\vulkan.h>
#include <GLFW\glfw3.h>
#include <vector>
#include <string>
#include <functional>

#include "vkhelpers\VkWrapper.h"
#include "vkhelpers\VkQueryHelper.h"
#include "vkhelpers\DeviceHelper.h"

class App {

public:
	App();
	~App();

	void runLoop();

	VkInstance getInstance() { return vkInstance; }
	VkSurfaceKHR getSurface() { return vkSurface; }
	std::vector<const char*>& getReqExtensions() { return reqExtensions; }
	std::vector<const char*>& getReqLayers() { return reqLayers; }

	const int WIDTH = 720;
	const int HEIGHT = 480;

private:
	GLFWwindow * window;
	VkWrapper<VkInstance> vkInstance{ vkDestroyInstance };
	VkWrapper<VkDebugReportCallbackEXT> vkCallback{ vkInstance, destroyDebugCallback };
	VkWrapper<VkSurfaceKHR> vkSurface{ vkInstance, vkDestroySurfaceKHR };
	VkWrapper<VkSwapchainKHR> vkSwapChain{ deviceHelper.getDeviceWrapper(), vkDestroySwapchainKHR };
	std::vector<VkImage> swapImages;
	std::vector<VkWrapper<VkImageView>> vkImageViews;

	VkFormat swapFormat;
	VkExtent2D swapExtent;

	std::vector<const char*> reqExtensions;
	std::vector<const char*> reqLayers;

#ifdef NDEBUG	
	const bool enableDebugLayers = false;
#else
	const bool enableDebugLayers = true;
#endif

	VkQueryHelper<VkExtensionProperties> extHelper;
	VkQueryHelper<VkLayerProperties> layerHelper;
	DeviceHelper deviceHelper;

	void initGLFW();
	void initVulkan();
	
	void createVkInstance();
	void loadExtensions();
	void loadLayers();
	void setDebugCallback();
	void createVkSurface();
	void createSwapChain();
	void createImageViews();

	static void destroyDebugCallback(VkInstance instance, VkDebugReportCallbackEXT callback, VkAllocationCallbacks* allocator);

	static VkBool32 debugLayerCallback(
		VkDebugReportFlagsEXT flags,
		VkDebugReportObjectTypeEXT objType,
		uint64_t obj,
		size_t location,
		int32_t code,
		const char* layerPrefix,
		const char* msg,
		void* userData);

};

