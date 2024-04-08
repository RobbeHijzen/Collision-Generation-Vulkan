#include "Mesh.h"
#include "Utils.h"

Mesh::Mesh()
{
	bool succeeded = Utils::ParseOBJ("C:/.School/.Howest/2DAE/Semester 2/GraphicsProg2/2DAE-GraphicsProgramming2/GraphicsProg2/Project/resources/lowpoly_bunny.obj", m_Vertices, m_Indices);
	//bool succeeded = Utils::ParseOBJ("resources/lowpoly_bunny.obj", m_Vertices, m_Indices);
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
