#include "App.h"

#include <iostream>

App::App() : deviceHelper(*this) {
	
	initGLFW();
	initVulkan();

}


App::~App() {


}

void App::runLoop() {	
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
	}
}

void App::initGLFW() {
	if (glfwInit() != GLFW_TRUE) {
		throw std::runtime_error("GLFW init failed.");
	}

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan Test", nullptr, nullptr);

	if (window == nullptr) {
		throw std::runtime_error("Window creation failed.");
	}
}

void App::initVulkan() {
	loadExtensions();
	loadLayers();
	createVkInstance();
	std::cout << "Loaded extensions: " << std::endl;
	for (auto ext : extHelper.getNames()) {
		std::cout << ext.extensionName << std::endl;
	}
	std::cout << "Available layers: " << std::endl;
	for (auto layer : layerHelper.getNames()) {
		std::cout << layer.layerName << std::endl;
	}
	if (enableDebugLayers) {
		setDebugCallback();
	}
	deviceHelper.selectPhysicalDevice();
	deviceHelper.createLogicalDevice();
}

void App::createVkInstance() {
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Vulkan Demo";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;
	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;	
	createInfo.enabledExtensionCount = reqExtensions.size();
	createInfo.ppEnabledExtensionNames = reqExtensions.data();
	createInfo.enabledLayerCount = reqLayers.size();
	createInfo.ppEnabledLayerNames = reqLayers.data();
	if (vkCreateInstance(&createInfo, nullptr, &vkInstance) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create Vulkan instance.");
	}

}

void App::loadExtensions() {
	unsigned int extCount = 0;
	const char** exts;
	exts = glfwGetRequiredInstanceExtensions(&extCount);
	for (int i = 0; i < extCount; i++) {
		reqExtensions.push_back(exts[i]);
	}
	if (enableDebugLayers) {
		reqExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
	}
	extHelper.query(std::bind(vkEnumerateInstanceExtensionProperties, nullptr, std::placeholders::_1, std::placeholders::_2));
	if (!extHelper.areNamesPresent(reqExtensions)) {
		throw std::runtime_error("Required extensions not present.");
	}

}

void App::loadLayers() {
	if (enableDebugLayers) {
		reqLayers.push_back("VK_LAYER_LUNARG_standard_validation");
	}	
	layerHelper.query(vkEnumerateInstanceLayerProperties);
	if (!layerHelper.areNamesPresent(reqLayers)) {
		throw std::runtime_error("Required validation layers not present");
	}
}

void App::setDebugCallback() {
	VkDebugReportCallbackCreateInfoEXT callbackInfo = {};
	callbackInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
	callbackInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
	callbackInfo.pfnCallback = (PFN_vkDebugReportCallbackEXT) debugLayerCallback;
	auto callbackCreateFunc = (PFN_vkCreateDebugReportCallbackEXT) vkGetInstanceProcAddr(vkInstance, "vkCreateDebugReportCallbackEXT");
	callbackCreateFunc(vkInstance, &callbackInfo, nullptr, &vkCallback);
}

void App::destroyDebugCallback(VkInstance instance, VkDebugReportCallbackEXT callback, VkAllocationCallbacks* allocator) {
	auto destroyFunc = (PFN_vkDestroyDebugReportCallbackEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT");
	destroyFunc(instance, callback, allocator);
}

VkBool32 App::debugLayerCallback(
	VkDebugReportFlagsEXT flags,
	VkDebugReportObjectTypeEXT objType,
	uint64_t obj,
	size_t location,
	int32_t code,
	const char* layerPrefix,
	const char* msg,
	void* userData) {

	std::cerr << "validation layer: " << msg << std::endl;

	return VK_FALSE;
}