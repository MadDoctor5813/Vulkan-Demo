#include "App.h"

#include <iostream>

App::App() : deviceHelper(*this), shaderLoader(*this) {
	
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
	createRenderPass();
	setupGraphicsPipeline();
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
	createInfo.oldSwapchain = VK_NULL_HANDLE;
	if (vkCreateSwapchainKHR(deviceHelper.getDevice(), &createInfo, nullptr, &vkSwapChain) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create VkSwapChain");
	}
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

void App::createRenderPass() {
	VkAttachmentDescription attachment = {};
	attachment.format = swapFormat;
	attachment.samples = VK_SAMPLE_COUNT_1_BIT;
	attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	VkAttachmentReference attachmentRef = {};
	attachmentRef.attachment = 0;
	attachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	VkSubpassDescription subPass = {};
	subPass.colorAttachmentCount = 1;
	subPass.pColorAttachments = &attachmentRef;
	VkRenderPassCreateInfo passInfo = {};
	passInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	passInfo.attachmentCount = 1;
	passInfo.pAttachments = &attachment;
	passInfo.subpassCount = 1;
	passInfo.pSubpasses = &subPass;
	if (vkCreateRenderPass(deviceHelper.getDevice(), &passInfo, nullptr, &vkRenderPass) != VK_SUCCESS) {
		throw std::runtime_error("Could not create render pass.");
	}
}

void App::setupGraphicsPipeline() {
	VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexAttributeDescriptionCount = 0;
	vertexInputInfo.vertexBindingDescriptionCount = 0;
	VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo = {};
	inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssemblyInfo.primitiveRestartEnable = false;
	VkViewport viewport = {};
	viewport.x = 0;
	viewport.y = 0;
	viewport.width = swapExtent.width;
	viewport.height = swapExtent.height;
	viewport.minDepth = 0;
	viewport.maxDepth = 1.0f;
	VkRect2D scissor = {};
	scissor.offset = { 0, 0 };
	scissor.extent = swapExtent;
	VkPipelineViewportStateCreateInfo viewportState = {};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;
	VkPipelineRasterizationStateCreateInfo rasterInfo = {};
	rasterInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterInfo.depthClampEnable = false;
	rasterInfo.rasterizerDiscardEnable = false;
	rasterInfo.polygonMode = VK_POLYGON_MODE_FILL;
	rasterInfo.lineWidth = 1.0f;
	rasterInfo.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterInfo.depthBiasEnable = false;
	VkPipelineMultisampleStateCreateInfo multisampleInfo = {};
	multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampleInfo.sampleShadingEnable = false;
	multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	VkPipelineColorBlendAttachmentState colorBlend = {};
	colorBlend.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlend.blendEnable = VK_TRUE;
	colorBlend.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	colorBlend.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	colorBlend.colorBlendOp = VK_BLEND_OP_ADD;
	colorBlend.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	colorBlend.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlend.alphaBlendOp = VK_BLEND_OP_ADD;
	VkPipelineColorBlendStateCreateInfo blendInfo = {};
	blendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	blendInfo.logicOpEnable = false;
	blendInfo.logicOp = VK_LOGIC_OP_COPY;
	blendInfo.attachmentCount = 1;
	blendInfo.pAttachments = &colorBlend;
	VkPipelineLayoutCreateInfo layoutInfo = {};
	layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	if (vkCreatePipelineLayout(deviceHelper.getDevice(), &layoutInfo, nullptr, &vkPipelineLayout) != VK_SUCCESS) {
		throw std::runtime_error("Could not create pipeline layout.");
	}
	VkPipelineShaderStageCreateInfo shaderInfos[] = { shaderLoader.getShader("shaders/phongshading.vert.spv").info, shaderLoader.getShader("shaders/phongshading.frag.spv").info };
	VkGraphicsPipelineCreateInfo pipelineInfo = {};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderInfos;
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssemblyInfo;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterInfo;
	pipelineInfo.pMultisampleState = &multisampleInfo;
	pipelineInfo.pColorBlendState = &blendInfo;
	pipelineInfo.layout = vkPipelineLayout;
	pipelineInfo.renderPass = vkRenderPass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
	if (vkCreateGraphicsPipelines(deviceHelper.getDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &vkGraphicsPipeline) != VK_SUCCESS) {
		throw std::runtime_error("Could not create graphics pipeline.");
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