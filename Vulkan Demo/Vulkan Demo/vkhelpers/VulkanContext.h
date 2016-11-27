#pragma once

#include <vulkan\vulkan.h>

#include "vkhelpers\VkQueryHelper.h"
#include "vkhelpers\VkWrapper.h"
#include "DeviceHelper.h"
#include "GraphicsPipelineHelper.h"
#include "VertexBuffer.h"

class App;

class VulkanContext
{
public:
	VulkanContext(App& app);
	~VulkanContext();

	void initContext();
	void recreateSwapchain();

	VkInstance getInstance() { return vkInstance; }
	VkSurfaceKHR getSurface() { return vkSurface; }
	std::vector<const char*>& getReqExtensions() { return reqExtensions; }
	std::vector<const char*>& getReqLayers() { return reqLayers; }
	DeviceHelper& getDeviceHelper() { return deviceHelper; }
	VkSwapchainKHR getSwapChain() { return vkSwapChain; }
	std::vector<VkCommandBuffer>& getCommandBuffers() { return vkCommandBuffers; }
	VkFormat getSwapFormat() { return swapFormat; }
	VkExtent2D getSwapExtent() { return swapExtent; }

private:
	App& app;

	VkQueryHelper<VkExtensionProperties> extHelper;
	VkQueryHelper<VkLayerProperties> layerHelper;

	VkWrapper<VkInstance> vkInstance{ vkDestroyInstance };
	VkWrapper<VkDebugReportCallbackEXT> vkCallback{ vkInstance, destroyDebugCallback };
	VkWrapper<VkSurfaceKHR> vkSurface{ vkInstance, vkDestroySurfaceKHR };
	DeviceHelper deviceHelper;
	VkWrapper<VkSwapchainKHR> vkSwapChain{ deviceHelper.getDeviceWrapper(), vkDestroySwapchainKHR };
	std::vector<VkImage> swapImages;
	std::vector<VkWrapper<VkImageView>> vkImageViews;
	std::vector<VkWrapper<VkFramebuffer>> swapFrameBuffers;

	GraphicsPipelineHelper graphicsPipelineHelper;

	VkWrapper<VkCommandPool> vkCommandPool{ deviceHelper.getDeviceWrapper(), vkDestroyCommandPool };
	std::vector<VkCommandBuffer> vkCommandBuffers;

	VkFormat swapFormat;
	VkExtent2D swapExtent;

	std::vector<const char*> reqExtensions;
	std::vector<const char*> reqLayers;

	VertexBuffer vertexBuffer;

	void loadExtensions();
	void loadLayers();
	void createVkInstance();
	void setDebugCallback();
	void createVkSurface();
	void createSwapChain();
	void createImageViews();
	void createFramebuffers();
	void createCommandPool();
	void createCommandBuffers();

#ifdef NDEBUG	
	const bool enableDebugLayers = false;
#else
	const bool enableDebugLayers = true;
#endif

	static void destroyDebugCallback(VkInstance instance, VkDebugReportCallbackEXT callback, VkAllocationCallbacks* allocator);

	static VkBool32 _stdcall debugLayerCallback(
		VkDebugReportFlagsEXT flags,
		VkDebugReportObjectTypeEXT objType,
		uint64_t obj,
		size_t location,
		int32_t code,
		const char* layerPrefix,
		const char* msg,
		void* userData);
};

