#pragma once

#include "VulkanUtil/VulkanUtil.h"
#include <glm/gtc/matrix_transform.hpp>
#include <optional>

struct QueueFamilyIndices
{
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	bool IsComplete() const
	{
		return graphicsFamily.has_value() && presentFamily.has_value();
	}
};

struct SwapChainSupportDetails
{
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

struct UniformBufferObject
{
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 proj;
};

struct AABB
{
	glm::vec3 min;
	glm::vec3 max;

	bool operator==(const AABB& other) const
	{
		return min == other.min && max == other.max;
	}
};
