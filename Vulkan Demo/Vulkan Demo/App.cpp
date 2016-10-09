#include "App.h"

#include <iostream>

App::App() : deviceHelper(*this), shaderLoader(*this), graphicsPipelineHelper(*this, deviceHelper) {
	
	initGLFW();
	initVulkan();

}


App::~App() {


}

void App::runLoop() {	
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		drawFrame();
	}

	vkDeviceWaitIdle(deviceHelper.getDevice());
}

void App::initGLFW() {
	if (glfwInit() != GLFW_TRUE) {
		throw std::runtime_error("GLFW init failed.");
	}

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

	window = glfwCreateWindow(windowWidth, windowHeight, "Vulkan Test", nullptr, nullptr);
	glfwSetWindowUserPointer(window, this);
	glfwSetWindowSizeCallback(window, onWindowResize);

	if (window == nullptr) {
		throw std::runtime_error("Window creation failed.");
	}
}

void App::initVulkan() {
	loadExtensions();
	loadLayers();
	createVkInstance();
	std::cout << "Loaded extensions: " << std::endl;
	extHelper.listNames(std::cout);
	std::cout << "Available layers: " << std::endl;
	layerHelper.listNames(std::cout);
	if (enableDebugLayers) {
		setDebugCallback();
	}
	createVkSurface();
	deviceHelper.selectPhysicalDevice();
	deviceHelper.createLogicalDevice();
	createSwapChain();
	createImageViews();
	shaderLoader.loadShaders();
	graphicsPipelineHelper.createRenderPass();
	graphicsPipelineHelper.initGraphicsPipeline();
	createFramebuffers();
	createCommandPool();
	createCommandBuffers();
	createSemaphores();
}

