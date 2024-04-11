#pragma once

#include "vulkan/vulkan_core.h"
#include "glm/glm.hpp"
#include <vector>
#include <string>

#include "VertexInfo.h"

class Mesh
{
public:
	Mesh(std::string objPath, uint32_t shaderIndex, glm::mat4 modelMatrix);

	void Draw(VkCommandBuffer buffer) const;


	glm::mat4 GetModelMatrix() const { return m_ModelMatrix; }

	const std::vector<Vertex>& GetVertices() { return m_Vertices; };
	const std::vector<uint32_t>& GetIndices() { return m_Indices; };

	uint32_t GetShaderIndex() const { return m_ShaderIndex; }
	uint32_t GetVertexIndexBufferIndex() const { return m_VertexIndexBufferIndex; }

	void SetVertexIndexBufferIndex(uint32_t index) { m_VertexIndexBufferIndex = index; }

private:

	glm::mat4 m_ModelMatrix{ glm::mat4{1.f} };

	std::vector<Vertex> m_Vertices{};
	std::vector<uint32_t> m_Indices{};

	uint32_t m_ShaderIndex{};
	uint32_t m_VertexIndexBufferIndex{};
};