#include "VulkanBase/VulkanBase.h"


void VulkanBase::CreateDescriptorSetLayouts()
{
	// TLAS
	m_DescriptorSetLayoutBindings.addBinding(0, VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR, 1,
		VK_SHADER_STAGE_RAYGEN_BIT_KHR); 

	// Output image
	m_DescriptorSetLayoutBindings.addBinding(1, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1,
		VK_SHADER_STAGE_RAYGEN_BIT_KHR); 

	// Camera matrices
	m_DescriptorSetLayoutBindings.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1,
		VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_RAYGEN_BIT_KHR);
	// Obj descriptions
	m_DescriptorSetLayoutBindings.addBinding(1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1,
		VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR);
	// Textures
	m_DescriptorSetLayoutBindings.addBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, nbTxt,
		VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR);

	m_DescriptorSetLayout = m_DescriptorSetLayoutBindings.createLayout(m_Device);
}

void VulkanBase::CreateUnfiformBuffers()
{
	m_UniformBuffersMapped.resize(m_Scene->GetMeshesAmount());
	m_UniformBuffers.resize(m_Scene->GetMeshesAmount());
	m_UniformBuffersMemory.resize(m_Scene->GetMeshesAmount());

	VkDeviceSize bufferSize = sizeof(UniformBufferObject);

	for (size_t i = 0; i < m_Scene->GetMeshesAmount(); ++i)
	{
		CreateBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_UniformBuffers[i], m_UniformBuffersMemory[i]);
		vkMapMemory(m_Device, m_UniformBuffersMemory[i], 0, bufferSize, 0, &m_UniformBuffersMapped[i]);
	}
}

void VulkanBase::CreateDescriptorPool()
{
	m_DescriptorPool = m_DescriptorSetLayoutBindings.createPool(m_Device);
}

void VulkanBase::CreateDescriptorSets()
{
	m_MeshDescriptorSets.resize(m_Scene->GetMeshesAmount());
	
	for (const auto& mesh : m_Scene->GetMeshes())
	{
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = m_DescriptorPool;
		allocInfo.descriptorSetCount = 1;
		allocInfo.pSetLayouts = &m_DescriptorSetLayout;

		if (vkAllocateDescriptorSets(m_Device, &allocInfo, &m_MeshDescriptorSets[mesh->GetMeshIndex()]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to allocate descriptor sets!");
		}

		m_Shader3D->SetupDescriptorSet(this, mesh);
	}
}

