#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include "VulkanUtil/VulkanUtil.h"
#include "Abstraction/Camera.h"
#include "Abstraction/VertexInfo.h"
#include "Abstraction/Scene/Scenemanager.h"
#include "Abstraction/Shaders/Shader.h"

#include "Abstraction/Shaders/ShaderManager.h"
#include "Abstraction/Shaders/DerivedShaders/MachineShader.h"

#include <iostream>
#include <stdexcept>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cstdint>
#include <optional>
#include <set>
#include <limits>
#include <algorithm>
#include <chrono>
#include <glm/gtc/matrix_transform.hpp>

const std::vector<const char*> validationLayers{ "VK_LAYER_KHRONOS_validation" };
const std::vector<const char*> deviceExtensions{ VK_KHR_SWAPCHAIN_EXTENSION_NAME };

struct QueueFamilyIndices 
{
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	bool IsComplete() const
	{
		return graphicsFamily.has_value() && presentFamily.has_value();
	}
};

struct SwapChainSupportDetails 
{
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

//void MyLoad()
//{
//	auto scene{ SceneManager::GetInstance().CreateScene() };
//
//	
//}


class VulkanBase 
{
public:
	void Run() 
	{
		InitializeWindow();
		InitializeVulkan();
		MainLoop();
		Cleanup();
	}

private:

	std::vector<Mesh> m_Meshes{ Mesh{"Resources/lowpoly_bunny.obj", nullptr} };

	void InitializeWindow()
	{
		glfwInit();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		m_Window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
	}
	void InitializeVulkan()
	{
		// Instance and Validation Layer setup

		CreateInstance();
		SetupDebugMessenger();

		// Surface setup
		CreateSurface();

		// Physical device, Logical device and queue Families setup
		PickPhysicalDevice();
		CreateLogicalDevice();

		// SwapChain setup
		CreateSwapChain();
		CreateImageViews();

		// GraphicsPipeline setup
		m_MachineShader.Initialize(m_Device);
		CreateRenderPass();
		CreateDescriptorSetLayout();
		CreateGraphicsPipelines();
		CreateFrameBuffers();

		// week 02 (and a bit of Week 3 (Index Buffer))
		CreateCommandPool();
		CreateVertexBuffer();
		CreateIndexBuffer();
		CreateUnfiformBuffers();
		CreateDescriptorPool();
		CreateDescriptorSets();

		CreateCommandBuffers();

		// Semaphore and Fence setup
		CreateSyncObjects();
	}
	void MainLoop()
	{
		static auto startTime = std::chrono::high_resolution_clock::now();

		while (!glfwWindowShouldClose(m_Window))
		{
			glfwPollEvents();

			DrawFrame();

			auto currentTime = std::chrono::high_resolution_clock::now();
			m_DeltaTime = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
			startTime = currentTime;
		}
		vkDeviceWaitIdle(m_Device);
	}
	void Cleanup()
	{
		// SwapChain cleanup
		for (auto framebuffer : m_SwapChainFramebuffers)
		{
			vkDestroyFramebuffer(m_Device, framebuffer, nullptr);
		}
		for (auto imageView : m_SwapChainImageViews)
		{
			vkDestroyImageView(m_Device, imageView, nullptr);
		}
		vkDestroySwapchainKHR(m_Device, m_SwapChain, nullptr);

		// Pipeline cleanup
		vkDestroyPipeline(m_Device, m_GraphicsPipeline, nullptr);
		vkDestroyPipelineLayout(m_Device, m_PipelineLayout, nullptr);
		vkDestroyRenderPass(m_Device, m_RenderPass, nullptr);

		// Uniform buffer cleanup
		
		vkDestroyBuffer(m_Device, m_UniformBuffer, nullptr);
		vkFreeMemory(m_Device, m_UniformBufferMemory, nullptr);


		vkDestroyDescriptorPool(m_Device, m_DescriptorPool, nullptr);
		vkDestroyDescriptorSetLayout(m_Device, m_DescriptorSetLayout, nullptr);

		// Destroy Vertex and Index buffers
		vkDestroyBuffer(m_Device, m_IndexBuffer, nullptr);
		vkFreeMemory(m_Device, m_IndexBufferMemory, nullptr);

		vkDestroyBuffer(m_Device, m_VertexBuffer, nullptr);
		vkFreeMemory(m_Device, m_VertexBufferMemory, nullptr);

		// Semaphore and Fence cleanup
		
		vkDestroySemaphore(m_Device, m_RenderFinishedSemaphore, nullptr);
		vkDestroySemaphore(m_Device, m_ImageAvailableSemaphore, nullptr);
		vkDestroyFence(m_Device, m_InFlightFence, nullptr);
		
		// CommandPool cleanup
		vkDestroyCommandPool(m_Device, m_CommandPool, nullptr);

		// Logical device and Surface cleanup
		vkDestroyDevice(m_Device, nullptr);
		vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);

