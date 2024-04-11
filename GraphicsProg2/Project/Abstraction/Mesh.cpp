#include "Mesh.h"
#include "Utils.h"

#include "Shaders/ShaderManager.h"

Mesh::Mesh(std::string objPath, uint32_t shaderIndex, glm::mat4 modelMatrix)
	: m_ShaderIndex{shaderIndex}
	, m_ModelMatrix{modelMatrix}
{
	//bool succeeded = Utils::ParseOBJ("Resources/lowpoly_bunny.obj", m_Vertices, m_Indices);
	bool succeeded = Utils::ParseOBJ(objPath, m_Vertices, m_Indices);
	assert(succeeded);

	for (auto& vertex : m_Vertices)
	{
		vertex.color = { 1.f, 1.f, 1.f };
	}

}

void Mesh::Draw(VkCommandBuffer buffer) const
{
	vkCmdDrawIndexed(buffer, static_cast<uint32_t>(m_Indices.size()), 1, 0, 0, 0);
}