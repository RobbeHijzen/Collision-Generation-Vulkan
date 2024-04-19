#include "VulkanBase/VulkanBase.h"

void VulkanBase::InitializeRayTracing()
{
	VkPhysicalDeviceProperties2 properties2{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2 };
	properties2.pNext = &m_RTProperties;
	vkGetPhysicalDeviceProperties2(m_PhysicalDevice, &properties2);

}