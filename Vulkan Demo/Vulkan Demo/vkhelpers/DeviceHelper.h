#pragma once

#include <vulkan\vulkan.h>

class App;

class DeviceHelper {
public:
	DeviceHelper(App& app);
	~DeviceHelper();

	void selectPhysicalDevice();
private:
	App& app;

	VkPhysicalDevice physDevice;
	int deviceQueueIndex;

	bool isSuitableDevice(VkPhysicalDevice device);
};

