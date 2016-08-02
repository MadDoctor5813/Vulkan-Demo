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

void DeviceHelper::createLogicalDevice() {
	VkDeviceQueueCreateInfo queueInfo = {};
	queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueInfo.queueFamilyIndex = deviceQueueIndex;
	queueInfo.queueCount = 1;
	float priority = 1.0f;
	queueInfo.pQueuePriorities = &priority;
	VkPhysicalDeviceFeatures features = {};
	VkDeviceCreateInfo deviceInfo = {};
	deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceInfo.pQueueCreateInfos = &queueInfo;
	deviceInfo.queueCreateInfoCount = 1;
	deviceInfo.enabledLayerCount = app.getReqLayers().size();
	deviceInfo.ppEnabledLayerNames = app.getReqLayers().data();
	if (vkCreateDevice(physDevice, &deviceInfo, nullptr, &device) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create vkLogicalDevice.");
	}
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


