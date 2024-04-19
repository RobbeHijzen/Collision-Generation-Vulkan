#pragma once

#include "vulkan/vulkan_core.h"
#include "glm/glm.hpp"
#include <vector>
#include <string>
#include <optional>

#include "VertexInfo.h"

class Mesh
{
public:
	Mesh(std::string objPath, 
			  std::string diffuseString, 
			  glm::vec3 translation = glm::vec3{0.f, 0.f, 0.f},
			  glm::vec3 rotation = glm::vec3{0.f, 0.f, 0.f},
			  glm::vec3 scale = glm::vec3{1.f, 1.f, 1.f});

	void Draw(VkCommandBuffer buffer) const;

	glm::mat4 GetModelMatrix() const { return m_ModelMatrix; }


	const std::vector<uint32_t>& GetIndices() const { return m_Indices; };
	const std::vector<Vertex>& GetVertices() const { return m_Vertices; }

	uint32_t GetMeshIndex() const { return m_MeshIndex.value(); }
	std::string GetDiffuseString() const { return m_DiffuseString; }

	void SetMeshIndex(uint32_t index) { m_MeshIndex = index; }

private:

	std::vector<uint32_t> m_Indices{};
	std::vector<Vertex> m_Vertices{};

	glm::mat4 m_ModelMatrix{ glm::mat4{1.f} };

	std::optional<uint32_t> m_MeshIndex{};

	std::string m_DiffuseString{};
};