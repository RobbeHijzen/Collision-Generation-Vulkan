#include "Mesh.h"
#include "Utils.h"

#include "Shaders/ShaderManager.h"

Mesh::Mesh(std::string objPath, Shader* shader)
{
	//bool succeeded = Utils::ParseOBJ("C:/.School/.Howest/2DAE/Semester 2/GraphicsProg2/2DAE-GraphicsProgramming2/GraphicsProg2/Project/resources/lowpoly_bunny.obj", m_Vertices, m_Indices);
	//bool succeeded = Utils::ParseOBJ("Resources/lowpoly_bunny.obj", m_Vertices, m_Indices);
	bool succeeded = Utils::ParseOBJ(objPath, m_Vertices, m_Indices);
	assert(succeeded);

	for (auto& vertex : m_Vertices)
	{
		vertex.color = { 1.f, 1.f, 1.f };
	}

	//m_ShaderIndex = ShaderManager::GetInstance().AddShader(shader);
}

void Mesh::Draw(VkCommandBuffer buffer) const
{
	vkCmdDrawIndexed(buffer, static_cast<uint32_t>(m_Indices.size()), 1, 0, 0, 0);
}