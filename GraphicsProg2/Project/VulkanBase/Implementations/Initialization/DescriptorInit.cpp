#include "VulkanBase/VulkanBase.h"

void VulkanBase::CreateDescriptorSetLayouts()
{
	auto bindings{ m_Shader3D->CreateDescriptorSetLayoutBindings() };

	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	layoutInfo.pBindings = bindings.data();

	if (vkCreateDescriptorSetLayout(m_Device, &layoutInfo, nullptr, &m_DescriptorSetLayout) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create descriptor set layout!");
	}
}

void VulkanBase::CreateUnfiformBuffers()
{
	m_UniformBuffersMapped.resize(m_Scene->GetMeshesAmount());
	m_UniformBuffers.resize(m_Scene->GetMeshesAmount());
	m_UniformBuffersMemory.resize(m_Scene->GetMeshesAmount());

	VkDeviceSize bufferSize = sizeof(UniformBufferObject);

	for (auto mesh : m_Scene->GetMeshes())
	{
		m_UniformBuffersMapped[mesh->GetMeshIndex()].emplace_back();
		m_UniformBuffers[mesh->GetMeshIndex()].emplace_back();
		m_UniformBuffersMemory[mesh->GetMeshIndex()].emplace_back();

		CreateBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_UniformBuffers[mesh->GetMeshIndex()][0], m_UniformBuffersMemory[mesh->GetMeshIndex()][0]);
		vkMapMemory(m_Device, m_UniformBuffersMemory[mesh->GetMeshIndex()][0], 0, bufferSize, 0, &m_UniformBuffersMapped[mesh->GetMeshIndex()][0]);
	
		if (auto col = mesh->GetComponent<CollisionComponent>())
		{
			auto AABBsize{ col->GetAABBs().size()};
			for (int index{1}; index <= AABBsize; ++index)
			{
				m_UniformBuffersMapped[mesh->GetMeshIndex()].emplace_back();
				m_UniformBuffers[mesh->GetMeshIndex()].emplace_back();
				m_UniformBuffersMemory[mesh->GetMeshIndex()].emplace_back();

				CreateBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_UniformBuffers[mesh->GetMeshIndex()][index], m_UniformBuffersMemory[mesh->GetMeshIndex()][index]);
				vkMapMemory(m_Device, m_UniformBuffersMemory[mesh->GetMeshIndex()][index], 0, bufferSize, 0, &m_UniformBuffersMapped[mesh->GetMeshIndex()][index]);
			}
		}
	}
}

static uint32_t GetDrawCount(Scene* scene)
{
	uint32_t drawCount{};
	for (auto mesh : scene->GetMeshes())
	{
		++drawCount;
		if (auto col = mesh->GetComponent<CollisionComponent>())
		{
			for (int index{}; index < col->GetAABBs().size(); ++index)
			{
				++drawCount;
			}
		}
	}
	return drawCount;
}

void VulkanBase::CreateDescriptorPool()
{
	uint32_t drawCount{ GetDrawCount(m_Scene) };

	std::array<VkDescriptorPoolSize, 2> poolSizes{};
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[0].descriptorCount = drawCount;
	poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[1].descriptorCount = drawCount;


	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = drawCount;

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
		m_MeshDescriptorSets[mesh->GetMeshIndex()].emplace_back();

		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = m_DescriptorPool;
		allocInfo.descriptorSetCount = 1;
		allocInfo.pSetLayouts = &m_DescriptorSetLayout;

		if (vkAllocateDescriptorSets(m_Device, &allocInfo, &m_MeshDescriptorSets[mesh->GetMeshIndex()][0]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to allocate descriptor sets!");
		}

		m_Shader3D->SetupDescriptorSet(this, mesh, 0);

		if (auto col = mesh->GetComponent<CollisionComponent>())
		{
			auto AABBsize{ col->GetAABBs().size() };
			for (int index{1}; index <= AABBsize; ++index)
			{
				m_MeshDescriptorSets[mesh->GetMeshIndex()].emplace_back();

				VkDescriptorSetAllocateInfo allocInfo{};
				allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
				allocInfo.descriptorPool = m_DescriptorPool;
				allocInfo.descriptorSetCount = 1;
				allocInfo.pSetLayouts = &m_DescriptorSetLayout;
				
				if (vkAllocateDescriptorSets(m_Device, &allocInfo, &m_MeshDescriptorSets[mesh->GetMeshIndex()][index]) != VK_SUCCESS)
				{
					throw std::runtime_error("failed to allocate descriptor sets!");
				}

				m_Shader3D->SetupDescriptorSet(this, mesh, index);
			}
		}
	}
}

