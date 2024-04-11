#pragma once

#include "vulkan/vulkan_core.h"
#include "glm/glm.hpp"
#include <vector>
#include <string>

#include "VertexInfo.h"
#include "Shaders/Shader.h"

class Mesh
{
public:
	Mesh(std::string objPath, Shader* shader);

	void Draw(VkCommandBuffer buffer) const;

	const std::vector<Vertex>& GetVertices() { return m_Vertices; };
	const std::vector<uint32_t>& GetIndices() { return m_Indices; };

private:

	std::vector<Vertex> m_Vertices{};
	std::vector<uint32_t> m_Indices{};

	uint32_t m_ShaderIndex{};
};