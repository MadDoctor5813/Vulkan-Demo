#include "DeviceHelper.h"

#include <stdexcept>
#include <vector>

#include "App.h"

DeviceHelper::DeviceHelper(App& app) : app(app) {
	
}


DeviceHelper::~DeviceHelper() {
}

void DeviceHelper::selectPhysicalDevice() {
	unsigned int deviceCount = 0;
	vkEnumeratePhysicalDevices(app.getInstance(), &deviceCount, nullptr);
	if (deviceCount == 0) {
		throw std::runtime_error("No physical devices found.");
	}
	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(app.getInstance(), &deviceCount, devices.data());
	//select the best device
	for (auto device : devices) {
		if (isSuitableDevice(device)) {
			physDevice = device;
			return;
		}	
	}
	throw std::runtime_error("Could not find suitable physical device.");
}

bool DeviceHelper::isSuitableDevice(VkPhysicalDevice device) {
	VkPhysicalDeviceProperties props;
	vkGetPhysicalDeviceProperties(device, &props);
	//find a usable queue family
	unsigned int queueCount;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueCount, nullptr);
	std::vector<VkQueueFamilyProperties> queueFamilies(queueCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueCount, queueFamilies.data());
	for (int i = 0; i < queueCount; i++) {
		if (queueFamilies[i].queueCount > 0 && queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			deviceQueueIndex = i;
			return true;
		}
	}
	return false;
}


