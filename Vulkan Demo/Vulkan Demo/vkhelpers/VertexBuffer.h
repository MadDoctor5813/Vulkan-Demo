#pragma once

#include <vulkan\vulkan.h>
#include <vector>

#include "graphics\Vertex.h"
#include "vkhelpers\VkWrapper.h"
#include "vkhelpers\DeviceHelper.h"

class VertexBuffer
{
public:
	VertexBuffer(DeviceHelper& deviceHelper);
	~VertexBuffer();

	VkBuffer getBuffer() { return buffer; }

	void uploadData(std::vector<Vertex>& data);
	
private: 
	DeviceHelper& deviceHelper;
	VkWrapper<VkBuffer> buffer{ deviceHelper.getDeviceWrapper(), vkDestroyBuffer };
	VkWrapper<VkDeviceMemory> bufferMemory{ deviceHelper.getDeviceWrapper(), vkFreeMemory };
};

