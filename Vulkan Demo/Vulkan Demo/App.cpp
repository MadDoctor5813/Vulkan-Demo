#include "App.h"

#include <iostream>

App::App() : vulkanContext(*this), shaderLoader(*this) {
	
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

	vkDeviceWaitIdle(vulkanContext.getDeviceHelper().getDevice());
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
	vulkanContext.initContext();
	createSemaphores();
}

void App::drawFrame() {
	uint32_t imgIdx;
	vkAcquireNextImageKHR(vulkanContext.getDeviceHelper().getDevice(), vulkanContext.getSwapChain(), std::numeric_limits<int64_t>::max(), imageAvailableSemaphore, VK_NULL_HANDLE, &imgIdx);
	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	VkSemaphore waitSemaphores[] = { imageAvailableSemaphore };
	VkPipelineStageFlags waitFlags[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitFlags;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &vulkanContext.getCommandBuffers()[imgIdx];
	VkSemaphore signalSemaphores[] = { renderFinishedSemaphore };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;
	if (vkQueueSubmit(vulkanContext.getDeviceHelper().graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) {
		throw std::runtime_error("Could not submit command buffer");
	}
	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;
	VkSwapchainKHR swapChain[] = { vulkanContext.getSwapChain() };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChain;
	presentInfo.pImageIndices = &imgIdx;
	vkQueuePresentKHR(vulkanContext.getDeviceHelper().presentQueue, &presentInfo);
}

void App::onWindowResize(GLFWwindow * window, int width, int height) {
	if (width == 0 || height == 0) {
		return;
	}
	App* app = (App*)glfwGetWindowUserPointer(window);
	app->windowWidth = width;
	app->windowHeight = height;
	app->getVulkanContext().recreateSwapchain();
}

void App::createSemaphores() {
	VkSemaphoreCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	if (vkCreateSemaphore(vulkanContext.getDeviceHelper().getDevice(), &createInfo, nullptr, &imageAvailableSemaphore) != VK_SUCCESS ||
		vkCreateSemaphore(vulkanContext.getDeviceHelper().getDevice(), &createInfo, nullptr, &renderFinishedSemaphore) != VK_SUCCESS) {
		throw std::runtime_error("Could not create semaphores.");
	}
}


