#pragma once

#include <vulkan\vulkan.h>

#include "VkWrapper.h"

class App;

class DeviceHelper {
public:
	DeviceHelper(App& app);
	~DeviceHelper();

	

	void selectPhysicalDevice();
	void createLogicalDevice();

private:
	App& app;

	VkWrapper<VkDevice> device{ vkDestroyDevice };

	VkPhysicalDevice physDevice;
	int deviceQueueIndex;

	bool isSuitableDevice(VkPhysicalDevice device);
};

