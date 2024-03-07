#include "Mesh.h"

void Mesh::Draw(VkCommandBuffer buffer)
{
	vkCmdDraw(buffer, static_cast<uint32_t>(m_Vertices.size()), 1, 0, 0);
}
