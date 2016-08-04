#include "DeviceHelper.h"

#include <stdexcept>
#include <vector>
#include <set>
#include <iostream>
#include <limits>
#include <algorithm>

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
		QueueInfo info;
		SwapChainDetails details;
		if (isSuitableDevice(device, info, details)) {
			physDeviceQueueInfo = info;
			physDeviceSwapDetails = details;
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
	deviceInfo.enabledExtensionCount = deviceReqExtensions.size();
	deviceInfo.ppEnabledExtensionNames = deviceReqExtensions.data();
	if (vkCreateDevice(physDevice, &deviceInfo, nullptr, &device) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create vkLogicalDevice.");
	}
	vkGetDeviceQueue(device, physDeviceQueueInfo.graphicsQueueIdx, 0, &graphicsQueue);
	vkGetDeviceQueue(device, physDeviceQueueInfo.presentQueueIdx, 0, &presentQueue);
}

VkSurfaceFormatKHR DeviceHelper::selectDeviceSurfaceFormat() {
	VkSurfaceFormatKHR desiredFormat{ VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
	if (physDeviceSwapDetails.formats.size == 1 && physDeviceSwapDetails.formats[0].format == VK_FORMAT_UNDEFINED) {
		return desiredFormat;
	}
	for (auto format : physDeviceSwapDetails.formats) {
		if (format.format == desiredFormat.format && format.colorSpace == desiredFormat.colorSpace) {
			return format;
		}
	}
	throw std::runtime_error("Could not find proper surface format.");
}

VkPresentModeKHR DeviceHelper::selectDevicePresentMode() {
	for (auto mode : physDeviceSwapDetails.modes) {
		if (mode == VK_PRESENT_MODE_MAILBOX_KHR) {
			return mode;
		}
	}
	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D DeviceHelper::selectDeviceExtent() {
	//if the extents are max use the provided extent
	if (physDeviceSwapDetails.caps.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
		return physDeviceSwapDetails.caps.currentExtent;
	}
	//select our own extent using the max and min extents from the capablities struct
	else {
		VkExtent2D extent { app.WIDTH, app.HEIGHT };
		extent.width = std::max(physDeviceSwapDetails.caps.maxImageExtent.width, std::min(extent.width, physDeviceSwapDetails.caps.minImageExtent.width));
		extent.height = std::max(physDeviceSwapDetails.caps.maxImageExtent.height, std::min(extent.height, physDeviceSwapDetails.caps.minImageExtent.height));
		return extent;
	}
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

SwapChainDetails DeviceHelper::querySwapChain(VkPhysicalDevice device) {
	SwapChainDetails details;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, app.getSurface(), &details.caps);
	unsigned int formatCount = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, app.getSurface(), &formatCount, nullptr);
	if (formatCount > 0) {
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, app.getSurface(), &formatCount, details.formats.data());
	}
	unsigned int modeCount = 0;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, app.getSurface(), &modeCount, nullptr);
	if (modeCount > 0) {
		details.modes.resize(modeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, app.getSurface(), &modeCount, details.modes.data());
	}
	return details;
}

bool DeviceHelper::isSuitableDevice(VkPhysicalDevice device, QueueInfo info, SwapChainDetails swapDetails) {
	deviceExtHelper.query(std::bind(vkEnumerateDeviceExtensionProperties, device, nullptr, std::placeholders::_1, std::placeholders::_2));
	std::cout << "Loaded device extensions:" << std::endl;
	deviceExtHelper.listNames(std::cout);
	return info.hasQueues() && swapDetails.isCompatible() && deviceExtHelper.areNamesPresent(deviceReqExtensions);
}


