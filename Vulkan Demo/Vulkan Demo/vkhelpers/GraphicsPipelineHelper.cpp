#include "GraphicsPipelineHelper.h"

#include "App.h"


GraphicsPipelineHelper::GraphicsPipelineHelper(App& app, DeviceHelper& deviceHelper) : appRef(app), deviceHelper(deviceHelper) {

}


GraphicsPipelineHelper::~GraphicsPipelineHelper() {
}

void GraphicsPipelineHelper::initGraphicsPipeline() {
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
	viewport.width = appRef.getSwapExtent().width;
	viewport.height = appRef.getSwapExtent().height;
	viewport.minDepth = 0;
	viewport.maxDepth = 1.0f;
	VkRect2D scissor = {};
	scissor.offset = { 0, 0 };
	scissor.extent = appRef.getSwapExtent();
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
	if (vkCreatePipelineLayout(appRef.getDeviceHelper().getDevice(), &layoutInfo, nullptr, &vkPipelineLayout) != VK_SUCCESS) {
		throw std::runtime_error("Could not create pipeline layout.");
	}
	VkPipelineShaderStageCreateInfo shaderInfos[] = { appRef.getShaderLoader().getShader("shaders/phongshading.vert.spv").info, appRef.getShaderLoader().getShader("shaders/phongshading.frag.spv").info };
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
	if (vkCreateGraphicsPipelines(appRef.getDeviceHelper().getDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &vkGraphicsPipeline) != VK_SUCCESS) {
		throw std::runtime_error("Could not create graphics pipeline.");
	}
}

void GraphicsPipelineHelper::createRenderPass() {
	VkAttachmentDescription attachment = {};
	attachment.format = appRef.getSwapFormat();
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
	if (vkCreateRenderPass(appRef.getDeviceHelper().getDevice(), &passInfo, nullptr, &vkRenderPass) != VK_SUCCESS) {
		throw std::runtime_error("Could not create render pass.");
	}
}
