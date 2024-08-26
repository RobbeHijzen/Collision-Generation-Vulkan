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
	m_UniformBuffersMapped.resize(m_Scene->GetRenderablesAmount());
	m_UniformBuffers.resize(m_Scene->GetRenderablesAmount());
	m_UniformBuffersMemory.resize(m_Scene->GetRenderablesAmount());

	VkDeviceSize bufferSize = sizeof(UniformBufferObject);

	for (auto renderable : m_Scene->GetRenderables())
	{
		if (!renderable->IsInstanceable())
		{
			m_UniformBuffersMapped[renderable->GetRenderID()].resize(1);
			m_UniformBuffers[renderable->GetRenderID()].resize(1);
			m_UniformBuffersMemory[renderable->GetRenderID()].resize(1);

			CreateBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_UniformBuffers[renderable->GetRenderID()][0], m_UniformBuffersMemory[renderable->GetRenderID()][0]);
			vkMapMemory(m_Device, m_UniformBuffersMemory[renderable->GetRenderID()][0], 0, bufferSize, 0, &m_UniformBuffersMapped[renderable->GetRenderID()][0]);
		}
		else
		{
			auto modelMatrices{ renderable->GetModelMatrices() };

			m_UniformBuffersMapped[renderable->GetRenderID()].resize(modelMatrices.size());
			m_UniformBuffers[renderable->GetRenderID()].resize(modelMatrices.size());
			m_UniformBuffersMemory[renderable->GetRenderID()].resize(modelMatrices.size());

			int instanceID{};
			for (const auto& modelMatrix : modelMatrices)
			{
				CreateBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_UniformBuffers[renderable->GetRenderID()][instanceID], m_UniformBuffersMemory[renderable->GetRenderID()][instanceID]);
				vkMapMemory(m_Device, m_UniformBuffersMemory[renderable->GetRenderID()][instanceID], 0, bufferSize, 0, &m_UniformBuffersMapped[renderable->GetRenderID()][instanceID]);

				++instanceID;
			}
		}
	}
}

static uint32_t GetDrawCount(Scene* scene)
{
	uint32_t drawCount{};
	for (auto mesh : scene->GetObjects())
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
	m_MeshDescriptorSets.resize(m_Scene->GetRenderablesAmount());
	
	for (const auto& renderable : m_Scene->GetRenderables())
	{
		if (!renderable->IsInstanceable())
		{
			m_MeshDescriptorSets[renderable->GetRenderID()].resize(1);

			VkDescriptorSetAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			allocInfo.descriptorPool = m_DescriptorPool;
			allocInfo.descriptorSetCount = 1;
			allocInfo.pSetLayouts = &m_DescriptorSetLayout;

			if (vkAllocateDescriptorSets(m_Device, &allocInfo, &m_MeshDescriptorSets[renderable->GetRenderID()][0]) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to allocate descriptor sets!");
			}

			m_Shader3D->SetupDescriptorSet(this, renderable, 0);
		}
		else
		{
			auto modelMatrices{ renderable->GetModelMatrices() };
			m_MeshDescriptorSets[renderable->GetRenderID()].resize(modelMatrices.size());

			int instanceID{};
			for (const auto& modelMatrix : modelMatrices)
			{				
				VkDescriptorSetAllocateInfo allocInfo{};
				allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
				allocInfo.descriptorPool = m_DescriptorPool;
				allocInfo.descriptorSetCount = 1;
				allocInfo.pSetLayouts = &m_DescriptorSetLayout;
				
				if (vkAllocateDescriptorSets(m_Device, &allocInfo, &m_MeshDescriptorSets[renderable->GetRenderID()][instanceID]) != VK_SUCCESS)
				{
					throw std::runtime_error("failed to allocate descriptor sets!");
				}
				
				m_Shader3D->SetupDescriptorSet(this, renderable, instanceID);

				++instanceID;
			}
		}
	}
}

