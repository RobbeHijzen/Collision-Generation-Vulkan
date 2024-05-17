#ifndef VULKANBASE_HEADER
#define VULKANBASE_HEADER

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>


#include "VulkanUtil/VulkanUtil.h"
#include "Abstraction/HelperStructs.h"
#include "Abstraction/Camera.h"
#include "Abstraction/VertexInfo.h"
#include "Abstraction/Shaders/Shader.h"
#include "Abstraction/Shaders/DerivedShaders/Shader3D.h"
#include "Abstraction/Scene/Scene.h"
#include "Abstraction/Meshes/Mesh.h"
#include "Abstraction/Meshes/DerivedMeshes/PlayerMesh.h"

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


	auto GetDevice() const { return m_Device; }
	
	auto GetUniformBuffers() const { return m_UniformBuffers; }

	auto GetTextureImageViews() const { return m_TextureImageViews; }
	auto GetTextureSampler() const { return m_TextureSampler; }

	auto GetMeshDescriptorSets() const { return m_MeshDescriptorSets; }


private:

	void InitializeWindow()
	{
		glfwInit();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		m_Window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
		glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
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

		// Load the scene
		InitializeAbstractions();
		LoadScene();

		// GraphicsPipeline setup
		CreateRenderPass();
		CreateDescriptorSetLayouts();
		CreateGraphicsPipelines();

		// Command Buffers setup
		CreateCommandPool();
		CreateCommandBuffer();

		// Depth Buffering setup
		CreateDepthResources();

		// Frame Buffers setup
		CreateFrameBuffers();

		// Index and Vertex buffer setup
		CreateVertexBuffers();
		CreateIndexBuffers();

		// Textures setup
		CreateTextureImages();
		CreateTextureImageViews();
		CreateTextureSampler();

		// Uniform buffer setup
		CreateUnfiformBuffers();
		CreateDescriptorPool();
		CreateDescriptorSets();

		// Semaphore and Fence setup
		CreateSyncObjects();
	}
	void MainLoop()
	{
		static auto startTime = std::chrono::high_resolution_clock::now();

		while (!glfwWindowShouldClose(m_Window))
		{
			glfwPollEvents();

			// Update Camera and Meshes
			m_Camera->Update(m_DeltaTime, m_Window);
			for (auto mesh : m_Scene->GetMeshes())
			{
				mesh->Update(m_DeltaTime, m_Window);
			}

			// Where the magic happens
			DrawFrame();

			auto currentTime = std::chrono::high_resolution_clock::now();
			m_DeltaTime = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
			startTime = currentTime;
		}
		vkDeviceWaitIdle(m_Device);
	}
	void Cleanup()
	{
		// Texture Images cleanup
		vkDestroySampler(m_Device, m_TextureSampler, nullptr);
		for (auto& textureImageView : m_TextureImageViews)
		{
			vkDestroyImageView(m_Device, textureImageView, nullptr);
		}
		for (auto& textureImage : m_TextureImages)
		{
			vkDestroyImage(m_Device, textureImage, nullptr);
		}
		for (auto& textureImageMemory : m_TextureImagesMemory)
		{
			vkFreeMemory(m_Device, textureImageMemory, nullptr);
		}
		

		// Depth Buffer cleanup
		vkDestroyImage(m_Device, m_DepthImage, nullptr);
		vkFreeMemory(m_Device, m_DepthImageMemory, nullptr);
		vkDestroyImageView(m_Device, m_DepthImageView, nullptr);

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
		vkDestroyDescriptorSetLayout(m_Device, m_DescriptorSetLayout, nullptr);
		
		vkDestroyPipelineLayout(m_Device, m_PipelineLayout, nullptr);
		vkDestroyRenderPass(m_Device, m_RenderPass, nullptr);

		// Uniform buffer cleanup
		for (auto& uniformBuffer : m_UniformBuffers)
		{
			vkDestroyBuffer(m_Device, uniformBuffer, nullptr);
		}
		for (auto& uniformBufferMemory : m_UniformBuffersMemory)
		{
			vkFreeMemory(m_Device, uniformBufferMemory, nullptr);
		}

		vkDestroyDescriptorPool(m_Device, m_DescriptorPool, nullptr);

		// Destroy Vertex and Index buffers
		for (auto& vertexBuffer : m_VertexBuffers)
		{
			vkDestroyBuffer(m_Device, vertexBuffer, nullptr);
		}
		for (auto& vertexBufferMemory : m_VertexBuffersMemory)
		{
			vkFreeMemory(m_Device, vertexBufferMemory, nullptr);
		}
		for (auto& indexBuffer : m_IndexBuffers)
		{
			vkDestroyBuffer(m_Device, indexBuffer, nullptr);
		}
		for (auto& indexBufferMemory : m_IndexBuffersMemory)
		{
			vkFreeMemory(m_Device, indexBufferMemory, nullptr);
		}

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
	

	void InitializeAbstractions()
	{
		m_Scene = std::make_unique<Scene>();

		m_Shader3D = std::make_unique<Shader3D>("Resources/Shaders/shader3D.vert.spv", "Resources/Shaders/shader3D.frag.spv");
		m_Shader3D->Initialize(m_Device);

		m_Camera = std::make_unique<Camera>(glm::vec3{}, 150.f);
	}
	void LoadScene()
	{
		m_Scene->AddMesh(new Mesh("Resources/xm177.obj", "Resources/xm177_basecolor.png", { 5.f, 5.f, 25.f }, {}, { 0.4f, 0.4f, 0.4f }));
		m_Scene->AddMesh(new Mesh("Resources/vehicle.obj", "Resources/vehicle_diffuse.png", { -20, 8.5f, 0 }, { 0, -90, 0 }));
		m_Scene->AddMesh(new Mesh("Resources/Plane.obj", "Resources/Plane_BaseColor.png", {}, {}, {0.5f, 0.5f, 0.5f}));
		
		auto manny{ new PlayerMesh("Resources/Manny.obj", "Resources/Manny_BaseColor.png", { 0, 0, 0 }, { -90, 180, 0 }, {0.1f, 0.1f, 0.1f}) };
		manny->AttachCamera(m_Camera.get());
		m_Scene->AddMesh(manny);


		for (auto mesh : m_Scene->GetMeshes())
		{
			mesh->GameStart();
		}
	}


	// General variables
	float m_DeltaTime{};

	std::unique_ptr<Scene> m_Scene;
	std::unique_ptr<Shader> m_Shader3D;

	std::unique_ptr<Camera> m_Camera;

	// Window / Surface setup
	GLFWwindow* m_Window{};
	VkSurfaceKHR m_Surface{};

	void CreateSurface()
	{
		if (glfwCreateWindowSurface(m_Instance, m_Window, nullptr, &m_Surface) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create window surface!");
		}
	}

	// Instance and Validation Layer setup
	VkInstance m_Instance{};
	VkDebugUtilsMessengerEXT m_DebugMessenger{};

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
	VkQueue m_GraphicsQueue{};
	VkQueue m_PresentQueue{};

	void CreateLogicalDevice();

	// Swap Chain setup
	VkSwapchainKHR m_SwapChain{};
	std::vector<VkImage> m_SwapChainImages{};
	VkFormat m_SwapChainImageFormat{};
	VkExtent2D m_SwapChainExtent{};

	std::vector<VkImageView> m_SwapChainImageViews{};
	std::vector<VkFramebuffer> m_SwapChainFramebuffers{};

	SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice m_Device);

	VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

	void CreateSwapChain();
	void CreateImageViews();
	VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
	void CreateFrameBuffers();

	// GraphicsPipeline setup
	VkDescriptorSetLayout m_DescriptorSetLayout{};
	VkPipeline m_GraphicsPipeline{};
	VkPipelineLayout m_PipelineLayout{};
	VkRenderPass m_RenderPass{};

	void CreateGraphicsPipelines();
	void CreateDescriptorSetLayouts();
	void CreateRenderPass();

	// Command Buffer setup
	VkCommandPool m_CommandPool{};
	VkCommandBuffer m_CommandBuffer{};

	void CreateCommandPool();
	void CreateCommandBuffer();


	// Runtime Functions
	void DrawFrame();
	void RecordCommandBuffer(VkCommandBuffer m_CommandBuffer, uint32_t imageIndex);
	void RecordRenderPass(uint32_t imageIndex);
	void BindPipelineInfo();
	void BindVertexIndexBuffers(uint32_t buffersIndex);

	// Semaphores and Fences
	VkSemaphore m_ImageAvailableSemaphore{};
	VkSemaphore m_RenderFinishedSemaphore{};
	VkFence m_InFlightFence{};

	void CreateSyncObjects();

	// Vertex and Index buffers
	std::vector<VkBuffer> m_VertexBuffers{};
	std::vector<VkDeviceMemory> m_VertexBuffersMemory{};

	std::vector<VkBuffer> m_IndexBuffers{};
	std::vector<VkDeviceMemory> m_IndexBuffersMemory{};

	void CreateVertexBuffers();
	void CreateIndexBuffers();

	void CreateVertexBuffer(std::vector<Vertex> vertices, VkBuffer& vertexBuffer, VkDeviceMemory& vertexBufferMemory);
	void CreateIndexBuffer(std::vector<uint32_t> indices, VkBuffer& indexBuffer, VkDeviceMemory& indexBufferMemory);


	// Uniform buffer
	VkDescriptorPool m_DescriptorPool{};

	std::vector<VkBuffer> m_UniformBuffers{};
	std::vector<VkDeviceMemory> m_UniformBuffersMemory{};
	std::vector<void*> m_UniformBuffersMapped{};
	std::vector<VkDescriptorSet> m_MeshDescriptorSets{};

	void CreateDescriptorSets();
	void CreateUnfiformBuffers();

	void UpdateUniformBuffer(uint32_t currentImage, uint32_t meshIndex, glm::mat4 meshModelMatrix);
	void CreateDescriptorPool();

	// Texture
	std::vector<VkImage> m_TextureImages;
	std::vector<VkImageView> m_TextureImageViews;
	std::vector<VkDeviceMemory> m_TextureImagesMemory;

	VkSampler m_TextureSampler;

	void CreateTextureImages();
	void CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage,
					 VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
	void CreateTextureImageViews();
	void CreateTextureSampler();

	void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
	void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

	// Depth Buffering
	VkImage m_DepthImage;
	VkDeviceMemory m_DepthImageMemory;
	VkImageView m_DepthImageView;

	void CreateDepthResources();


	// Helper functions
	void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
	void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

	uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

	VkCommandBuffer BeginSingleTimeCommands();
	void EndSingleTimeCommands(VkCommandBuffer commandBuffer);
	
	VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
	VkFormat FindDepthFormat();
	bool HasStencilComponent(VkFormat format);

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
#endif