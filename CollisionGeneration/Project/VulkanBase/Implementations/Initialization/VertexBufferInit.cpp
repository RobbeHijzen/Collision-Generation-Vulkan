#include "VulkanBase/VulkanBase.h"

void VulkanBase::CreateVertexBuffers()
{
	m_VertexBuffers.resize(m_Scene->GetRenderablesAmount());
	m_VertexBuffersMemory.resize(m_Scene->GetRenderablesAmount());

	// renderable vertices
	for (auto& renderable : m_Scene->GetRenderables())
	{
		VkBuffer vertexBuffer; 
		VkDeviceMemory vertexBufferMemory;

		CreateVertexBuffer(renderable->GetVertices(), vertexBuffer, vertexBufferMemory);
		
		m_VertexBuffers[renderable->GetRenderID()] = vertexBuffer;
		m_VertexBuffersMemory[renderable->GetRenderID()] = vertexBufferMemory;
	}
}
void VulkanBase::CreateIndexBuffers()
{
	m_IndexBuffers.resize(m_Scene->GetRenderablesAmount());
	m_IndexBuffersMemory.resize(m_Scene->GetRenderablesAmount());

	// renderable indices
	for (auto& renderable : m_Scene->GetRenderables())
	{
		VkBuffer indexBuffer;
		VkDeviceMemory indexBufferMemory;
		CreateIndexBuffer(renderable->GetIndices(), indexBuffer, indexBufferMemory);

		m_IndexBuffers[renderable->GetRenderID()] = indexBuffer;
		m_IndexBuffersMemory[renderable->GetRenderID()] = indexBufferMemory;
	}
}

void VulkanBase::CreateVertexBuffer(std::vector<Vertex> vertices, VkBuffer& vertexBuffer, VkDeviceMemory& vertexBufferMemory)
{
	VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;

	CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(m_Device, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, vertices.data(), (size_t)bufferSize);
	vkUnmapMemory(m_Device, stagingBufferMemory);


	CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		vertexBuffer, vertexBufferMemory);


	CopyBuffer(stagingBuffer, vertexBuffer, bufferSize);

	vkDestroyBuffer(m_Device, stagingBuffer, nullptr);
	vkFreeMemory(m_Device, stagingBufferMemory, nullptr);
}

void VulkanBase::CreateIndexBuffer(std::vector<uint32_t> indices, VkBuffer& indexBuffer, VkDeviceMemory& indexBufferMemory)
{
	VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(m_Device, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, indices.data(), (size_t)bufferSize);
	vkUnmapMemory(m_Device, stagingBufferMemory);

	CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		indexBuffer, indexBufferMemory);

	CopyBuffer(stagingBuffer, indexBuffer, bufferSize);

	vkDestroyBuffer(m_Device, stagingBuffer, nullptr);
	vkFreeMemory(m_Device, stagingBufferMemory, nullptr);
}

