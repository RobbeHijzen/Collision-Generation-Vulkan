#include "VulkanBase/VulkanBase.h"

void VulkanBase::CreateDescriptorSetLayouts()
{
	auto shaders{ ShaderManager::GetInstance().GetShaders() };
	int index{};
	m_DescriptorSetLayouts.resize(shaders.size());

	for (auto& shader : shaders)
	{
		auto bindings{ shader->CreateDescriptorSetLayoutBindings() };

		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
		layoutInfo.pBindings = bindings.data();

		if (vkCreateDescriptorSetLayout(m_Device, &layoutInfo, nullptr, &m_DescriptorSetLayouts[index]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create descriptor set layout!");
		}

		++index;
	}
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
	std::array<VkDescriptorPoolSize, 2> poolSizes{};
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[0].descriptorCount = 1;
	poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[1].descriptorCount = 1;


	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = static_cast<uint32_t>(m_Scene->GetMeshesAmount());

	if (vkCreateDescriptorPool(m_Device, &poolInfo, nullptr, &m_DescriptorPool) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create descriptor pool!");
	}
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
		allocInfo.pSetLayouts = &m_DescriptorSetLayouts[mesh->GetShaderIndex()];

		if (vkAllocateDescriptorSets(m_Device, &allocInfo, &m_MeshDescriptorSets[mesh->GetMeshIndex()]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to allocate descriptor sets!");
		}

		auto shader{ ShaderManager::GetInstance().GetShaders()[mesh->GetShaderIndex()] };
		shader->SetupDescriptorSet(this, mesh);
	}
}