void App::drawFrame() {
	uint32_t imgIdx;
	vkAcquireNextImageKHR(deviceHelper.getDevice(), vkSwapChain, std::numeric_limits<int64_t>::max(), imageAvailableSemaphore, VK_NULL_HANDLE, &imgIdx);
	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	VkSemaphore waitSemaphores[] = { imageAvailableSemaphore };
	VkPipelineStageFlags waitFlags[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitFlags;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &vkCommandBuffers[imgIdx];
	VkSemaphore signalSemaphores[] = { renderFinishedSemaphore };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;
	if (vkQueueSubmit(deviceHelper.graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) {
		throw std::runtime_error("Could not submit command buffer");
	}
	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;
	VkSwapchainKHR swapChain[] = { vkSwapChain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChain;
	presentInfo.pImageIndices = &imgIdx;
	vkQueuePresentKHR(deviceHelper.presentQueue, &presentInfo);
}

void App::recreateSwapchain() {
	vkDeviceWaitIdle(deviceHelper.getDevice());

	createSwapChain();
	createImageViews();
	graphicsPipelineHelper.createRenderPass();
	graphicsPipelineHelper.initGraphicsPipeline();
	createFramebuffers();
	createCommandBuffers();
}

void App::onWindowResize(GLFWwindow * window, int width, int height) {
	if (width == 0 || height == 0) {
		return;
	}
	App* app = (App*)glfwGetWindowUserPointer(window);
	app->windowWidth = width;
	app->windowHeight = height;
	app->recreateSwapchain();
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

void App::createVkSurface() {
	if (glfwCreateWindowSurface(vkInstance, window, nullptr, &vkSurface) != VK_SUCCESS) {
		throw std::runtime_error("Could not create vkSurface.");
	}
}

void App::createSwapChain() {
	VkSwapchainCreateInfoKHR createInfo = {};
	deviceHelper.querySwapDetails();
	SwapChainDetails details = deviceHelper.getSwapDetails();
	QueueInfo queueInfo = deviceHelper.getQueueInfo();
	VkSurfaceFormatKHR format = deviceHelper.selectDeviceSurfaceFormat();
	VkPresentModeKHR mode = deviceHelper.selectDevicePresentMode();
	VkExtent2D extent = deviceHelper.selectDeviceExtent();
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = vkSurface;
	//the +1 is for triple buffering
	createInfo.minImageCount = details.caps.minImageCount + 1;
	createInfo.imageFormat = format.format;
	createInfo.imageColorSpace = format.colorSpace;
	createInfo.presentMode = mode;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	//determine sharing mode based on whether the present and graphics queues differ
	uint32_t queueIdxArr[] = { queueInfo.graphicsQueueIdx, queueInfo.presentQueueIdx };
	if (queueInfo.graphicsQueueIdx != queueInfo.presentQueueIdx) {
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueIdxArr;
	}
	else {
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}
	createInfo.preTransform = details.caps.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = mode;
	createInfo.clipped = VK_TRUE;
	//handle a previous swapchain here
	VkSwapchainKHR oldSwapchain = vkSwapChain;
	createInfo.oldSwapchain = oldSwapchain;
	VkSwapchainKHR newSwapchain;
	if (vkCreateSwapchainKHR(deviceHelper.getDevice(), &createInfo, nullptr, &newSwapchain) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create VkSwapChain");
	}
	vkSwapChain = newSwapchain;
	unsigned int imageCount;
	vkGetSwapchainImagesKHR(deviceHelper.getDevice(), vkSwapChain, &imageCount, nullptr);
	swapImages.resize(imageCount);
	vkGetSwapchainImagesKHR(deviceHelper.getDevice(), vkSwapChain, &imageCount, swapImages.data());
	swapFormat = createInfo.imageFormat;
	swapExtent = createInfo.imageExtent;
}

void App::createImageViews() {
	vkImageViews.resize(swapImages.size(), VkWrapper<VkImageView>{deviceHelper.getDeviceWrapper(), vkDestroyImageView});
	VkImageViewCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	createInfo.image = VK_NULL_HANDLE;
	createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	createInfo.format = swapFormat;
	createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
	createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	createInfo.subresourceRange.baseMipLevel = 0;
	createInfo.subresourceRange.levelCount = 1;
	createInfo.subresourceRange.baseArrayLayer = 0;
	createInfo.subresourceRange.layerCount = 1;
	for (int i = 0; i < swapImages.size(); i++) {
		createInfo.image = swapImages[i];
		if (vkCreateImageView(deviceHelper.getDevice(), &createInfo, nullptr, &vkImageViews[i]) != VK_SUCCESS) {
			throw std::runtime_error("Could not create vkImageView");
		}
	}
}

void App::createFramebuffers() {
	swapFrameBuffers.resize(swapImages.size(), VkWrapper<VkFramebuffer> { deviceHelper.getDeviceWrapper(), vkDestroyFramebuffer } );
	for (int i = 0; i < swapImages.size(); i++) {
		VkImageView attachments[] = {
			vkImageViews[i]
		};
		VkFramebufferCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		createInfo.renderPass = graphicsPipelineHelper.getRenderPass();
		createInfo.attachmentCount = 1;
		createInfo.pAttachments = attachments;
		createInfo.width = swapExtent.width;
		createInfo.height = swapExtent.height;
		createInfo.layers = 1;
		if (vkCreateFramebuffer(deviceHelper.getDevice(), &createInfo, nullptr, swapFrameBuffers[i].replace()) != VK_SUCCESS) {
			throw std::runtime_error("Couldn't create framebuffer");
		}
	}
}

void App::createCommandPool() {
	VkCommandPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = deviceHelper.getQueueInfo().graphicsQueueIdx;
	if (vkCreateCommandPool(deviceHelper.getDevice(), &poolInfo, nullptr, &vkCommandPool) != VK_SUCCESS) {
		throw std::runtime_error("Could not create command pool.");
	}
}

void App::createCommandBuffers() {
	if (vkCommandBuffers.size() != 0) {
		vkFreeCommandBuffers(deviceHelper.getDevice(), vkCommandPool, vkCommandBuffers.size(), vkCommandBuffers.data());
	}

	vkCommandBuffers.resize(swapFrameBuffers.size());
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = vkCommandPool;
	allocInfo.commandBufferCount = vkCommandBuffers.size();
	if (vkAllocateCommandBuffers(deviceHelper.getDevice(), &allocInfo, vkCommandBuffers.data()) != VK_SUCCESS) {
		throw std::runtime_error("Could not create command buffers.");
	}
	for (int i = 0; i < vkCommandBuffers.size(); i++) {
		VkCommandBufferBeginInfo bufferInfo = {};
		bufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		bufferInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
		vkBeginCommandBuffer(vkCommandBuffers[i], &bufferInfo);
		VkRenderPassBeginInfo passInfo = {};
		passInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		passInfo.renderPass = graphicsPipelineHelper.getRenderPass();
		passInfo.framebuffer = swapFrameBuffers[i];
		passInfo.renderArea.offset = { 0, 0 };
		passInfo.renderArea.extent = swapExtent;
		VkClearValue clearColor = { 1.0f, 0.0f, 1.0f, 1.0f };
		passInfo.clearValueCount = 1;
		passInfo.pClearValues = &clearColor;
		vkCmdBeginRenderPass(vkCommandBuffers[i], &passInfo, VK_SUBPASS_CONTENTS_INLINE);
		vkCmdBindPipeline(vkCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipelineHelper.getPipeline());
		vkCmdDraw(vkCommandBuffers[i], 3, 1, 0, 0);
		vkCmdEndRenderPass(vkCommandBuffers[i]);
		if (vkEndCommandBuffer(vkCommandBuffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("Could not create command buffer.");
		}
	}
}

void App::createSemaphores() {
	VkSemaphoreCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	if (vkCreateSemaphore(deviceHelper.getDevice(), &createInfo, nullptr, &imageAvailableSemaphore) != VK_SUCCESS ||
		vkCreateSemaphore(deviceHelper.getDevice(), &createInfo, nullptr, &renderFinishedSemaphore) != VK_SUCCESS) {
		throw std::runtime_error("Could not create semaphores.");
	}
}

void App::destroyDebugCallback(VkInstance instance, VkDebugReportCallbackEXT callback, VkAllocationCallbacks* allocator) {
	auto destroyFunc = (PFN_vkDestroyDebugReportCallbackEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT");
	destroyFunc(instance, callback, allocator);
}

VkBool32 _stdcall App::debugLayerCallback(
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

