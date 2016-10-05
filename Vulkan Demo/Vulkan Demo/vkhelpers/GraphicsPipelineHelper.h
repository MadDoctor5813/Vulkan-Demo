#pragma once

#include <vulkan\vulkan.h>

class App;

#include "vkhelpers/DeviceHelper.h"

class GraphicsPipelineHelper {
public:
	GraphicsPipelineHelper(App& app, DeviceHelper& deviceHelper);
	~GraphicsPipelineHelper();

	VkRenderPass getRenderPass() { return vkRenderPass; }
	VkPipelineLayout getPipelineLayout() { return vkPipelineLayout; }
	VkPipeline getPipeline() { return vkGraphicsPipeline; }

	void createRenderPass();
	void initGraphicsPipeline();
private:
	App& appRef;

	DeviceHelper& deviceHelper;

	VkWrapper<VkRenderPass> vkRenderPass{ deviceHelper.getDeviceWrapper(), vkDestroyRenderPass };
	VkWrapper<VkPipelineLayout> vkPipelineLayout{ deviceHelper.getDeviceWrapper(), vkDestroyPipelineLayout };
	VkWrapper<VkPipeline> vkGraphicsPipeline{ deviceHelper.getDeviceWrapper(), vkDestroyPipeline };
};

