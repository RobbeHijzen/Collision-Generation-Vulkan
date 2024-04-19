#include "VulkanBase/VulkanBase.h"

void VulkanBase::CreateSyncObjects() 
{
	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	if (vkCreateSemaphore(m_Device, &semaphoreInfo, nullptr, &m_ImageAvailableSemaphore) != VK_SUCCESS
		|| vkCreateSemaphore(m_Device, &semaphoreInfo, nullptr, &m_RenderFinishedSemaphore) != VK_SUCCESS
		|| vkCreateFence(m_Device, &fenceInfo, nullptr, &m_InFlightFence) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create synchronization objects for a frame!");
	}	

}

