#pragma once

#include <glm\glm.hpp>
#include <vulkan\vulkan.h>
#include <array>

struct Vertex {

	glm::vec3 position;
	glm::vec4 color;
	glm::vec3 normal;
	glm::vec2 uv;

	static VkVertexInputBindingDescription getBindingDescription() {
		VkVertexInputBindingDescription desc;
		desc.stride = sizeof(Vertex);
		desc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		desc.binding = 0;
		return desc;
	}

	static std::array<VkVertexInputAttributeDescription, 4> getAttributeDescriptions() {
		std::array<VkVertexInputAttributeDescription, 4> descriptions;
		//pos description
		descriptions[0].binding = 0;
		descriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		descriptions[0].location = 0;
		descriptions[0].offset = offsetof(Vertex, position);
		//color description
		descriptions[1].binding = 0;
		descriptions[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
		descriptions[1].location = 1;
		descriptions[1].offset = offsetof(Vertex, color);
		//normal description
		descriptions[2].binding = 0;
		descriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
		descriptions[2].location = 2;
		descriptions[2].offset = offsetof(Vertex, normal);
		//uv description
		descriptions[3].binding = 0;
		descriptions[3].format = VK_FORMAT_R32G32_SFLOAT;
		descriptions[3].location = 3;
		descriptions[3].offset = offsetof(Vertex, uv);
		return descriptions;
	}

};