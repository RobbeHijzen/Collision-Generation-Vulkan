#include "vulkanbase/VulkanBase.h"

void VulkanBase::InitializeWindow()
{
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	m_Window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
}

void VulkanBase::DrawScene() 
{
	vkCmdDraw(m_CommandBuffer, 6, 1, 0, 0);
}