#include "DeviceHelper.h"

#include <stdexcept>
#include <vector>
#include <set>

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
		QueueInfo info = findQueues(device);
		if (isSuitableDevice(device, info)) {
			physDeviceQueueInfo = info;
			physDevice = device;
			return;
		}	
	}
	throw std::runtime_error("Could not find suitable physical device.");
}

void DeviceHelper::createLogicalDevice() {
	std::set<int> uniqueQueues{ physDeviceQueueInfo.graphicsQueueIdx, physDeviceQueueInfo.presentQueueIdx };
	std::vector<VkDeviceQueueCreateInfo> queueInfos;
	for (auto queueIdx : uniqueQueues) {
		VkDeviceQueueCreateInfo queueInfo = {};
		queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueInfo.queueFamilyIndex = queueIdx;
		queueInfo.queueCount = 1;
		float priority = 1.0f;
		queueInfo.pQueuePriorities = &priority;
		queueInfos.push_back(queueInfo);
	}
	VkPhysicalDeviceFeatures features = {};
	VkDeviceCreateInfo deviceInfo = {};
	deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceInfo.pQueueCreateInfos = queueInfos.data();
	deviceInfo.queueCreateInfoCount = queueInfos.size();
	deviceInfo.enabledLayerCount = app.getReqLayers().size();
	deviceInfo.ppEnabledLayerNames = app.getReqLayers().data();
	if (vkCreateDevice(physDevice, &deviceInfo, nullptr, &device) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create vkLogicalDevice.");
	}
	vkGetDeviceQueue(device, physDeviceQueueInfo.graphicsQueueIdx, 0, &graphicsQueue);
	vkGetDeviceQueue(device, physDeviceQueueInfo.presentQueueIdx, 0, &presentQueue);
}

QueueInfo DeviceHelper::findQueues(VkPhysicalDevice device) {
	QueueInfo info;
	//find a usable queue family
	unsigned int queueCount;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueCount, nullptr);
	std::vector<VkQueueFamilyProperties> queueFamilies(queueCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueCount, queueFamilies.data());
	for (int i = 0; i < queueCount; i++) {
		if (queueFamilies[i].queueCount > 0 && queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			info.graphicsQueueIdx = i;
		}
		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, app.getSurface(), &presentSupport);
		if (queueFamilies[i].queueCount > 0 && presentSupport == true) {
			info.presentQueueIdx = i;
		}
		if (info.hasQueues()) {
			break;
		}
	}
	return info;
}

bool DeviceHelper::isSuitableDevice(VkPhysicalDevice device, QueueInfo info) {
	if (!info.hasQueues()) {
		return false;
	}
	deviceExtHelper.query(std::bind(vkEnumerateDeviceExtensionProperties, device, nullptr, std::placeholders::_1, std::placeholders::_2));
	if (!deviceExtHelper.areNamesPresent(deviceReqExtensions)) {
		return false;
	}
	return true;
}


