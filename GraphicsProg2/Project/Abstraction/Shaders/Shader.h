#pragma once

#include <vector>
#include <string>

#include "vulkan/vulkan_core.h"

class Shader 
{
public:

	explicit Shader(const std::string& vertexShaderFile, const std::string& fragmentShaderFile)
		: m_VertexShaderFile{ vertexShaderFile }
		, m_FragmentShaderFile{ fragmentShaderFile }
	{
	}

	virtual ~Shader() = default;


	virtual void Initialize(const VkDevice& m_Device) = 0;

	auto& GetShaderStages() { return m_ShaderStages; }
	void DestroyShaderStages(const VkDevice& m_Device)
	{
		for (auto& shaderStage : m_ShaderStages)
		{
			vkDestroyShaderModule(m_Device, shaderStage.module, nullptr);
		}
	}

	virtual VkPipelineShaderStageCreateInfo CreateFragmentShaderInfo(const VkDevice& m_Device) = 0;
	virtual VkPipelineShaderStageCreateInfo CreateVertexShaderInfo(const VkDevice& m_Device) = 0;

	virtual VkPipelineVertexInputStateCreateInfo CreateVertexInputStateInfo() = 0;
	virtual VkPipelineInputAssemblyStateCreateInfo CreateInputAssemblyStateInfo() = 0;

	virtual VkShaderModule CreateShaderModule(const VkDevice& m_Device, const std::vector<char>& code) = 0;

protected:

	std::string m_FragmentShaderFile;
	std::string m_FSEntryPoint{ "main" };

	std::string m_VertexShaderFile;
	std::string m_VSEntryPoint{ "main" };

	std::vector<VkPipelineShaderStageCreateInfo> m_ShaderStages;
};