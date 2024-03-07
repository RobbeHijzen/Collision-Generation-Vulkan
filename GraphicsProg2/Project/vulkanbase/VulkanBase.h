#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include "VulkanUtil.h"

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

	Mesh m_Mesh{};
	VkBuffer m_VertexBuffer{};
	VkDeviceMemory m_VertexBufferMemory{};


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
		CreateGraphicsPipeline();
		CreateFrameBuffers();
		// week 02
		CreateCommandPool();
		CreateVertexBuffer();
		CreateCommandBuffer();

		// week 06
		CreateSyncObjects();
	}

	void MainLoop() 
	{
		while (!glfwWindowShouldClose(m_Window)) 
		{
			glfwPollEvents();
			// week 06
			DrawFrame();
		}
		vkDeviceWaitIdle(m_Device);
	}

	void Cleanup() 
	{
		vkDestroySemaphore(m_Device, m_RenderFinishedSemaphore, nullptr);
		vkDestroySemaphore(m_Device, m_ImageAvailableSemaphore, nullptr);
		vkDestroyFence(m_Device, m_InFlightFence, nullptr);

		vkDestroyCommandPool(m_Device, m_CommandPool, nullptr);
		for (auto framebuffer : m_SwapChainFramebuffers) {
			vkDestroyFramebuffer(m_Device, framebuffer, nullptr);
		}

		vkDestroyPipeline(m_Device, m_GraphicsPipeline, nullptr);
		vkDestroyPipelineLayout(m_Device, m_PipelineLayout, nullptr);
		vkDestroyRenderPass(m_Device, m_RenderPass, nullptr);

		for (auto imageView : m_SwapChainImageViews) {
			vkDestroyImageView(m_Device, imageView, nullptr);
		}

		if (enableValidationLayers) {
			DestroyDebugUtilsMessengerEXT(m_Instance, m_DebugMessenger, nullptr);
		}
		vkDestroySwapchainKHR(m_Device, m_SwapChain, nullptr);

		vkDestroyBuffer(m_Device, m_VertexBuffer, nullptr);
		vkFreeMemory(m_Device, m_VertexBufferMemory, nullptr);

		vkDestroyDevice(m_Device, nullptr);

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

	
	// Command Buffer
	VkCommandPool m_CommandPool;
	VkCommandBuffer m_CommandBuffer;

	QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice m_Device);

	void DrawFrame(uint32_t imageIndex);
	void CreateCommandBuffer();
	void CreateCommandPool(); 
	void RecordCommandBuffer(VkCommandBuffer m_CommandBuffer, uint32_t imageIndex);
	
	void CreateVertexBuffer();
	uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
	
	// RenderPass
	std::vector<VkFramebuffer> m_SwapChainFramebuffers;
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