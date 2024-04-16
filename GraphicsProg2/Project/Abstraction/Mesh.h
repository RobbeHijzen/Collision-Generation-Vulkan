#pragma once

#include "vulkan/vulkan_core.h"
#include "glm/glm.hpp"
#include <vector>
#include <string>

#include "VertexInfo.h"

class Mesh
{
public:
	Mesh(uint32_t shaderIndex, glm::mat4 modelMatrix) : m_ShaderIndex{ shaderIndex }, m_ModelMatrix{modelMatrix} {}

	virtual void Draw(VkCommandBuffer buffer) const = 0;

	glm::mat4 GetModelMatrix() const { return m_ModelMatrix; }

	uint32_t GetShaderIndex() const { return m_ShaderIndex; }
	uint32_t GetMeshIndex() const { return m_MeshIndex; }

	void SetMeshIndex(uint32_t index) { m_MeshIndex = index; }

	const std::vector<Vertex>& GetVertices() const { return m_Vertices; };
	const std::vector<uint32_t>& GetIndices() const { return m_Indices; };

protected:

	std::vector<Vertex> m_Vertices{};
	std::vector<uint32_t> m_Indices{};

private:

	glm::mat4 m_ModelMatrix{ glm::mat4{1.f} };


	uint32_t m_ShaderIndex{};
	uint32_t m_MeshIndex{};
};

class Mesh3D : public Mesh
{
public:
	Mesh3D(std::string objPath, std::string diffuseString, uint32_t shaderIndex, glm::mat4 modelMatrix);

	virtual void Draw(VkCommandBuffer buffer) const override;

	std::string GetDiffuseString() const { return m_DiffuseString; }

	uint32_t GetTextureIndex() const { return m_TextureIndex; }
	void SetTextureIndex(uint32_t textureIndex)  { m_TextureIndex = textureIndex; }

private:
	uint32_t m_TextureIndex{};

	std::string m_DiffuseString{};
};

class Mesh2D : public Mesh
{
public:
	Mesh2D(uint32_t shaderIndex, glm::mat4 modelMatrix, RectangleInfo rec);
	Mesh2D(uint32_t shaderIndex, glm::mat4 modelMatrix, OvalInfo oval);

	virtual void Draw(VkCommandBuffer buffer) const override;

private:
	Mesh2D(uint32_t shaderIndex, glm::mat4 modelMatrix);

	void UseRectangleInfo(RectangleInfo rec);
	void UseOvalInfo(OvalInfo oval);
};