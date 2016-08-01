#include "App.h"

#include <iostream>

App::App() : extHelper(*this) {
	
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
	setReqExtensions();
	createVkInstance();
	std::cout << "Loaded extensions: " << std::endl;
	for (auto ext : extHelper.getExtensions()) {
		std::cout << ext.extensionName << std::endl;
	}
	std::cout << "Available layers: " << std::endl;
	for (auto layer : layerHelper.getLayers()) {
		std::cout << layer.layerName << std::endl;
	}
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
	createInfo.enabledLayerCount = 0;
	createInfo.ppEnabledLayerNames = nullptr;
	if (vkCreateInstance(&createInfo, nullptr, &vkInstance) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create Vulkan instance.");
	}

}

void App::setReqExtensions() {
	unsigned int extCount = 0;
	const char** exts;
	exts = glfwGetRequiredInstanceExtensions(&extCount);
	for (int i = 0; i < extCount; i++) {
		reqExtensions.push_back(exts[i]);
	}
#ifndef NDEBUG
	reqExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
#endif
	if (!extHelper.areExtensionsPresent(reqExtensions)) {
		throw std::runtime_error("Required extensions not present.");
	}

}

void App::setReqLayers() {
#ifndef NDEBUG
	reqLayers.push_back("VK_LAYER_LUNARG_standard_validation");
#endif
	if (!layerHelper.areLayersPresent(reqLayers)) {
		throw std::runtime_error("Required validation layers not present");
	}
}
