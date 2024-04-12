#include "Mesh.h"
#include "Utils.h"

#include "Shaders/ShaderManager.h"

Mesh::Mesh(std::string objPath, std::string diffuseString, uint32_t shaderIndex, glm::mat4 modelMatrix)
	: m_ShaderIndex{shaderIndex}
	, m_ModelMatrix{modelMatrix}
	, m_DiffuseString{diffuseString}
{
	bool succeeded = Utils::ParseOBJ(objPath, m_Vertices, m_Indices);
	assert(succeeded);

}

void Mesh::Draw(VkCommandBuffer buffer) const
{
	vkCmdDrawIndexed(buffer, static_cast<uint32_t>(m_Indices.size()), 1, 0, 0, 0);
}