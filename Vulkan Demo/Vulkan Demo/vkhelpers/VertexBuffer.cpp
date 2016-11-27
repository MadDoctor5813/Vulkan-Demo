#include "VertexBuffer.h"

VertexBuffer::VertexBuffer(DeviceHelper& deviceHelper) : deviceHelper(deviceHelper)
{
}


VertexBuffer::~VertexBuffer()
{
}

void VertexBuffer::uploadData(std::vector<Vertex>& vertexData) {
	//create the buffer
	VkBufferCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	createInfo.size = sizeof(Vertex) * vertexData.size();
	createInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	if (vkCreateBuffer(deviceHelper.getDevice(), &createInfo, nullptr, buffer.replace()) != VK_SUCCESS) {
		throw std::runtime_error("Could not create buffer.");
	}
	VkMemoryRequirements memReqs = {};
	vkGetBufferMemoryRequirements(deviceHelper.getDevice(), buffer, &memReqs);
	//allocate the memory
	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memReqs.size;
	allocInfo.memoryTypeIndex = deviceHelper.findMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	if (vkAllocateMemory(deviceHelper.getDevice(), &allocInfo, nullptr, bufferMemory.replace()) != VK_SUCCESS) {
		throw std::runtime_error("Could not allocate buffer memory.");
	}
	vkBindBufferMemory(deviceHelper.getDevice(), buffer, bufferMemory, 0);
	//upload the data
	void* dataPtr;
	vkMapMemory(deviceHelper.getDevice(), bufferMemory, 0, createInfo.size, 0, &dataPtr);
	memcpy(dataPtr, vertexData.data(), createInfo.size);
	vkUnmapMemory(deviceHelper.getDevice(), bufferMemory);

}
