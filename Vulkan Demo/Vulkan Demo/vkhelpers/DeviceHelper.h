#pragma once

#include <vulkan\vulkan.h>
#include <vector>

#include "VkWrapper.h"
#include "VkQueryHelper.h"

struct QueueInfo {
	int graphicsQueueIdx = -1;
	int presentQueueIdx = -1;

	bool hasQueues() {
		return (graphicsQueueIdx != -1 && presentQueueIdx != -1);
	}
};

struct SwapChainDetails {
	VkSurfaceCapabilitiesKHR caps;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> modes;

	bool isCompatible() {
		return !formats.empty() && !modes.empty();
	}
};

class App;

class DeviceHelper {
public:
	DeviceHelper(App& app);
	~DeviceHelper();

	VkQueue graphicsQueue;
	VkQueue presentQueue;

	void selectPhysicalDevice();
	void createLogicalDevice();

private:
	App& app;

	VkWrapper<VkDevice> device{ vkDestroyDevice };

	VkPhysicalDevice physDevice;
	QueueInfo physDeviceQueueInfo;

	QueueInfo findQueues(VkPhysicalDevice device);
	SwapChainDetails querySwapChain(VkPhysicalDevice device);
	bool isSuitableDevice(VkPhysicalDevice device);


	VkQueryHelper<VkExtensionProperties> deviceExtHelper;
	std::vector<const char*> deviceReqExtensions {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME	
	};
};

