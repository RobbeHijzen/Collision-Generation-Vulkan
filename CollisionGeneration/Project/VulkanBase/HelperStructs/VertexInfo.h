#pragma once

#include "glm/glm.hpp"
#include "vulkan/vulkan_core.h"
#include <array>
#include <vector>

struct Vertex
{
	Vertex() {}
	Vertex(glm::vec3 p, glm::vec2 tex = {}, glm::vec3 norm = {0.f, 1.f, 0.f})
		: pos{ p }, texCoord{ tex }, normal{ norm }
	{}

	glm::vec3 pos{};
	glm::vec2 texCoord{};
	glm::vec3 normal{};

	static auto GetBindingDescription()
	{
		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(Vertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescription;
	}

	static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions()
	{
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
		attributeDescriptions.resize(3);

		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, pos);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, texCoord);

		attributeDescriptions[2].binding = 0;
		attributeDescriptions[2].location = 2;
		attributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[2].offset = offsetof(Vertex, normal);
		
		return attributeDescriptions;
	}

	bool operator==(Vertex other)
	{
		return pos == other.pos;
	}
	bool operator<(Vertex other)
	{
		return pos.length() < other.pos.length();
	}
};
