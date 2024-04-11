#include "VulkanBase/VulkanBase.h"

void VulkanBase::DrawScene() 
{
	for (const auto& mesh : m_Meshes)
	{
		mesh.Draw(m_CommandBuffer);
	}
}