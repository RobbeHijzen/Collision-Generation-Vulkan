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
	Mesh(uint32_t shaderIndex, glm::mat4 modelMatrix) : m_ShaderIndex{ shaderIndex }, m_ModelMatrix{ modelMatrix } {}

	virtual void Draw(VkCommandBuffer buffer) const = 0;

	glm::mat4 GetModelMatrix() const { return m_ModelMatrix; }

	uint32_t GetShaderIndex() const { return m_ShaderIndex; }
	uint32_t GetMeshIndex() const { return m_MeshIndex.value(); }

	void SetMeshIndex(uint32_t index);

	//const std::vector<Vertex>& GetVertices() const { return m_Vertices; };
	const std::vector<uint32_t>& GetIndices() const { return m_Indices; };

protected:

	std::vector<Vertex> m_Vertices{};
	std::vector<uint32_t> m_Indices{};

private:

	glm::mat4 m_ModelMatrix{ glm::mat4{1.f} };

	uint32_t m_ShaderIndex{};
	std::optional<uint32_t> m_MeshIndex{};
};

class Mesh3D : public Mesh
{
public:
	Mesh3D(std::string objPath, std::string diffuseString, uint32_t shaderIndex, glm::mat4 modelMatrix);

	virtual void Draw(VkCommandBuffer buffer) const override;

	std::string GetDiffuseString() const { return m_DiffuseString; }

	uint32_t GetTextureIndex() const { return m_TextureIndex; }
	void SetTextureIndex(uint32_t textureIndex) { m_TextureIndex = textureIndex; }

	auto Get3DVertices() const { return m_3DVertices; }

private:
	uint32_t m_TextureIndex{};

	std::string m_DiffuseString{};

	std::vector<Vertex3D> m_3DVertices{};

};

class Mesh2D : public Mesh
{
public:
	Mesh2D(uint32_t shaderIndex, glm::mat4 modelMatrix, RectangleInfo rec, glm::vec3 color = {1.f, 1.f, 1.f});
	Mesh2D(uint32_t shaderIndex, glm::mat4 modelMatrix, OvalInfo oval, glm::vec3 color = { 1.f, 1.f, 1.f });

	virtual void Draw(VkCommandBuffer buffer) const override;

	auto Get2DVertices() const { return m_2DVertices; }

private:
	Mesh2D(uint32_t shaderIndex, glm::mat4 modelMatrix);

	void UseRectangleInfo(RectangleInfo rec);
	void UseOvalInfo(OvalInfo oval);

	std::vector<Vertex2D> m_2DVertices{};
};