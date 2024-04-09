#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include "VulkanUtil.h"
#include "labwork/Camera.h"

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
#include <MachineShader.h>
#include <chrono>
#include <glm/gtc/matrix_transform.hpp>

const std::vector<const char*> validationLayers{ "VK_LAYER_KHRONOS_validation" };
const std::vector<const char*> deviceExtensions{ VK_KHR_SWAPCHAIN_EXTENSION_NAME };

struct QueueFamilyIndices 
{
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	bool IsComplete() 
	{
		return graphicsFamily.has_value() && presentFamily.has_value();
	}
};

struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

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

	float m_DeltaTime{};

	std::vector<Mesh> m_Meshes{ Mesh{} };

	VkBuffer m_VertexBuffer{};
	VkDeviceMemory m_VertexBufferMemory{};

	VkBuffer m_IndexBuffer{};
	VkDeviceMemory m_IndexBufferMemory{};

	std::vector<VkBuffer> m_UniformBuffers;
	std::vector<VkDeviceMemory> m_UniformBuffersMemory;
	std::vector<void*> m_UniformBuffersMapped;
	VkDescriptorPool m_DescriptorPool;
	std::vector<VkDescriptorSet> m_DescriptorSets;

	uint32_t m_CurrentFrame = 0;

	Camera m_Camera{ glm::vec3{0.f, 0.f, 1.f}, 90.f };

	void InitializeVulkan() 
	{
		// week 06
		
		CreateInstance();
		SetupDebugMessenger();
		CreateSurface();

		// week 05
		PickPhysicalDevice();
		CreateLogicalDevice();

		// week 04 
		CreateSwapChain();
		CreateImageViews();
		
		// week 03
		m_MachineShader.Initialize(m_Device);
		CreateRenderPass();
		CreateDescriptorSetLayout();
		CreateGraphicsPipeline();
		CreateFrameBuffers();

		// week 02 (and a bit of Week 3 (Index Buffer))
		CreateCommandPool();
		CreateVertexBuffer();
		CreateIndexBuffer();
		CreateUnfiformBuffers();
		CreateDescriptorPool();
		CreateDescriptorSets();

		CreateCommandBuffer();

		// week 06
		CreateSyncObjects();
	}

	void MainLoop() 
	{
		static auto startTime = std::chrono::high_resolution_clock::now();

		while (!glfwWindowShouldClose(m_Window)) 
		{
			glfwPollEvents();
			// week 06
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

		// Uniform buffers cleanup
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			vkDestroyBuffer(m_Device, m_UniformBuffers[i], nullptr);
			vkFreeMemory(m_Device, m_UniformBuffersMemory[i], nullptr);
		}

		vkDestroyDescriptorPool(m_Device, m_DescriptorPool, nullptr);
		vkDestroyDescriptorSetLayout(m_Device, m_DescriptorSetLayout, nullptr);

		// Destroy Vertex and Index buffers
		vkDestroyBuffer(m_Device, m_IndexBuffer, nullptr);
		vkFreeMemory(m_Device, m_IndexBufferMemory, nullptr);

		vkDestroyBuffer(m_Device, m_VertexBuffer, nullptr);
		vkFreeMemory(m_Device, m_VertexBufferMemory, nullptr);
		//---------------------------------

		vkDestroySemaphore(m_Device, m_RenderFinishedSemaphore, nullptr);
		vkDestroySemaphore(m_Device, m_ImageAvailableSemaphore, nullptr);
		vkDestroyFence(m_Device, m_InFlightFence, nullptr);

		vkDestroyCommandPool(m_Device, m_CommandPool, nullptr);
		
		vkDestroyDevice(m_Device, nullptr);


		if (enableValidationLayers) 
		{
			DestroyDebugUtilsMessengerEXT(m_Instance, m_DebugMessenger, nullptr);
		}

		vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
		vkDestroyInstance(m_Instance, nullptr);

		glfwDestroyWindow(m_Window);
		glfwTerminate();
	}

	void CreateSurface() 
	{
		if (glfwCreateWindowSurface(m_Instance, m_Window, nullptr, &m_Surface) != VK_SUCCESS) 
		{
			throw std::runtime_error("failed to create window surface!");
		}
	}

	
	GLFWwindow* m_Window;
	// Important to initialize before creating the graphics pipeline
	MachineShader m_MachineShader{"shaders/shader.vert.spv", "shaders/shader.frag.spv"};
	void InitializeWindow();
	void DrawScene();
	

	void CreateDescriptorSetLayout();

	
	// Command Buffer
	VkCommandPool m_CommandPool;
	VkCommandBuffer m_CommandBuffer;

	QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice m_Device);

	void DrawFrame(uint32_t imageIndex);
	void CreateCommandBuffer();
	void CreateCommandPool(); 
	void RecordCommandBuffer(VkCommandBuffer m_CommandBuffer, uint32_t imageIndex);
	
	void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
	void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
	
	void CreateVertexBuffer();
	void CreateIndexBuffer();

	void CreateUnfiformBuffers();
	void UpdateUniformBuffer(uint32_t currentImage);
	void CreateDescriptorPool();
	void CreateDescriptorSets();

	
	uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
	
	// RenderPass
	std::vector<VkFramebuffer> m_SwapChainFramebuffers;
	VkDescriptorSetLayout m_DescriptorSetLayout;
	VkPipelineLayout m_PipelineLayout;
	VkPipeline m_GraphicsPipeline;
	VkRenderPass m_RenderPass;

	void CreateFrameBuffers();
	void CreateRenderPass();
	void CreateGraphicsPipeline();

	// SwapChain
	VkSwapchainKHR m_SwapChain;
	std::vector<VkImage> m_SwapChainImages;
	VkFormat m_SwapChainImageFormat;
	VkExtent2D m_SwapChainExtent;

	std::vector<VkImageView> m_SwapChainImageViews;

	SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice m_Device);
	VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
	VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	void CreateSwapChain();
	void CreateImageViews();


	// Logical and Physical Devices
	VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
	VkQueue m_GraphicsQueue;
	VkQueue m_PresentQueue;
	
	void PickPhysicalDevice();
	bool IsDeviceSuitable(VkPhysicalDevice m_Device);
	void CreateLogicalDevice();

	
	// Main Initialization
	VkInstance m_Instance;
	VkDebugUtilsMessengerEXT m_DebugMessenger;
	VkDevice m_Device = VK_NULL_HANDLE;
	VkSurfaceKHR m_Surface;

	VkSemaphore m_ImageAvailableSemaphore;
	VkSemaphore m_RenderFinishedSemaphore;
	VkFence m_InFlightFence;

	void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
	void SetupDebugMessenger();
	std::vector<const char*> GetRequiredExtensions();
	bool CheckDeviceExtensionSupport(VkPhysicalDevice m_Device);
	void CreateInstance();

	void CreateSyncObjects();
	void DrawFrame();

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