		// Validation layers and Instance cleanup
		if (enableValidationLayers) DestroyDebugUtilsMessengerEXT(m_Instance, m_DebugMessenger, nullptr);
		vkDestroyInstance(m_Instance, nullptr);

		// GLFW Window cleanup
		glfwDestroyWindow(m_Window);
		glfwTerminate();
	}


	// Window / Surface setup

	GLFWwindow* m_Window;
	VkSurfaceKHR m_Surface;

	void CreateSurface()
	{
		if (glfwCreateWindowSurface(m_Instance, m_Window, nullptr, &m_Surface) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create window surface!");
		}
	}

	// Instance and Validation Layer setup

	VkInstance m_Instance;
	VkDebugUtilsMessengerEXT m_DebugMessenger;

	void CreateInstance();
	std::vector<const char*> GetRequiredExtensions();
	void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

	void SetupDebugMessenger();


	// Physical Device setup

	VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;

	void PickPhysicalDevice();
	bool IsDeviceSuitable(VkPhysicalDevice device);

	bool CheckDeviceExtensionSupport(VkPhysicalDevice m_Device);

	
	// Logical device and queue Families setup

	VkDevice m_Device = VK_NULL_HANDLE;
	VkQueue m_GraphicsQueue;
	VkQueue m_PresentQueue;

	void CreateLogicalDevice();

	// Swap Chain setup

	VkSwapchainKHR m_SwapChain;
	std::vector<VkImage> m_SwapChainImages;
	VkFormat m_SwapChainImageFormat;
	VkExtent2D m_SwapChainExtent;

	std::vector<VkImageView> m_SwapChainImageViews;
	std::vector<VkFramebuffer> m_SwapChainFramebuffers;

	SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice m_Device);

	VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

	void CreateSwapChain();
	void CreateImageViews();
	void CreateFrameBuffers();

	// GraphicsPipeline setup

	VkPipeline m_GraphicsPipeline;
	VkPipelineLayout m_PipelineLayout;
	VkDescriptorSetLayout m_DescriptorSetLayout;
	VkRenderPass m_RenderPass;

	void CreateGraphicsPipelines();
	void CreateDescriptorSetLayout();
	void CreateRenderPass();

	// Command Buffer setup

	VkCommandPool m_CommandPool;
	VkCommandBuffer m_CommandBuffer;

	void CreateCommandPool();
	void CreateCommandBuffers();


	// Runtime Functions

	void DrawFrame();
	void RecordCommandBuffer(VkCommandBuffer m_CommandBuffer, uint32_t imageIndex);
	void RecordRenderPass(uint32_t imageIndex);
	void BindPipelineInfo();
	void BindVertexBuffers();
	void DrawScene();

	// Semaphores and Fences

	VkSemaphore m_ImageAvailableSemaphore;
	VkSemaphore m_RenderFinishedSemaphore;
	VkFence m_InFlightFence;

	void CreateSyncObjects();





	float m_DeltaTime{};

	VkBuffer m_VertexBuffer{};
	VkDeviceMemory m_VertexBufferMemory{};

	VkBuffer m_IndexBuffer{};
	VkDeviceMemory m_IndexBufferMemory{};

	VkBuffer m_UniformBuffer;
	VkDeviceMemory m_UniformBufferMemory;
	void* m_UniformBufferMapped;
	VkDescriptorPool m_DescriptorPool;
	VkDescriptorSet m_DescriptorSet;


	Camera m_Camera{ glm::vec3{0.f, 1.f, -3.f}, 90.f };

	

	
	MachineShader m_MachineShader{"Shaders/shader.vert.spv", "Shaders/shader.frag.spv"};
		
	
	
	void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
	void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
	
	void CreateVertexBuffer();
	void CreateIndexBuffer();

	void CreateUnfiformBuffers();
	void UpdateUniformBuffer(uint32_t currentImage);
	void CreateDescriptorPool();
	void CreateDescriptorSets();

	
	uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
	


	






	QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice m_Device)
	{
		QueueFamilyIndices indices;

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(m_Device, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(m_Device, &queueFamilyCount, queueFamilies.data());

		int i = 0;
		for (const auto& queueFamily : queueFamilies)
		{
			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				indices.graphicsFamily = i;
			}

			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(m_Device, i, m_Surface, &presentSupport);

			if (presentSupport)
			{
				indices.presentFamily = i;
			}

			if (indices.IsComplete())
			{
				break;
			}

			i++;
		}

		return indices;
	}



	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback
	(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, 
		VkDebugUtilsMessageTypeFlagsEXT messageType, 
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, 
		void* pUserData
	) 
	{
		std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
		return VK_FALSE;
	}
};