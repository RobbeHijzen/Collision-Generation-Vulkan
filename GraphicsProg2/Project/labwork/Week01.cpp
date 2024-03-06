#include "vulkanbase/VulkanBase.h"

void VulkanBase::InitializeWindow()
{
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
}

void VulkanBase::DrawScene() 
{
	vkCmdDraw(commandBuffer, 6, 1, 0, 0);
}