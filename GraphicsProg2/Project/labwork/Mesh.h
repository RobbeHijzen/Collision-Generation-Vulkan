#pragma once

#include "vulkan/vulkan_core.h"
#include "glm/glm.hpp"
#include <array>
#include <vector>

struct UniformBufferObject 
{
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 proj;
};


struct Vertex
{
	glm::vec3 pos;
	glm::vec3 color;

	static auto GetBindingDescription()
	{
		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(Vertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescription;
	}

	static auto GetAttributeDescriptions()
	{
		std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, pos);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, color);

		return attributeDescriptions;
	}
};

class Mesh
{
public:
	Mesh();

	void Draw(VkCommandBuffer buffer) const;

	const std::vector<Vertex>& GetVertices() { return m_Vertices; };
	const std::vector<uint32_t>& GetIndices() { return m_Indices; };

private:

	std::vector<Vertex> m_Vertices
	{
	{{-0.5f, -0.5f, 0.f}, {1.0f, 0.0f, 0.0f}},
	{{0.5f, -0.5f, 0.f}, {0.0f, 1.0f, 0.0f}},
	{{0.5f, 0.5f, 0.f}, {0.0f, 0.0f, 1.0f}},
	{{-0.5f, 0.5f, 0.f}, {1.0f, 1.0f, 1.0f}}
	};

	std::vector<uint32_t> m_Indices { 0, 1, 2, 2, 3, 0 };

};