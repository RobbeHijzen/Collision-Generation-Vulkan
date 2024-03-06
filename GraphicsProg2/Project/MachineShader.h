#ifndef MACHINESHADER_H
#define MACHINESHADER_H

#include "vulkan/vulkan_core.h"
#include <string>
#include <vector>
#include <array>
#include "vulkanbase/VulkanUtil.h"
#include "glm/glm.hpp"


struct Vertex
{
	glm::vec2 pos;
	glm::vec3 color;

	static VkVertexInputBindingDescription GetBindingDescription() 
	{
		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(Vertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescription;
	}

	static std::array<VkVertexInputAttributeDescription, 2> GetAttributeDescriptions() 
	{
		std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, pos);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, color);

		return attributeDescriptions;
	}
};
const std::vector<Vertex> vertices = {
	{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
	{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
	{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
};


class MachineShader
{
public:
	explicit MachineShader(const std::string& vertexShaderFile, const std::string& fragmentShaderFile);

	// shader stages and shader modules
	void Initialize(const VkDevice& device)
	{
		m_ShaderStages.clear();
		m_ShaderStages.emplace_back(CreateVertexShaderInfo(device));
		m_ShaderStages.emplace_back(CreateFragmentShaderInfo(device));
	}

	std::vector<VkPipelineShaderStageCreateInfo>& GetShaderStages()
	{
		return m_ShaderStages;
	}
	void DestroyShaderStages(const VkDevice& device)
	{
		for (auto& shaderStage : m_ShaderStages)
		{
			vkDestroyShaderModule(device, shaderStage.module, nullptr);
		}
	}

	VkPipelineShaderStageCreateInfo CreateFragmentShaderInfo(const VkDevice& device)
	{
		std::vector<char> fragShaderCode = ReadFile("shaders/shader.frag.spv");
		VkShaderModule fragShaderModule = CreateShaderModule(device, fragShaderCode);

		VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
		fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderStageInfo.module = fragShaderModule;
		fragShaderStageInfo.pName = m_FSEntryPoint.c_str();

		return fragShaderStageInfo;
	}

	VkPipelineShaderStageCreateInfo CreateVertexShaderInfo(const VkDevice& device)
	{
		std::vector<char> vertShaderCode = ReadFile("shaders/shader.vert.spv");
		VkShaderModule vertShaderModule = CreateShaderModule(device, vertShaderCode);

		VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
		vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertShaderStageInfo.module = vertShaderModule;
		vertShaderStageInfo.pName = m_VSEntryPoint.c_str();
		return vertShaderStageInfo;
	}

	VkPipelineVertexInputStateCreateInfo CreateVertexInputStateInfo()
	{
		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexBindingDescriptionCount = 0;
		vertexInputInfo.vertexAttributeDescriptionCount = 0;
		return vertexInputInfo;
	}

	VkPipelineInputAssemblyStateCreateInfo CreateInputAssemblyStateInfo()
	{
		VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssembly.primitiveRestartEnable = VK_FALSE;
		return inputAssembly;
	}

	VkShaderModule CreateShaderModule(const VkDevice& device, const std::vector<char>& code)
	{
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

		VkShaderModule shaderModule;
		if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
			throw std::runtime_error("failed to create shader module!");
		}

		return shaderModule;
	}

	~MachineShader() = default;

private:

	std::string m_FragmentShaderFile;
	std::string m_FSEntryPoint{ "main" };
	
	std::string m_VertexShaderFile;
	std::string m_VSEntryPoint{ "main" };

	std::vector<VkPipelineShaderStageCreateInfo> m_ShaderStages;
};

#endif // !MACHINESHADER_H