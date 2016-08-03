#pragma once

#include <vulkan\vulkan.h>

#include "VkWrapper.h"

struct QueueInfo {
	int graphicsQueueIdx = -1;
	int presentQueueIdx = -1;

	bool hasQueues() {
		return (graphicsQueueIdx != -1 && presentQueueIdx != -1);
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
	bool isSuitableDevice(VkPhysicalDevice device, QueueInfo info);
};

