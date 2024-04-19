#include "VulkanBase/VulkanBase.h"

void VulkanBase::CreateVertexBuffers()
{
	m_VertexBuffers.resize(m_Scene->GetMeshesAmount());
	m_VertexBuffersMemory.resize(m_Scene->GetMeshesAmount());

	for (auto& mesh : m_Scene->GetMeshes())
	{
		VkBuffer vertexBuffer; 
		VkDeviceMemory vertexBufferMemory;

		CreateVertexBuffer(mesh->GetVertices(), vertexBuffer, vertexBufferMemory);
		
		m_VertexBuffers[mesh->GetMeshIndex()] = vertexBuffer;
		m_VertexBuffersMemory[mesh->GetMeshIndex()] = vertexBufferMemory;
	}
}
void VulkanBase::CreateIndexBuffers()
{
	m_IndexBuffers.resize(m_Scene->GetMeshesAmount());
	m_IndexBuffersMemory.resize(m_Scene->GetMeshesAmount());

	for (auto& mesh : m_Scene->GetMeshes())
	{
		VkBuffer indexBuffer;
		VkDeviceMemory indexBufferMemory;
		CreateIndexBuffer(mesh->GetIndices(), indexBuffer, indexBufferMemory);

		m_IndexBuffers[mesh->GetMeshIndex()] = indexBuffer;
		m_IndexBuffersMemory[mesh->GetMeshIndex()] = indexBufferMemory;
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

