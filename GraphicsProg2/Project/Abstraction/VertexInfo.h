#pragma once

#include "glm/glm.hpp"
#include "vulkan/vulkan_core.h"
#include <array>

struct RectangleInfo
{
	RectangleInfo(float t, float l, float b, float r)
		: top{ t }, left{ l }, bot{ b }, right{ r } {}

	float top;
	float left;
	float bot;
	float right;
};
struct OvalInfo
{
	OvalInfo(float xRadius, float yRadius, int _segments)
		: radiusX{ xRadius }, radiusY{ yRadius }, segments{_segments} {}

	float radiusX;
	float radiusY;
	int segments;
};

struct UniformBufferObject
{
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 proj;
};

struct Vertex
{
};

struct Vertex3D : public Vertex
{
	Vertex3D() {}
	Vertex3D(glm::vec3 p, glm::vec2 tex, glm::vec3 norm)
		: pos{ p }, texCoord{ tex }, normal{ norm }
	{}

	glm::vec3 pos;
	glm::vec2 texCoord;
	glm::vec3 normal{};

	static auto GetBindingDescription()
	{
		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(Vertex3D);
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
		attributeDescriptions[0].offset = offsetof(Vertex3D, pos);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex3D, texCoord);

		attributeDescriptions[2].binding = 0;
		attributeDescriptions[2].location = 2;
		attributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[2].offset = offsetof(Vertex3D, normal);
		
		return attributeDescriptions;
	}
};

struct Vertex2D : public Vertex
{
	Vertex2D(glm::vec2 p, glm::vec3 col = {1.f, 1.f, 1.f})
		: pos{ p }, color{ col } {}

	glm::vec2 pos;
	glm::vec3 color;

	static auto GetBindingDescription()
	{
		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(Vertex2D);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescription;
	}

	static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions()
	{
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
		attributeDescriptions.resize(2);

		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex2D, pos);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex2D, color);

		return attributeDescriptions;
	}
};

